CUDA_VISIBLE_DEVICES=0 TF_XLA_FLAGS=--tf_xla_cpu_global_jit  \
python3 -m Tacotron-2.new_train \
--input_dir=dataset/training_data \
--tacotron_input=dataset/training_data/train.txt
