import string

from zhon.hanzi import punctuation as hanzi_punctuation
from .fix_duoyinzi import is_hanzi, is_english_letter


# if a char is a chinese punction, or a english punction,
# or chinese char, or arab number, it is a legal char.
# Otherwise, it is illegal.
def isLegalChar(char):
	if is_hanzi(char):
		return True
	elif is_english_letter(char):
		return True
	elif char in hanzi_punctuation:
		return True
	elif char in string.punctuation:
		return True
	elif char.isdigit():
		return True
	else:
		return False


def containsIllegalChar(sentence):
	for char in sentence:
		if not isLegalChar(char):
			print("illegal char found:{}".format(char))
			return True

	return False