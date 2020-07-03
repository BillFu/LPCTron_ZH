
import argparse
import os
import numpy as np

from keras.backend.tensorflow_backend import set_session
import tensorflow as tf

from .lpcnet_model_new import new_lpcnet_model, Sparsify
from .ulaw import lin2ulaw, ulaw2lin


def load_model(model_file):
	set_session(tf.compat.v1.Session())

	model, encoder, decoder = new_lpcnet_model(use_gpu=False)

	model.compile(optimizer='adam',
					loss='sparse_categorical_crossentropy',
					metrics=['sparse_categorical_accuracy'])
	model.load_weights(model_file)
	return model, encoder, decoder


def infer_pcm(model, encoder, decoder, features_file, pcm_file):
	points_in_frame = 160
	nb_features = 55
	nb_used_features = model.nb_used_features

	features = np.fromfile(features_file, dtype='float32')
	print("the points in raw features: {}".format(features.shape[0]))

	features = np.resize(features, (-1, nb_features))
	nb_frames = 1
	feature_chunk_size = features.shape[0]  # the total number of frames in this voice clip
	pcm_chunk_size = points_in_frame * feature_chunk_size
	print("num_frames: {}".format(feature_chunk_size))
	print("pcm_chunk_size: {}".format(pcm_chunk_size))

	features = np.reshape(features, (nb_frames, feature_chunk_size, nb_features))
	features[:, :, 18:36] = 0
	periods = (.1 + 13.774 * features[:, :, 36:37] + 158.5).astype('int16')
	periods = np.clip(periods, 0, 255)

	order = 16

	pcm = np.zeros((nb_frames * pcm_chunk_size,))
	fexc = np.zeros((1, 1, 2), dtype='float32')
	iexc = np.zeros((1, 1, 1), dtype='int16')
	state1 = np.zeros((1, model.rnn_units1), dtype='float32')
	state2 = np.zeros((1, model.rnn_units2), dtype='float32')

	mem = 0
	coef = 0.85

	file_out_pcm = open(pcm_file, 'wb')

	skip = order + 1
	for c in range(0, nb_frames):
		cfeat = encoder.predict([features[c:c + 1, :, :nb_used_features],
								 periods[c:c + 1, :, :]])
		for fr in range(0, feature_chunk_size):
			f = c * feature_chunk_size + fr
			a = features[c, fr, nb_features - order:]
			for i in range(skip, points_in_frame):
				pred = -sum(a * pcm[f * points_in_frame + i - 1:f * points_in_frame + i - order - 1:-1])
				fexc[0, 0, 1] = lin2ulaw(pred)

				p, state1, state2 = decoder.predict([fexc, iexc, cfeat[:, fr:fr + 1, :], state1, state2])
				# Lower the temperature for voiced frames to reduce noisiness
				p *= np.power(p, np.maximum(0, 1.5 * features[c, fr, 37] - .5))
				p = p / (1e-18 + np.sum(p))
				# Cut off the tail of the remaining distribution
				p = np.maximum(p - 0.002, 0).astype('float64')
				p = p / (1e-8 + np.sum(p))

				iexc[0, 0, 0] = np.argmax(np.random.multinomial(1, p[0, 0, :], 1))
				pcm[f * points_in_frame + i] = pred + ulaw2lin(iexc[0, 0, 0])
				fexc[0, 0, 0] = lin2ulaw(pcm[f * points_in_frame + i])
				mem = coef * mem + pcm[f * points_in_frame + i]
				# print(mem)
				# write pcm chunk to file
				np.array([np.round(mem)], dtype='int16').tofile(file_out_pcm)
			skip = 0

	file_out_pcm.close()


def main():
	parser = argparse.ArgumentParser()
	parser.add_argument('--lpcnet_model_file',
		default='',
		help='Path to model checkpoint')

	parser.add_argument('--sentence_id',
						default='1001',
						help='sentence id')

	parser.add_argument('--in_feature_file',
						default='',
						help='the features produced by Tacotron-2.')

	parser.add_argument('--out_pcm_dir',
						default='out_pcm',
						help='folder to save the output pcm files.')

	parser.add_argument('--out_wav_dir',
		default='out_wav',
		help='folder to save the output wav files.')

	args = parser.parse_args()

	print("------------------------------------------------------------------------")
	for arg in vars(args):
		print(arg, getattr(args, arg))
	print("------------------------------------------------------------------------")

	model, encoder, decoder = load_model(args.lpcnet_model_file)
	print("LPCNet model loaded successfully!")

	# load_features(args.in_feature_file, model.nb_used_features)
	pcm_base_file = args.sentence_id + ".pcm"
	pcm_file = os.path.join(args.out_pcm_dir, pcm_base_file)
	infer_pcm(model, encoder, decoder,
			args.in_feature_file, pcm_file)


if __name__ == '__main__':
	main()
