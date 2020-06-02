import json
from pathlib import Path
from os.path import join
import os
import logging
import pickle

from .frontend.fix_duoyinzi import load_duoyinzi_dict

# read setting parameters from the config file;
# init the common data, such as duoyinzi dict
#	return config, False, "No Error", duoyinzi_registered, duoyinzi_dict

def load_config_data(config_file):
	my_file = Path(config_file)
	if not my_file.is_file():
		#  does not exist
		error_reason = "'{}' does not exist!".format(config_file)
		return None, True, error_reason

	with open(config_file, 'r', encoding='utf-8') as f:
		try:
			config = json.load(f, strict=False)
		except ValueError:  # includes json.decoder.JSONDecodeError
			# logger.log(level=logging.DEBUG, msg="Exception occurred", exc_info=True)
			error_reason = "error in config file: '{}'.".format(config_file)
			return None, True, error_reason

	needed_keys = ["ws_server_id", "ws_server_port",
		"duoyinzi_dict", "chars_limit", "backend_host", "backend_port"]

	for key in needed_keys:
		if key not in config.keys():
			error_reason = "'{}' not contained in config file: {}.". \
				format(key, config_file)
			return config, True, error_reason

	# intermediate_dir = config["intermediate_dir"]
	# out_wav_dir = config["out_wav_dir"]
	"""
	try:
		os.makedirs(intermediate_dir, exist_ok=True)
		os.makedirs(out_wav_dir, exist_ok=True)
	except OSError as e:
		error_reason = \
			"failed to create dir: '{}' or dir: '{}'.".format(intermediate_dir, out_wav_dir)
		module_logger.log(level=logging.ERROR,
			msg=error_reason, exc_info=True)
		return True, error_reason
	"""

	return config, False, "No Error"


def load_duoyinzi_dict(config, module_logger):
	duoyinzi_dict_filename = config['duoyinzi_dict']
	print(duoyinzi_dict_filename)
	# -----------load dictionary of duoyinzi---------------------------------
	# check the validness of the file of duoyinzi dict
	dyz_dict_file = Path(duoyinzi_dict_filename)
	if not dyz_dict_file.is_file():
		error_reason = "the duoyinzi dict file does not exist: {}". \
			format(duoyinzi_dict_filename)
		return True, error_reason, None

	duoyinzi_registered, duoyinzi_dict, error_lines, duplicated_lines = \
		load_duoyinzi_dict(duoyinzi_dict_filename)

	if len(error_lines) > 0:
		error_reason = "Format Errors happened in those lines: {} \nin file: {}.". \
			format(error_lines, duoyinzi_dict_filename)
		return True, error_reason, None

	if len(duplicated_lines) > 0:
		error_reason = "Duplication happened in those lines: {} \nin file: {}.". \
			format(duplicated_lines, duoyinzi_dict_filename)
		return True, error_reason, None

	module_logger.info("duoyinzi dict has been loaded successfully.")

	result = {"duoyinzi_registered": duoyinzi_registered,
					"duoyinzi_dict": duoyinzi_dict}

	return False, None, result