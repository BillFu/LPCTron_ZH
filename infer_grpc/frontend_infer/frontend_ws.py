import logging
import argparse
import sys
import grpc

from flask import Flask, jsonify, request, render_template

from .my_logger import Log_info
from .assistant import load_config_data
from .message import Request_Illegal_Codes

from .frontend.hz2ipa import createCmdPairTuple
from .frontend_utils import build_ipa_seq_str, combine_normal_result

from .request_check import check_request

from .backend_infer_pb2 import JobRequest
from .backend_infer_pb2_grpc import backend_inferStub

from timeit import default_timer as timer
from datetime import timedelta

#######################################################################
# LPCTron TTS Web Service
# Version 1.0.0
lpctron_tts_version = "1.0.0"

#######################################################################

app = Flask(__name__)
app.config['SECRET_KEY'] = 'f4scuy0bbucy10bG78gd2o9G1V3GkQ4=\n'
backend_stub = None


@app.route('/')
def hello_world():
	return render_template('index.html')


@app.route('/lpctron_api/', methods=['GET'])
def inference():
	global config

	sentence_id, code, reason, job = check_request(config, request)

	if code != Request_Illegal_Codes.ALL_IS_OK.value:  # FAILED to pass check
		result = {"code": -100, "sentence_id": sentence_id, "reason": reason}
		return jsonify(result)

	# -------# should be deleted when to release ------------
	# start = timer()
	# -------------------------------------------------------

	normal_result = job["normal_result"]
	normalized_sentence = combine_normal_result(normal_result)
	print("final normal string: {}".format(normalized_sentence))

	# up here, text normalization finished, basic cleaning done,
	# punctuations still remained.
	ipa_id_seq_str = build_ipa_seq_str(normalized_sentence)
	print("ipa_id_seq_str: {}".format(ipa_id_seq_str))

	if job["sample_rate"] == 16000:
		sr = "16k"
	else:
		sr = "8k"

	job_request = JobRequest(sentence_id=sentence_id, sr=sr,
							 ipa_id_seq=ipa_id_seq_str)

	try:
		job_response = backend_stub.commitJob(job_request)
		print("response received, isOK: {}".format(job_response.isOK))
		# return (True, job_response, "gRPC call is OK.")

		# -------# should be deleted when to release--------------
		"""
		end = timer()
		consumed_time = timedelta(seconds=end - start)
		print("====time elapsed: {}====".format(consumed_time))
		"""
		# --------------------------------------------------------
		if job_response.isOK:  # everything is OK
			wav_file_name = sentence_id +".wav"
			result = {"code": 100, "sentence_id": sentence_id,
				  "wav_file": wav_file_name}
			return jsonify(result)
		else:  # something wrong happened
			result = {"code": -100, "sentence_id": sentence_id,
					  "reason": job_response.error_msg}
			return jsonify(result)
	except grpc.RpcError as rpc_error_call:
		code = rpc_error_call.code()
		# print("Exception Happened to call gRPC, error code: {}".format(code))
		error_reason0 = "gRPC Error: {}".format(code)
		result = {"code": -100, "sentence_id": sentence_id,
				  "reason": error_reason0}
		return jsonify(result)


def make_prepare():
	global config, logger
	global backend_stub

	# don't forget to initialize py2ipa module by calling the following function!!!
	createCmdPairTuple()

	backend_host = config["backend_host"]
	backend_port = config["backend_port"]
	is_ok, backend_stub = init_grpc_stub(backend_host, backend_port)

	return (is_ok, "No_Error")


def init_grpc_stub(backend_host, backend_port):
	server_address = "%s:%d" % (backend_host, backend_port)

	# 连接 rpc 服务器
	channel = grpc.insecure_channel(server_address)
	# 调用 rpc 服务
	backend_stub0 = backend_inferStub(channel)

	return True, backend_stub0


if __name__ == '__main__':
	parser = argparse.ArgumentParser(
		description="LPCTron TTS Web Service.")

	parser.add_argument(
		"--config_file",
		help="full path of config file.")

	args = parser.parse_args()

	config, has_error, error_reason = load_config_data(args.config_file)
	if has_error:
		logger.error(error_reason)
		sys.exit(0)

	server_id = config["ws_server_id"]
	server_port = config["ws_server_port"]

	log_info = Log_info("lt_ws_{}".format(server_id))
	# logger is a global variable
	logger = log_info.main(logging.DEBUG, logging.DEBUG, logging.DEBUG)
	logger.info("This Server with an ID: {}.".format(server_id))
	logger.info("The config data has been successfully loaded.")

	is_ok, error_reason = make_prepare()
	if not is_ok:
		logger.error(error_reason)
		sys.exit(0)

	logger.info("LPCTron TTS Web Service (version: {}) is ready to serve ..."
				.format(lpctron_tts_version))

	app.run(host='0.0.0.0', port=server_port, debug=False)
	"""
	sentence = "一样的日子，异样的你和我。"
	# sentence = "您的账户余额为[22222.78]{General_Numeric}元，拖欠数额为[10234.22]{General_Numeric}元"

	can_normal, failed_reason, normal_result = canNormalizeV3(sentence)
	print("normal_result: {}".format(normal_result))

	normalized_sentence = combine_normal_result(normal_result)
	print("final normal string: {}".format(normalized_sentence))

	ipa_id_seq_str = build_ipa_seq_str(normalized_sentence)
	print("ipa_id_seq_str: {}".format(ipa_id_seq_str))
	"""
