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
#include <boost/algorithm/algorithm.hpp>
#include <boost/filesystem.hpp>

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
    cout << "Test: \"ls && cat test.txt\" | Status: passed" << endl;

    str_command = "cat test.txt || ls";
    shell.execute(str_command, result);
    assert(result.success_result=="hello!");
    cout << "Test: \"cat test.txt || ls\" | Status: passed" << endl;

    str_command = "(ls asas && cd ../) || (ls && cat test.txt)";
    shell.execute(str_command, result);
    assert(result.success_result=="hello!");
    cout << "Test: \"(ls asas && cd ../) || (ls && cat test.txt)\" | Status: passed" << endl;

    str_command = "ls | sort | head -n 5 | head -n 4 | head -n 3 | sort -r | tail -n 1";
    shell.execute_piped_command(str_command, result);
    assert(result.success_result=="CMakeCache.txt\n");
    cout << "Test: \"ls | sort | head -n 5 | head -n 4 | head -n 3 | sort -r | tail -n 1\" | Status: passed" << endl;

    str_command = "(ls asas && cd ../) || cat test.txt > test2.txt";
    shell.execute(str_command, result);
    std::ifstream ifs ("test2.txt", std::ofstream::in);
    string data;
    ifs >> data;
    ifs.close();
    assert(data=="hello!");
    cout << "Test: \"(ls asas && cd ../) || cat test.txt > test2.txt\" | Status: passed" << endl;

    str_command = "/Users/nikitagorojanin/CLionProjects/shell/shell/cmake-build-debug/mygrep grep -c \"word\" grep_test.txt";
    shell.execute(str_command, result);
    assert(result.success_result=="1\n");
    cout << "Test: \"grep -c ...\" | Status: passed" << endl;

    str_command = "/Users/nikitagorojanin/CLionProjects/shell/shell/cmake-build-debug/mygrep grep -o \"word\" grep_test.txt";
    shell.execute(str_command, result);
    assert(result.success_result=="word\n");
    cout << "Test: \"grep -0 ...\" | Status: passed" << endl;

    str_command = "cp ar.zip ../";
    shell.execute_background(str_command, [](CmdExecResult& res){
        std::ofstream ofs ("test3.txt", std::ofstream::out | std::ofstream::app);
        ofs << "here: " << boost::filesystem::exists("../ar.zip");
        ofs.close();
    });

    cout << "All tests passed!";

    return 0;
}
