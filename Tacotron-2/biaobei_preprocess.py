import os
import argparse

from multiprocessing import cpu_count
from tqdm import tqdm
from datasets import ipa_preprocessor
from my_hparams import hparams


def preprocess(args, raw_wave_dir, anno_file, out_dir, hparams):
	print('out_dir: {}'.format(out_dir))
	mel_dir = os.path.join(out_dir, 'mels')
	trimmed_wav_dir = os.path.join(out_dir, 'audio')
	linear_dir = os.path.join(out_dir, 'linear')
	os.makedirs(mel_dir, exist_ok=True)
	os.makedirs(trimmed_wav_dir, exist_ok=True)
	os.makedirs(linear_dir, exist_ok=True)
	n_jobs = int(args.n_jobs)
	#preprocessor.build_from_path( hparams, wave_dir, pinyin_ano, mel_dir, linear_dir, wav_dir, n_jobs, tqdm=tqdm)
	metadata = ipa_preprocessor.build_from_path(hparams, raw_wave_dir, anno_file,
		mel_dir, linear_dir, trimmed_wav_dir, n_jobs, tqdm=tqdm)
	write_metadata(metadata, out_dir)


'''
def preprocess(args, input_folders, out_dir, hparams):
	mel_dir = os.path.join(out_dir, 'mels')
	# wav_dir = os.path.join(out_dir, 'audio')
	linear_dir = os.path.join(out_dir, 'linear')
	os.makedirs(mel_dir, exist_ok=True)
	# os.makedirs(wav_dir, exist_ok=True)
	os.makedirs(linear_dir, exist_ok=True)
	metadata = preprocessor.build_from_path(hparams, input_folders, mel_dir, linear_dir, wav_dir, 
		args.n_jobs, tqdm=tqdm)
'''


def write_metadata(metadata, out_dir):
	with open(os.path.join(out_dir, 'train.txt'), 'w', encoding='utf-8') as f:
		for m in metadata:
			f.write('|'.join([str(x) for x in m]) + '\n')
	mel_frames = sum([int(m[4]) for m in metadata])
	timesteps = sum([int(m[3]) for m in metadata])
	sr = hparams.sample_rate
	hours = timesteps / sr / 3600
	print('Write {} utterances, {} mel frames, {} audio timesteps, ({:.2f} hours)'.format(
		len(metadata), mel_frames, timesteps, hours))
	print('Max input length (text chars): {}'.format(max(len(m[5]) for m in metadata)))
	print('Max mel frames length: {}'.format(max(int(m[4]) for m in metadata)))
	print('Max audio timesteps length: {}'.format(max(m[3] for m in metadata)))


def main():
	print('initializing preprocessing..')
	parser = argparse.ArgumentParser()
	parser.add_argument('--base_dir', default='dataset')
	parser.add_argument('--hparams', default='', 
		help='Hyperparameter overrides as a comma-separated list of name=value pairs')
	parser.add_argument('--dataset', default='biaobei')
	parser.add_argument('--language', default='zh_CN')
	parser.add_argument('--voice', default='female')
	parser.add_argument('--reader', default='dasha')
	parser.add_argument('--merge_books', default='False')
	parser.add_argument('--book', default='northandsouth')
	parser.add_argument('--output', default='training_data')
	parser.add_argument('--n_jobs', type=int, default=cpu_count())
	parser.add_argument('--wave_dir', default='input wave files dir')
	parser.add_argument('--anno_file', default='ipa annotation file')
	args = parser.parse_args()

	args.n_jobs = 1 # just for debug
	modified_hp = hparams.parse(args.hparams)

	assert args.merge_books in ('False', 'True')

	# run_preprocess(args, modified_hp)
	input_folder = os.path.join(args.base_dir, args.dataset) # where .wav files are stored
	output_folder = os.path.join(args.base_dir, args.output)
	os.makedirs(output_folder, exist_ok=True)
	preprocess(args, input_folder, args.anno_file, output_folder, hparams)


if __name__ == '__main__':
	main()