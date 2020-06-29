//
// Created by Bill Fu on 2020/6/29.
//

#include "backend_imp3.h"
#include "wav_utils_v2.h"

using grpc::Status;


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

BackendImpl3::BackendImpl3()
{
    _lpcnet = NULL;
}

BackendImpl3::~BackendImpl3()
{
    if(_lpcnet != NULL)
    {
        lpcnet_destroy(_lpcnet);
        _lpcnet = NULL;
    }
}
