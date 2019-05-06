//
// Created by Nikita Gorojanin on 2019-04-25.
//

#include <algorithm>
#include "shell.h"
#include "utils.h"
#include "conditional.h"
#include <boost/algorithm/string.hpp>


//===== private ======

vector<string> Shell::parse_command_line(const string& command_line){
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

void Shell::parse_command_line_for_pipe(const string &command_line, vector<string>& commands) {
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


CmdExecResult Shell::execute_cmd(char** parsed, const string& output_type, const string& path){
    pid_t pid;
    int ed[2]; // дескрипторы для записи ошибок запускаемых команд
    int wd[2]; // дескрипторы для записи результата запускаемых команд

    pipe(ed);
    pipe(wd);

    switch (pid = fork())
    {
        case -1: {
            return CmdExecResult();
        }
        case 0: {

            redirect(ed[1], STDERR_FILENO); //редирект стандартного потока для вывода ошибок
            close(ed[0]);

            redirect(wd[1], STDOUT_FILENO);
            close(wd[0]);

            if (execvp(parsed[0], parsed) < 0) {
                cerr << "Invalid command";
            }
        }
        default: {
            int status;
            string error;
            string result;

            if (waitpid(pid, &status, 0) > 0) {
                redirect(ed[0], STDERR_FILENO);//редирект для считывания со стандартного потока ощибок
                close(ed[1]);

                redirect(wd[0], STDIN_FILENO);
                close(wd[1]);

                //Результат выполнения команды считывается в строку
                char result_buff;
                while(read(STDIN_FILENO, &result_buff, 1))
                    result += result_buff;

                if(!result.empty() && (output_type==">" || output_type==">>" || output_type=="1&>"))
                {
                    bool writing_status = Utils::write_to_file(path, result);

                    if (!writing_status)
                        cout << "Error: file does not exist"  << endl;
                }

                //Ошибка выполнения команды считывается в строку
                char error_buff;
                while(read(STDERR_FILENO, &error_buff, 1))
                    error += error_buff;

                if (!error.empty() && (output_type==">" || output_type==">>" || output_type=="2&>")) {
                    bool writing_status = Utils::write_to_file(path, error);

                    if (!writing_status)
                        cout << "Error: file does not exist"  << endl;
                }

                //WIFEXITED(status) говорит о статусе выполнения fork, error.empty() о наличии ошибок при выполнении команды
                return  CmdExecResult(result, error, (WIFEXITED(status) && error.empty()));
            } else {
                return CmdExecResult();
            }
        }
    }
}

bool Shell::validate_pipe_command(const string& cmd) {
    if(cmd.find(" | ")!=string::npos){
        for(const string& del : delimeters){
            if (cmd.find(del)!=string::npos && del!="|"){
                return false;
            }
        }
        return true;
    }
    return false;
}

CmdExecResult Shell::run_piped_cmd(vector<std::string>& pipe_chunks) {
    int savedStdout = dup(STDOUT_FILENO);
    int savedStdin = dup(STDIN_FILENO);

    string result, error;
    int ed[2]; // дескрипторы для записи ошибок запускаемых команд
    int wd[2]; // дескрипторы для записи результата запускаемых команд

    pipe(ed);
    pipe(wd);

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

            unique_ptr<char*[]> argv = Utils::to_cstring(pipe_chunks[i]);
            if (execvp(argv[0], argv.get())<0) {
                cerr << "Unrecognized command";
            }

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

        redirect(wd[1], STDOUT_FILENO);
        close(wd[0]);

        redirect(ed[1], STDERR_FILENO);
        close(ed[0]);

        unique_ptr<char*[]> argv = Utils::to_cstring(pipe_chunks.back());

        if (execvp(argv[0], argv.get())<0){
            cerr << "Unrecognized command";
        }

        exit(EXIT_FAILURE);
    } else {
        for (int i = 0; i < pipe_chunks.size(); ++i) m_child_time_total += timeChild();

        dup2(savedStdout, STDOUT_FILENO);
        dup2(savedStdin, STDIN_FILENO);

        redirect(ed[0], STDERR_FILENO);//редирект для считывания со стандартного потока ощибок
        close(ed[1]);
        redirect(wd[0], STDIN_FILENO);
        close(wd[1]);

        char result_buff;
        while(read(STDIN_FILENO, &result_buff, 1))
            result += result_buff;

        char error_buff;
        while(read(STDERR_FILENO, &error_buff, 1))
            error += error_buff;
    }

    return CmdExecResult(result, error, error.empty());
}

bool Shell::is_delimeter(const string &str){
    for(const string &del : delimeters){
        if (del==str)
            return true;
    }
    return false;
}

//====== public ======

void Shell::execute(const string &command_line) {
    vector<string> commands = parse_command_line(command_line);

    if (commands.empty()) {
        cerr << "Empty command";

        return;
    }

    CmdExecResult result;

    bool skip_command = false;
    bool output_branch_in_file = false;
    string output_type, output_path;

    for (int i = 0; i < commands.size(); i++) {
        if (commands[i] != "(") {
            if (skip_command) {
                if (is_delimeter(commands[i]) &&
                    ((result.success && commands[i] != "||") || (!result.success && commands[i] != "&&"))) {
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

            unique_ptr<char*[]> argv = Utils::to_cstring(commands[i]);

            if (output_branch_in_file) {
                result = execute_cmd(argv.get(), output_type, output_path);

                if (result.success)
                    cout << result.success_result;
                else
                    cout << result.error_result;
            } else {
                if (i < commands.size() - 2 &&
                    (commands[i + 1] == "1&>" || commands[i + 1] == "2&>" || commands[i + 1] == ">"
                     || commands[i + 1] == ">>")) {
                    result = execute_cmd(argv.get(), move(commands[i + 1]), move(commands[i + 2]));
                    i += 2;
                } else {
                    result = execute_cmd(argv.get());

                    if (result.success)
                        cout << result.success_result;
                    else
                        cout << result.error_result;
                }
            }

            if (i < commands.size() - 1) {
                if ((!result.success && commands[i + 1] == "&&") || (result.success && commands[i + 1] == "||"))
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

void Shell::execute(const string& command_line, CmdExecResult &result){
    vector<string> commands = parse_command_line(command_line);

    if (commands.empty()) {
        cerr << "Empty command";

        return;
    }

    CmdExecResult res = CmdExecResult();

    bool skip_command = false;
    bool output_branch_in_file = false;
    string output_type, output_path;

    for (int i = 0; i < commands.size(); i++) {
        if (commands[i] != "(") {
            if (skip_command) {
                if (is_delimeter(commands[i]) &&
                    ((res.success && commands[i] != "||") || (!res.success && commands[i] != "&&"))) {
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

            unique_ptr<char*[]> argv = Utils::to_cstring(commands[i]);

            if (output_branch_in_file) {
                res = execute_cmd(argv.get(), output_type, output_path);
            } else {
                if (i < commands.size() - 2 &&
                    (commands[i + 1] == "1&>" || commands[i + 1] == "2&>" || commands[i + 1] == ">"
                     || commands[i + 1] == ">>")) {
                    res = execute_cmd(argv.get(), move(commands[i + 1]), move(commands[i + 2]));
                    i += 2;
                } else {
                    res = execute_cmd(argv.get());
                }
            }

            if (i < commands.size() - 1) {
                if ((!res.success && commands[i + 1] == "&&") || (res.success && commands[i + 1] == "||"))
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
    result = res;
}

void Shell::execute_piped_command(string& command_line){
    if (validate_pipe_command(command_line)) {
        vector<string> commands;
        parse_command_line_for_pipe(command_line, commands);

        CmdExecResult result = run_piped_cmd(commands);

        if (result.success)
            cout << result.success_result << endl;
        else
            cout << result.error_result << endl;
    }
    else {
        cerr << "Command consist not only pipe delimeter";
    }
}

void Shell::execute_piped_command(string& command_line, CmdExecResult& result){
    if (validate_pipe_command(command_line)) {
        vector<string> commands;
        parse_command_line_for_pipe(command_line, commands);

        CmdExecResult res = run_piped_cmd(commands);

        result = res;
    }
    else {
        result = CmdExecResult("", "Command consist not only pipe delimeter", false);
    }
}

void Shell::execute_background(string& command_line, function<void(CmdExecResult&)> f){
    int pid = fork();

    if (!pid){
        CmdExecResult result;
        execute(command_line, result);
        f(result);
    }
    else{
        signal(SIGCHLD, Utils::sigChld);
    }
}

void Shell::execute_conditional(string& command_line) {
    map <string, string> vars;
    vector<string> passToShell;
    vector<string> lines;

    boost::split(lines, command_line, boost::is_any_of("\n"));

    Conditional c = Conditional();
    Conditions cType = c.checkType(command_line);

    vector<CmdExecResult> results;
    switch (cType)
    {
        case Conditions::IF_ELSE:
            c.findInit(lines, vars);
            results = c.parseIf(lines, vars, passToShell, this);
            break;
        case Conditions::WHILE:
            c.findInit(lines, vars);
            results = c.parseWhile(lines, vars, passToShell, this);
            break;
        case Conditions::FOR:
            results = c.parseFor(lines, vars, passToShell, this);

            break;
        default:
            break;
    }

    for(CmdExecResult& res : results){
        if (res.success){
            cout << res.success_result << endl;
        }
        else{
            cout << res.error_result << endl;
        }
    }
}

void Shell::execute_conditional(string& command_line, vector<CmdExecResult>& result) {
    map <string, string> vars;
    vector<string> passToShell;
    vector<string> lines;

    boost::split(lines, command_line, boost::is_any_of("\n"));

    Conditional c = Conditional();
    Conditions cType = c.checkType(command_line);

    vector<CmdExecResult> results;
    switch (cType)
    {
        case Conditions::IF_ELSE:
            c.findInit(lines, vars);
            results = c.parseIf(lines, vars, passToShell, this);
            break;
        case Conditions::WHILE:
            c.findInit(lines, vars);
            results = c.parseWhile(lines, vars, passToShell, this);
            break;
        case Conditions::FOR:
            results = c.parseFor(lines, vars, passToShell, this);

            break;
        default:
            break;
    }

    result = results;
}
