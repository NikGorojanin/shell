//
// Created by Nikita Gorojanin on 2019-04-28.
//
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <fstream>
#include "shell.h"
#include <boost/algorithm/algorithm.hpp>

#ifndef SHENN_UTILS_H
#define SHENN_UTILS_H


namespace Utils {
    int find_nearest_delim(string &str, vector<string> &delimeters, string &delim) {
        int pos = -1, cur_pos = 0;
        string nearest_delim;

        for (string &del : delimeters) {
            if ((cur_pos = str.find(del, 0)) != string::npos) {
                if (cur_pos < pos || pos == -1) {
                    pos = cur_pos;
                    nearest_delim = del;
                }
            }
        }
        delim = nearest_delim;

        return pos;
    }

    void trim(string &str) {
        int first_pos = -1;
        int last_pos = str.length();

        for (int i = 0; i < str.length(); i++) {
            if (str[i] != ' ') {
                if (first_pos == -1)
                    first_pos = i;

                last_pos = i;
            }
        }

        str = str.substr(first_pos, last_pos + 1);
    }

    vector<string> split(string& str, char delimiter) {
        vector<string> internal;
        stringstream ss(str);
        string tok;

        while(getline(ss, tok, delimiter)) {
            internal.push_back(tok);
        }

        return internal;
    }

    char** to_cstring(string& cmd){
        vector<string> input_args = split(cmd, ' ');

        char** argv = new char*[input_args.size() + 1];

        for (unsigned int i = 0; i < input_args.size(); ++i) {
            // Последним аргументом должен быть Null, поэтому + 1
            argv[i] = new char[input_args.at(i).length() + 1];
            strncpy(argv[i], input_args[i].c_str(), input_args[i].size() + 1);
        }
        // NULL необходим чтобы команда exec знала что дальше аргументов нет
        argv[input_args.size()] = nullptr;

        return argv;
    }

    bool file_exists(const string& filename) {
        ifstream f(filename.c_str());

        return f.good();
    }

    bool write_to_file(const string& filepath, const string& data){
        if (file_exists(filepath)){
            std::ofstream ofs (filepath, std::ofstream::out | std::ofstream::app);
            ofs << data;
            ofs.close();

            return true;
        }

        return false;
    }

    void sigChld(int arg) {
        wait(NULL);
    }
}

#endif //SHENN_UTILS_H
