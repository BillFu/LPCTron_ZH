# -*- coding:utf-8 -*-

import json
import time
from io import BytesIO
import os.path
import functools
import struct
import threading
import argparse
from pathlib import Path
import sys

from .message import MSG_TYPE, build_msg_package

from twisted.protocols.ftp import FTPClient, FTPFileListProtocol
from twisted.internet.protocol import Protocol, ClientFactory, ClientCreator
from twisted.internet import reactor, threads
from twisted.internet.endpoints import TCP4ServerEndpoint


kill_flag = threading.Event()

def progress_monitor():
	loop = True
	while loop:
		time.sleep(0.2)
		if kill_flag.isSet():
			loop = False

	reactor.callFromThread(reactor.stop)


class TTS_Client_Protocol(Protocol):
	def __init__(self, ftp_manager, config):
		self.ftp_manager = ftp_manager
		self.__data_buffer = bytes()
		self.config = config
		if "sample_rate" in config:
			self.sample_rate = config["sample_rate"]
		else:
			self.sample_rate = 16000  # default

	def handle_wav_ready(self, received_json):
		sentence_id = received_json['sentence_id']
		wav_file_name = received_json['wav_file_name']
		wav_file_size = received_json['wav_file_size']
		self.ftp_manager.download_wav(wav_file_name, wav_file_size)
		print("OK: downloading request has been submit for sentence id: {}."
			  .format(sentence_id))

	def handle_check_passed(self, received_json):
		sentence_id = received_json['sentence_id']
		print("OK: Succeed to pass check, sentence_id: {}".format(sentence_id))

	def handle_check_failed(self, received_json):
		sentence_id = received_json['sentence_id']
		sentence = received_json['sentence']
		print("ERROR: Failed to pass check, sentence_id: {}, sentence: {}".
			  format(sentence_id, sentence))
		self.ftp_manager.job_count -= 1
		if self.ftp_manager.job_count == 0:
			kill_flag.set()

	def dataReceived(self, data):
		self.__data_buffer += data
		headerSize = 12

		while True:
			if len(self.__data_buffer) < headerSize:
				return

			# struct:!Network order3I3unsigned int
			headPack = struct.unpack('!3I', self.__data_buffer[:headerSize])
			bodySize = headPack[1]
			msg_type = headPack[2]

			if len(self.__data_buffer) < headerSize + bodySize:
				return

			body = self.__data_buffer[headerSize:headerSize + bodySize]
			msg_json = json.loads(body, encoding='utf-8')
			self.msgHandle(msg_type, msg_json)
			self.__data_buffer = self.__data_buffer[headerSize + bodySize:]

	def msgHandle(self, msg_type, msg_json):
		print("msg type: {}, body: {}".format(msg_type, msg_json))
		if msg_type == MSG_TYPE.REQUEST_CHECK_PASSED.value:
			self.handle_check_passed(msg_json)
		elif msg_type == MSG_TYPE.REQUEST_CHECK_FAILED.value:
			self.handle_check_failed(msg_json)
		elif msg_type == MSG_TYPE.WAV_READY.value:
			self.handle_wav_ready(msg_json)
		else:
			print("Error: Received Message cannot be handled: {}".format(msg_json))

	def connectionLost(self, reason):
		global kill_flag
		print("connection Lost!")
		kill_flag.set()
		if reactor.running:
			reactor.stop()

	def request_thread(self):
		Sentences = self.factory.sentences
		SentenceIndex = 0
		while SentenceIndex < len(Sentences):
			SentenceID = "{:06d}".format(SentenceIndex+1)
			self.send_tts_request(SentenceID, Sentences[SentenceIndex])
			SentenceIndex += 1
			time.sleep(0.5)

	def send_tts_request(self, SentenceID, Sentence):
		request_json = {
			'sentence_id': SentenceID,
			'sentence': Sentence,
			'sample_rate': self.sample_rate,
			'volume_boost': 0,
			'speed_scale': 0.83}

		msg_package = build_msg_package(MSG_TYPE.JOB_REQUEST, request_json)
		self.transport.write(msg_package)

	def connectionMade(self):
		cb = functools.partial(self.request_thread)
		d = threads.deferToThread(cb)
		# d.addCallback(self.send_result)


class TTS_Client_Factory(ClientFactory):
	def __init__(self, ftp_manager, sentences, config):
		self.ftp_manager = ftp_manager
		self.sentences = sentences
		self.config = config

	def buildProtocol(self, addr):
		protocol = TTS_Client_Protocol(self.ftp_manager, self.config)
		protocol.factory = self
		return protocol

########################## download wav file via FTP ###########################################
class FTP_Managet:
	def __init__(self, total_job_num, wav_save_dir):
		self.job_count = total_job_num  # !!! must use self
		self.wav_save_dir = wav_save_dir

	class DownloadProtocol(Protocol):
		def __init__(self, wav_filename, file_size, ftp_manager):
			self.buffer = BytesIO()
			self.file_name = wav_filename
			self.file_szie = file_size
			self.ftp_manager = ftp_manager

		def dataReceived(self, data):
			self.buffer.write(data)
			size_received = len(self.buffer.getvalue())
			# print("data size received: {}".format(size_received))
			self.trySaveWavFile()

		def trySaveWavFile(self):
			global kill_flag

			size_received = len(self.buffer.getvalue())
			if self.file_szie == size_received:  # downloading finished
				file_to_save = os.path.join(
					self.ftp_manager.wav_save_dir, self.file_name)
				with open(file_to_save, mode="wb") as file:
					file.write(self.buffer.getvalue())
					print("OK: wave file has been saved as: {}".format(file_to_save))
				self.buffer.close()
				self.ftp_manager.job_count -= 1
				'''
				print("self.ftp_manager.job_count : {}".
					  format(self.ftp_manager.job_count))
				'''
				if self.ftp_manager.job_count == 0:
					kill_flag.set()

		def connectionLost(self, reason):
			lost_reason = reason.getErrorMessage()
			if lost_reason == "Connection was closed cleanly.":
				pass
			else:
				print("FTP channel broken by exception: {}".format(lost_reason))

	def connection_failed(self, ftpClient):
		print("failed to connect tts server via FTP!")
		reactor.stop()

	def success(self, response):
		print('Success!  Got response:')
		print('---')
		if response is None:
			print(None)
		else:
			print("\n".join(response))
		print('---')

	def connection_made(self, ftpClient):
		self.ftpClient = ftpClient
		print("succeed to connect tts server via FTP!")

	def connect(self, username, password, tts_server_ip, ftp_server_port):
		passive = 0
		creator = ClientCreator(reactor, FTPClient, username, password, passive)
		d = creator.connectTCP(tts_server_ip, ftp_server_port)
		d.addCallback(self.connection_made)
		d.addErrback(self.connection_failed)

	def download_wav(self, wav_file_name, wav_file_size):
		download_proto = FTP_Managet.DownloadProtocol(
			wav_file_name, wav_file_size, self)
		d = self.ftpClient.retrieveFile(wav_file_name, download_proto)

	def fail(self, error):
		print('Failed.  Error was:')
		print(error)

################################################################################

def load_config(config_file):
	my_file = Path(config_file)
	if not my_file.is_file():
		#  does not exist
		error_reason = "/'{}' does not exist!".format(config_file)
		return None, error_reason

	with open(config_file, 'r', encoding='utf-8') as f:
		config = json.load(f, strict=False)
	return config, "OK"

def main():
	parser = argparse.ArgumentParser()
	parser.add_argument("--config_file",
		help="full path to confile file."
	)
	args = parser.parse_args()

	config, error_reason = load_config(args.config_file)
	if config is None:
		print(error_reason)
		sys.exit(0)

	sample_rate = config["sample_rate"]

	in_text_file = config["in_text_file"]
	wav_save_dir = config["wav_save_dir"]

	tts_server_ip = config["tts_server_ip"]
	tts_server_port = config["tts_server_port"]
	ftp_server_port = config["ftp_server_port"]

	username = 'anonymous'  # 'test'
	password = 'twisted'  # '2018'

	with open(in_text_file, 'r', encoding='utf-8') as in_txt_file:
		raw_sentences = in_txt_file.readlines()

	sentences = []
	for sentence in raw_sentences:
		cleaned_sentence = sentence.strip()
		sentences.append(cleaned_sentence)

	total_job_num = len(sentences)

	ftp_mgr = FTP_Managet(total_job_num, wav_save_dir)
	ftp_mgr.connect(username, password, tts_server_ip, ftp_server_port)

	tts_client_factory = TTS_Client_Factory(ftp_mgr, sentences, config)
	reactor.connectTCP(tts_server_ip, tts_server_port, tts_client_factory)

	poller = threading.Thread(target=progress_monitor)
	poller.start()

	reactor.run()


if __name__ == '__main__':
	main()