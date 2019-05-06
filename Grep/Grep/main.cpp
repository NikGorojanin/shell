#include <sys/types.h>
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
#include "Grep.h"
#include <boost\filesystem.hpp>
#include <boost\filesystem\path.hpp>
#include <boost\filesystem\fstream.hpp>

using namespace std;

bool read_command_cpp(string& str_cmd){
	cout << endl << "Your command: ";
	getline(cin, str_cmd);

	return ((str_cmd).length() > 0);
}

//int main()
//{
//	string str_command;
//	Grep g = Grep();
//
//	if (read_command_cpp(str_command)) {
//		g.grep(str_command);
//	}
//	
//	system("pause");
//	return 0;
//}


int main(int argc, char** argv)
{
	string str_command;

	/*for (int i = 1; i<argc; i++) {
		str_command += argv[i];

		if (i<argc - 1)
			str_command += " ";
	}*/

	str_command = "grep \"*.txt\" D:\\grep\\grep";

	Grep g = Grep();

	g.grep(str_command);

	return 0;
}