
#include "tensorflow/c/c_api.h"

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <assert.h>
#include <vector>
#include <algorithm>
#include <iterator>
#include <iostream>


TF_Buffer* read_file(const char* file);

void free_buffer(void* data, size_t length) 
{
}

static void Deallocator(void* data, size_t length, void* arg) 
{
}

int main() 
{
  TF_Buffer* graph_def = read_file("data/graph.pb");
  TF_Graph* graph = TF_NewGraph();

  TF_Status* status = TF_NewStatus();
  TF_ImportGraphDefOptions* graph_opts = TF_NewImportGraphDefOptions();
  TF_GraphImportGraphDef(graph, graph_def, graph_opts, status);
  if (TF_GetCode(status) != TF_OK) 
  {
          fprintf(stderr, "ERROR: Unable to import graph %s", TF_Message(status));
          return 1;
  }
  else 
  {
          fprintf(stdout, "Successfully imported graph\n");
  }
  const int num_bytes_in = 1 * 50 * 50 * 3 * sizeof(float);
  const int num_bytes_out = 1 * 2 * sizeof(float);

  int64_t in_dims[] = {1, 50, 50, 3};
  int64_t out_dims[] = {1, 2};

  float values[1 * 50 * 50 * 3] = {0xff};

  std::vector<TF_Output> inputs;
  std::vector<TF_Tensor*> input_values;

  inputs.push_back({TF_GraphOperationByName(graph, "input_1"), 0});
  input_values.push_back(TF_NewTensor(TF_FLOAT, in_dims, 4, values, num_bytes_in, &Deallocator, 0));
  input_values.push_back(TF_NewTensor(TF_FLOAT, in_dims, 4, values, num_bytes_in, &Deallocator, 0));

  std::vector<TF_Output> outputs;
  outputs.push_back({TF_GraphOperationByName(graph, "dense_3/Softmax"), 0});

  std::vector<TF_Tensor*> output_values;

  output_values.push_back(TF_AllocateTensor(TF_FLOAT, out_dims, 2, num_bytes_out));
  output_values.push_back(TF_AllocateTensor(TF_FLOAT, out_dims, 2, num_bytes_out));

  fprintf(stdout, "Running session...\n");
  TF_SessionOptions* sess_opts = TF_NewSessionOptions();
  TF_Session* session = TF_NewSession(graph, sess_opts, status);
  assert(TF_GetCode(status) == TF_OK);

  TF_SessionRun(session, nullptr,
                &inputs[0], &input_values[0], input_values.size(),
                &outputs[0], &output_values[0], output_values.size(),
                nullptr, 0, nullptr, status);

  TF_Code c = TF_GetCode(status);

  std::cout << c << std::endl;

  for(size_t i = 0; i < output_values.size(); ++i)
  {
      if (output_values.at(i) == nullptr)
      {
          std::cout << "bad parameters" << std::endl;
      }
      else
      {
          const auto data = static_cast<float*>(TF_TensorData(output_values.at(i)));
          std::cout << ((data[1] > 0.5f) ? true : false) << std::endl;
      }
  }

  fprintf(stdout, "Successfully run session\n");

  TF_CloseSession(session, status);
  TF_DeleteSession(session, status);
  TF_DeleteSessionOptions(sess_opts);
  TF_DeleteImportGraphDefOptions(graph_opts);
  TF_DeleteGraph(graph);
  TF_DeleteStatus(status);
  return 0;
}

TF_Buffer* read_file(const char* file) 
{
  FILE *f = fopen(file, "rb");
  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET);  //same as rewind(f);

  void* data = malloc(fsize);
  fread(data, fsize, 1, f);
  fclose(f);

  TF_Buffer* buf = TF_NewBuffer();
  buf->data = data;
  buf->length = fsize;
  buf->data_deallocator = free_buffer;
  return buf;
}
