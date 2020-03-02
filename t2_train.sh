CUDA_VISIBLE_DEVICES=1 TF_XLA_FLAGS=--tf_xla_cpu_global_jit  \
python3 Tacotron-2/new_train.py \
--input_dir=dataset/training_data \
--tacotron_input=dataset/training_data/train.txt
