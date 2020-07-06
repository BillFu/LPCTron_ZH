//
// Created by Bill Fu on 2020/7/6.
//

#ifndef BACKEND_BACKENDIMP_H
#define BACKEND_BACKENDIMP_H

#include <iostream>
#include <vector>
#include <string>

#include "TFUtils.hpp"
#include "lpcnet.h"

using namespace std;

// split the string of ipa id seq into a list of integer IDs.
void tokenize_ipa_seq(string const &ipa_id_seq_str,
                      vector<int> &out_ipa_id_seq);

class BackendImp {

private:
    TFUtils         _tfu;
    LPCNetState*    _lpcnet;
    TF_Graph*       _graph;
    TF_Output       _inputs_op;
    TF_Output       _input_lengths_op;
    TF_Output       _out_op;
    string          _out_wav_dir;

public:
    BackendImp();
    ~BackendImp();

    // return true for success; otherwise false for failure
    bool initialize(const string& taco_model_file_name,
                    const string& out_wav_dir, string &error_msg);

    // return a boolean value, isOK
    // another return variable is error_msg
    bool commitJob(const string& sentence_id, int sr,
            const string& ipa_id_seq_str,  string& error_msg);

private:
    // return true for OK;
    // otherwise false for error happened, and error_msg contains the detail.
    bool infer(const vector<int>& ipa_id_list, const int out_sr,
               const string& out_wave_file_name, string& error_msg);
};


#endif //BACKEND_BACKENDIMP_H
