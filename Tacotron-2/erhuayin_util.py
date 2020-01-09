
import os, re
import argparse


pattern_prosody_annotation = re.compile(r"#[1,2,3,4]", flags=re.U)

def is_hanzi(char):
	# for python 3.x
	return '\u4e00' <= char <= '\u9fff'


# return id and cleaned text which only contain chinese characters.
def parse_hanzi_line(raw_hanzi_line):
	global pattern_prosody_annotation

	# fristly, split raw hanzi line into sentence id and hanzi sequence
	result = re.split(r'\t+', raw_hanzi_line)
	id = result[0]
	# hanzi_line contains prosody annotations, punctuations, and chinese characters
	hanzi_line = result[1]

	# remove prosody annotations in hanzi_line
	# hanzi_line_no_prosody = re.sub(pattern=pattern_prosody_annotation,
	#	repl="", string=hanzi_line)

	hanzi_list = filter(is_hanzi, hanzi_line)
	hanzi_list = list(hanzi_list)

	return id, hanzi_list


def detect_erhuayin(hanzi_line, py_line):
	id, pure_hanzi_list = parse_hanzi_line(hanzi_line)

	# print("hanzi_line: {}".format(hanzi_line))

	# print("id: {}".format(id))
	# print("pure_hanzi_list: {}".format(pure_hanzi_list))

	py_list = py_line.split()
	py_count = len(py_list)
	hz_count = len(pure_hanzi_list)

	# print("hz_count: {}".format(hz_count))
	# print("py_count: {}".format(py_count))

	if hz_count != py_count and '儿' in pure_hanzi_list :
		print("er_hua_yin detected with ID: {}".format(id))


def test_detect_erhuayin():
	# hanzi_line = '000029	遛弯儿#2都得#2躲远点#4。'
	# py_line = 'liu4 wan2 er5 dou1 dei3 duo2 yuan2 dian3'
	hanzi_line = '000029	遛弯儿#2都得#2躲远点#4。'
	py_line = 'liu4 wanr1 dou1 dei3 duo2 yuan2 dian3'

	detect_erhuayin(hanzi_line, py_line)


def main():
	parser = argparse.ArgumentParser()
	parser.add_argument('--in_transcript_file', type=str, required=True)
	parser.add_argument('--out_transcript_file', type=str, required=True)

	args = parser.parse_args()

	print("------------------------------------------------------------------------")
	for arg in vars(args):
		print(arg, getattr(args, arg))
	print("------------------------------------------------------------------------")

	# out_file = open(args.out_transcript_file, 'w')

	with open(args.in_transcript_file, 'r') as f:
		while True:
			line1 = f.readline()
			if not line1:
				break

			hanzi_line = line1.strip()
			line2 = f.readline()
			py_line = line2.strip()

			detect_erhuayin(hanzi_line, py_line)

			# out_file.write(hanzi_line + os.linesep)
			# out_file.write(ipa_line + os.linesep)

	# out_file.close()

	print("--------------All Done!-----------------------------")


if __name__ == '__main__':
	main()
	# test_detect_erhuayin()
