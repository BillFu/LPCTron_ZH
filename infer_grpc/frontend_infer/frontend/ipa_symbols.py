'''
Defines the set of symbols used in text input to the model.

The default is a set of ASCII characters that works well for English or text that has been run
through Unidecode. For other data, you can modify _characters. See TRAINING_DATA.md for details.
'''

_pad        = '_'
_eos        = '~'
# _characters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890!\'(),-.:;? '

_tokens_str = '. 1 2 3 4 5 a e f i k kʰ l m n o p pʰ r s sʰ t tʰ u x y ŋ ɑ ɕ ɕʰ ə ɛ ɤ ɿ ʂ ʂʰ ʅ … 、 。 ！ ， ： ？'

_tokens = _tokens_str.split()

# Export all symbols:
symbols = [_pad, _eos] + _tokens
