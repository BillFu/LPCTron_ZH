import re

def is_english_letter(char):
	english_letters = [
		'a','b','c','d','e','f','g','h','i','j','k','l',
		'm','n','o','p','q','r','s','t','u','v','w','x','y','z',
		'A','B','C','D','E','F','G','H','I','J','K','L',
		'M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z']
	if char in english_letters:
		return True
	else:
		return False


def is_hanzi(char):
	# for python 3.x
	return '\u4e00' <= char <= '\u9fff'

# remove whitespaces, tabs, newlines
def primary_clean_text(raw_sentence):
	'''
	sentence = raw_sentence.replace(" ", "")
	sentence = sentence.replace("\r", "")
	sentence = sentence.replace("\n", "")
	sentence = sentence.replace("\t", "")
	'''
	sentence = re.sub('[ \n\r\t]', '', raw_sentence)
	return sentence

def load_test_duoyinzi_dict():
	# get pinyin syllables and phones
	duoyinzi_in_dict = ['还', '行', '长']
	duoyinzi_dict = {'银行.': 'hang2',
					 '行.长': 'hang2',
					 '行长.': 'zhang3',
					 '行.人': 'xing2',
					 '行.走': 'xing2',
					 '自行.车': 'xing2',
					 '先还.': 'huan2',
					 '还.清': 'huan2',
					 '还.款额': 'huan2',
					 '还.未': 'hai2'
					 }

	return duoyinzi_in_dict, duoyinzi_dict


###### Not very strict checking, maybe need refining later #####################
def is_valid_dyz_item(dyz_item):
	if len(dyz_item) > 4:  # can not be more than 4 characters (including '.')
		return False

	if len(dyz_item) < 3:  # can not be less than 3 characters (including '.')
		return False

	indicator_index = dyz_item.find('.')

	if indicator_index < 0:
		return False
	else:
		return True


# for example, given the input of "还.款额", return "还"
def get_dyz_in_item(dyz_item):
	indicator_index = dyz_item.index('.')
	return dyz_item[indicator_index - 1]


###### Not very strict checking, maybe need refining later #####################
def is_valid_pinyin(pinyin):
	if len(pinyin) < 2:
		return False

	tone_digit = pinyin[-1]
	if tone_digit not in ['1', '2', '3', '4', '5']:
		return False

	return True
##################################################################

################################################################################
## the dictionary file of duoyinzi takes the forllowing format:
## 行.长 hang2
## 行长. zhang3
################################################################################
def load_duoyinzi_dict(duoyinzi_dict_file):
	# firstly, read from file and construct duoyinzi_dict
	with open(duoyinzi_dict_file, 'r', encoding='utf-8') as f:
		lines = f.readlines()

	duoyinzi_dict = {}
	duoyinzi_registered = []
	error_lines = []
	duplicated_lines = []

	for index, line in enumerate(lines):
		line = line.rstrip('\n')
		split_results = line.split()

		if len(split_results) != 2:
			error_lines.append(index + 1)
			continue

		dyz_item = split_results[0]
		dyz_py = split_results[1]

		if not is_valid_dyz_item(dyz_item):
			error_lines.append(index + 1)
			continue

		if not is_valid_pinyin(dyz_py):
			error_lines.append(index + 1)
			continue

		duoyinzi = get_dyz_in_item(dyz_item)

		if not is_hanzi(duoyinzi):
			error_lines.append(index + 1)
			continue

		if duoyinzi not in duoyinzi_registered:
			duoyinzi_registered.append(duoyinzi)

		if dyz_item in duoyinzi_dict: # key duplicated
			duplicated_lines.append(index + 1)
			continue

		duoyinzi_dict.update({dyz_item: dyz_py})

	return duoyinzi_registered, duoyinzi_dict, error_lines, duplicated_lines


###############################################################################
# 纠正多音字
# 解决思路：
# （1）先检测出句子的多音字，结果信息包含多音字和出现的位置
# （2）提取多音字的上下文
# （3）利用上下文去检索多音字发音字典
# （4）用检索结果去修正拼音和phone_type等信息

# here sentence just contains only chinese characters
def extract_duoyinzi(sentence, poly_py_chars):
	duoyinzi_info = []
	for i, hz_char in enumerate(sentence):
		if hz_char in poly_py_chars:
			if i - 2 >= 0:
				front = i - 2
			else:
				front = 0
			head_padding_num = front - (i - 2)

			if i + 2 <= len(sentence) - 1:
				end = i + 2
			else:
				end = len(sentence) - 1
			tail_padding_num = i + 2 - end

			prefix = '*' * head_padding_num
			suffix = '*' * tail_padding_num
			context = prefix + sentence[front: end + 1] + suffix

			# insert a '.' to indicator current focus character
			context = context[:3] + '.' + context[3:]
			duoyinzi_info.append([hz_char, i, context])

	return duoyinzi_info


## now char_context has a fixed length of 5.
## it has been padded with '*' when there is no enough char to construct
## a context of 5 chars.
def lookup_duoyinzi_dict(char_context, duoyinzi_dict):
	candidate_items = [char_context[0:4], char_context[1:5], char_context[2:6],
		char_context[1:4], char_context[2:5]]

	for candidate_item in candidate_items:
		# print(candidate_item)
		if candidate_item in duoyinzi_dict:
			# print("found candidate in dyz-dict: {}".format(duoyinzi_dict[candidate_item]))
			return duoyinzi_dict[candidate_item]

	return ""   # no matching found, just return a empty string


def test_load_dyz_dict():
	duoyinzi_dict_file = 'data/biaobei_16k_1w/duoyinzi_dict_test.txt'
	duoyinzi_in_dict, duoyinzi_dict, error_lines, duplicated_lines = \
		load_duoyinzi_dict(duoyinzi_dict_file)

	hasError = False
	if len(error_lines) > 0:
		hasError = True
		print("***Format Errors happended in those lines: {}.".format(error_line))

	if len(duplicated_lines) > 0:
		hasError = True
		print("***Duplication Errors happended in those lines: {}.".format(duplicated_lines))

	if hasError:
		print("please fix format errors and duplication error.")
	else:
		print("duoyinzi dict has been loaded successfully.")


if __name__ == '__main__':
	test_load_dyz_dict()
