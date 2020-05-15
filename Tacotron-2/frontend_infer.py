import argparse
import os

from .tacotron.utils.ipa_text import text_to_sequence

from .frontend.hz2ipa import createCmdPairTuple, hz2py, \
	filter_punct_mark, cal_ipa_seq


def build_ipa_seq(normalized_hz_line):
	cleaned_sentence = filter_punct_mark(normalized_hz_line)
	py_seq = hz2py(cleaned_sentence)
	py_line = " ".join(py_seq)  # convert list into string
	print("py_line: {}".format(py_line))

	ipa_seq = cal_ipa_seq(normalized_hz_line, py_line)
	print("ipa_seq: {}".format(ipa_seq))

	ipa_id_seq = text_to_sequence(ipa_seq)
	return ipa_id_seq


def main():
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

	# don't forget to initialize py2ipa module by calling the following function!!!
	createCmdPairTuple()

	ipa_id_seq = build_ipa_seq(args.input_sentence)
	print("ipa_id_seq: {}".format(ipa_id_seq))

	out_file_name = "{}_ipa.txt".format(args.sentence_id)
	out_file_name = os.path.join(args.output_dir, out_file_name)

	ipa_str_id_seq = [str(an_id) for an_id in ipa_id_seq]
	out_str = " ".join(ipa_str_id_seq)
	with open(out_file_name, 'w') as f:
		f.write(out_str)

	print("ipa id seq has been saved in file: {}".format(out_file_name))


if __name__ == '__main__':
	main()
