#include <vector>
#include <ctime>
#include <cmath>
#include <sstream>
#include <ctime>
#include <regex>
#include <clocale>
#include "Conditions.h"

using namespace std;

#define STRING_T 0;
#define NUMERIC_T 1;
#define BOOLEAN_T 2;

string& Conditions::ltrim(string& str, const string& chars = "\t\n\v\f\r ")
{
	str.erase(0, str.find_first_not_of(chars));
	return str;
}

string& Conditions::rtrim(string& str, const string& chars = "\t\n\v\f\r ")
{
	str.erase(str.find_last_not_of(chars) + 1);
	return str;
}

string& Conditions::trim(string& str, const string& chars = "\t\n\v\f\r ")
{
	return Conditions::ltrim(Conditions::rtrim(str, chars), chars);
}


bool Conditions::findReservedWords(string str)
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


void Conditions::findInit(vector<string> str, map <string, string> &vars)
{
	for (int i = 0; i < str.size(); i++)
	{
		if (!Conditions::findReservedWords(str[i]))
		{//делим на две части, до знака равно и после
			vars.insert(make_pair(Conditions::trim(str[i].substr(0, str[i].find('='))), Conditions::trim(str[i].substr(str[i].find('=') + 1))));
		}
		else
		{
			break;
		}
	}
}


bool Conditions::isBoolean(string leftExpr, string rightExpr, map <string, string> vars, float &leftValue, float &rightValue)
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

bool Conditions::isDigit(string str){
	locale loc;
	//TO DO: check point to float
	for (int i = 0; i < str.size(); i++){
		if (!isdigit(str[i], loc)){
			return false;
		}
	}
	return true;
}

bool Conditions::isNumeric(string leftExpr, string rightExpr, map <string, string> vars, float &leftValue, float &rightValue)
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

	if (rightExpr[0] != '$' && (rightExpr[0] == '-' && isDigit(rightExpr.substr(1, rightExpr.size() - 1))) || isDigit(rightExpr))
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

bool Conditions::parseCondition(string str, map <string, string> vars)
{
	string boolSigns[] = { "<=", ">=", "<", ">", "==", "!=",
		"-ge", "-le", "-gt", "-lt", "-eq", "-ne" };
	string sign;
	string leftExpr;
	string rightExpr;
	string var, left, right;
	float leftNumValue;
	float rightNumValue;
	int type = -1;

	//в цикле проходимся по каждому знаку и ищем соответсвующий
	for (int i = 0; i < 12; i++)
	{
		if (str.find(boolSigns[i]) != string::npos)
		{
			sign = boolSigns[i];
			leftExpr = trim(str.substr(0, str.find(sign)));
			rightExpr = trim(str.substr(str.find(sign) + sign.size(), str.size() - str.find(boolSigns[i]) - sign.size()));
			break;
		}
	}


	if (isNumeric(leftExpr, rightExpr, vars, leftNumValue, rightNumValue))
	{
		type = NUMERIC_T;

	}
	else if (isBoolean(leftExpr, rightExpr, vars, leftNumValue, rightNumValue))
	{
		type = BOOLEAN_T;
	}
	else
	{
		type = STRING_T;

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

	if (type == 1)
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

	if (type == 1 || type == 2)
	{
		if (sign == "==" || sign == "-eq")
			return leftNumValue == rightNumValue;

		if (sign == "!=" || sign == "-ne")
			return leftNumValue != rightNumValue;
	}

	if (type == 0)
	{
		if (sign == "==" || sign == "-eq")
			return left == right;

		if (sign == "!=" || sign == "-ne")
			return left != right;
	}

	return true;
}

void Conditions::parseIf(vector<string> str, map <string, string> vars, vector<string> &passToShell)
{
	string condition;
	vector<string> conditions;
	vector<int> conditionI;
	string line;
	int fl = 0;

	for (int i = 0; i < str.size(); i++)
	{
		line = Conditions::trim(str[i]);

		if (line.find("if") != string::npos || line.find("elif") != string::npos || line.find("while") != string::npos)
		{
			condition = line.substr(line.find('(') + 1, line.size() - line.find('(') - 2);
			conditions.push_back(condition);
			conditionI.push_back(i);
		}
	}

	bool elseCase = true;

	for (int j = 0; j < conditions.size(); j++){

		if (Conditions::parseCondition(conditions[j], vars))
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
				cout << str[i] << endl;
			}
		}
	}
}

void Conditions::parseWhile(vector<string> str, map <string, string> vars, vector<string> &passToShell){

	string condition;
	string changeC;
	string line;
	int fl = 0;
	string var;

	for (int i = 0; i < str.size(); i++)
	{
		line = Conditions::trim(str[i]);

		if (line.find("while") != string::npos)
		{
			condition = line.substr(line.find('(') + 1, line.size() - line.find('(') - 2);

		}
		else if (line.find("++") != string::npos || line.find("--") != string::npos)
		{
			changeC = line.substr(line.find('(') + 2, line.size() - line.find('(') - 4);
		}
	}

	while (Conditions::parseCondition(condition, vars)){

		for (int i = 0; i < str.size(); i++)
		{
			if (fl == 0 && str[i].find("while") != string::npos)
			{
				fl = 1;
			}
			if (fl == 1 && str[i].find("do") != string::npos)
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
				cout << str[i] << endl;
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


}

void Conditions::parseFor(vector<string> str, map <string, string> vars, vector<string> &passToShell){

	string condition;
	string changeC;
	string line;
	int fl = 0;
	string var;
	char colon = 0x3b; //  [5] = 0x3b ';'                        

	for (int i = 0; i < str.size(); i++)
	{
		line = Conditions::trim(str[i]);
		if (line.find("for") != string::npos)
		{
			
			string initVar = line.substr(line.find("(") + 2, line.find(";") - line.find("(") - 2);
			vars.insert(make_pair(Conditions::trim(initVar.substr(0, initVar.find("="))), Conditions::trim(initVar.substr(initVar.find("=") + 1))));
			condition = line.substr(line.find(";") + 1, line.find_last_of(";") - line.find(";") - 1);
			changeC = line.substr(line.find_last_of(";") + 1, line.find(")") - line.find_last_of(";") - 1);
			break;
		}
		
	}

	while (Conditions::parseCondition(condition, vars)){

		for (int i = 0; i < str.size(); i++)
		{
			if (fl == 0 && str[i].find("for") != string::npos)
			{
				fl = 1;
			}
			if (fl == 1 && str[i].find("do") != string::npos)
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
				cout << str[i] << endl;
			}

		}
		fl = 0;
		//change variables
		string varName = Conditions::trim(changeC.substr(0, changeC.size() - 2));

		if (changeC.find("+") != string::npos){
			vars[varName] = to_string(stoi(vars[varName]) + 1);
		}
		else if (changeC.find("-") != string::npos){
			vars[varName] = to_string(stoi(vars[varName]) - 1);
		}
	}

}

void Conditions::Start(string str_command){

	map <string, string> vars;
	vector<string> stringToParse;
	string line;
	vector<string> passToShell;
	int cType = -1;

	//read_command_cpp(stringToParse, cType);

	//ifstream filename("in.txt");

	//while (getline(filename, line))
	//{
	//	stringToParse.push_back(line);
	//	if (cType == -1){
	//		cType = checkType(line);
	//	}
	//}


	for (int i = 0; i < str_command.size(); i++) {
		if (str_command[i] != '\n'){
			line += str_command[i];

		}
		else
		{
			stringToParse.push_back(line);
			if (cType == -1){
				cType = checkType(line);
			}
			line = "";
		}

	}
	// add last line 
	stringToParse.push_back(line);

	Conditions c = Conditions();

	switch (cType)
	{
	case 0:
		c.findInit(stringToParse, vars);
		c.parseIf(stringToParse, vars, passToShell);
		break;
	case 1:
		c.findInit(stringToParse, vars);
		c.parseWhile(stringToParse, vars, passToShell);
		break;
	case 2:
		c.parseFor(stringToParse, vars, passToShell);
		break;
	default:
		break;
	}


}

int Conditions::checkType(string str){

	if (str.find("if") != string::npos){
		return IF_ELSE;
	}
	else if (str.find("while") != string::npos){
		return WHILE;
	}
	else if (str.find("for") != string::npos){
		return FOR;
	}
	else{
		return -1;
	}

}