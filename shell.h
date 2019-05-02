//
// Created by Nikita Gorojanin on 2019-04-25.
//
#include <string.h>
#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <unistd.h>
#include <signal.h>

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
    vector<string> parse_command_line(const string& command_line);
    void parse_command_line_for_pipe(const string &command_line, vector<string>& commands);
    void redirect(int oldfd, int newfd);
    bool is_delimeter(const string& str);
    bool validate_pipe_command(const string& cmd);

    Duration timeChild() const;

    CmdExecResult execute_cmd(char** parsed, const string& output_type="", const string& path="");
    CmdExecResult run_piped_cmd(vector<std::string>& commands);
public:
    Shell() : m_child_time_total(0){};

    void execute(const string& command_line);
    void execute(const string& command_line, CmdExecResult& result);

    void execute_background(string& command_line, function<void(CmdExecResult&)> f);

    void execute_piped_command(string& command_line);
    void execute_piped_command(string& command_line, CmdExecResult& result);
};


#endif //SHENN_SHELL_H
