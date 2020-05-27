import grpc
from .backend_infer_pb2 import JobRequest
from .backend_infer_pb2_grpc import backend_inferStub

from .frontend.ipa_text import text_to_sequence

from .frontend.hz2ipa import createCmdPairTuple, hz2py, \
	filter_punct_mark, cal_ipa_seq


# input a normalized and cleaned sentence, return the ipa id sequence of it
# the return value has such form: [55, 46, 21, ..., 85]
def build_ipa_seq(normalized_hz_line):
	cleaned_sentence = filter_punct_mark(normalized_hz_line)
	py_seq = hz2py(cleaned_sentence)
	py_line = " ".join(py_seq)  # convert list into string
	print("py_line: {}".format(py_line))

	ipa_seq = cal_ipa_seq(normalized_hz_line, py_line)
	print("ipa_seq: {}".format(ipa_seq))

	ipa_id_seq = text_to_sequence(ipa_seq)
	return ipa_id_seq


def build_ipa_seq_str(normalized_hz_line):
	ipa_id_seq = build_ipa_seq(normalized_hz_line)
	ipa_id_str_seq = [str(id) for id in ipa_id_seq]
	ipa_id_seq_str = " ".join(ipa_id_str_seq)
	return ipa_id_seq_str


def initialize(server_address):
	# don't forget to initialize py2ipa module by calling the following function!!!
	createCmdPairTuple()

	# 连接 rpc 服务器
	channel = grpc.insecure_channel(server_address)
	# 调用 rpc 服务
	backend_stub = backend_inferStub(channel)

	return True, backend_stub


def do_infer(backend_stub, sentence_id, normalized_hz_line, sr):
	ipa_id_seq_str = build_ipa_seq_str(normalized_hz_line)

	job_request = JobRequest(sentence_id=sentence_id, sr=sr,
		ipa_id_seq=ipa_id_seq_str)

	try:
		job_response = backend_stub.commitJob(job_request)
		print("response received, isOK: {}".format(job_response.isOK))
	except grpc.RpcError as rpc_error_call:
		code = rpc_error_call.code()
		print("Exception Happened to call gRPC, error code: {}".format(code))


if __name__ == '__main__':
	backend_host = "localhost"
	backend_port = 50051

	if not backend_host:
		print("the Host of backend server Not set correctly!")
		exit(1)

	if not backend_port:
		print("the Port of backend server is listening to Not set correctly!")
		exit(1)

	server_address = "%s:%d" % (backend_host, backend_port)

	"""
	parser = argparse.ArgumentParser()

	parser.add_argument('--input_sentence',
		help='chinese sentence to be synthesized.')

	parser.add_argument('--sentence_id',
						default='1001',
						help='sentence id')

	parser.add_argument('--output_dir',
						default='frontend_output/',
						help='folder to contain synthesized mel spectrograms')

	args = parser.parse_args()

	print("------------------------------------------------------------------------")
	for arg in vars(args):
		print("{}: {}".format(arg, getattr(args, arg)))
	print("------------------------------------------------------------------------")
	"""

	is_ok, backend_stub = initialize(server_address)

	if not is_ok:
		print("failed to initialize, maybe because backend server is NOT RUNNING!")
		exit(1)

	normalized_hz_line = "我的头，向山沟，追寻逝去的从前。"
	sentence_id = "1008"
	sr = "8k"
	print("successfully initialized!")

	do_infer(backend_stub, sentence_id, normalized_hz_line, sr)
