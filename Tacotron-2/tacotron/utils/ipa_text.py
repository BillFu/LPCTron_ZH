import re

# from . import cleaners
from .ipa_symbols import symbols

# Mappings from symbol to numeric ID and vice versa:

_symbol_to_id = {s: i for i, s in enumerate(symbols)}
_id_to_symbol = {i: s for i, s in enumerate(symbols)}

# kʰ a 2$ə r 2$pʰ u 3$pʰ e i 2$u a i 4$s u n 1$u a n 2$x u a 2$tʰ i 1$。

def remove_separator(ipa_text):
  ipa_text1 = re.sub(r'\$', " ", ipa_text)
  return ipa_text1


def text_to_sequence(text):
  cleaned_text = remove_separator(text)
  symbol_list = cleaned_text.split()
  sequence =  _symbols_to_sequence(symbol_list)

  # Append EOS token
  sequence.append(_symbol_to_id['~'])
  return sequence


def sequence_to_text(sequence):
  '''Converts a sequence of IDs back to a string'''
  result = ''
  for symbol_id in sequence:
    if symbol_id in _id_to_symbol:
      s = _id_to_symbol[symbol_id]
      # Enclose ARPAbet back in curly braces:
      if len(s) > 1 and s[0] == '@':
        s = '{%s}' % s[1:]
      result += s
  return result.replace('}{', ' ')

"""
def _clean_text(text, cleaner_names):
  for name in cleaner_names:
    cleaner = getattr(cleaners, name)
    if not cleaner:
      raise Exception('Unknown cleaner: %s' % name)
    text = cleaner(text)
  return text
"""

def _symbols_to_sequence(symbol_list):
  return [_symbol_to_id[s] for s in symbol_list if _should_keep_symbol(s)]


def _should_keep_symbol(s):
  return s in _symbol_to_id and s is not '_' and s is not '~'

"""
if __name__ == "__main__":
  ipa_text = "kʰ a 2$ə r 2$pʰ u 3$pʰ e i 2$u a i 4$s u n 1$u a n 2$x u a 2$tʰ i 1$。"

  ipa_text1 = remove_separator(ipa_text)
  print(ipa_text1)
"""