//
// Created by Bill Fu on 2020/5/21.
//

#include <iostream>
#include <vector>

#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>

#include "backend_infer.grpc.pb.h"

using namespace std;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using std::chrono::system_clock;

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

class BackendInferImpl final : public backend_infer::Service
{
    Status commitJob(ServerContext* context,
            const ::JobRequest* request, ::JobReply* response)
    override
    {
        // feature->set_name(GetFeatureName(*point, feature_list_));
        // feature->mutable_location()->CopyFrom(*point);
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
        else
        {
            cout << "number of ids: " << num_ids << endl;
        };

        response->set_isok(true);
        response->set_error_msg("OK");
        return Status::OK;
    }
};

void RunServer(const std::string& server_address)
{
    BackendInferImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    unique_ptr<Server> server(builder.BuildAndStart());
    cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char** argv) {
    // Expect only arg: --db_path=path/to/route_guide_db.json.
    string server_address("0.0.0.0:50051");

    RunServer(server_address);

    return 0;
}

