# -*- coding:utf-8 -*-

from enum import Enum
import struct
import json


class MSG_TYPE(Enum):
	JOB_REQUEST = 101
	REQUEST_CHECK_PASSED = 102
	REQUEST_CHECK_FAILED = 103
	WAV_READY = 104
	WAV_ERROR = 105

class Request_Illegal_Codes(Enum):
	ALL_IS_OK = 100
	TOO_MANY_CHARS = 101
	ENGLISH_NOT_ALLOWED = 102
	UNKNOWN_TAG_IN_SENTENCE = 103
	SENTENCE_ID_MISSING = 201
	SENTENCE_MISSING = 202
	SENTENCE_IS_EMPTY = 203
	FAILED_TO_NORMALIZE = 204
	ILLEGAL_CHAR_IN_SENTENCE = 205
	NOT_NEED_PROSODY_MARK = 206
	SAMPLE_RATE_MISSING = 210
	SAMPLE_RATE_ERROR = 211


Request_Illegal_Info = {
	Request_Illegal_Codes.TOO_MANY_CHARS:
		"The length of sentence can not be more than",
	Request_Illegal_Codes.ENGLISH_NOT_ALLOWED:
		"English letters cannot be handled in sentence.",
	Request_Illegal_Codes.UNKNOWN_TAG_IN_SENTENCE:
		"There are tags in sentence that cannot be processed.",

	##
	Request_Illegal_Codes.SENTENCE_ID_MISSING:
		"\'sentence_id\' is missing in tts request.",
	Request_Illegal_Codes.SENTENCE_MISSING:
		"\'sentence\' is missing in tts request.",
	Request_Illegal_Codes.SENTENCE_IS_EMPTY:
		"\'sentence\' is empty string in tts request.",
	Request_Illegal_Codes.ILLEGAL_CHAR_IN_SENTENCE:
		"sentence contains illegal char.",
	Request_Illegal_Codes.SAMPLE_RATE_MISSING:
		"\'sample_rate\' is missing in tts request.",
	Request_Illegal_Codes.SAMPLE_RATE_ERROR:
		"\'sample_rate\' can only be 8k or 16k now.",
	Request_Illegal_Codes.FAILED_TO_NORMALIZE:
		"failed to make sentence normalized.",
	Request_Illegal_Codes.NOT_NEED_PROSODY_MARK:
		"prosody marks NOT needed in sentence to be synthesized."
}

inner_version = 2   # a integer


## msg_type: a integer
## body: json object
def build_msg_package(msg_type, body):
	global inner_version
	
	str_of_body = json.dumps(body, ensure_ascii=False)
	bytes_of_body =str_of_body.encode("utf-8")
	body_size = len(bytes_of_body)
	# print("body_size: {}".format(body_size))

	header = [inner_version, body_size, msg_type.value]
	headPack = struct.pack("!3I", *header)
	msg_package = headPack + bytes_of_body
	return msg_package