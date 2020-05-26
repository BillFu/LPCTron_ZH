import grpc
from backend_infer_pb2 import JobRequest
from backend_infer_pb2_grpc import backend_inferStub


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

	print("dummy frontend infer is running...")
	run(server_address)
	