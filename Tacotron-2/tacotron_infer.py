import argparse
# import os
# import time

import tensorflow as tf
# from tqdm import tqdm

from .my_hparams import hparams

from .infolog import log
from .tacotron.synthesizer import Synthesizer
from .frontend.hz2ipa import cal_ipa_seq

def load_sentences(input_text_file):
	with open(input_text_file, 'rb') as f:
		sentences = list(map(lambda l: l.decode("utf-8")[:-1], f.readlines()))

	return sentences


"""
def run_eval(args, checkpoint_path, hparams, sentences):
	eval_dir = os.path.join(args.output_dir, 'eval')
	log_dir = os.path.join(args.output_dir, 'logs-eval')

	# if args.model == 'Tacotron-2':
	#	assert os.path.normpath(eval_dir) == os.path.normpath(args.mels_dir)

	#Create output path if it doesn't exist
	os.makedirs(eval_dir, exist_ok=True)
	os.makedirs(log_dir, exist_ok=True)
	os.makedirs(os.path.join(log_dir, 'wavs'), exist_ok=True)
	os.makedirs(os.path.join(log_dir, 'plots'), exist_ok=True)

	log(hparams_debug_string())
	synth = Synthesizer()
	synth.load(checkpoint_path, hparams)

	#Set inputs batch wise
	sentences = [sentences[i: i+hparams.tacotron_synthesis_batch_size] 
		for i in range(0, len(sentences), hparams.tacotron_synthesis_batch_size)]

	log('Starting Synthesis......')
	with open(os.path.join(eval_dir, 'map.txt'), 'w') as file:
		for batch_index, texts in enumerate(tqdm(sentences)):
			start = time.time()

			basenames = []
			for sentence_index in range(len(texts)):
				basename = construct_basename(args.naming_prefix, args.naming_start_index,
					hparams.tacotron_synthesis_batch_size, batch_index, sentence_index)
			basenames.append(basename)

			mel_filenames, speaker_ids = synth.synthesize(texts, basenames, eval_dir, log_dir, None)

			for elems in zip(texts, mel_filenames, speaker_ids):
				file.write('|'.join([str(x) for x in elems]) + '\n')
	log('synthesized mel spectrograms at {}'.format(eval_dir))
	return eval_dir
"""


def load_model(model_path):
	try:
		checkpoint_path = tf.train.get_checkpoint_state(model_path).model_checkpoint_path
		log('model found at {}'.format(checkpoint_path))
	except:
		raise RuntimeError('Failed to load model checkpoint at {}'.format(model_path))

	synthesizer = Synthesizer()
	synthesizer.load(checkpoint_path, hparams)
	return synthesizer


def inference(sentence_id, hanzi_line, pinyin_line, synthesizer):
	ipa_seq = cal_ipa_seq(hanzi_line, pinyin_line)
	print("ipa_seq: {}".format(ipa_seq))


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

	synthesizer = load_model(args.checkpoint_dir)

	hz_line = "黑熊闯进王明辉家后院觅食。"
	py_line = "hei1 xiong2 chuang3 jin4 wang2 ming2 hui1 jia1 hou4 yuan4 mi4 shi2"
	sentence_id = "1001"
	print("hz_line: {}".format(hz_line))
	print("py_line: {}".format(py_line))

	inference(sentence_id, hz_line, py_line, synthesizer)


if __name__ == '__main__':
	main()
