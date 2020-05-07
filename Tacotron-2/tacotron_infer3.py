import argparse
import posix_ipc
import mmap

import tensorflow as tf

from .my_hparams import hparams

from .infolog import log
from .tacotron.synthesizer2 import Synthesizer
from .frontend.hz2ipa import createCmdPairTuple, hz2py, \
	filter_punct_mark, cal_ipa_seq


def load_sentences(input_text_file):
	with open(input_text_file, 'rb') as f:
		sentences = list(map(lambda l: l.decode("utf-8")[:-1], f.readlines()))
	return sentences


def load_model(model_path):
	try:
		checkpoint_path = tf.train.get_checkpoint_state(model_path).model_checkpoint_path
		log('model found at {}'.format(checkpoint_path))
	except:
		raise RuntimeError('Failed to load model checkpoint at {}'.format(model_path))

	synthesizer = Synthesizer()
	synthesizer.load(checkpoint_path, hparams)
	return synthesizer


def inference(sentence_id, normalized_hz_line, synthesizer,
			ipc_semaphore, mmap_file):
	cleaned_sentence = filter_punct_mark(normalized_hz_line)
	py_seq = hz2py(cleaned_sentence)
	py_line = " ".join(py_seq)  # convert list into string
	print("py_line: {}".format(py_line))

	ipa_seq = cal_ipa_seq(normalized_hz_line, py_line)
	print("ipa_seq: {}".format(ipa_seq))

	mels = synthesizer.synthesize(ipa_seq)
	mels = mels.reshape((-1,))  # convert to 1D

	"""
	print("npy_data.dtype: {}".format(npy_data.dtype))
	print("npy_data's total bytes: {}".format(npy_data.nbytes))
	print("npy_data's item bytes: {}".format(npy_data.itemsize))
	"""

	ipc_semaphore.acquire()
	mmap_file.seek(0)
	mmap_file.write(mels.data)
	# mmap_file.close()
	mmap_file.flush()  # if flush() doesn't work, use close()
	ipc_semaphore.release()


def main():
	parser = argparse.ArgumentParser()
	parser.add_argument('--checkpoint_dir',
		default='taco_pretrained/',
		help='Path to model checkpoint')

	parser.add_argument('--output_dir',
		default='synthesize_output/',
		help='folder to contain synthesized mel spectrograms')

	parser.add_argument('--input_text_file',
		default='',
		help='Text file contains sentences to be synthesized.')

	args = parser.parse_args()

	print("------------------------------------------------------------------------")
	for arg in vars(args):
		print(arg, getattr(args, arg))
	print("------------------------------------------------------------------------")

	# sentences = load_sentences(args.input_text_file)

	# don't forget to initialize py2ipa module by calling the following function!!!
	createCmdPairTuple()

	synthesizer = load_model(args.checkpoint_dir)

	IPC_KEY = 42
	SEMAPHORE_NAME = "/LPCTron_SEM1"
	SHARED_MEMORY_NAME = "/LPCTron_SHM1"
	SHARED_MEMORY_SIZE = 2000 * 20 * 4   # 2000 is max number of frames

	ipc_memory = posix_ipc.SharedMemory(SHARED_MEMORY_NAME,
		posix_ipc.O_CREX, size=SHARED_MEMORY_SIZE)
	ipc_semaphore = posix_ipc.Semaphore(SEMAPHORE_NAME, posix_ipc.O_CREX)

	# MMap the shared memory
	mmap_file = mmap.mmap(ipc_memory.fd, ipc_memory.size)

	# Once I've mmapped the file descriptor, I can close it without
	# interfering with the mmap.
	ipc_memory.close_fd()

	# hz_line = "蓝蓝的天上白云飘，放羊娃坐在草地上发呆。"
	# hz_line = "羊儿在山坡上吃草，放羊娃在为娶媳妇送彩礼的事儿犯愁。"
	hz_line = "抱歉，我的账户只剩三元，而我的智商欠费却为一百五十二元。"

	sentence_id = "1004"
	print("hz_line: {}".format(hz_line))
	# print("py_line: {}".format(py_line))

	inference(sentence_id, hz_line, synthesizer, ipc_semaphore, mmap_file)

	mmap_file.close()

	ipc_memory.unlink()
	ipc_semaphore.unlink()



if __name__ == '__main__':
	main()
