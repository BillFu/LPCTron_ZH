
import argparse
import os

#############################################################
############################################################

english_letters = [
		'a','b','c','d','e','f','g','h','i','j','k','l',
		'm','n','o','p','q','r','s','t','u','v','w','x','y','z',
		'A','B','C','D','E','F','G','H','I','J','K','L',
		'M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z']


def is_english_letter(char):
	if char in english_letters:
		return True
	else:
		return False


def analysis_one_sentence(line, letter_statis_dict):
	for char in line:
		if is_english_letter(char):
			char = char.lower()
			letter_statis_dict[char] = letter_statis_dict[char] + 1


def main():
	parser = argparse.ArgumentParser()
	parser.add_argument('--in_text_file', type=str, required=True)

	args = parser.parse_args()

	print("------------------------------------------------------------------------")
	for arg in vars(args):
		print(arg, getattr(args, arg))
	print("------------------------------------------------------------------------")

	sum_letters = 0
	letter_statis_dict = {}
	lower_letters = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
	'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z']
	for char in lower_letters:
		letter_statis_dict[char] = 0

	with open(args.in_text_file, 'r') as f:
		while True:
			line1 = f.readline()
			if not line1:
				break
			raw_line = line1.strip()
			analysis_one_sentence(raw_line, letter_statis_dict)

	print(letter_statis_dict)


if __name__ == '__main__':
	main()