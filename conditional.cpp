#include <vector>
#include <ctime>
#include <cmath>
#include <sstream>
#include <ctime>
#include <regex>
#include <clocale>
#include "conditional.h"

using namespace std;

Conditions Conditional::checkType(const string &str) const {

    if (str.find("if") != string::npos){
        return Conditions::IF_ELSE;
    }
    else if (str.find("while") != string::npos){
        return Conditions::WHILE;
    }
    else if (str.find("for") != string::npos){
        return Conditions::FOR;
    }
    else{
        return Conditions::INVALID;
    }
}

string& Conditional::ltrim(string& str, const string& chars = "\t\n\v\f\r ")
{
    str.erase(0, str.find_first_not_of(chars));
    return str;
}

string& Conditional::rtrim(string& str, const string& chars = "\t\n\v\f\r ")
{
    str.erase(str.find_last_not_of(chars) + 1);
    return str;
}

string& Conditional::trim(string& str, const string& chars = "\t\n\v\f\r ")
{
    return Conditional::ltrim(Conditional::rtrim(str, chars), chars);
}


bool Conditional::findReservedWords(string str)
{
    string reservedWords[] = { "if", "for", "while" };

    for (int i = 0; i < 3; i++)
    {
        if (str.find(reservedWords[i]) != string::npos)
        {
            return true;
        }
    }

    return false;
}


void Conditional::findInit(vector<string> str, map <string, string> &vars)
{
    for (int i = 0; i < str.size(); i++)
    {
        if (!Conditional::findReservedWords(str[i]))
        {
            string before_equal = str[i].substr(0, str[i].find('='));
            string after_equal = str[i].substr(str[i].find('=') + 1);

            vars.insert(make_pair(Conditional::trim(before_equal),
                    Conditional::trim(after_equal)));
        }
        else
        {
            break;
        }
    }
}


bool Conditional::isBoolean(string leftExpr, string rightExpr, map <string, string> vars, float &leftValue, float &rightValue)
{
    string left, right, var;

    if (leftExpr[0] != '$')
    {
        left = leftExpr;
    }
    else
    {
        var = leftExpr.substr(1, leftExpr.size() - 1);
        left = vars[var];
    }

    if (rightExpr[0] != '$')
    {
        right = rightExpr;
    }
    else
    {
        var = rightExpr.substr(1, rightExpr.size() - 1);
        right = vars[var];
    }

    if ((left == "true" || left == "1" || left == "false" || left == "0") &&
        (right == "true" || right == "1" || right == "false" || right == "0"))
    {
        leftValue = left == "true" || left == "1" ? 1 : 0;
        rightValue = right == "true" || right == "1" ? 1 : 0;
        return true;
    }

    return false;
}

bool Conditional::isDigit(string str){
    locale loc;
    //TO DO: check point to float
    for (int i = 0; i < str.size(); i++){
        if (!isdigit(str[i], loc)){
            return false;
        }
    }
    return true;
}

bool Conditional::isNumeric(string leftExpr, string rightExpr, map <string, string> vars, float &leftValue, float &rightValue)
{
    string left, right, var;

    if (leftExpr[0] != '$' && ((leftExpr[0] == '-' && isDigit(leftExpr.substr(1, leftExpr.size() - 1))) || isDigit(leftExpr)))
    {
        left = leftExpr;
    }
    else if (leftExpr[0] == '$')
    {
        var = leftExpr.substr(1, leftExpr.size() - 1);
        left = vars[var];
    }
    else
    {
        var = leftExpr;
        left = vars[var];
    }

    if ((rightExpr[0] != '$' && (rightExpr[0] == '-' && isDigit(rightExpr.substr(1, rightExpr.size() - 1)))) || isDigit(rightExpr))
    {
        right = rightExpr;
    }
    else if (rightExpr[0] == '$')
    {
        var = rightExpr.substr(1, rightExpr.size() - 1);
        right = vars[var];
    }
    else
    {
        var = rightExpr;
        right = vars[var];
    }

    if (((left[0] == '-' && isDigit(left.substr(1, left.size() - 1))) || isDigit(left)) &&
        ((right[0] == '-' && isDigit(right.substr(1, right.size() - 1))) || isDigit(right)))
    {
        if (left.find(".") != string::npos)
        {
            leftValue = stof(left);
        }
        else
        {
            leftValue = (float)stoi(left);
        }

        if (right.find(".") != string::npos)
        {
            rightValue = stof(right);
        }
        else
        {
            rightValue = (float)stoi(right);
        }

        return true;
    }
    /*else if (((left[0] == '-' && isDigit(left.substr(1, left.size() - 1))) || isDigit(left)) &&
        ((right[0] == '-' && isDigit(right.substr(1, left.size() - 1))) || isDigit(right))){

    }*/

    return false;
}

bool Conditional::parseCondition(string str, map <string, string> vars)
{
    string boolSigns[] = { "<=", ">=", "<", ">", "==", "!=",
                           "-ge", "-le", "-gt", "-lt", "-eq", "-ne" };
    string sign;
    string leftExpr;
    string rightExpr;
    string var, left, right;
    float leftNumValue;
    float rightNumValue;
    Tokens type = Tokens::INVALID;

    for (int i = 0; i < 12; i++)
    {
        if (str.find(boolSigns[i]) != string::npos)
        {
            sign = boolSigns[i];
            string before_sign = str.substr(0, str.find(sign));
            string after_sign = str.substr(str.find(sign) + sign.size(), str.size() - str.find(boolSigns[i]) - sign.size());
            leftExpr = trim(before_sign);
            rightExpr = trim(after_sign);

            break;
        }
    }


    if (isNumeric(leftExpr, rightExpr, vars, leftNumValue, rightNumValue))
    {
        type = Tokens::NUMERIC_T;
    }
    else if (isBoolean(leftExpr, rightExpr, vars, leftNumValue, rightNumValue))
    {
        type = Tokens::BOOLEAN_T;
    }
    else
    {
        type = Tokens::STRING_T;

        if (leftExpr[0] != '$')
        {
            left = leftExpr;
        }
        else
        {
            var = leftExpr.substr(1, leftExpr.size() - 1);
            left = vars[var];
        }

        if (rightExpr[0] != '$')
        {
            right = rightExpr;
        }
        else
        {
            var = rightExpr.substr(1, rightExpr.size() - 1);
            right = vars[var];
        }
    }

    if (type == Tokens::NUMERIC_T)
    {
        if (sign == "<" || sign == "-gt")
            return leftNumValue < rightNumValue;

        if (sign == "<=" || sign == "-ge")
            return leftNumValue <= rightNumValue;

        if (sign == ">" || sign == "-lt")
            return leftNumValue > rightNumValue;

        if (sign == ">=" || sign == "-le")
            return leftNumValue >= rightNumValue;
    }

    if (type == Tokens::NUMERIC_T|| type == Tokens::BOOLEAN_T)
    {
        if (sign == "==" || sign == "-eq")
            return leftNumValue == rightNumValue;

        if (sign == "!=" || sign == "-ne")
            return leftNumValue != rightNumValue;
    }

    if (type == Tokens::STRING_T)
    {
        if (sign == "==" || sign == "-eq")
            return left == right;

        if (sign == "!=" || sign == "-ne")
            return left != right;
    }

    return true;
}

vector<CmdExecResult> Conditional::parseIf(vector<string> str, map <string, string> vars, vector<string> &passToShell, Shell* shell)
{
    string condition;
    vector<string> Conditional;
    vector<int> conditionI;
    string line;
    int fl = 0;

    for (int i = 0; i < str.size(); i++)
    {
        line = Conditional::trim(str[i]);

        if (line.find("if") != string::npos || line.find("elif") != string::npos || line.find("while") != string::npos)
        {
            condition = line.substr(line.find('(') + 1, line.size() - line.find('(') - 2);
            Conditional.push_back(condition);
            conditionI.push_back(i);
        }
    }

    bool elseCase = true;

    vector<CmdExecResult> results;
    for (int j = 0; j < Conditional.size(); j++){

        if (Conditional::parseCondition(Conditional[j], vars))
        {
            elseCase = false;

            for (int i = conditionI[j]; i < str.size(); i++)
            {
                if (fl == 0 && (str[i].find("if") != string::npos || str[i].find("elif") != string::npos || str[i].find("else") != string::npos))
                {
                    fl = 1;
                }
                if (fl == 1 && str[i].find("then") != string::npos)
                {
                    fl = 2;
                    continue;
                }
                if (fl == 2 && (str[i].find("fi") != string::npos || str[i].find("elif") != string::npos || str[i].find("else") != string::npos))
                {
                    break;
                }
                if (fl == 2)
                {
                    passToShell.push_back(trim(str[i]));

                    CmdExecResult result;
                    shell->execute(trim(str[i]), result);
                    results.push_back(result);
                }
            }

            break;
        }
    }


    if (elseCase)
    {
        for (int i = 0; i < str.size(); i++)
        {
            if (fl == 0 && str[i].find("else") != string::npos)
            {
                fl = 1;
            }
            if (fl == 1 && str[i].find("then") != string::npos)
            {
                fl = 2;
                continue;
            }
            if (fl == 2 && str[i].find("fi") != string::npos)
            {
                break;
            }
            if (fl == 2)
            {
                passToShell.push_back(trim(str[i]));

                CmdExecResult result;
                shell->execute(trim(str[i]), result);
                results.push_back(result);
            }
        }
    }

    return results;
}

vector<CmdExecResult> Conditional::parseWhile(vector<string> str, map <string, string> vars, vector<string> &passToShell, Shell* shell){

    string condition;
    string changeC;
    string line;
    int fl = 0;
    string var;

    for (int i = 0; i < str.size(); i++)
    {
        line = Conditional::trim(str[i]);

        if (line.find("while") != string::npos)
        {
            condition = line.substr(line.find('(') + 1, line.size() - line.find('(') - 2);

        }
        else if (line.find("++") != string::npos || line.find("--") != string::npos)
        {
            changeC = line.substr(line.find('(') + 2, line.size() - line.find('(') - 4);
        }
    }

    vector<CmdExecResult> results;
    while (Conditional::parseCondition(condition, vars)){

        for (int i = 0; i < str.size(); i++)
        {
            if (fl == 0 && str[i].find("while") != string::npos)
            {
                fl = 1;
            }
            if (fl == 1 && str[i].find("then") != string::npos)
            {
                fl = 2;
                continue;
            }
            if (fl == 2 && str[i].find("done") != string::npos)
            {
                break;
            }
            if (fl == 2 && str[i].find("((") == string::npos)
            {
                passToShell.push_back(trim(str[i]));
                CmdExecResult result;
                shell->execute(trim(str[i]), result);
                results.push_back(result);
            }
        }
        fl = 0;
        //change variables

        if (changeC.find("+") != string::npos){
            string varName = changeC.substr(0, changeC.size() - 2);
            vars[varName] = to_string(stoi(vars[varName]) + 1);
        }
        else if (changeC.find("-") != string::npos){
            string varName = changeC.substr(0, changeC.size() - 2);
            vars[varName] = to_string(stoi(vars[varName]) - 1);
        }
    }

    return results;
}

int Conditional::SemicolonPosition(string str){
    int i = 0;
    for (;i < str.size();i++){
        if (str[i] == ';'){
            break;
        }
    }
    return i;
}

vector<CmdExecResult> Conditional::parseFor(vector<string> str, map <string, string> vars, vector<string> &passToShell, Shell* shell){
    string condition;
    string changeC;
    string line;
    int fl = 0;
    string var;
    char colon = 0x3b; //  [5] = 0x3b ';'                        

    for (int i = 0; i < str.size(); i++)
    {
        line = Conditional::trim(str[i]);
        if (line.find("for") != string::npos)
        {
            string initVar = line.substr(line.find("(") + 2, line.find(";") - line.find("(") - 2);

            string before_sign = initVar.substr(0, initVar.find("="));
            string after_sign = initVar.substr(initVar.find("=") + 1);
            vars.insert(make_pair(Conditional::trim(before_sign),
                    Conditional::trim(after_sign)));

            condition = line.substr(line.find(";") + 1, line.find_last_of(";") - line.find(";") - 1);
            changeC = line.substr(line.find_last_of(";") + 1, line.find(")") - line.find_last_of(";") - 1);
            break;
        }

    }

    vector<CmdExecResult> results;
    while (Conditional::parseCondition(condition, vars)) {

        for (int i = 0; i < str.size(); i++)
        {
            if (fl == 0 && str[i].find("for") != string::npos)
            {
                fl = 1;
            }
            if (fl == 1 && str[i].find("then") != string::npos)
            {
                fl = 2;
                continue;
            }
            if (fl == 2 && str[i].find("done") != string::npos)
            {
                break;
            }
            if (fl == 2)
            {
                passToShell.push_back(trim(str[i]));
                CmdExecResult result;
                shell->execute(trim(str[i]), result);
                results.push_back(result);
            }
        }
        fl = 0;
        //change variables
        string without_last_two_symbols = changeC.substr(0, changeC.size() - 2);
        string varName = Conditional::trim(without_last_two_symbols);

        if (changeC.find("+") != string::npos){
            vars[varName] = to_string(stoi(vars[varName]) + 1);
        }
        else if (changeC.find("-") != string::npos){
            vars[varName] = to_string(stoi(vars[varName]) - 1);
        }
    }

    return results;
}