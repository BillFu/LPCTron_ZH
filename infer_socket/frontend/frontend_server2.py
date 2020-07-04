# -*- coding:utf-8 -*-

import sys
import socket
# from socket import error as socket_error


def main():
	''''
	parser = argparse.ArgumentParser()
	parser.add_argument("--config_file",
		help="full path to confile file.")
	args = parser.parse_args()

	config, error_reason = load_config(args.config_file)
	if config is None:
		print(error_reason)
		sys.exit(0)

	tts_server_ip = config["tts_server_ip"]
	tts_server_port = config["tts_server_port"]
	'''

	tts_server_ip = "127.0.0.1"
	tts_server_port = 8001

	# 创建 socket 对象
	socket_to_backend = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	socket_to_backend.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)

	print("connecting to the Backend Server...")

	try:
		socket_to_backend.connect((tts_server_ip, tts_server_port))
	except socket.gaierror as e:
		print("Address-related error happened when to connect the backend server: % s" % e)
		sys.exit(1)
	except socket.error as e:
		print("Connection error: {}".format(e))
		print("Please check network and the Backend Server.")
		sys.exit(1)

	print("Successfully connected with the Backend Server!")

	# sock.send(b'{"host": localhost, "port": 8787}}')
	socket_to_backend.send(b'I am frontend_server.')
	print("send out request!")

	print("waiting for reply from the Backend Server!")

	# 接收小于 1024 字节的数据
	msg_received = socket_to_backend.recv(1024)
	print("message received: {}".format(msg_received))

	socket_to_backend.close()
	print("socket_to_backend closed!")


if __name__ == '__main__':
	main()