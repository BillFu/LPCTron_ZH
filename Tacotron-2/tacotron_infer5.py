import argparse
import os

from .my_hparams import hparams
from .infolog import log
from .tacotron.pb_synthesizer import PB_Synthesizer, load_graph, analyze_inputs_outputs
from .frontend.hz2ipa import createCmdPairTuple, hz2py, \
	filter_punct_mark, cal_ipa_seq


def load_sentences(input_text_file):
	with open(input_text_file, 'rb') as f:
		sentences = list(map(lambda l: l.decode("utf-8")[:-1], f.readlines()))
	return sentences

def inference(output_dir, sentence_id, normalized_hz_line, pb_synthesizer):
	cleaned_sentence = filter_punct_mark(normalized_hz_line)
	py_seq = hz2py(cleaned_sentence)
	py_line = " ".join(py_seq)  # convert list into string
	print("py_line: {}".format(py_line))

	ipa_seq = cal_ipa_seq(normalized_hz_line, py_line)
	print("ipa_seq: {}".format(ipa_seq))

	mels = pb_synthesizer.synthesize(ipa_seq)

	npy_data = mels.reshape((-1,))
	out_feature_filename = "feature_{}.f32".format(sentence_id)
	out_feature_filename = os.path.join(output_dir, out_feature_filename)
	npy_data.tofile(out_feature_filename)


def main():
	parser = argparse.ArgumentParser()
	parser.add_argument('--model_file',
		default='tacotron.pb',
		help='Path to model checkpoint')

	parser.add_argument('--output_dir',
		default='synthesize_output/',
		help='folder to contain synthesized mel spectrograms')

	"""
	parser.add_argument('--input_text_file',
		default='',
		help='Text file contains sentences to be synthesized.')
	"""

	args = parser.parse_args()

	print("------------------------------------------------------------------------")
	for arg in vars(args):
		print(arg, getattr(args, arg))
	print("------------------------------------------------------------------------")

	# sentences = load_sentences(args.input_text_file)

	# don't forget to initialize py2ipa module by calling the following function!!!
	createCmdPairTuple()

	pb_synthesizer = PB_Synthesizer()
	pb_synthesizer.load(args.model_file, hparams)

	hz_line = "抱歉，我的账户只剩三元，而我的智商欠费却为一百五十二元。"

	sentence_id = "1004"
	print("hz_line: {}".format(hz_line))

	inference(args.output_dir, sentence_id, hz_line, pb_synthesizer)


if __name__ == '__main__':
	main()
