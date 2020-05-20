//
// Created by Bill Fu on 2020/5/14.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#include "TFUtils.hpp"
#include <tensorflow/c/c_api.h>
#include "lpcnet.h"
#include "arch.h"
#include "freq.h"

using namespace std;

// merge Tacotron-2 and LPCNet into this module.

// if ok, return true; otherwise return false.
bool load_ipa_id_seq(const string& ipa_file_name, vector<int>& ipa_id_list)
{
    ifstream ipa_file(ipa_file_name);
    if (!ipa_file.is_open())  //failed to open ipa seq file
    {
        return false;
    }

    string ipa_seq_line;
    getline(ipa_file,ipa_seq_line);
    cout << "the content in ipa file: " << ipa_seq_line << '\n';
    ipa_file.close();

    // split the string (represent the ipa id sequence) into list containing int ids
    istringstream in_ss(ipa_seq_line);
    string id_str;
    while (in_ss >> id_str) {
        int id = std::stoi(id_str);  //convert string into int
        ipa_id_list.push_back(id);
    }

    return true;
}

// return true for OK;
// otherwise false for error happened, and error_msg contains the detail.
bool infer(TFUtils& TFU, const string& ipa_seq_file_name,
        LPCNetState* lpcnet, const string& out_sr,
        const string& out_pcm_file_name, const string& out_wave_file_name,
        string& error_msg)
{
    TF_Graph* graph = TFU.getGraph();

    vector<int> ipa_id_list;
    bool is_OK = load_ipa_id_seq(ipa_seq_file_name, ipa_id_list);
    if(!is_OK)
    {
        error_msg = "Can't load ipa id seq from file: "  + ipa_seq_file_name;
        return false;
    }
    else
    {
        std::cout << "ipa id seq loaded successfully." << endl;
    }

    TF_Output inputs_op = TFU.GetOperationByName("inputs", 0);
    TF_Output input_lengths_op = TFU.GetOperationByName("input_lengths", 0);

    if((inputs_op.oper == nullptr) || (input_lengths_op.oper == nullptr))
    {
        error_msg = "inputs OP or input_lengths OP NOT found!";
        return false;
    }

    // Input Tensor Create
    const int id_num = ipa_id_list.size();
    const std::vector<std::int64_t> inputs_dims = {1, id_num};
    const std::vector<int> inputs_vals = ipa_id_list;
    TF_Tensor* inputs_tensor = TFUtils::CreateTensor(TF_INT32, inputs_dims, inputs_vals);

    const std::vector<std::int64_t> input_lengths_dims = {1};
    const std::vector<int> input_lengths_vals = {id_num};
    TF_Tensor* input_lengths_tensor = TFUtils::CreateTensor(
            TF_INT32, input_lengths_dims, input_lengths_vals);
    if((inputs_tensor == nullptr) || (input_lengths_tensor == nullptr))
    {
        error_msg = "inputs_tensor or input_lengths_tensor are NULL!";
        return false;
    }

    const std::vector<TF_Output> all_input_ops = {inputs_op, input_lengths_op};
    const std::vector<TF_Tensor*> all_input_tensors = {inputs_tensor, input_lengths_tensor};

    // Output Tensor Create
    TF_Output out_op = TFU.GetOperationByName("model/inference/add", 0);
    if(out_op.oper == nullptr)
    {
        error_msg = "mel_outputs op NOT found!";
        return false;
    }

    const std::vector<TF_Output> output_ops = {out_op};
    std::vector<TF_Tensor*> output_tensors = {nullptr};

    TFUtils::STATUS status = TFU.RunSession(all_input_ops, all_input_tensors,
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

        lpcnet_synthesize(lpcnet, out_pcm, in_features, FRAME_SIZE);
        ofs_pcm.write(reinterpret_cast<char *>(out_pcm), bytes_frame);

        raw_features = raw_features + (NB_BANDS+2);  // move to the next frame
    }
    ofs_pcm.close();

    TFUtils::DeleteTensors(all_input_tensors);
    TFUtils::DeleteTensors(output_tensors);

    string ffmpeg_cmd;
    if(out_sr == "16k")
        ffmpeg_cmd = "ffmpeg -f s16le -ar 16k -ac 1 -i " +
            out_pcm_file_name + " -ar 16k " + out_wave_file_name;
    else  // 8k for the output wav file
        ffmpeg_cmd = "ffmpeg -f s16le -ar 16k -ac 1 -i " +
            out_pcm_file_name + " -ar 8k " + out_wave_file_name;

    system(ffmpeg_cmd.c_str());

    return true;
}


int main(int argc, char* argv[])
{
    // <out_sr> must be "8k" or "16k"
    if (argc != 6)
    {
        std::cerr << std::endl <<
        "Usage: ./backend_infer <sentence_id> <ipa_id_seq file> <tacotron model file> " <<
        "<out_sr> <out_folder>" << std::endl;
        return 1;
    }

    std::string sentence_id = argv[1];
    std::string ipa_seq_file_name = argv[2];
    std::string taco_model_file_name = argv[3];
    std::string out_sr = argv[4];
    string out_folder = argv[5];
    string out_pcm_file_name = out_folder + "/" + sentence_id + ".pcm";
    string out_wave_file_name =  out_folder + "/" + sentence_id + ".wav";;

    cout << "sentence_id: " << sentence_id << endl;
    cout << "IPA ID Seq File: " << ipa_seq_file_name << endl;
    cout << "Tacotron Model File: " << taco_model_file_name << endl;
    cout << "out sr: " << out_sr << endl;
    cout << "out_folder: " << out_folder << endl;
    cout << "out pcm file: " << out_pcm_file_name << endl;
    cout << "out wave file: " << out_wave_file_name << endl;

    // TFUtils init
    TFUtils TFU;
    TFUtils::STATUS status = TFU.LoadModel(taco_model_file_name);
    if (status != TFUtils::SUCCESS)
    {
        std::cerr << "Can't load tacotron model from file: " << taco_model_file_name << endl;
        return 1;
    }
    else
    {
        cout << "Tacotron Model has been loaded Successfully!" << endl;
    }

    LPCNetState* lpcnet;
    lpcnet = lpcnet_create();

    std::string  error_msg;

    bool isOK = infer(TFU, ipa_seq_file_name, lpcnet, out_sr,
                      out_pcm_file_name, out_wave_file_name, error_msg);
    if (!isOK)
    {
        cout << "Error: " << error_msg << endl;
    }

    lpcnet_destroy(lpcnet);

    return 0;
}