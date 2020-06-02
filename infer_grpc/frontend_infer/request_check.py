import re

from .message import Request_Illegal_Codes, Request_Illegal_Info

from .frontend.text_normalize_v5 import canNormalizeV3
from .frontend.fix_duoyinzi import primary_clean_text
from .frontend.illegal_char import containsIllegalChar


# if Request_Illegal_Codes.ALL_IS_OK as the first value returned,
# it means the request is legal, otherwise illegal
# code.value, error_reason, normalized_result
def check_request(config, request):
	chars_limit = config["chars_limit"]

	sentence_id = request.args.get('sentence_id')
	if sentence_id is None or sentence_id == "":
		code = Request_Illegal_Codes.SENTENCE_ID_MISSING
		return sentence_id, code.value, Request_Illegal_Info[code], None

	raw_sentence = request.args.get('sentence')
	if raw_sentence is None or raw_sentence == "":
		code = Request_Illegal_Codes.SENTENCE_MISSING
		return sentence_id, code.value, Request_Illegal_Info[code], None,

	# prosody marks are prohibited to exist in this version
	prosody_mark_pattern = r'<([0-9])>'  # match <1>, <2>, etc
	result_matched = re.findall(prosody_mark_pattern, raw_sentence)
	if len(result_matched) > 0:  # found prosody mark in the previous version
		code = Request_Illegal_Codes.NOT_NEED_PROSODY_MARK
		return sentence_id, code.value, Request_Illegal_Info[code], None,

	sentence = primary_clean_text(raw_sentence)

	if len(sentence) == 0:
		code = Request_Illegal_Codes.SENTENCE_IS_EMPTY
		return sentence_id, code.value, Request_Illegal_Info[code], None

	if len(sentence) > chars_limit:
		illegal_code = Request_Illegal_Codes.TOO_MANY_CHARS
		raw_error_msg = Request_Illegal_Info[illegal_code]
		illegal_reason = "{} {}.".format(raw_error_msg, chars_limit)
		return sentence_id, illegal_code.value, illegal_reason, None

	sample_rate = request.args.get("sample_rate", default=8000, type=int)
	if sample_rate not in [8000, 16000]:
		code = Request_Illegal_Codes.SAMPLE_RATE_ERROR
		return sentence_id, code.value, Request_Illegal_Info[code], None

	volume_boost_db = request.args.get('volume_boost', default=0.0, type=float)
	if volume_boost_db > 6.0:
		volume_boost_db = 6.0
	elif volume_boost_db < -6.0:
		volume_boost_db = -6.0

	if containsIllegalChar(sentence):
		code = Request_Illegal_Codes.ILLEGAL_CHAR_IN_SENTENCE
		return sentence_id, code.value, Request_Illegal_Info[code], None

	can_normal, failed_reason, normal_result = canNormalizeV3(sentence)
	if not can_normal:
		code = Request_Illegal_Codes.FAILED_TO_NORMALIZE
		return sentence_id, code.value, failed_reason, None
	else:
		job = {
			"sentence_id": sentence_id,
			"raw_sentence": raw_sentence,
			"normal_result": normal_result,
			"sample_rate": sample_rate,
			"volume_boost": volume_boost_db
		}
		return sentence_id, Request_Illegal_Codes.ALL_IS_OK.value, None, job