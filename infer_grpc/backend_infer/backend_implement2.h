//
// Created by Bill Fu on 2020/5/26.
//

#ifndef BACKEND_INFER_BACKEND_IMPLEMENT_H_2
#define BACKEND_INFER_BACKEND_IMPLEMENT_H_2

#include <iostream>

#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>

using namespace std;

#include "backend_infer.grpc.pb.h"
#include "TFUtils.hpp"
#include "lpcnet.h"

using grpc::Status;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using std::chrono::system_clock;

// split the string of ipa id seq into a list of integer IDs.
void tokenize_ipa_seq(string const &ipa_id_seq_str,
                      vector<int> &out_ipa_id_seq);

class BackendInferImpl final : public backend_infer::Service
{
private:
    TFUtils         _tfu;
    LPCNetState*    _lpcnet;
    TF_Graph*       _graph;
    TF_Output       _inputs_op;
    TF_Output       _input_lengths_op;
    TF_Output       _out_op;
    string          _out_wav_dir;

public:
    BackendInferImpl();
    ~BackendInferImpl();

    // return true for success; otherwise false for failure
    bool initialize(const string& taco_model_file_name,
            const string& out_wav_dir, string &error_msg);

    Status commitJob(ServerContext* context,
                     const ::JobRequest* request, ::JobReply* response)
    override;

private:
    // return true for OK;
    // otherwise false for error happened, and error_msg contains the detail.
    bool infer(const vector<int>& ipa_id_list, const string& out_sr,
               const string& out_wave_file_name, string& error_msg);
};

#endif //BACKEND_INFER_BACKEND_IMPLEMENT_H_2
