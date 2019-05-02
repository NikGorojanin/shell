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
#include <streambuf>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include <array>
#include <sstream>
#include <fstream>
#include <assert.h>
#include "shell.h"

using namespace std;

bool read_command_cpp(string& str_cmd){
    cout << endl << "Your command: ";
    getline(cin, str_cmd);

    return ((str_cmd).length() > 0);
}

int main()
{
    string str_command;
    Shell shell = Shell();

    CmdExecResult result;

    str_command = "ls && cat test.txt";
    shell.execute(str_command, result);
    assert(result.success_result=="hello!");

    str_command = "cat test.txt || ls";
    shell.execute(str_command, result);
    assert(result.success_result=="hello!");

    str_command = "(ls asas && cd ../) || (ls && cat test.txt)";
    shell.execute(str_command, result);
    assert(result.success_result=="hello!");

    str_command = "(ls asas && cd ../) || cat test.txt > test2.txt";
    shell.execute(str_command, result);

    std::ifstream ifs ("test2.txt", std::ofstream::in);
    string data;
    ifs >> data;
    ifs.close();

    assert(data=="hello!");

    str_command = "ls | sort | head -n 5 | head -n 4 | head -n 3 | sort -r | tail -n 1";
    shell.execute_piped_command(str_command, result);
    assert(result.success_result=="CMakeCache.txt\n");

    str_command = "cp copy.txt ../";
    shell.execute_background(str_command, [](CmdExecResult& res){
        std::ofstream ofs ("test3.txt", std::ofstream::out | std::ofstream::app);
        ofs << "here";
        ofs.close();
    });

    shell.execute("ls");
    for (int i=0;i<1000000000;i++){}

    cout << "Tests passed!";

    return 0;
}
