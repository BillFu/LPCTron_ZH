
import argparse
import os


def main():
	parser = argparse.ArgumentParser()
	parser.add_argument('--lpcnet_model_file',
		default='',
		help='Path to model checkpoint')

	parser.add_argument('--out_pcm_dir',
						default='out_pcm',
						help='folder to save the output pcm files.')

	parser.add_argument('--out_wav_dir',
		default='out_wav',
		help='folder to save the output wav files.')

	args = parser.parse_args()

	print("------------------------------------------------------------------------")
	for arg in vars(args):
		print(arg, getattr(args, arg))
	print("------------------------------------------------------------------------")


if __name__ == '__main__':
	main()
