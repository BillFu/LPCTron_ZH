//
// Created by Bill Fu on 2020/5/21.
//

#include <iostream>
#include <vector>

#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/security/server_credentials.h>

#include "backend_imp3.h"

using namespace std;
using grpc::ServerBuilder;
using grpc::Status;


void RunServer(BackendImp3& service,
        const std::string& server_address, int num_work_threads)
{
    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    /*
    grpc::ResourceQuota rq;
    rq.SetMaxThreads(num_work_threads);
    builder.SetResourceQuota(rq);
    */

    unique_ptr<Server> server(builder.BuildAndStart());
    cout << "*****An Instance of LPCTron TTS Backend Server listening on "
        << server_address << "*****" << std::endl;
    server->Wait();
}

int main(int argc, char** argv) {
    // Expect only arg: --db_path=path/to/route_guide_db.json.
    string server_address("0.0.0.0:50051");

    if (argc != 3)
    {
        cout << "Usage: ./backend_server <tacotron model file> <out_wav_dir>" << endl;
        return 1;
    }

    std::string taco_model_file_name = argv[1];
    string out_wav_dir = argv[2];

    cout << "Tacotron Model File: " << taco_model_file_name << endl;
    cout << "out wav dir: " << out_wav_dir << endl;

    BackendImpl3 service;

    string error_msg;
    bool is_ok = service.initialize(taco_model_file_name,
            out_wav_dir, error_msg);
    if(!is_ok)
    {
        cout << "Failed to initialize the Infer Service: " << error_msg << endl;
        return 1;
    }
    else
    {
        cout << "The Inference Service has been initialized successfully." << endl;
    }

    int num_work_threads = 1;
    RunServer(service, server_address, num_work_threads);

    return 0;
}

