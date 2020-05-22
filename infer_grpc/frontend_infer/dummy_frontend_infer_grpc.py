import grpc
from backend_infer_pb2 import JobRequest
from backend_infer_pb2_grpc import backend_inferStub


def run(server_address):
	# 连接 rpc 服务器
	channel = grpc.insecure_channel(server_address)

	# 调用 rpc 服务
	backend_stub = backend_inferStub(channel)
	
	job_request = JobRequest(sentence_id='1009', sr='16k',
		ipa_id_seq="1 3 5 7 9 2 4 6 8 10")

	job_response = backend_stub.commitJob(job_request)
	print("response received, isOK: {}".format(job_response.isOK))



if __name__ == '__main__':
	server_address = "localhost:50051"

	print("dummy frontend infer is running...")
	run(server_address)
	