import grpc
from .backend_infer_pb2 import JobRequest
from .backend_infer_pb2_grpc import backend_inferStub

from .frontend.ipa_text import text_to_sequence

from .frontend.hz2ipa import createCmdPairTuple, hz2py, \
	filter_punct_mark, cal_ipa_seq


# input a normalized and cleaned sentence, return the ipa id sequence of it
def build_ipa_seq(normalized_hz_line):
	cleaned_sentence = filter_punct_mark(normalized_hz_line)
	py_seq = hz2py(cleaned_sentence)
	py_line = " ".join(py_seq)  # convert list into string
	print("py_line: {}".format(py_line))

	ipa_seq = cal_ipa_seq(normalized_hz_line, py_line)
	print("ipa_seq: {}".format(ipa_seq))

	ipa_id_seq = text_to_sequence(ipa_seq)
	return ipa_id_seq


def run(server_address):
	# 连接 rpc 服务器
	channel = grpc.insecure_channel(server_address)

	# 调用 rpc 服务
	backend_stub = backend_inferStub(channel)
	
	job_request = JobRequest(sentence_id='1009', sr='16k',
		ipa_id_seq="18 29 25 6 23 31 11 33 16 6 43 25 17 5 23 34 7 23 36 29 28 6 26 25 6 23 36 38 5 36 32 28 6 21 8 16 3 27 33 16 4 43 32 20 4 25 17 5 23 34 7 23 36 38 6 36 29 28 3 23 31 11 33 16 6 10 9 11 6 23 31 27 33 6 25 9 11 6 11 6 18 8 11 5 25 5 36 38 4 32 20 6 27 33 16 4 41 1")

	job_response = backend_stub.commitJob(job_request)
	print("response received, isOK: {}".format(job_response.isOK))



if __name__ == '__main__':
	server_address = "localhost:50051"

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

	# print("dummy frontend infer is running...")

	# don't forget to initialize py2ipa module by calling the following function!!!
	createCmdPairTuple()

	input_sentence = "我的头，向山沟，追寻逝去的从前。" 
	sentence_id = "1008" 

	ipa_id_seq = build_ipa_seq(input_sentence)
	print("ipa_id_seq: {}".format(ipa_id_seq))

	# run(server_address)
