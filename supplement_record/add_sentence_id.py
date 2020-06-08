
import argparse
import os

#############################################################
# prefix an sentence id with each sentence, started by 010001
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
	sentence_id = 10000
	with open(args.in_text_file, 'r') as f:
		while True:
			line1 = f.readline()
			if not line1:
				break
			sentence_id = sentence_id + 1
			raw_line = line1.strip()
			sentence_id_str = "0%d" % sentence_id
			new_line = sentence_id_str + " " + raw_line
			print(new_line)
			out_file.write(new_line + os.linesep)

	out_file.close()


if __name__ == '__main__':
	main()
