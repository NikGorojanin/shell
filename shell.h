//
// Created by Nikita Gorojanin on 2019-04-25.
//
#include <string.h>
#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <unistd.h>

#ifndef SHENN_SHELL_H
#define SHENN_SHELL_H

using namespace std;

using Duration = std::chrono::duration<double>;
using High_Res_Clock = std::chrono::high_resolution_clock;
using Time_Point = std::chrono::time_point<High_Res_Clock>;

struct CmdExecResult {
    string success_result;
    string error_result;
    bool success=false;

    CmdExecResult()
            : success_result(""), error_result(""), success(false){};

    CmdExecResult(string success_result, string error_result, bool success=false)
            : success_result(move(success_result)), error_result(move(error_result)), success(success){};

};

class Shell {
    vector<string> delimeters = {"||", "|", "&&", ";", "1&>", "2&>", ">>", ">", "(", ")", "&"};

    Duration m_child_time_total;
    vector<string> parse_command_line(string& command_line);
    void parse_command_line_for_pipe(string &command_line, vector<string>& commands);
    void redirect(int oldfd, int newfd);
    bool is_delimeter(string& str);
    bool check_command(string& cmd, bool& piped);
    Duration timeChild() const;

    bool execute_cmd(char** parsed, string output_type="", string path="");
    void exec_cmd(string &input_args);
    bool run_piped_cmd(vector<std::string>& commands);
public:
    Shell(){};

    void execute_commands(string& command_line);

};


#endif //SHENN_SHELL_H
