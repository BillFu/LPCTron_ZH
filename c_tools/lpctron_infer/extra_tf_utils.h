//
// Created by Bill Fu on 2020/5/15.
//

#ifndef C_TOOLS_EXTRA_TF_UTILS_H
#define C_TOOLS_EXTRA_TF_UTILS_H

#include <tensorflow/c/c_api.h> // TensorFlow C API header.

void PrintInputs(TF_Graph* graph, TF_Operation* op);

void PrintOutputs(TF_Graph* graph, TF_Operation* op, TF_Status* status);

void PrintTensorInfo(TF_Graph* graph, const char* layer_name, TF_Status* status);


#endif //C_TOOLS_EXTRA_TF_UTILS_H
