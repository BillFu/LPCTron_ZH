import argparse
import os
import tensorflow as tf

from .tacotron.models import create_model
from .my_hparams import hparams


class ckpt_loader:
	def load(self, checkpoint_path, hparams, model_name='Tacotron'):
		inputs = tf.placeholder(tf.int32, [1, None], 'inputs')
		input_lengths = tf.placeholder(tf.int32, [1], 'input_lengths')
		with tf.variable_scope('model') as scope:
			self.model = create_model(model_name, hparams)
			self.model.initialize(inputs, input_lengths)
			self.mel_outputs = self.model.mel_outputs
			self.alignment = self.model.alignments[0]

		# log('Loading checkpoint: %s' % checkpoint_path)
		self.session = tf.Session()
		self.session.run(tf.global_variables_initializer())
		saver = tf.train.Saver()
		saver.restore(self.session, checkpoint_path)

	def toPB(self, pb_dir, pb_file):
		#convert checkpoint to frozen model
		minimal_graph = tf.graph_util.convert_variables_to_constants(self.session,
						self.session.graph_def, ["model/inference/add"])
		tf.train.write_graph(minimal_graph, pb_dir, pb_file, as_text=False)


def convert_model(ckpt_path, out_pb_dir, out_pb_file):
	try:
		checkpoint_path = tf.train.get_checkpoint_state(ckpt_path).model_checkpoint_path
		print('model found at {}'.format(checkpoint_path))
	except:
		raise RuntimeError('Failed to load model checkpoint at {}'.format(ckpt_path))

	loader = ckpt_loader()
	loader.load(checkpoint_path, hparams)
	loader.toPB(out_pb_dir, out_pb_file)
	pb_full_path = os.path.join(out_pb_dir, out_pb_file)
	print('**** PB model saved as: {} ****'.format(pb_full_path))


def main():
	parser = argparse.ArgumentParser()
	parser.add_argument('--checkpoint_dir',
		default='taco_pretrained/',
		help='Path to model checkpoint')

	parser.add_argument('--out_pb_dir',
		default='.',
		help='folder to save the pb model.')

	parser.add_argument('--out_pb_file',
		default='tacotron.pb',
		help='the file to save the pb model.')

	args = parser.parse_args()

	print("------------------------------------------------------------------------")
	for arg in vars(args):
		print(arg, getattr(args, arg))
	print("------------------------------------------------------------------------")

	convert_model(args.checkpoint_dir, args.out_pb_dir, args.out_pb_file)


if __name__ == '__main__':
	main()