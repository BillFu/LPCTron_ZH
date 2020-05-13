import os
import numpy as np
import tensorflow as tf
from .models import create_model
from .utils.ipa_text import text_to_sequence

"""
from .utils import plot
from datasets import audio
from datetime import datetime
import sounddevice as sd
import pyaudio
import wave
"""
from ..infolog import log

# load frozen model with pb format
class PB_Synthesizer:

    def load(self, model_file, hparams, model_name='Tacotron'):
        log('loading model: %s' % model_name)
        self.graph = tf.Graph()
        self.sess = tf.compat.v1.InteractiveSession(graph=self.graph)

        with tf.io.gfile.GFile(model_file, 'rb') as f:
            graph_def = tf.compat.v1.GraphDef()
            graph_def.ParseFromString(f.read())

        # Then, we import the graph_def into a new Graph and returns it
        with tf.Graph().as_default() as graph:
            # The name var will prefix every op/nodes in your graph
            # Since we load everything in a new graph, this is not needed
            tf.import_graph_def(graph_def)

        self.mel_outputs = self.graph.get_tensor_by_name("model/inference/add")
        self.inputs = self.graph.get_tensor_by_name("inputs")
        self.input_lengths = self.graph.get_tensor_by_name("input_lengths")

        print('tacotron pb model loading complete!')
        self._hparams = hparams

    def synthesize(self, ipa_text):
        hparams = self._hparams
        seq = text_to_sequence(ipa_text)

        feed_dict = {
            self.inputs: [np.asarray(seq, dtype=np.int32)],
            self.input_lengths: np.asarray([len(seq)], dtype=np.int32),
        }

        mels = self.session.run([self.mel_outputs], feed_dict=feed_dict)
        mels = mels.reshape(-1, hparams.num_mels)  # Thanks to @imdatsolak for pointing this out
        return mels


def load_graph(frozen_graph_filename):
    with tf.io.gfile.GFile(frozen_graph_filename, "rb") as f:
        graph_def = tf.compat.v1.GraphDef()
        graph_def.ParseFromString(f.read())
    with tf.Graph().as_default() as graph:
        tf.import_graph_def(graph_def)
    return graph


def analyze_inputs_outputs(graph):
    ops = graph.get_operations()
    outputs_set = set(ops)
    inputs = []
    for op in ops:
        if len(op.inputs) == 0 and op.type != 'Const':
            inputs.append(op)
        else:
            for input_tensor in op.inputs:
                if input_tensor.op in outputs_set:
                    outputs_set.remove(input_tensor.op)
    outputs = list(outputs_set)
    return (inputs, outputs)