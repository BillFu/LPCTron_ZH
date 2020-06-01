#!/usr/bin/env python
# coding=utf-8

# logging模块打印log日志到文件和屏幕
# Log级别：CRITICAL(50)、ERROR(40)、WARNING(30)、INFO(20)、DEBUG(10)、NOTSET(0)
# https://www.cnblogs.com/liujiacai/p/7804848.html
import logging, os, sys, time


class Log_info(object):
	def __init__(self, basename):
		# 指定日志文件名，获取当前执行的py文件名
		# 获取当天日期
		file_name_date = time.strftime("%Y-%m-%d", time.localtime())
		self.filename = basename + "_" + file_name_date + '.log'
		# 指定输出的格式和内容
		self.format = '%(asctime)s [%(filename)s] %(levelname)s:%(message)s'
		# 设置日志级别，默认为logging.WARNNING
		# self.level = logging.INFO
		# 和file函数意义相同，指定日志文件的打开模式，'w'或者'a'
		self.filemode = 'w+'
		# 指定时间格式
		self.datefmt = '%Y-%m-%d %H:%M:%S'
		# self.logger = logging.getLogger(__name__)
		self.logger = logging.getLogger(basename)


	def main(self, generic_level, console_level, file_level):
		self.logger.setLevel(generic_level)
		format = logging.Formatter(self.format, datefmt=self.datefmt)
		# 日志输出到文件
		file_handler = logging.FileHandler(self.filename, self.filemode)
		file_handler.setLevel(file_level)
		file_handler.setFormatter(format)
		# 使用StreamHandler输出到屏幕
		console = logging.StreamHandler()
		console.setLevel(console_level)
		console.setFormatter(format)
		# 添加两个Handler
		self.logger.addHandler(file_handler)
		self.logger.addHandler(console)
		# 执行输出信息: 2019-01-02 15:51:13 [print_debug.py] INFO:输出信息内容
		# self.logger.info("输出信息内容")
		# 这么写就可以
		# self.logger.info("经营许可证字段值个数:%s"%2)
		# self.logger.info("经营许可证字段值个数:%s",3)
		# try:
		#     (a== 2)
		# except Exception as e:
		#     self.logger.info(e)
		return self.logger


if __name__ == "__main__":
	log_info = Log_info()
	logger = log_info.main()
