
from .frontend.hz2ipa import hz2py, filter_punct_mark, cal_ipa_seq
from .frontend.ipa_text import text_to_sequence


def build_ipa_seq(normalized_hz_line):
	cleaned_sentence = filter_punct_mark(normalized_hz_line)
	py_seq = hz2py(cleaned_sentence)
	py_line = " ".join(py_seq)  # convert list into string
	print("py_line: {}".format(py_line))

	ipa_seq = cal_ipa_seq(normalized_hz_line, py_line)
	print("ipa_seq: {}".format(ipa_seq))

	ipa_id_seq = text_to_sequence(ipa_seq)
	return ipa_id_seq


def build_ipa_seq_str(normalized_hz_line):
	ipa_id_seq = build_ipa_seq(normalized_hz_line)
	ipa_id_str_seq = [str(id) for id in ipa_id_seq]
	ipa_id_seq_str = " ".join(ipa_id_str_seq)
	return ipa_id_seq_str

#---------------------------------------------------------------------------------
#  normal_result: {'normalized_mother_sentence': '您的账户余额为*元，拖欠数额为*元',
#                  'normalized_content_set':
#                  		[('两万两千二百二十二点七八', 'General_Numeric'),
#                  		 ('一万零二百三十四点二二', 'General_Numeric')]
#                 }
# replace all the * in the normalized_mother_sentence with the elements
# in normalized_content_set
def combine_normal_result(normal_result):
