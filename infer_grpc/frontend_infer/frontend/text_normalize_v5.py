import re
import argparse


general_num_dict = {'1':'一','2':'二','3':'三','4':'四','5':'五',
			'6':'六','7':'七','8':'八','9':'九','0':'零',
			'.':'点','-':'杠','X':'x'}

phone_num_dict = {'1':'幺','2':'二','3':'三','4':'四','5':'五',
			'6':'六','7':'七','8':'八','9':'九','0':'零',
			'.':'点','-':'杠','X':'x'}

digital_hanzi_dict = {'1':'一','2':'二','3':'三','4':'四','5':'五',
			'6':'六','7':'七','8':'八','9':'九','0':'零', '-':'杠'}

index_dict = {1:'', 2:'十', 3:'百', 4:'千', 5:'万',
			  6:'十', 7:'百', 8:'千', 9:'亿', 10:'十', 11:'百', 12:'千'}


not_implemented_tag = ['English_Letters', 'English_Word', 'Name']
implemented_tag = ['General_Numeric', 'Digital_Sequence', 'Telephone','Zip', 'ID_Card']


## remove non-stop symbol, such as 引号、书名号、各类括号、空格
## remove digits, english, etc.
def clean_text(in_text):
	cleaned_text = re.sub('[\'\"“”<>《》（）(){}\[\] ]', '', in_text)
	cleaned_text = cleaned_text.replace("……", "…")

	return cleaned_text  #


def is_pure_arab_number(num_str):
	legal_num_chars = ['1','2', '3', '4', '5',
		'6','7', '8', '9', '0', '.', '-']
	for char in num_str:
		if char not in legal_num_chars:
			return False
	return True


def hasIllegalTag(sentence):
	matches = re.finditer(r'\[(.+?)\]{(\w+?)}', sentence)
	for match in matches:
		info_str, type_str = match.group(1), match.group(2)
		if type_str in not_implemented_tag:
			return True
		elif type_str not in implemented_tag:
			return True
		# now only consider arab numbers
		if not is_pure_arab_number(info_str):
			return True

	return False


# 输入的句子中没有韵律标注
# 对标点符号不清除
"""
a example for normal_result:
	{'normalized_mother_sentence': '你的手机号码是*，账户余额为*元，对吗？', 
	 'normalized_content_set': [('幺三九-幺七四零-六八七四', 'Telephone'),
		 						('五千二百八十', 'General_Numeric')]}

"""
def canNormalizeV3(input_sentence):
	if hasIllegalTag(input_sentence):
		return False, "found tags cannot handled or tag error.", None

	need_normal_pattern = r'\[(.+?)\]{(\w+?)}'

	matched_parts = re.findall(need_normal_pattern, input_sentence)
	# print("normal_parts_matched: {}".format(matched_parts))
	normalized_content_set = []
	for matched_part in matched_parts:
		content_to_normalize = matched_part[0]
		normalize_type = matched_part[1]
		# print('info_str=', content_to_normalize, ' type_str=', normalize_type)

		normalized_content = normalize(matched_part)
		# print("normalized content: {}".format(normalized_content))
		normalized_tuple = (normalized_content, normalize_type)
		normalized_content_set.append(normalized_tuple)

	# construct the template sentence, where every element that needs normalization
	# is presented with a '*'
	normalized_template_sentence = re.sub(need_normal_pattern, '*', input_sentence)
	# print("normalized template sentence: {}".format(normalized_template_sentence))

	normal_result = {"normalized_mother_sentence": normalized_template_sentence,
					 "normalized_content_set": normalized_content_set}

	# print("normalized result : {}".format(normal_result))
	return True, None, normal_result


# 将初始正则化后的"二"视情况更改为"两"
def fix_2_reading(init_normal_num_str):
	if "二" not in init_normal_num_str:
		return init_normal_num_str

	advanced_normal = re.sub('二千', '两千', init_normal_num_str)
	advanced_normal = re.sub('二万', '两万', advanced_normal)
	advanced_normal = re.sub('二亿', '两亿', advanced_normal)

	return advanced_normal

def handle_1x_int(num_str):
	if num_str == "10":
		return "十"
	else:
		second_digital = num_str[1]
		return "十" + digital_hanzi_dict[second_digital]

# 处理阿拉伯数字串的整数部分
def handleIntegerPart(left_num_str):
	if len(left_num_str) == 2 and left_num_str[0] == '1':
		return handle_1x_int(left_num_str)

	left_nums = list(left_num_str)
	left_index = list(range(1, len(left_nums) + 1))
	left_index.reverse()
	basic_normal = ''
	for index, digit in enumerate(left_nums):
		digit_in_hanzi = general_num_dict[digit]
		index_in_dict = left_index[index]
		unit_in_hanzi = index_dict[index_in_dict]
		basic_normal = ''.join((basic_normal, digit_in_hanzi, unit_in_hanzi))

	basic_normal = re.sub('零[十百千零]*', '零', basic_normal)
	basic_normal = re.sub('零万', '万', basic_normal)
	basic_normal = re.sub('亿万', '亿零', basic_normal)
	basic_normal = re.sub('零零', '零', basic_normal)
	basic_normal = re.sub('零\\b', '', basic_normal)

	advanced_normal = fix_2_reading(basic_normal)

	return advanced_normal

def handlePositive_Numeric(num_str):
	if '.' in num_str:
		#按小数点进行分割
		left_num_str, right = num_str.split('.')
		normalized_left_str = handleIntegerPart(left_num_str)
		# 处理小数点右边的数字
		normalized_right_str = ''.join([general_num_dict[i] for i in right])
		result = normalized_left_str + '点' + normalized_right_str
		return result
	else:
		normalized_str = handleIntegerPart(num_str)
		return normalized_str

#处理一般性数字
def handleGeneral_Numeric(num_str):
	# should fork into two branches: positive or negative
	if num_str[0] == '-': # negative
		positive_num_str = num_str[1:]
		result = handlePositive_Numeric(positive_num_str)
		result = "负" + result
	else:
		result = handlePositive_Numeric(num_str)
	return result


#处理手机号码或固定电话号码
def handleMobilePhone(num_str):
	normal_chars = []
	for i, char in enumerate(num_str):
		if i == 3 or i == 7:
			# normal_chars.append("，")
			normal_chars.append("-")

		normal_chars.append(phone_num_dict[char])
	
	result = ''.join(normal_chars)
	return result


# divide into multiple parts per 4 digits, separated by comma, 
# and translate into chinese chars
def normal_arab_num_perN(num_str, N):
	assert N > 1 and type(N) == int
	normal_chars = []
	for i, char in enumerate(num_str):
		i1 = i + 1
		normal_chars.append(phone_num_dict[char])
		if i1 != 0 and i1 % N == 0:
			normal_chars.append("-")

	result = ''.join(normal_chars)
	return result


def normal_digital_seq_perN(num_str, N):
	assert N > 1 and type(N) == int
	normal_chars = []
	for i, char in enumerate(num_str):
		i1 = i + 1
		normal_chars.append(digital_hanzi_dict[char])
		if i1 != 0 and i1 % N == 0:
			normal_chars.append("-")

	result = ''.join(normal_chars)
	return result


def handleTelephone(num_str):
	length = len(num_str)
	if length == 11:  # assume it's a mobile phone
		result = handleMobilePhone(num_str)
	else: # telephone
		if '-' in num_str:
			split_result = re.split(r'[\s-]+', num_str)
			raw_1st_part = split_result[0]  # + "-"
			normal_1nd_part = ''.join(phone_num_dict[i] for i in raw_1st_part)
			normal_1nd_part += '-'  # ???
			raw_2nd_part = split_result[1]
			normal_2nd_part = normal_arab_num_perN(raw_2nd_part, 4)
			result = normal_1nd_part + normal_2nd_part
		else:  # no hyphen
			result = normal_arab_num_perN(num_str, 4)

	if result[-1] == '-':
		result = result[:-1]  # trim off the last '-'
	return result

def handleEnglish_letters(num_str):
	convert_letters = num_str.lower()
	convert_letters_list = str([ch for ch in convert_letters])
	#print('convert_letter_list=',convert_letters_list)
	#vocabulary = re.sub('\[(.*?)\]', convert_list, context)
	return convert_letters_list

#处理英文名字
def handleEnglish_Word(num_str):
	convert_word = num_str.lower()
	convert_word_list = str([w for w in convert_word])
	return convert_word_list

def handleName(num_str):
	return num_str

#邮政编码
def handleZip(num_str):
	result = normal_arab_num_perN(num_str, 3)
	if result[-1] == '-':
		result = result[:-1]  # trim off the last '-'
	return result


# trimming comma is placed into parent function
def handleIDCard_18digits(num_str):
	raw_1st_part = num_str[:6] 
	raw_2nd_part = num_str[6:]

	normal_1st_part = normal_arab_num_perN(raw_1st_part, 3)
	normal_2nd_part = normal_arab_num_perN(raw_2nd_part, 4)

	result = normal_1st_part + normal_2nd_part
	if result[-1] == '-':
		result = result[:-1]  # trim off the last '-'
	return result


# trimming comma is placed into parent function
def handleIDCard_15digits(num_str):
	# divide three part:
	# for example, 620102(1st) 72(2nd), 0604531(3rd)
	raw_1st_part = num_str[:6] 
	raw_2nd_part = num_str[6:8]
	raw_3rd_part = num_str[8:]

	normal_1st_part = normal_arab_num_perN(raw_1st_part, 3)
	normal_2nd_part = normal_arab_num_perN(raw_2nd_part, 2)
	normal_3rd_part = normal_arab_num_perN(raw_3rd_part, 4)

	result = normal_1st_part + normal_2nd_part + normal_3rd_part
	if result[-1] == '-':
		result = result[:-1]  # trim off the last '-'
	return result


#身份证
def handleIDCard(num_str):
	length = len(num_str)
	if length == 18: 
		result = handleIDCard_18digits(num_str)
	elif length == 15:
		result = handleIDCard_15digits(num_str)
	else:
		result = normal_arab_num_perN(num_str, 4)

	if result[-1] == '-':
		result = result[:-1]  # trim off the last '-'

	return result


def handle_Digital_Sequence(num_str):
	if len(num_str) >= 4:
		result = normal_digital_seq_perN(num_str, 4)
	else:
		normal_chars = []
		for char in num_str:
			normal_chars.append(digital_hanzi_dict[char])

		result = ''.join(normal_chars)
	return result

def normalize(matched):
	info_str, type_str = matched[0], matched[1]
	#print('info_str=',info_str,'type_str=',type_str)
	if type_str == 'General_Numeric':
		normalized_text = handleGeneral_Numeric(info_str)
	elif type_str == 'Digital_Sequence':
		normalized_text = handle_Digital_Sequence(info_str)
	elif type_str == 'Telephone':
		normalized_text = handleTelephone(info_str)
	elif type_str == 'English_Letters':
		normalized_text = handleEnglish_letters(info_str)
	elif type_str == 'English_Word':
		normalized_text = handleEnglish_Word(info_str)
	elif type_str == 'Name':
		normalized_text = handleName(info_str)
	elif type_str == 'Zip':
		normalized_text = handleZip(info_str)
	elif type_str == 'ID_Card':
		normalized_text = handleIDCard(info_str)

	# return '[' + normalized_text + ']' + '{' + type_str + '}'
	return normalized_text


def main():
	parser = argparse.ArgumentParser()
	parser.add_argument('--in_txt_file', type=str, required=True)
	parser.add_argument('--out_txt_file', type=str, required=True)

	args = parser.parse_args()

	print("------------------------------------------------------------------------")
	for arg in vars(args):
		print(arg, getattr(args, arg))

	print("------------------------------------------------------------------------")

	# out_txt_file = open(args.out_txt_file, "w", encoding='utf-8')

	with open(args.in_txt_file, 'r', encoding='utf-8') as f:
		raw_sentences = f.readlines()

	index = 1
	for line in raw_sentences:
		line = line.strip()
		'''
		result = re.sub(r'\[(.+?)\]{(\w+?)}', replace, line)
		print(result + "\n")
		out_txt_file.write(result + "\n")
		'''
		can_normal, failed_reason, normal_result = canNormalizeV2(line)
		if not can_normal:
			print("**index: {}, raw_text:{} \nError: {}\n".format(index, line, failed_reason))
		else:
			print("**index: {}, raw_text:{} \nNormalized: {}\n".format(index, line, normal_result))

		index += 1

	# out_txt_file.close()


if __name__ == '__main__':
	# input_sentence = \
	#	"你的手机号码是[13917406874]{Telephone}，账户余额为[5280]{General_Numeric}元，对吗？"

	input_sentence = "我来自黄土高原，是喝西北风长大的。"

	isOK, error_reason, normal_result = canNormalizeV3(input_sentence)
	if isOK:
		print("OK, normalized result: {}".format(normal_result))
	else:
		print("Failed, error reason: {}".format(error_reason))
