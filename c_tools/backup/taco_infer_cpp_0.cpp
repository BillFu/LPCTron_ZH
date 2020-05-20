//
// Created by Bill Fu on 2020/5/14.
//

#include <iostream>
#include <vector>
#include <fstream>
#include<sstream>
#include <string>

#include "TFUtils.hpp"
#include "tf_diagnosis.h"
#include "scope_guard.hpp"
#include <tensorflow/c/c_api.h>

using namespace std;


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
    if (argc != 4)
    {
        std::cerr << std::endl <<
            "Usage: ./taco_infer_cpp <ipa_id_seq file> <tacotron model file> <out_feature_file.dat>" << std::endl;
        return 1;
    }

    std::string ipa_seq_file_name = argv[1];
    std::string taco_model_file_name = argv[2];
    std::string out_feature_file_name = argv[3];

    cout << "IPA ID Seq File: " << ipa_seq_file_name << endl;
    cout << "Tacotron Model File: " << taco_model_file_name << endl;

    // TFUtils init
    TFUtils TFU;
    TFUtils::STATUS status = TFU.LoadModel(taco_model_file_name);
    if (status != TFUtils::SUCCESS)
    {
        std::cerr << "Can't load tacotron graph from file: " << taco_model_file_name << endl;
        return 1;
    }
    else
    {
        cout << "Tacotron Model has been loaded Successfully!" << endl;
    }

    TF_Graph* graph = TFU.getGraph();

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

    // Iterate through the operations of a graph.  To use:
    /*
    size_t pos = 0;
    TF_Operation* oper;
    while ((oper = TF_GraphNextOperation(TFU.getGraph(), &pos)) != nullptr)
    {
        //DoSomethingWithOperation(oper);
        const char* op_name = TF_OperationName(oper);
        cout << op_name << endl;
    }
    */

    auto status0 = TF_NewStatus();
    SCOPE_EXIT{ TF_DeleteStatus(status0); }; // Auto-delete on scope exit.

    PrintOpInfo(graph, "inputs", status0);
    std::cout << std::endl;

    PrintOpInfo(graph, "input_lengths", status0);
    std::cout << std::endl;

    PrintOpInfo(graph, "model/inference/add", status0);
    std::cout << std::endl;

    TF_Output inputs_op = TFU.GetOperationByName("inputs", 0);
    TF_Output input_lengths_op = TFU.GetOperationByName("input_lengths", 0);

    if((inputs_op.oper == nullptr) || (input_lengths_op.oper == nullptr))
    {
        cout << "inputs OP or input_lengths OP NOT found!" << endl;
        return 1;
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
        cout << "inputs_tensor or input_lengths_tensor are NULL!" << endl;
        return 1;
    }

    const std::vector<TF_Output> all_input_ops = {inputs_op, input_lengths_op};
    const std::vector<TF_Tensor*> all_input_tensors = {inputs_tensor, input_lengths_tensor};

    // Output Tensor Create
    TF_Output out_op = TFU.GetOperationByName("model/inference/add", 0);
    if(out_op.oper == nullptr)
    {
        cout << "mel_outputs op NOT found!" << endl;
        return 1;
    }

    const std::vector<TF_Output> output_ops = {out_op};
    std::vector<TF_Tensor*> output_tensors = {nullptr};

    status = TFU.RunSession(all_input_ops, all_input_tensors,
                            output_ops, output_tensors);

    TFUtils::PrinStatus(status);

    if (status == TFUtils::SUCCESS)
    {
        //https://github.com/tensorflow/tensorflow/blob/master/tensorflow/c/c_api_test.cc
        cout << "run session successfully done!"<< endl;
        TF_Tensor* mels_out_tensor = output_tensors[0];

        int dims = TF_NumDims(mels_out_tensor);
        cout << "the dims of mels_out_tensor: "<< dims << endl;

        int dim0 = TF_Dim(mels_out_tensor, 0);
        int dim1 = TF_Dim(mels_out_tensor, 1);
        int dim2 = TF_Dim(mels_out_tensor, 2);

        cout << "the dim 0 of mels_out_tensor: "<< dim0 << endl;
        cout << "the dim 1 of mels_out_tensor: "<< dim1 << endl;
        cout << "the dim 2 of mels_out_tensor: "<< dim2 << endl;

        cout << "tensor type: " << TF_TensorType(mels_out_tensor) << endl;
        int num_bytes = TF_TensorByteSize(mels_out_tensor);
        cout << "Bytes: " << num_bytes << endl;

        float* mels_data = static_cast<float*>(TF_TensorData(mels_out_tensor));

        ofstream out(out_feature_file_name, ios::out | ios::binary);
        if(!out) {
            cout << "Cannot open out feature data file.";
            return 1;
        }

        out.write((char*)mels_data, num_bytes);
        out.close();
    }
    else
    {
        cout << "Error run session" << endl;
        return 1;
    }

    TFUtils::DeleteTensors(all_input_tensors);
    TFUtils::DeleteTensors(output_tensors);

    return 0;
}