//
// Created by Bill Fu on 2020/5/26.
//

#include "backend_implement.h"
#include <fstream>
#include <sstream>

#include "arch.h"
#include "freq.h"

using grpc::Status;

// split the string of ipa id seq into a list of integer IDs.
void tokenize_ipa_seq(string const &ipa_id_seq_str,
                      vector<int> &out_ipa_id_seq)
{
    size_t start;
    size_t end = 0;
    string delim(" ");
    while ((start = ipa_id_seq_str.find_first_not_of(delim, end)) != string::npos)
    {
        end = ipa_id_seq_str.find(delim, start);
        string id_str = ipa_id_seq_str.substr(start, end - start);
        int id = stoi(id_str);
        out_ipa_id_seq.push_back(id);
    }
}

BackendInferImpl::BackendInferImpl()
{
    _lpcnet = NULL;
}

BackendInferImpl::~BackendInferImpl()
{
    if(_lpcnet != NULL)
    {
        lpcnet_destroy(_lpcnet);
        _lpcnet = NULL;
    }
}

bool BackendInferImpl::initialize(const string& taco_model_file_name,
                                  const string& out_pcm_dir,
                                  const string& out_wav_dir,
                                  string &error_msg)
{
    _out_pcm_dir = out_pcm_dir;
    _out_wav_dir = out_wav_dir;

    TFUtils::STATUS status = _tfu.LoadModel(taco_model_file_name);
    if (status != TFUtils::SUCCESS)
    {
        error_msg = "Can't load tacotron model from file: " + taco_model_file_name;
        return false;
    }
    else
    {
        cout << "Tacotron Model has been loaded Successfully!" << endl;
    }

    _lpcnet = lpcnet_create();

    _graph = _tfu.getGraph();

    _inputs_op = _tfu.GetOperationByName("inputs", 0);
    _input_lengths_op = _tfu.GetOperationByName("input_lengths", 0);

    if((_inputs_op.oper == nullptr) || (_input_lengths_op.oper == nullptr))
    {
        error_msg = "inputs OP or input_lengths OP NOT found!";
        return false;
    }

    // Output Tensor Create
    _out_op = _tfu.GetOperationByName("model/inference/add", 0);
    if(_out_op.oper == nullptr)
    {
        error_msg = "mel_outputs op NOT found!";
        return false;
    }

    return true;
}

Status BackendInferImpl::commitJob(ServerContext* context,
                 const ::JobRequest* request, ::JobReply* response)
{
    cout << "============new job request arrived============" << endl;
    cout << "sentence_id: " << request->sentence_id() << endl;
    cout << "sr: " << request->sr() << endl;
    cout << "ipa id seq: " << request->ipa_id_seq() << endl;

    vector<int> ipa_id_seq;
    tokenize_ipa_seq(request->ipa_id_seq(), ipa_id_seq);
    int num_ids = ipa_id_seq.size();
    if(num_ids == 0)
    {
        response->set_isok(false);
        response->set_error_msg("the ipa id seq is EMPTY.");
        return Status::OK;
    }

    string error_msg;
    string out_pcm_file_name = _out_pcm_dir + "/" + request->sentence_id() + ".pcm";
    string out_wave_file_name = _out_wav_dir + "/" + request->sentence_id() + ".wav";;

    bool is_ok = infer(ipa_id_seq, request->sr(),
                       out_pcm_file_name, out_wave_file_name,
                       error_msg);
    if(!is_ok)  // Error happened
    {
        response->set_isok(false);
        response->set_error_msg(error_msg);
    }
    else
    {
        response->set_isok(true);
        response->set_error_msg("OK");
    }

    return Status::OK;
}


bool BackendInferImpl::infer(const vector<int>& ipa_id_list, const string& out_sr,
           const string& out_pcm_file_name, const string& out_wave_file_name,
           string& error_msg)
{
    // Input Tensor Create
    const int id_num = ipa_id_list.size();
    const std::vector<std::int64_t> inputs_dims = {1, id_num};
    // const std::vector<int> inputs_vals = ipa_id_list;
    TF_Tensor* inputs_tensor = TFUtils::CreateTensor(TF_INT32, inputs_dims, ipa_id_list);

    const std::vector<std::int64_t> input_lengths_dims = {1};
    const std::vector<int> input_lengths_vals = {id_num};
    TF_Tensor* input_lengths_tensor = TFUtils::CreateTensor(
            TF_INT32, input_lengths_dims, input_lengths_vals);
    if((inputs_tensor == nullptr) || (input_lengths_tensor == nullptr))
    {
        error_msg = "inputs_tensor or input_lengths_tensor are NULL!";
        return false;
    }

    const std::vector<TF_Output> all_input_ops = {_inputs_op, _input_lengths_op};
    const std::vector<TF_Tensor*> all_input_tensors = {inputs_tensor, input_lengths_tensor};

    const std::vector<TF_Output> output_ops = {_out_op};
    std::vector<TF_Tensor*> output_tensors = {nullptr};

    TFUtils::STATUS status = _tfu.RunSession(all_input_ops, all_input_tensors,
                                             output_ops, output_tensors);

    TFUtils::PrinStatus(status);

    if (status != TFUtils::SUCCESS)
    {
        TFUtils::DeleteTensors(all_input_tensors);
        TFUtils::DeleteTensors(output_tensors);

        error_msg = "Error run session in Tacotron reasoning phase.";
        return false;
    }

    //https://github.com/tensorflow/tensorflow/blob/master/tensorflow/c/c_api_test.cc
    cout << "run session successfully done!"<< endl;
    TF_Tensor* mels_out_tensor = output_tensors[0];

    int num_frames = TF_Dim(mels_out_tensor, 1);
    cout << "num_frames: " << num_frames << endl;

    int num_bytes = TF_TensorByteSize(mels_out_tensor);
    cout << "Bytes: " << num_bytes << endl;

    float* mels_data = static_cast<float*>(TF_TensorData(mels_out_tensor));

    ofstream ofs_pcm(out_pcm_file_name, ios::binary);

    float* raw_features = mels_data;
    int bytes_frame = sizeof(short) * FRAME_SIZE;
    for (int i = 0; i < num_frames; i++)
    {
        float in_features[NB_FEATURES];
        short out_pcm[FRAME_SIZE];

        RNN_COPY(in_features, raw_features, NB_BANDS);
        RNN_CLEAR(in_features+18, 18);
        RNN_COPY(in_features+36, raw_features+NB_BANDS, 2);

        lpcnet_synthesize(_lpcnet, out_pcm, in_features, FRAME_SIZE);
        ofs_pcm.write(reinterpret_cast<char *>(out_pcm), bytes_frame);

        raw_features = raw_features + (NB_BANDS+2);  // move to the next frame
    }
    ofs_pcm.close();

    TFUtils::DeleteTensors(all_input_tensors);
    TFUtils::DeleteTensors(output_tensors);

    string ffmpeg_cmd;
    if(out_sr == "16k")
        ffmpeg_cmd = "ffmpeg -y -loglevel panic -f s16le -ar 16k -ac 1 -i " +
                     out_pcm_file_name + " -ar 16k " + out_wave_file_name;
    else  // 8k for the output wav file
        ffmpeg_cmd = "ffmpeg -y -loglevel panic -f s16le -ar 16k -ac 1 -i " +
                     out_pcm_file_name + " -ar 8k " + out_wave_file_name;

    system(ffmpeg_cmd.c_str());

    return true;
}

