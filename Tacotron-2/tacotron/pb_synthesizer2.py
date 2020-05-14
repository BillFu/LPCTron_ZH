
import numpy as np
import tensorflow as tf

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

    def synthesize(self, ipa_id_seq):
        hparams = self._hparams

        feed_dict = {
            self.inputs: [np.asarray(ipa_id_seq, dtype=np.int32)],
            self.input_lengths: np.asarray([len(ipa_id_seq)], dtype=np.int32),
        }

        mels = self.session.run([self.mel_outputs], feed_dict=feed_dict)[0]
        mels = mels.reshape(-1, hparams.num_mels)  # Thanks to @imdatsolak for pointing this out
        return mels
