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
#include "TFUtils.hpp"
#include "lpcnet.h"
#include "arch.h"
#include "freq.h"

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
private:
    TFUtils         _tfu;
    LPCNetState*    _lpcnet;

public:
    BackendInferImpl(TFUtils& tfu, LPCNetState* lpcnet)
    {
        _tfu = tfu;
        _lpcnet = lpcnet;
    }

    Status commitJob(ServerContext* context,
            const ::JobRequest* request, ::JobReply* response)
    override
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
        else
        {
            cout << "number of ids: " << num_ids << endl;
        };



        response->set_isok(true);
        response->set_error_msg("OK");
        return Status::OK;
    }

private:
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

};

void RunServer(const std::string& server_address, int num_work_threads,
               TFUtils& tfu, LPCNetState* lpcnet)
{
    BackendInferImpl service(tfu, lpcnet);

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    grpc::ResourceQuota rq;
    rq.SetMaxThreads(num_work_threads);
    builder.SetResourceQuota(rq);

    unique_ptr<Server> server(builder.BuildAndStart());
    cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char** argv) {
    // Expect only arg: --db_path=path/to/route_guide_db.json.
    string server_address("0.0.0.0:50051");

    if (argc != 4)
    {
        cout << "Usage: ./backend_server <tacotron model file> <out_pcm_dir> <out_wav_dir>" << endl;
        return 1;
    }

    std::string taco_model_file_name = argv[1];
    string out_pcm_dir = argv[2];
    string out_wav_dir = argv[3];

    //string out_pcm_file_name = out_folder + "/" + sentence_id + ".pcm";
    //string out_wave_file_name =  out_folder + "/" + sentence_id + ".wav";;

    cout << "Tacotron Model File: " << taco_model_file_name << endl;
    cout << "out pcm dir: " << out_pcm_dir << endl;
    cout << "out wav dir: " << out_wav_dir << endl;

    // TFUtils init
    TFUtils tfu;
    TFUtils::STATUS status = tfu.LoadModel(taco_model_file_name);
    if (status != TFUtils::SUCCESS)
    {
        cout << "Can't load tacotron model from file: " << taco_model_file_name << endl;
        return 1;
    }
    else
    {
        cout << "Tacotron Model has been loaded Successfully!" << endl;
    }

    LPCNetState* lpcnet;
    lpcnet = lpcnet_create();

    int num_work_threads = 1;
    RunServer(server_address, num_work_threads, tfu, lpcnet);
    lpcnet_destroy(lpcnet);

    return 0;
}

