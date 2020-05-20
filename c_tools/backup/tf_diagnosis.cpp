//
// Created by Bill Fu on 2020/5/15.
//

#include <iostream>

#include "tf_diagnosis.h"
#include "tf_utils.hpp"


void PrintInputs(TF_Graph*, TF_Operation* op) {
    auto num_inputs = TF_OperationNumInputs(op);

    std::cout << "input tensors:" << std::endl;

    for (auto i = 0; i < num_inputs; ++i) {
        auto input = TF_Input{op, i};
        auto type = TF_OperationInputType(input);
        std::cout << "Input: " << i << " type: " << tf_utils::DataTypeToString(type) << std::endl;
    }
}

void PrintOutputs(TF_Graph* graph, TF_Operation* op, TF_Status* status)
{
    std::cout << "output tensors:" << std::endl;

    auto num_outputs = TF_OperationNumOutputs(op);

    for (int i = 0; i < num_outputs; ++i) {
        auto output = TF_Output{op, i};
        auto type = TF_OperationOutputType(output);
        auto num_dims = TF_GraphGetTensorNumDims(graph, output, status);

        if (TF_GetCode(status) != TF_OK) {
            std::cout << "Can't get tensor dimensionality" << std::endl;
            continue;
        }

        std::cout << " dims: " << num_dims << std::endl;

        if (num_dims <= 0) {
            std::cout << " []" << std::endl;;
            continue;
        }

        std::vector<std::int64_t> dims(num_dims);

        std::cout << "Output: " << i << " type: " << tf_utils::DataTypeToString(type);
        TF_GraphGetTensorShape(graph, output, dims.data(), num_dims, status);

        if (TF_GetCode(status) != TF_OK) {
            std::cout << "Can't get get tensor shape" << std::endl;
            continue;
        }

        std::cout << " [";
        for (auto d = 0; d < num_dims; ++d) {
            std::cout << dims[d];
            if (d < num_dims - 1) {
                std::cout << ", ";
            }
        }
        std::cout << "]" << std::endl;
    }
}

void PrintOpInfo(TF_Graph* graph, const char* layer_name, TF_Status* status) {
    std::cout << "======Operation: " << layer_name << "======" << std::endl;
    auto op = TF_GraphOperationByName(graph, layer_name);

    if (op == nullptr) {
        std::cout << "Could not get " << layer_name << std::endl;
        return;
    }

    auto num_inputs = TF_OperationNumInputs(op);
    auto num_outputs = TF_OperationNumOutputs(op);
    std::cout << " inputs: " << num_inputs << " outputs: " << num_outputs << std::endl;

    PrintInputs(graph, op);
    PrintOutputs(graph, op, status);

    std::cout << "===============================" << std::endl;
}

void PrintOpInputs(TF_Graph*, TF_Operation* op) {
    auto num_inputs = TF_OperationNumInputs(op);

    std::cout << "Number inputs: " << num_inputs << std::endl;

    for (auto i = 0; i < num_inputs; ++i) {
        auto input = TF_Input{op, i};
        auto type = TF_OperationInputType(input);
        std::cout << std::to_string(i) << " type : " << tf_utils::DataTypeToString(type) << std::endl;
    }
}

void PrintOpOutputs(TF_Graph* graph, TF_Operation* op, TF_Status* status) {
    auto num_outputs = TF_OperationNumOutputs(op);

    std::cout << "Number outputs: " << num_outputs << std::endl;

    for (auto i = 0; i < num_outputs; ++i) {
        auto output = TF_Output{op, i};
        auto type = TF_OperationOutputType(output);
        std::cout << std::to_string(i) << " type : " << tf_utils::DataTypeToString(type);

        auto num_dims = TF_GraphGetTensorNumDims(graph, output, status);

        if (TF_GetCode(status) != TF_OK) {
            std::cout << "Can't get tensor dimensionality" << std::endl;
            continue;
        }

        std::cout << " dims: " << num_dims;

        if (num_dims <= 0) {
            std::cout << " []" << std::endl;;
            continue;
        }

        std::vector<std::int64_t> dims(num_dims);
        TF_GraphGetTensorShape(graph, output, dims.data(), num_dims, status);

        if (TF_GetCode(status) != TF_OK) {
            std::cout << "Can't get get tensor shape" << std::endl;
            continue;
        }

        std::cout << " [";
        for (auto j = 0; j < num_dims; ++j) {
            std::cout << dims[j];
            if (j < num_dims - 1) {
                std::cout << ",";
            }
        }
        std::cout << "]" << std::endl;
    }
}

void PrintOps(TF_Graph* graph, TF_Status* status)
{
    TF_Operation* op;
    std::size_t pos = 0;

    while ((op = TF_GraphNextOperation(graph, &pos)) != nullptr) {
        auto name = TF_OperationName(op);
        auto type = TF_OperationOpType(op);
        auto device = TF_OperationDevice(op);

        auto num_outputs = TF_OperationNumOutputs(op);
        auto num_inputs = TF_OperationNumInputs(op);

        std::cout << pos << ": " << name << " type: " << type << " device: " << device << " number inputs: " << num_inputs << " number outputs: " << num_outputs << std::endl;

        PrintOpInputs(graph, op);
        PrintOpOutputs(graph, op, status);
        std::cout << std::endl;
    }
}