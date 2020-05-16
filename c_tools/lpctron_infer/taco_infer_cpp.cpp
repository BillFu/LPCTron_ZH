//
// Created by Bill Fu on 2020/5/14.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <tensorflow/c/c_api.h>

#include "tf_diagnosis.h"
#include "scope_guard.hpp"
#include "tf_utils.hpp"

using namespace std;
//using namespace tf_utils;


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

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << std::endl <<
            "Usage: ./taco_infer_cpp <ipa_id_seq file> <tacotron model file>" << std::endl;
        return 1;
    }

    // std::string ipa_seq_file_name = argv[1];
    const char* ipa_seq_file_name = argv[1];
    // std::string taco_model_file_name = argv[2];
    const char* taco_model_file_name = argv[2];

    cout << "IPA ID Seq File: " << ipa_seq_file_name << endl;
    cout << "Tacotron Model File: " << taco_model_file_name << endl;

    auto taco_graph = tf_utils::LoadGraph(taco_model_file_name);
    SCOPE_EXIT{ tf_utils::DeleteGraph(taco_graph); }; // Auto-delete on scope exit.
    if (taco_graph == nullptr) {
        std::cerr << "Can't load tacotron graph from file: " << taco_model_file_name << endl;
        return 1;
    }
    else
    {
        cout << "Tacotron Model has been loaded Successfully!" << endl;
    }

    /*
    auto status = TF_NewStatus();
    SCOPE_EXIT{ TF_DeleteStatus(status); }; // Auto-delete on scope exit.

    PrintOps(taco_graph, status);
    */

    vector<int> ipa_id_list;
    bool is_OK = load_ipa_id_seq(ipa_seq_file_name, ipa_id_list);
    if(!is_OK)
    {
        std::cerr << "Can't load ipa id seq from file: " << ipa_seq_file_name << endl;
        return 1;
    }
    else
    {
        std::cerr << "ipa id seq loaded successfully." << endl;
    }

    auto status0 = TF_NewStatus();
    SCOPE_EXIT{ TF_DeleteStatus(status0); }; // Auto-delete on scope exit.

    PrintOpInfo(taco_graph, "inputs", status0);
    std::cout << std::endl;

    PrintOpInfo(taco_graph, "input_lengths", status0);
    std::cout << std::endl;

    PrintOpInfo(taco_graph, "model/inference/add", status0);
    std::cout << std::endl;

    TF_Operation* inputs_op = TF_GraphOperationByName(taco_graph, "inputs");
    TF_Operation* input_lengths_op = TF_GraphOperationByName(taco_graph, "input_lengths");

    if(inputs_op == nullptr || input_lengths_op == nullptr)
    {
        cout << "inputs OP or input_lengths OP NOT found!" << endl;
        return 1;
    }

    // Input Tensor Create
    const int id_num = ipa_id_list.size();
    cout << "seq length: " << id_num << endl;
    const std::vector<std::int64_t> inputs_dims = {1, id_num};
    const std::vector<int> inputs_vals = ipa_id_list;
    TF_Tensor* inputs_tensor = tf_utils::CreateTensor(TF_INT32, inputs_dims, inputs_vals);

    const std::vector<std::int64_t> input_lengths_dims = {1};
    const std::vector<int> input_lengths_vals = {id_num};
    TF_Tensor* input_lengths_tensor = tf_utils::CreateTensor(
            TF_INT32, input_lengths_dims, input_lengths_vals);
    if((inputs_tensor == nullptr) || (input_lengths_tensor == nullptr))
    {
        cout << "inputs_tensor or input_lengths_tensor are NULL!" << endl;
        return 1;
    }

    const std::vector<TF_Output> all_input_ops =
            { {inputs_op, 0},  // a TF_Output object in the inner brace.
              {input_lengths_op, 0} };
    const std::vector<TF_Tensor*> all_input_tensors = {inputs_tensor, input_lengths_tensor};
    SCOPE_EXIT{ tf_utils::DeleteTensors(all_input_tensors); }; // Auto-delete on scope exit.

    // Output Tensor Create
    TF_Operation* out_op = TF_GraphOperationByName(taco_graph, "model/inference/add");
    if(out_op == nullptr)
    {
        cout << "mel_outputs OP NOT found!" << endl;
        return 1;
    }

    const std::vector<TF_Output> output_ops = {{out_op, 0}};
    std::vector<TF_Tensor*> output_tensors = {nullptr};
    SCOPE_EXIT{ tf_utils::DeleteTensors(output_tensors); }; // Auto-delete on scope exit.

    auto session = tf_utils::CreateSession(taco_graph);
    SCOPE_EXIT{ tf_utils::DeleteSession(session); }; // Auto-delete on scope exit.
    if (session == nullptr) {
        std::cout << "Can't create tf session" << std::endl;
        return 2;
    }

    auto code = tf_utils::RunSession(session,
                                     all_input_ops, all_input_tensors,
                                     output_ops, output_tensors);

    if (code == TF_OK)
    {
        auto mels_out = tf_utils::GetTensorData<std::vector<float>>(output_tensors[0]);
        int frame_num = mels_out.size();
        cout << "the number of the predicted frames: " << frame_num << endl;
    }
    else
    {
        std::cout << "Error run session TF_CODE: " << code;
        return code;
    }

    /*
    {
        cout << "run session successfully done!"<< endl;
        TF_Tensor* mels_out_tensor = output_tensors[0];
        const std::vector<std::vector<float>> mels_data =
                TFUtils::GetTensorsData<std::vector<float>>(mels_out_tensor);
        int frame_num = mels_data.size();
        cout << "the number of the predicted frames: " << frame_num << endl;

    */


    return 0;
}