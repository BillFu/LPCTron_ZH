import argparse
import os

import time

import tensorflow as tf

from .my_hparams import hparams
# from tqdm import tqdm

from .infolog import log
# from my_synthesizer import Synthesizer

def load_sentences(input_text_file):
	with open(input_text_file, 'rb') as f:
		sentences = list(map(lambda l: l.decode("utf-8")[:-1], f.readlines()))

	return sentences

"""
## construct the basename for the wave file that would to be synthesized
def construct_basename(prefix, start_index,
						batch_size, batch_index, sentence_index):
	id = batch_index * batch_size + sentence_index

	if prefix in (None, ''):
		clip_file_name = '%06d' % (start_index + id)
	else:
		basename = '%s%06d' % (prefix, start_index + id)

	return basename


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


# def synthesize(args, hparams, sentences):

	"""
	if hparams.tacotron_synthesis_batch_size < hparams.tacotron_num_gpus:
		raise ValueError('Defined synthesis batch size {} is smaller than minimum required {} (num_gpus)! Please verify your synthesis batch size choice.'.format(
			hparams.tacotron_synthesis_batch_size, hparams.tacotron_num_gpus))

	if hparams.tacotron_synthesis_batch_size % hparams.tacotron_num_gpus != 0:
		raise ValueError('Defined synthesis batch size {} is not a multiple of {} (num_gpus)! Please verify your synthesis batch size choice!'.format(
			hparams.tacotron_synthesis_batch_size, hparams.tacotron_num_gpus))
	"""

	# return run_eval(args, checkpoint_path, hparams, sentences)


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

	sentences = load_sentences(args.input_text_file)

	load_model(args.checkpoint_dir)

	# _ = synthesize(args, hparams, sentences)
	

if __name__ == '__main__':
	main()
