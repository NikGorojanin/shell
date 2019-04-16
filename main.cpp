#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>
#include <signal.h>

using namespace std;

#define MAXLIST 100

void sigChld(int arg) {
    int pid, status;
    waitpid(pid, &status, 0);
}


bool parse_space(char* str, char** parsed, bool *background)
{
    char *last_symbol = "";
    int i = 0;

    for (i = 0; i < MAXLIST; i++)
    {
        parsed[i] = strsep(&str, " ");

        if (parsed[i] == nullptr)
            break;

        if (strlen(parsed[i]) == 0)
            i--;
        else
            last_symbol = parsed[i];
    }

    if (last_symbol != "" && last_symbol[0] == '&') {
        parsed[i-1] = nullptr;
        *background = true;
    }
}

bool execute_cmd(char** parsed)
{
    pid_t pid;

    switch (pid = fork())
    {
        case -1:
            cerr << "Error in fork()";

            return false;
        case 0:
            if (execvp(parsed[0], parsed) < 0)
                cerr << "Could not execute command";

            return false;
        default:
            int status;
            if (waitpid(pid, &status, 0) > 0) {
                if (WIFEXITED(status))
                {
                    return true;
                }
                else{
                    cerr << "Child process terminated with error";

                    return false;
                }
            }
            else {
                cerr << "waitpid failed";

                return false;
            }
    }
}


void execute_upipe(vector<char*> pipe_commands){
    u_long command_cnt = pipe_commands.size();
    bool background;
    int i;
    int* fd = new int[2*command_cnt-2];

    for (i=0; i<command_cnt-1; i++) {
        pipe(fd+2*i);
    }

    for (i=0; i<command_cnt; i++) {
        char* cur_command = pipe_commands[i];

        char *parsed[MAXLIST];
        parse_space(cur_command, parsed, &background);

        int pid;
        switch(pid=fork()) {
            case -1:
                cerr << "Error in fork()";
                return;
            case 0:
                if (i == 0) {
                    close(STDOUT_FILENO);
                    dup2(fd[i*2+1], STDOUT_FILENO);
                    close(fd[i*2]);
                    close(fd[i*2+1]);
                    execvp(parsed[0], parsed);
                }
                else {
                    if (i == command_cnt-1) {
                        close(STDIN_FILENO);
                        dup2(fd[2*(i-1)], STDIN_FILENO);

                        int k;
                        for (k=0;k<2*command_cnt-2;k++) {
                            close(fd[k]);
                        }
                        execvp(parsed[0], parsed);
                    }
                    else {
                        close(STDIN_FILENO);
                        dup2(fd[2*(i-1)], STDIN_FILENO);
                        close(fd[2*(i-1)]);
                        close(fd[2*(i-1)+1]);

                        close(STDOUT_FILENO);
                        dup2(fd[2*i+1], STDOUT_FILENO);

                        close(fd[2*i]);
                        close(fd[2*i+1]);
                        execvp(parsed[0], parsed);
                    }
                }
            default:
                if (i == command_cnt-1) {
                    int k;
                    for (k=0;k<2*command_cnt-2;k++) {
                        close(fd[k]);
                    }
                }

                wait(nullptr);
                break;
        }
    }
}


bool execute_background(char** parsed){
    pid_t pid;

    switch (pid = fork())
    {
        case -1:
            cerr << "Error in fork()";

            return false;
        case 0:
            if (execvp(parsed[0], parsed) < 0)
                cerr << "Could not execute command";

            return false;
        default:
            signal(SIGCHLD, sigChld);
    }
}


bool read_command_cpp(string *str_cmd){
    cout << endl << "Your command: ";

    getline(cin, *str_cmd);

    return ((*str_cmd).length() > 0);
}


void split_by_delimiter(vector<char*> *commands, string str_command, string delim) {
    u_long new_delimiter_position = 0;
    u_long last_delimiter_position = 0;
    bool delimiter_not_founded = false;

    while(!delimiter_not_founded){
        new_delimiter_position = str_command.find(delim, new_delimiter_position);

        if (new_delimiter_position != -1) {
            string command = str_command.substr(last_delimiter_position, new_delimiter_position - last_delimiter_position);

            char * ccommand = new char[command.size()+1];
            copy(command.begin(), command.end(), ccommand);
            ccommand[command.size()] = '\0';
            (*commands).push_back(ccommand);

            new_delimiter_position += delim.size();
            last_delimiter_position = new_delimiter_position;
        }
        else {
            string command = str_command.substr(last_delimiter_position);

            char * ccommand = new char[command.size()+1];
            copy(command.begin(), command.end(), ccommand);
            ccommand[command.size()] = '\0';
            (*commands).push_back(ccommand);

            delimiter_not_founded = true;
        }
    }
}


string define_delimeter(string command){
    if (command.find("&&") != -1){
        return "&&";
    }
    else{
        if (command.find("||") != -1){
            return "||";
        }
        else{
            if (command.find(",") != -1){
                return ",";
            }
            else {
                if (command.find("|") != -1) {
                    return "|";
                }
                else {
                    return "";
                }
            }
        }
    }
}

vector<string> commands_storage;
int main()
{
    signal(SIGCHLD, sigChld);
    string str_command;

    bool exit = false;

    while(not exit) {
        if (read_command_cpp(&str_command)) {

            if (str_command != "exit()") {

                commands_storage.push_back(str_command);

                string delimeter = define_delimeter(str_command);

                vector<char*> commands;

                if (delimeter != "") {
                    split_by_delimiter(&commands, str_command, delimeter);
                }
                else{
                    char * ccommand = new char[str_command.size()+1];
                    copy(str_command.begin(), str_command.end(), ccommand);
                    ccommand[str_command.size()] = '\0';

                    commands.push_back(ccommand);
                }

                if (delimeter != "|") {
                    for (unsigned long i = 0; i < commands.size(); i++) {

                        char *parsed[MAXLIST];

                        bool background;
                        parse_space(commands.at(i), parsed, &background);

                        if (!background) {
                            bool result;
                            result = execute_cmd(parsed);

                            if ((delimeter == "&&" && !result) || (delimeter == "||" && result)) {
                                break;
                            }
                        }
                        else {
                            execute_background(parsed);
                        }
                    }
                }
                else {
                    execute_upipe(commands);
                }
            }
            else
                exit = true;
        }
    }
    return 0;
}
