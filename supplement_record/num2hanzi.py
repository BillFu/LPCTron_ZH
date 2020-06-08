

import argparse
import os

num2hz_dict = {'1':'幺','2':'二','3':'三','4':'四','5':'五',
			'6':'六','7':'七','8':'八','9':'九','0':'零','-':'杠'}

def num2hz_sentence(line):
	chars = []
	for char in line:
		if char in num2hz_dict.keys():
			chars.append(num2hz_dict[char])
		else:
			chars.append(char)

	new_line = "".join(chars)
	return new_line

#############################################################
# convert arabic number into chinese characters
############################################################
def main():
	parser = argparse.ArgumentParser()
	parser.add_argument('--in_text_file', type=str, required=True)
	parser.add_argument('--out_text_file', type=str, required=True)

	args = parser.parse_args()

	print("------------------------------------------------------------------------")
	for arg in vars(args):
		print(arg, getattr(args, arg))
	print("------------------------------------------------------------------------")

	out_file = open(args.out_text_file, 'w')
	with open(args.in_text_file, 'r') as f:
		while True:
			line1 = f.readline()
			if not line1:
				break
			raw_line = line1.strip()
			new_line = num2hz_sentence(raw_line)
			print(new_line)
			out_file.write(new_line + os.linesep)

	out_file.close()


if __name__ == '__main__':
	main()