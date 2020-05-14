
import os, re
import argparse

from pypinyin import pinyin, Style
from .txt2pinyin import pinyin_format

"""
"卡尔普#2陪外孙#1玩滑梯#4。"

py_line: "ka2 er2 pu3 pei2 wai4 sun1 wan2 hua2 ti1"

ipa_line: "kʰ a 2 $ ə r 2 $ pʰ u 3 $ pʰ e i 2 $ u a i 4 $
s u n 1 $ u a n 2 $ x u a 2 $ tʰ i 1 $ 。"
Note: '$' is used as separator 
"""
######################## 全局变量 ########################

# 正则表达式替换命令元组和内部标志
cmdPairTuple = tuple()  # 预备空对象
PATTERN, SUBST = 0, 1  # 匹配段和替换段

# 经过编译的正则表达式对象列表
reList = []  # 预备空对象

# 创建正则表达式替换命令元组的默认设置表（固定集合对象）
defaultPrefSet = frozenset([
	### 以下各行字符串，凡行首用#号注释者均表示无效；
	### 凡行首未用#号注释者均有效，效用如后面注释所述：
	# "NUMBER_2_BY_AR4",                      # 数字“二”有大开口度韵腹
	# "AI_INC_NEAR_OPEN_FRONT",               # ai/uai韵腹为舌面前次开元音
	# "AIR_ANR_INC_NEAR_OPEN_CENTRAL",        # air/anr韵腹为舌面央次低元音
	# "CENTRAL_A_BY_SMALLCAPS_A",             # “央a”采用小型大写[A]标明
	# "IE_YUE_INC_SMALLCAPS_E",               # ie/yue采用小型大写[E]标明
	# "IE_YUE_INC_E",                         # ie/yue采用[e]标明（覆盖上一条）
	# "IAN_YUAN_AS_AN",                       # ian/yuan韵腹和an一样
	# "ONLY_YUAN_AS_AN",                      # 仅yuan韵腹和an一样（覆盖上一条）
	# "OU_INC_SCHWA",                         # ou/iou采用舌面央中元音韵腹
	# "IONG_BY_IUNG",                         # iong韵母采用i韵头u韵腹
	# "ASPIRATE_BY_TURNED_COMMA",             # 采用反逗号弱送气符号
	# "RHOTICITY_BY_RHOTIC_HOOK",             # 儿化韵卷舌动作采用卷舌小钩
	# "ONLY_ER_BY_RHOTIC_HOOK",               # 只有er音节卷舌动作采用卷舌小钩
	# "INITIAL_R_BY_VED_RETRO_FRIC",          # 声母r为卷舌浊擦音而非卷舌通音
	# "R_TURNED_WITH_HOOK",                   # 严格采用卷舌通音符号
	# "TRANSITIONAL_SCHWA_IN_ING",            # ing有舌面央中元音韵腹
	# "TRANSITIONAL_SCHWA_IN_UEN",            # 合口un有舌面央中元音韵腹
	# "NO_TRANSITIONAL_U",                    # bo/po/mo/fo没有[u]韵头
	# "SYLLABLE_JUNCTURE_BY_PLUS",            # 音节间断采用开音渡+号而非.号
	# "HTML_SUP_TAG_FOR_TONE_VALUE",          # 调值采用HTML上标标签格式
	# "HIDE_ALL_TONE_VALUE",                  # 隐藏所有声调转换
	### 以下选项仅存设想，目前尚未实现：
	# "RETROFLEX_INITIAL_STANDALONE",         # 卷舌声母可成音节而无需舌尖元音
	# "ZERO_INITIAL_HAS_CONSONANT",           # 零声母有辅音
])
# 正则表达式替换命令元组一揽子创建设置方案（元组，[0]位为说明）
recipeLinWang1992 = ("林焘、王理嘉《语音学教程》",
					 "NO_TRANSITIONAL_U", "ASPIRATE_BY_TURNED_COMMA", "AI_INC_NEAR_OPEN_FRONT"
					 )
recipeBeidaCN2004 = ("北京大学中文系《现代汉语》（重排本）",
					 "NO_TRANSITIONAL_U", "ASPIRATE_BY_TURNED_COMMA", "INITIAL_R_BY_VED_RETRO_FRIC",
					 "TRANSITIONAL_SCHWA_IN_UEN", "IONG_BY_IUNG", "IAN_YUAN_AS_AN"
					 )
recipeHuangLiao2002 = ("黄伯荣、廖序东《现代汉语》（增订三版）",
					   "NO_TRANSITIONAL_U", "ASPIRATE_BY_TURNED_COMMA", "CENTRAL_A_BY_SMALLCAPS_A",
					   "TRANSITIONAL_SCHWA_IN_UEN", "ONLY_YUAN_AS_AN", "ONLY_ER_BY_RHOTIC_HOOK",
					   "INITIAL_R_BY_VED_RETRO_FRIC"
					   )
recipeYangZhou1995 = ("杨润陆、周一民《现代汉语》（北师大文学院教材）",
					  "NO_TRANSITIONAL_U", "ASPIRATE_BY_TURNED_COMMA", "TRANSITIONAL_SCHWA_IN_UEN",
					  "INITIAL_R_BY_VED_RETRO_FRIC"
					  )
recipeYuan2001 = ("袁家骅等《汉语方言概要》（第二版重排）",
				  "NO_TRANSITIONAL_U", "ASPIRATE_BY_TURNED_COMMA", "INITIAL_R_BY_VED_RETRO_FRIC",
				  "ONLY_ER_BY_RHOTIC_HOOK", "IAN_YUAN_AS_AN", "TRANSITIONAL_SCHWA_IN_UEN",
				  "IE_YUE_INC_E"
				  )
recipeTang2002 = ("唐作藩《音韵学教程》（第三版）",
				  "NO_TRANSITIONAL_U", "ASPIRATE_BY_TURNED_COMMA", "INITIAL_R_BY_VED_RETRO_FRIC",
				  "ONLY_ER_BY_RHOTIC_HOOK", "IAN_YUAN_AS_AN", "TRANSITIONAL_SCHWA_IN_UEN",
				  "IE_YUE_INC_E", "OU_INC_SCHWA"
				  )


def is_hanzi(char):
	# for python 3.x
	return '\u4e00' <= char <= '\u9fff'


######################## 函数声明 ########################
def createCmdPairTuple(prefSet=defaultPrefSet):
	"""
	创建正则表达式替换命令元组
	然后编译便于反复使用的匹配段正则表达式对象列表
	参数prefSet是一个控制命令元组创建的选项设置序列
	"""
	### 预先处理同系列设置的覆盖关系——defaultPrefSet也可能让人动了手脚
	prefSet = set(prefSet)  # 先换为可变集合副本，以防固定类型参数
	if "IE_YUE_INC_E" in prefSet and "IE_YUE_INC_SMALLCAPS_E" in prefSet:
		prefSet.remove("IE_YUE_INC_SMALLCAPS_E")
	if "ONLY_YUAN_AS_AN" in prefSet and "IAN_YUAN_AS_AN" in prefSet:
		prefSet.remove("IAN_YUAN_AS_AN")

	global cmdPairTuple
	cmdPairTuple = (
		### 转换声母前的预处理
		# 音节间断与隔音符号
		(R"([aoeiuvüê][1-5]?)([yw])",  # a/o/e前有元音时必须写隔音符号
		 R"\1'\2"),  # 标明不必写出隔音符号的音节间断
		(R"'", (R"+" if "SYLLABLE_JUNCTURE_BY_PLUS" in prefSet else
				R".")),  # 音节间断（开音渡）标记
		# 兼容正规的印刷体字母ɡ/ɑ->g/a
		(R"ɡ", R"g"),
		(R"ɑ", R"a"),
		# 特殊的ê韵母，只能搭配零声母（“诶”字等）
		(R"(ê|ea)", R"ɛ"),  # ea是ê的GF 3006表示
		# 消除因可以紧邻韵腹充当声母或韵尾的辅音的歧义
		(R"r([aoeiu])", R"R\1"),  # 声母r暂改为R，以免与卷舌标志r混淆
		(R"n([aoeiuvü])", R"N\1"),  # 声母n暂改为N，以免与韵尾n/ng混淆
		# 需要排除式匹配转换的韵母
		(R"ng(?![aeu])", R"ŋ"),  # 双字母ng，后鼻音韵尾或自成音节
		(R"(?<![ieuyüv])e(?![inŋr])",
		 R"ɤ"),  # 单韵母e，此前已转换ea和声母r/n
		(R"(?<![bpmfdtNlgkhjqxzcsRiywuüv])er4",
		 (R"ar4" if "NUMBER_2_BY_AR4" in prefSet else
		  R"er4")),  # 数字“二”是否有大开口度韵腹
		(R"(?<![bpmfdtNlgkhjqxzcsRiywuüv])ar4",
		 (R"a˞4" if "ONLY_ER_BY_RHOTIC_HOOK" in prefSet else
		  R"ar4")),  # 数字“二”也属于er音节，可特别选用小钩
		(R"a(?![ionŋ])", (R"ᴀ" if "CENTRAL_A_BY_SMALLCAPS_A" in prefSet else
						  R"a")),  # “央a”是否用小型大写[A]标明，已转换“二”
		(R"(?<![bpmfdtNlgkhjqxzcsRiywuüv])er",
		 (R"ə˞" if "ONLY_ER_BY_RHOTIC_HOOK" in prefSet else
		  R"ər")),  # 一般的卷舌单韵母er，此前已排除“二”
		(R"(?<![iuüv])er", R"ər"),  # 构成单韵母e的儿化韵的er
		(R"(?<=[bpmf])o(?![uŋ])",
		 (R"o" if "NO_TRANSITIONAL_U" in prefSet else
		  R"uo")),  # bo/po/mo/fo是否有韵头u
		### 声母——无需转换m/f/n/l/s/r(但r可根据设置执行转换)
		# 送气清辅音声母
		(R"([ptk])", R"\1ʰ"),
		(R"q", R"tɕʰ"),
		(R"(ch|ĉ)", R"tʂʰ"),  # 后者是ch的注音变体
		(R"c", R"tsʰ"),  # 此前已排除ch
		# 不送气清辅音声母
		(R"b", R"p"),  # 此前已排除送气的p/t/k
		(R"d", R"t"),
		(R"g", R"k"),  # 此前已排除后鼻音双字母中的g，注意隔音
		(R"j", R"tɕ"),
		(R"(zh|ẑ)", R"tʂ"),  # 后者是zh的注音变体
		(R"z", R"ts"),  # 此前已排除zh
		# 擦音声母
		(R"(sh|ŝ)", R"ʂ"),  # 后者是sh的注音变体
		(R"x", R"ɕ"),  # 声母x，排除后再转换h
		(R"h", R"x"),  # 声母h，此前已排除zh/ch/sh
		### 韵母
		# 舌尖元音韵母
		(R"(sʰ?)i", R"\1ɿ"),  # zi/ci/si
		(R"([ʂR]ʰ?)i", R"\1ʅ"),  # zhi/chi/shi/ri
		# 部分韵腹省略表示的韵母+隔音符号和韵头w/y
		(R"iu", R"iou"),  # 无需转换iou
		(R"ui", R"uei"),  # 无需转换uei
		(R"wu?", R"u"),
		(R"yi?", R"i"),  # 此前已排除iu
		# 韵头[i]/[y]的韵母
		(R"iu|[üv]", R"y"),  # 转换ü/v，恢复yu/yue，准备yuan/yun
		(R"ian", (R"ian" if "IAN_YUAN_AS_AN" in prefSet else
				  R"iɛn")),  # 是否认为ian韵腹和an一样
		(R"yan", (R"yan" if ("ONLY_YUAN_AS_AN" in prefSet)
							or ("IAN_YUAN_AS_AN" in prefSet) else
				  R"yɛn")),  # 是否认为yuan韵腹和an一样（覆盖上一选项）
		(R"(ɕʰ?)uan", (R"\1yan" if ("ONLY_YUAN_AS_AN" in prefSet)
								   or ("IAN_YUAN_AS_AN" in prefSet) else
					   R"\1yɛn")),  # {j/q/x}uan，是否认为和an一样
		(R"(ɕʰ?)u", R"\1y"),  # {j/q/x}u{e/n}韵头，此前已排除{j/q/x}uan
		(R"([iy])e(?!n)", (R"\1ᴇ" if "IE_YUE_INC_SMALLCAPS_E" in prefSet else
						   R"\1e")),  # ie/yue/üe/ve，此前已转换{j/q/x}u
		(R"([iy])e(?!n)", (R"\1e" if "IE_YUE_INC_E" in prefSet else
						   R"\1ɛ")),  # ie/yue是否采用[e]标明，此前已判断[E]
		# 舌面央中元音轻微过渡韵腹
		(R"iŋ", (R"iəŋ" if "TRANSITIONAL_SCHWA_IN_ING" in prefSet else
				 R"iŋ")),  # ing是否有舌面央中元音韵腹
		(R"(un|uen)", (R"uən" if "TRANSITIONAL_SCHWA_IN_UEN" in prefSet else
					   R"un")),  # 合口un/uen过渡，此前已排除撮口[j/q/x]un
		# 后移的a
		(R"ao", R"ɑu"),  # 包括ao/iao，o改为u
		(R"aŋ", R"ɑŋ"),  # 包括ang/iang/uang
		# 韵母en/eng韵腹是舌面央中元音
		(R"e([nŋ])", R"ə\1"),
		# ong类韵母
		(R"ioŋ", (R"iuŋ" if "IONG_BY_IUNG" in prefSet else
				  R"yŋ")),  # iong的两种转换，此前yong已转换为iong
		(R"oŋ", R"uŋ"),  # ong，此前已排除iong
		### 儿化音变——无需转换ar/ir/ur/aur/our/yur
		# 舌尖元音韵母
		(R"[ɿʅ]r", R"ər"),  # {zh/ch/sh/r/z/c/s}ir
		# 鼻韵尾脱落和相应的韵腹元音替换
		(R"[aɛ][in]r", (R"ɐr" if "AIR_ANR_INC_NEAR_OPEN_CENTRAL" in prefSet else
						R"ar")),  # air/anr韵尾脱落后的韵腹替换
		(R"eir|ənr", R"ər"),  # eir韵腹央化，韵尾脱落；enr韵尾脱落
		(R"([iy])r", R"\1ər"),  # ir/yr增加央化韵腹
		(R"([iuy])nr", R"\1ər"),  # inr/ynr/unr韵尾脱落后增加央化韵腹
		(R"iuŋr", R"iũr"),  # iungr(iongr的可选变换)
		(R"([iuy])ŋr", R"\1ə̃r"),  # ingr/yngr/ungr韵尾脱落后增加央化鼻化韵腹
		(R"ŋr", R"̃r"),  # ang/eng韵尾儿化脱落后韵腹鼻化
		### 声母、韵母转换的善后扫尾工作
		# 处理儿化韵卷舌动作
		(R"r", (R"˞" if "RHOTICITY_BY_RHOTIC_HOOK" in prefSet else
				R"r")),  # 是否先把儿化韵卷舌动作改为卷舌小钩
		(R"R", (R"ʐ" if "INITIAL_R_BY_VED_RETRO_FRIC" in prefSet else
				R"r")),  # 恢复声母r，是否采用卷舌浊擦音表示声母r
		# 此前已处理完卷舌动作和声母，现在处理剩下的r字符的可选转换
		(R"r", (R"ɻ" if "R_TURNED_WITH_HOOK" in prefSet else
				R"r")),  # 是否严格采用卷舌通音符号[ɻ]
		# 恢复声母n
		(R"N", R"n"),  # 此前已处理完单元音韵母e和ie/yue
		# 其他选项
		(R"ʰ", (R"ʻ" if "ASPIRATE_BY_TURNED_COMMA" in prefSet else
				R"ʰ")),  # 是否采用反逗号送气符号
		(R"ai", (R"æi" if "AI_INC_NEAR_OPEN_FRONT" in prefSet else
				 R"ai")),  # （非儿化的）ai/uai韵腹为舌面前次开元音
		(R"ou", (R"əu" if "OU_INC_SCHWA" in prefSet else
				 R"ou")),  # ou/iou是否采用舌面央中元音韵腹
		### 声调
		"""
		# 先期处理
		((R"[1-5]" if "HIDE_ALL_TONE_VALUE" in prefSet else
		  R"5"), R""),  # 只隐藏轻声还是隐藏所有的声调转换
		(R"([1-4])",
		 (R"<sup>\1</sup>" if "HTML_SUP_TAG_FOR_TONE_VALUE" in prefSet else
		  R"(\1)")),  # 隔离单个数字调号
		# 标出普通话调值
		("([(>])1([)<])", R"\1 55\2"),  # 阴平（第一声）
		("([(>])2([)<])", R"\1 35\2"),  # 阳平（第二声）
		("([(>])3([)<])", R"\1 214\2"),  # 上声（第三声）
		("([(>])4([)<])", R"\1 51\2"),  # 去声（第四声）
		("([(>]) ([235])", R"\1\2")  # 去掉此前标调值时加上的空格
		"""
	)
	### 以上，替换命令元组创建完成！
	# 编译正则表达式对象，以便反复使用
	global reList
	reList = list(map(re.compile, [pair[PATTERN] for pair in cmdPairTuple]))


# _xqb 代表徐清白
def py2ipa_xqb(pinyinLine):
	"""对每一行拼音字符串依次执行全部替换命令，像流编辑器sed一样工作"""

	ipaLine = pinyinLine.lower()  # 大写字母在替换过程中另有意义
	for eachRe, eachCmdPair in zip(reList, cmdPairTuple):
		ipaLine = eachRe.sub(eachCmdPair[SUBST], ipaLine)
	return ipaLine


ipa_letter_dict = set()  # collect every ipa letter found in processing

def extract_punct_from_hzline(hanzi_line):
	# print(hanzi_line_no_prosody)
	punctuations_info = []  # collection of punctuations
	next_hanzi_index = 0
	for char in hanzi_line:
		if is_hanzi(char):
			next_hanzi_index = next_hanzi_index + 1
		else:
			punctuations_info.append((char, next_hanzi_index - 1))

	return punctuations_info


punctuation_to_keep = ['。', '，', '、', '：', '？', '！', '…']


def filter_punctuation(punctuations_info):
	remained_punctuations_info = []

	for punctuation_info in punctuations_info:
		if punctuation_info[0] in punctuation_to_keep:
			remained_punctuations_info.append(punctuation_info)

	return remained_punctuations_info

# split the IPA of one chinese char into a list of IPA letter.
# the returned result with such format: 't ʂʰ ən 2'
def split_ipa(ipa):
	global ipa_letter_dict

	ipa_phonemes = []
	for letter in ipa:
		if letter == 'ʰ':
			ipa_phonemes[-1] = ipa_phonemes[-1] + 'ʰ'
		else:
			ipa_phonemes.append(letter)

	ipa_letter_dict.update(ipa_phonemes)

	result_str = ' '.join(ipa_phonemes) # with such format: 't ʂʰ ən 2'
	return result_str


# convert ipa sequence into the new format:
# old format is a list, including punctuations, such as:
# 沉鱼#1落雁#3，闭月#1羞花#4。
# ['tʂʰən2', 'y2', 'luo4', 'iɛn4', ',', 'pi4', 'yɛ4', 'ɕiou1', 'xua1', '。']
# new format is a string, such as
# 't ʂʰ ə n 2|y 2|l u o 4|i ɛ n 4|,|p i 4|y ɛ 4|ɕ i o u 1|x u a 1|。'
def flatten_ipa_seq(ipa_list):
	ipa_str_list = map(split_ipa, ipa_list)

	total_ipa_seq = '$'.join(ipa_str_list)
	return total_ipa_seq


def filter_punct_mark(raw_sentence):
	hz_chars = []
	for char in raw_sentence:
		if is_hanzi(char):
			hz_chars.append(char)
	cleaned_sentence = "".join(hz_chars)
	return cleaned_sentence


# for example, "de" ==> "de5"
def fix_qinsheng(in_pinyin):
	'''format pinyin to mtts's format'''
	if not in_pinyin[-1].isdigit():
		in_pinyin = in_pinyin + '5'

	return in_pinyin


## only contains chinese characters, no punction
def hz2py(sentence_cleaned):
	pinyin_list = pinyin(sentence_cleaned, style=Style.TONE3)
	# print("pinyin_list before transform: {}".format(pinyin_list))

	pinyin_sequence = [fix_qinsheng(item[0]) for item in pinyin_list]

	"""
	# ------------- fix the pinyin of duoyinzi ----------------------
	duoyinzi_set = extract_duoyinzi(sentence_cleaned, duoyinzi_in_dict)
	# print("duoyinzi_set: {}".format(duoyinzi_set))

	for duoyinzi_item in duoyinzi_set:
		duoyinzi_context = duoyinzi_item[2]
		correct_pinyin = lookup_duoyinzi_dict(duoyinzi_context, duoyinzi_dict)
		if correct_pinyin == "":  # no matching found
			continue
		else:
			# need make it formatted
			correct_pinyin = pinyin_format(correct_pinyin)
			# print(duoyinzi_item, correct_pinyin)
			duoyinzi_index = duoyinzi_item[1]
			pinyin_sequence[duoyinzi_index] = correct_pinyin  # fixing happened here
	"""
	return pinyin_sequence


# hanzi_line, py_line have been cleaned, containing no new line symbol.
# py_line is a string, and contains no punctuation mark.
def cal_ipa_seq(hanzi_line, py_line):
	punctuations_info = extract_punct_from_hzline(hanzi_line)
	ipa_line = py2ipa_xqb(py_line)

	ipa_list = ipa_line.split()
	# if no punctuation existed at the end of sentence,
	# a '。' appended manually.
	if len(punctuations_info) == 0:
		punctuations_info.append(('。', len(ipa_list) - 1))

	# we should filter the punctuations at first,
	# print(hanzi_line)
	# print(punctuations_info)
	remained_punctuations_info = filter_punctuation(punctuations_info)

	# print(remained_punctuations_info)

	# and get the remained ones embedded into the ipa sequence
	next_punct_info = remained_punctuations_info.pop(0) # from the beginning
	next_punct_char = next_punct_info[0]
	next_punct_index = next_punct_info[1]

	new_ipa_list = []  # new_ipa_list contains punctuations
	for index, ipa in enumerate(ipa_list):
		new_ipa_list.append(ipa)

		if index == next_punct_index:
			new_ipa_list.append(next_punct_char)
			if len(remained_punctuations_info) == 0:
				next_punct_char = '。'
				next_punct_index = len(ipa_list) - 1
			else:
				next_punct_info = remained_punctuations_info.pop(0)  # from the beginning
				next_punct_char = next_punct_info[0]
				next_punct_index = next_punct_info[1]

	# finally, a string built from the new ipa list embedded with break punctuations
	total_ipa_seq = flatten_ipa_seq(new_ipa_list)
	return total_ipa_seq


def load_ipa_letter_dict():
	pass


if __name__ == '__main__':
	pass
	# test_proc_one_sentence()
