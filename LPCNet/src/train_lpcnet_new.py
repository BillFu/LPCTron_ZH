#!/usr/bin/python3

# Train a LPCNet model (note not a Wavenet model)

import sys
import numpy as np
import argparse

from keras.optimizers import Adam
from .ulaw import ulaw2lin, lin2ulaw
import keras.backend as K
import h5py
import os as os
import tensorflow as tf
from keras.backend.tensorflow_backend import set_session

from .save_model_new import save_model
from .train_val_tb import TrainValTensorBoard
from .lpcnet_new import new_lpcnet_model, Sparsify


if __name__ == '__main__':
	parser = argparse.ArgumentParser(
		description="train the lpcnet model.")

	parser.add_argument(
		"--feature_file",
		help="the path of feature file.")

	parser.add_argument(
		"--pcm_file",
		help="the path of pcm data file.")

	args = parser.parse_args()

	nb_epochs = 120

	# Try reducing batch_size if you run out of memory on your GPU
	batch_size = 64

	feature_file = args.feature_file
	pcm_file = args.pcm_file  # 16 bit unsigned short PCM samples
	frame_size = 160
	nb_features = 55
	nb_used_features = 38  #
	feature_chunk_size = 15
	pcm_chunk_size = frame_size * feature_chunk_size

	# u for unquantised, load 16 bit PCM samples and convert to mu-law
	config = tf.ConfigProto()

	# use this option to reserve GPU memory, e.g. for running more than
	# one thing at a time.  Best to disable for GPUs with small memory
	config.gpu_options.per_process_gpu_memory_fraction = 0.9
	set_session(tf.Session(config=config))

	pcm_data = np.fromfile(pcm_file, dtype='uint8')
	nb_frames = len(pcm_data) // (4 * pcm_chunk_size)

	features = np.fromfile(feature_file, dtype='float32')

	# limit to discrete number of frames
	pcm_data = pcm_data[:nb_frames * 4 * pcm_chunk_size]
	print("shape of pcm_data: {}".format(pcm_data.shape))

	features = features[:nb_frames * feature_chunk_size * nb_features]
	print("shape of features before reshape: {}".format(features.shape))
	features = np.reshape(features, (nb_frames * feature_chunk_size, nb_features))
	print("shape of features after reshape: {}".format(features.shape))

	sig = np.reshape(pcm_data[0::4], (nb_frames, pcm_chunk_size, 1))
	pred = np.reshape(pcm_data[1::4], (nb_frames, pcm_chunk_size, 1))
	in_exc = np.reshape(pcm_data[2::4], (nb_frames, pcm_chunk_size, 1))
	out_exc = np.reshape(pcm_data[3::4], (nb_frames, pcm_chunk_size, 1))
	del pcm_data

	print("ulaw std = ", np.std(out_exc))

	features = np.reshape(features, (nb_frames, feature_chunk_size, nb_features))
	features = features[:, :, :nb_used_features]
	features[:, :, 18:36] = 0

	periods = (.1 + 50 * features[:, :, 36:37] + 100).astype('int16')

	in_data = np.concatenate([sig, pred], axis=-1)
	del sig
	del pred

	"""
	model, _, _ = new_lpcnet_model()

	model.compile(optimizer='adam',
				  loss='sparse_categorical_crossentropy',
				  metrics=['sparse_categorical_accuracy'])
	model.summary()

	checkpoint = save_model(model)

	adaptation = False
	if adaptation:
		f = open("checkpoint", "r")
		latest_model = f.read().replace('\n', '')
		model.load_weights(latest_model)
		print("***Successfully loaded checkpoint %s" % latest_model)

	# model.load_weights('lpcnet9b_384_10_G16_01.h5')
	model.compile(optimizer=Adam(0.001, amsgrad=True, decay=5e-5), loss='sparse_categorical_crossentropy')
	# model.fit([in_data, in_exc, features, periods], out_exc, batch_size=batch_size, epochs=nb_epochs, validation_split=0.0, callbacks=[checkpoint, lpcnet.Sparsify(2000, 40000, 400, (0.05, 0.05, 0.2)),TrainValTensorBoard(histogram_freq = 5)])

	model.fit([in_data, in_exc, features, periods],
			out_exc, batch_size=batch_size,
			epochs=nb_epochs, validation_split=0.0,
			callbacks=[checkpoint, Sparsify(2000, 40000, 400, (0.05, 0.05, 0.2))])
	"""