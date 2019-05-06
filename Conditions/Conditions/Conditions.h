#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>

using namespace std;

#define IF_ELSE 0;
#define WHILE 1;
#define FOR 2;


class Conditions
{
public:

	static string& ltrim(string& str, const string& chars);

	static string& rtrim(string& str, const string& chars);

	//!
	///������� ��� �������� ��������, ��������� � �.�. � string
	///str - ������ � ������� ��������� ������� ���� �������
	///return ���������� str ��� ���� ��������
	static string& trim(string& str, const string& chars);

	//! ������� ����������� �������� �����(if, for, while) � ������
	///str - ������ �� ��������� ����
	///return bool
	static bool findReservedWords(string str);

	//! ������� ������� ������������� ���������� �� �������� ���� if, for, while
	/// str - �������� ���
	/// vars - ��������� ��� ��������� ����������
	/// return void
	static void findInit(vector<string> str, map<string, string> &vars);

	//! ������� ����������� ������� if
	/// str - �������� ���
	/// vars - ����������
	/// passToShell - ������ ������� ����� ����� ��������� � shell
	/// return void
	static void parseIf(vector<string> str, map <string, string> vars, vector<string> &passToShell);

	//! ������� ����������� ������� ��� ����� while
	/// str - �������� ���
	/// vars - ����������
	/// passToShell - ������ ������� ����� ����� ��������� � shell
	/// return void
	static void parseWhile(vector<string> str, map <string, string> vars, vector<string> &passToShell);

	//! ������� ����������� ������� ��� ����� for
	/// str - �������� ���
	/// vars - ����������
	/// passToShell - ������ ������� ����� ����� ��������� � shell
	/// return void
	static void parseFor(vector<string> str, map <string, string> vars, vector<string> &passToShell);

	static bool isBoolean(string leftExpr, string rightExpr, map <string, string> vars, float &leftValue, float &rightValue);

	static bool isNumeric(string leftExpr, string rightExpr, map <string, string> vars, float &leftValue, float &rightValue);

	//! ������� ������� ��������� �������
	/// str - �������
	/// vars - ����������
	/// return bool
	static bool parseCondition(string str, map <string, string> vars);

	//! �������, ������� ��������� ������ �������� �� ��� ������
	/// str - ������, ������� ���������� ���������
	/// returns - true if str is number
	static bool isDigit(string str);
	
	//! ������� ��������� ��� ����� 
	/// 0 - ���� if else 
	/// 1 - ���� while ����
	/// 2 - ���� for ����
	static int checkType(string str);

	static void Start(string str_command);
	Conditions(){};
};

