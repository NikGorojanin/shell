#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include "Conditions.h"

using namespace std;

bool read_command_cpp(vector<string> &vec_loop, int type){

	string str;
	getline(cin, str);
	// если команда введенная с cmd имеет такой вид
	// sh -e filename, то запускаем с файла
	if (str.substr(0, 5) == "sh -e"){
		string filename = str.substr(6, str.size() - 6);
		ifstream file(filename);
		while (getline(file, str)){
			vec_loop.push_back(str);
			if (type == -1){
				type = Conditions::checkType(str);
			}
		}
	}
	//иначе считываем со строки
	else{
		vec_loop.push_back(str);
		while (str != "done" && str != "fi"){
			getline(cin, str);
			vec_loop.push_back(str);
			if (type == -1){
				type = Conditions::checkType(str);
			}
		}
	}
	
	return (vec_loop.size() > 0);
}

int main(int argc, char** argv){

	string str_command;

	for (int i = 1; i<argc; i++) {
		str_command += argv[i];

		if (i<argc - 1)
			str_command += " ";
	}

//	str_command = "for((c = 1; c < 4; c++)) \n do \n ls \n cat test.txt \n done";
	str_command = "c = a \n b = a \n if ($c == $b) \n then \n ls \n cat test.txt \n fi";
	Conditions c = Conditions();
	c.Start(str_command);

	return 0;
}