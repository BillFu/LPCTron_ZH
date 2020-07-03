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


from twisted.internet.protocol import Protocol, ClientFactory, ClientCreator
from twisted.internet import reactor, threads
from twisted.internet.endpoints import TCP4ServerEndpoint

# ensulate the communication session with backend_server
class Network_Protocol(Protocol):
	# def __init__(self, config):
	def __init__(self):
		self.__data_buffer = bytes()
		# self.config = config

	'''
	def handle_wav_ready(self, received_json):
		sentence_id = received_json['sentence_id']
		wav_file_name = received_json['wav_file_name']
		wav_file_size = received_json['wav_file_size']
		print("OK: downloading request has been submit for sentence id: {}."
			  .format(sentence_id))
	'''

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
		"""
		if msg_type == MSG_TYPE.REQUEST_CHECK_PASSED.value:
			self.handle_check_passed(msg_json)
		elif msg_type == MSG_TYPE.REQUEST_CHECK_FAILED.value:
			self.handle_check_failed(msg_json)
		elif msg_type == MSG_TYPE.WAV_READY.value:
			self.handle_wav_ready(msg_json)
		else:
			print("Error: Received Message cannot be handled: {}".format(msg_json))
		"""


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


class Network_Factory(ClientFactory):
	#def __init__(self, , config):
	# self.config = config

	def buildProtocol(self, addr):
		protocol = Network_Protocol()
		protocol.factory = self
		return protocol


def main():
	parser = argparse.ArgumentParser()
	parser.add_argument("--config_file",
		help="full path to confile file."
	)
	args = parser.parse_args()

	'''
	config, error_reason = load_config(args.config_file)
	if config is None:
		print(error_reason)
		sys.exit(0)

	tts_server_ip = config["tts_server_ip"]
	tts_server_port = config["tts_server_port"]
	'''

	tts_server_ip = "127.0.0.1"
	tts_server_port = 8001

	network_factory = Network_Factory()
	reactor.connectTCP(tts_server_ip, tts_server_port, network_factory)

	# poller = threading.Thread(target=progress_monitor)
	# poller.start()

	reactor.run()


if __name__ == '__main__':
	main()