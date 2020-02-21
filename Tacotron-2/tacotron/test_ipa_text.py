
from utils.ipa_text import text_to_sequence, remove_separator
from utils.ipa_symbols import symbols


if __name__ == "__main__":
    print(symbols)

    ipa_text = "kʰ a 2$ə r 2$pʰ u 3$pʰ e i 2$u a i 4$s u n 1$u a n 2$x u a 2$tʰ i 1$。"

    ipa_text1 = remove_separator(ipa_text)
    symbol_list = ipa_text1.split()
    print(symbol_list)
    print(len(symbol_list))

    id_seq = text_to_sequence(ipa_text)
    print(id_seq)