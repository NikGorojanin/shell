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
//    while(not exit) {

        if (read_command_cpp(str_command)) {
            shell.execute_commands(str_command);
        }
//    }
    return 0;
}
