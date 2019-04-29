//
// Created by Nikita Gorojanin on 2019-04-25.
//

#include <algorithm>
#include "shell.h"
#include "utils.h"


//==== private ======

vector<string> Shell::parse_command_line(string& command_line){
    vector<string> command_line_members;
    string cur_object;

    int delim_pos=0;
    string delimeter;
    string str = command_line;

    while(true){
        delim_pos = Utils::find_nearest_delim(str, delimeters, delimeter);

        if (delim_pos == -1) {
            Utils::trim(str);
            command_line_members.push_back(str);

            break;
        }

        if (!delim_pos) {
            command_line_members.push_back(delimeter);
        }
        else {

            string command;
            command = str.substr(0, delim_pos);
            Utils::trim(command);

            command_line_members.push_back(command);
            command_line_members.push_back(delimeter);
        }

        if (delim_pos==str.length()-1)
            break;

        str =  str.substr(delim_pos + delimeter.length(), str.length()+1);
        Utils::trim(str);
    }

    return command_line_members;
}

void Shell::parse_command_line_for_pipe(string &command_line, vector<string>& commands) {
    u_long new_delimiter_position = 0;
    u_long last_delimiter_position = 0;
    bool delimiter_not_founded = false;

    while(!delimiter_not_founded){
        new_delimiter_position = command_line.find(" | ", new_delimiter_position);

        if (new_delimiter_position != -1) {
            string command = command_line.substr(last_delimiter_position, new_delimiter_position - last_delimiter_position);
            commands.push_back(command);

            new_delimiter_position += 3;
            last_delimiter_position = new_delimiter_position;
        }
        else {
            string command = command_line.substr(last_delimiter_position);

            commands.push_back(command);

            delimiter_not_founded = true;
        }
    }
}

void Shell::redirect(int oldfd, int newfd) {
    if (oldfd != newfd) {
        dup2(oldfd, newfd);
        close(oldfd);
    }
}

Duration Shell::timeChild() const {
    Time_Point start = High_Res_Clock::now();
    wait(NULL);
    Time_Point end = High_Res_Clock::now();
    return end - start;
}

//bool Shell::execute_cmd(char** parsed)
//{
//    pid_t pid;
//    int ed[2]; // дескрипторы для записи ошибок запускаемых команд
//
//    pipe(ed);
//
//    switch (pid = fork())
//    {
//        case -1: {
//            return false;
//        }
//        case 0: {
//
//            redirect(ed[1], STDERR_FILENO); //редирект стандартного потока для вывода ошибок
//            close(ed[0]);
//
//            if (execvp(parsed[0], parsed) < 0) {
//                return false;
//            }
//        }
//        default: {
//            int status;
//
//            if (waitpid(pid, &status, 0) > 0) {
//
//                redirect(ed[0], STDERR_FILENO);//редирект для считывания со стандартного потока ощибок
//                close(ed[1]);
//
//                //Ошибка выполнения команды считывается в строку
//                char error_buff;
//                string error;
//                while(read(STDERR_FILENO, &error_buff, 1))
//                    error += error_buff;
//
//                if (!error.empty())
//                    cout << "Error: " << error << endl;
//
//                //WIFEXITED(status) говорит о статусе выполнения fork, error.empty() о наличии ошибок при выполнении команды
//                return WIFEXITED(status) && error.empty();
//            } else {
//                return false;
//            }
//        }
//    }
//}

bool Shell::execute_cmd(char** parsed, string output_type, string path){
    pid_t pid;
    int ed[2]; // дескрипторы для записи ошибок запускаемых команд
    int wd[2]; // дескрипторы для записи результата запускаемых команд

    pipe(ed);
    pipe(wd);

    switch (pid = fork())
    {
        case -1: {
            return false;
        }
        case 0: {

            redirect(ed[1], STDERR_FILENO); //редирект стандартного потока для вывода ошибок
            close(ed[0]);

            if (output_type==">" || output_type==">>" || output_type=="1&>"){
                redirect(wd[1], STDOUT_FILENO);
                close(wd[0]);
            }

            if (execvp(parsed[0], parsed) < 0) {
                return false;
            }
        }
        default: {
            int status;

            if (waitpid(pid, &status, 0) > 0) {

                redirect(ed[0], STDERR_FILENO);//редирект для считывания со стандартного потока ощибок
                close(ed[1]);

                if (output_type==">" || output_type==">>" || output_type=="1&>"){
                    redirect(wd[0], STDIN_FILENO);
                    close(wd[1]);

                    //Результат выполнения команды считывается в строку
                    char result_buff;
                    string result;
                    while(read(STDIN_FILENO, &result_buff, 1))
                        result += result_buff;

                    if(!result.empty())
                    {
                        bool writing_status = Utils::write_to_file(path, result);

                        if (!writing_status)
                            cout << "Error: file does not exist"  << endl;
                    }
                }

                //Ошибка выполнения команды считывается в строку
                char error_buff;
                string error;
                while(read(STDERR_FILENO, &error_buff, 1))
                    error += error_buff;

                if (!error.empty()) {
                    if (output_type==">" || output_type==">>" || output_type=="2&>") {
                        bool writing_status = Utils::write_to_file(path, error);

                        if (!writing_status)
                            cout << "Error: file does not exist"  << endl;
                    }
                    else {
                        cout << "Error: " << error << endl;
                    }
                }

                //WIFEXITED(status) говорит о статусе выполнения fork, error.empty() о наличии ошибок при выполнении команды
                return WIFEXITED(status) && error.empty();
            } else {
                return false;
            }
        }
    }
}

bool Shell::check_command(string& cmd, bool& piped) {
    if(cmd.find(" | ")!=string::npos){
        for(string& del : delimeters){
            if (cmd.find(del)!=string::npos && del!="|"){
                return false;
            }
        }
        piped = true;

        return true;
    }
    piped = false;

    return true;
}

void Shell::exec_cmd(string &command) {
    char **argv = Utils::to_cstring(command);
    execvp(argv[0], argv);
    // protect against fork bomb
    exit(EXIT_FAILURE);
}

bool Shell::run_piped_cmd(vector<std::string>& pipe_chunks) {
    int savedStdout = dup(STDOUT_FILENO);
    int savedStdin = dup(STDIN_FILENO);

    int in = STDIN_FILENO;
    for (int i = 0; i < pipe_chunks.size() - 1; ++i) {
        int fd[2];
        pid_t pid;

        int res = pipe(fd);
        pid = fork();
        if (pid == 0) {
            close(fd[0]);
            redirect(in, STDIN_FILENO);
            redirect(fd[1], STDOUT_FILENO);
            exec_cmd(pipe_chunks[i]);
            exit(EXIT_FAILURE);
        } else {
            close(fd[1]);
            close(in);
            in = fd[0];
            m_child_time_total += timeChild();
        }
    }
    if (fork() == 0) {

        redirect(in, STDIN_FILENO);
        exec_cmd(pipe_chunks.back());
        exit(EXIT_FAILURE);
    } else {
        for (int i = 0; i < pipe_chunks.size(); ++i) m_child_time_total += timeChild();

        dup2(savedStdout, STDOUT_FILENO);
        dup2(savedStdin, STDIN_FILENO);
    }
}

bool Shell::is_delimeter(string &str){
    for(string &del : delimeters){
        if (del==str)
            return true;
    }
    return false;
}

//====== public ======

void Shell::execute_commands(string &command_line) {
    bool piped;
    bool correct = check_command(command_line, piped);

    if (!correct){
        cout << "Команада с пайп не должна модержать других разделителей";

        return;
    }

    if(piped){
        vector<string> commands;
        parse_command_line_for_pipe(command_line, commands);

        run_piped_cmd(commands);
    }
    else {
        vector<string> commands = parse_command_line(command_line);

        if (commands.empty()) {
            cerr << "Empty command";

            return;
        }

        bool skip_command = false;
        bool status;
        bool output_branch_in_file = false;
        string output_type, output_path;

        for (int i = 0; i < commands.size(); i++) {
            if (commands[i] != "(") {

                if (skip_command) {
                    if (is_delimeter(commands[i]) &&
                        ((status && commands[i] != "||") || (!status && commands[i] != "&&"))) {
                        if ((commands[i] != "1&>" || commands[i] != "2&>" || commands[i] != ">" ||
                             commands[i] != ">>")) {
                            skip_command = false;
                            continue;
                        } else
                            continue;
                    } else
                        continue;
                }

                if (is_delimeter(commands[i])) {
                    if (commands[i] == ")" && output_branch_in_file)
                        output_branch_in_file = false;
                    continue;
                }

                char **argv = Utils::to_cstring(commands[i]);

                if (output_branch_in_file) {
                    status = execute_cmd(argv, output_type, output_path);
                } else {
                    if (i < commands.size() - 2 &&
                        (commands[i + 1] == "1&>" || commands[i + 1] == "2&>" || commands[i + 1] == ">"
                         || commands[i + 1] == ">>")) {
                        status = execute_cmd(argv, move(commands[i + 1]), move(commands[i + 2]));
                        i += 2;
                    } else
                        status = execute_cmd(argv);
                }

                if (i < commands.size() - 1) {
                    if ((!status && commands[i + 1] == "&&") || (status && commands[i + 1] == "||"))
                        skip_command = true;
                }
            } else {
                if (skip_command) {
                    int open_branch_cnt = 1;
                    int j;

                    for (j = i + 1; j < commands.size(); j++) {
                        if (commands[j] == ")") {
                            open_branch_cnt--;
                            if (open_branch_cnt == 0)
                                break;
                        } else if (commands[j] == "(") {
                            open_branch_cnt++;
                        }
                    }
                    i = j;
                } else {
                    int open_branch_cnt = 1;
                    int j;

                    for (j = i + 1; j < commands.size(); j++) {
                        if (commands[j] == ")") {
                            open_branch_cnt--;
                            if (open_branch_cnt == 0)
                                break;
                        } else if (commands[j] == "(") {
                            open_branch_cnt++;
                        }
                    }

                    if (j < commands.size() - 2 &&
                        (commands[j + 1] == "1&>" || commands[j + 1] == "2&>" || commands[j + 1] == ">"
                         || commands[j + 1] == ">>")) {
                        output_branch_in_file = true;
                        output_type = commands[j + 1];
                        output_path = commands[j + 2];
                    }
                }
            }
        }
    }
}
