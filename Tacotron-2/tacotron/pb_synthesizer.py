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
        self.session = tf.compat.v1.InteractiveSession(graph=self.graph)

        with tf.io.gfile.GFile(model_file, 'rb') as f:
            graph_def = tf.compat.v1.GraphDef()
            graph_def.ParseFromString(f.read())

        tf.import_graph_def(graph_def)

        self.mel_outputs = self.graph.get_tensor_by_name("import/model/inference/add:0")
        self.inputs = self.graph.get_tensor_by_name("import/inputs:0")
        self.input_lengths = self.graph.get_tensor_by_name("import/input_lengths:0")

        print('tacotron pb model loading complete!')
        self._hparams = hparams

    def synthesize(self, ipa_text):
        hparams = self._hparams
        seq = text_to_sequence(ipa_text)

        feed_dict = {
            self.inputs: [np.asarray(seq, dtype=np.int32)],
            self.input_lengths: np.asarray([len(seq)], dtype=np.int32),
        }

        mels = self.session.run([self.mel_outputs], feed_dict=feed_dict)[0]
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