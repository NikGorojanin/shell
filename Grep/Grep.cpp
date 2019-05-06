//#include "../GrepUnitTest/stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <cmath>
#include <sstream>
#include <ctime>
#include <regex>
#include <clocale>
#include "Grep.h"

using namespace std; 


void Grep::parse(const string &commandLine, vector<string> &files, char &option, regex &pattern)
{
	char spc = ' ', dash = '-', quote = '"';
	char options[] = { 'n', 'c', 'o', 'r' , 'f'};
	
	string line = commandLine;
	size_t initialPos = 0;
	//search dash sign, option must follow the dah sign
	size_t dashPos = line.find(dash);

	int result = 1;

	if (dashPos != string::npos){
		option = line[dashPos + 1];
		result = 0;

		//search option
		for (int i = 0; i < sizeof(options); i++){
			if (options[i] == option)
				result = 1;
		}
	}

	//Wrong option
	if (result == 0){
		cerr << "wrong option" << endl;
	}

	line = line.substr(dashPos + 2, line.size());
	//search quotes? pattern must be in quotes
	size_t quotePos = line.find(quote);
	line = line.substr(quotePos + 1, line.size());
	quotePos = line.find(quote);

	//search pattern
	if (quotePos != string::npos){
		pattern = regex(line.substr(initialPos, quotePos));
	}

	size_t spcPos = line.find(spc);
	line = line.substr(spcPos + 1, line.size());
	spcPos = line.find(spc);
	files.clear();
	//search files 
	while (spcPos != string::npos) {
		files.push_back(line.substr(0, spcPos));
		initialPos = spcPos + 1;
		line = line.substr(initialPos, line.size());
		spcPos = line.find(spc);
	}

	files.push_back(line);

	return;
}

void Grep::PrintMatches(string str, regex reg){

	smatch matches;

	while (regex_search(str, matches, reg)){
		cout << matches.str() << endl;
		str = matches.suffix().str();
	}
}

void Grep::CountMatches(string str, regex reg, int &matchC){

	smatch matches;

	while (regex_search(str, matches, reg)){
		matchC++;
		str = matches.suffix().str();
	}

}

void Grep::PrintMatchesLineNum(string str, regex reg, int line){

	smatch matches;

	while (regex_search(str, matches, reg)){
		cout << line << ": " << matches.str() << endl;
		str = matches.suffix().str();
	}

}

int Grep::grep(string &command){

	namespace fs = boost::filesystem;
	
	vector<string> commandArgs;
	char option = NULL;
	regex pattern;
	vector<string> files;

	parse(command, files, option, pattern);

	string line;
	int matchC = 0, lineNum = 0, filesize = 0;
	filesize = files.size();

	for (int i = 0; i < filesize; i++){

		ifstream filename(files[i]);
		lineNum = 0;

		if (filename.is_open() || option == 'r'){
			switch (option){
			case 'o': //only-matching
			{
				while (getline(filename, line))
				{
					PrintMatches(line, pattern);
				}
				break;
			}
			case 'c': //count of matching lines
			{
				while (getline(filename, line))
				{
					CountMatches(line, pattern, matchC);
				}
				break;
			}
			case 'n': //line-number
			{
				while (getline(filename, line))
				{
					lineNum++;
					PrintMatchesLineNum(line, pattern, lineNum);
				}
				break;
			}
			case 'r': //recursive
			{
				fs::path p{ files[i] };
				fs::recursive_directory_iterator it{ p };

				for (const fs::directory_entry &x : fs::recursive_directory_iterator(p)){
					string file = x.path().string();
					ifstream filename(file);
					while (getline(filename, line))
					{
						PrintMatches(line, pattern);
					}
				}

				break;
			}
			case 'f': //find files
			{
				fs::path p{ files[i] };
				fs::recursive_directory_iterator it{ p };

				for (const fs::directory_entry &x : fs::recursive_directory_iterator(p)){
					string file = x.path().string();
					PrintMatches(file, pattern);
				}

				break;
			}
			default:
			{
				while (getline(filename, line))
				{
					PrintMatches(line, pattern);
				}
				break;
			}
				filename.close();
			}

		}
		else if (!filename.is_open()) {
			return -1;
		}

	}
	//prints total matchs number after loop finish
	if (option == 'c'){
		cout << matchC << endl;
	}


	return 0;
}