//
// Created by Nikita Gorojanin on 2019-05-06.
//
#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include "shell.h"

#ifndef SHELL_CONDITIONAL_H
#define SHELL_CONDITIONAL_H

//#define STRING_T 0;
//#define NUMERIC_T 1;
//#define BOOLEAN_T 2;

enum class Tokens{
    STRING_T,
    NUMERIC_T,
    BOOLEAN_T,
    INVALID = -1
};
enum class Conditions {
    IF_ELSE,
    WHILE,
    FOR,
    INVALID = -1
};
//#define IF_ELSE 3;
//#define WHILE 4;
//#define FOR 5;

using namespace std;

class Conditional {

    static string& ltrim(string& str, const string& chars);

    static string& rtrim(string& str, const string& chars);

    static string& trim(string& str, const string& chars);

    static bool findReservedWords(string str);

    static bool isBoolean(string leftExpr, string rightExpr, map <string, string> vars, float &leftValue, float &rightValue);

    static bool isNumeric(string leftExpr, string rightExpr, map <string, string> vars, float &leftValue, float &rightValue);

    static bool parseCondition(string str, map <string, string> vars);

    static bool isDigit(string str);

    static int SemicolonPosition(string str);

public:
    Conditions checkType(const string& str) const;

    static void findInit(vector<string> str, map<string, string> &vars);

    static vector<CmdExecResult> parseIf(vector<string> str, map <string, string> vars, vector<string> &passToShell, Shell *shell);

    static vector<CmdExecResult> parseWhile(vector<string> str, map <string, string> vars, vector<string> &passToShell, Shell* shell);

    static vector<CmdExecResult> parseFor(vector<string> str, map <string, string> vars, vector<string> &passToShell, Shell* shell);

    Conditional(){};
};

#endif //SHELL_CONDITIONAL_H
