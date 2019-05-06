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
	///Функция для удаления пробелов, табуляций и т.д. в string
	///str - строка в которой требуется удлаить спец символы
	///return возвращает str без спец символов
	static string& trim(string& str, const string& chars);

	//! Функция проверяетна ключевые слова(if, for, while) в строке
	///str - строка из исходного кода
	///return bool
	static bool findReservedWords(string str);

	//! Функция находит инициализацию переменных до ключевых слов if, for, while
	/// str - исходный код
	/// vars - сохраняет все найденные переменные
	/// return void
	static void findInit(vector<string> str, map<string, string> &vars);

	//! Функция разбирающая условие if
	/// str - исходный код
	/// vars - переменные
	/// passToShell - строки которые нужно будет отправить в shell
	/// return void
	static void parseIf(vector<string> str, map <string, string> vars, vector<string> &passToShell);

	//! Функция разбирающая условие для цикла while
	/// str - исходный код
	/// vars - переменные
	/// passToShell - строки которые нужно будет отправить в shell
	/// return void
	static void parseWhile(vector<string> str, map <string, string> vars, vector<string> &passToShell);

	//! Функция разбирающая условие для цикла for
	/// str - исходный код
	/// vars - переменные
	/// passToShell - строки которые нужно будет отправить в shell
	/// return void
	static void parseFor(vector<string> str, map <string, string> vars, vector<string> &passToShell);

	static bool isBoolean(string leftExpr, string rightExpr, map <string, string> vars, float &leftValue, float &rightValue);

	static bool isNumeric(string leftExpr, string rightExpr, map <string, string> vars, float &leftValue, float &rightValue);

	//! Функция которая проверяет условие
	/// str - условие
	/// vars - переменные
	/// return bool
	static bool parseCondition(string str, map <string, string> vars);

	//! Функция, которая проверяет строку является ли она числом
	/// str - строка, которую необходимо проверить
	/// returns - true if str is number
	static bool isDigit(string str);
	
	//! Функция проверяет тип цикла 
	/// 0 - если if else 
	/// 1 - если while цикл
	/// 2 - если for цикл
	static int checkType(string str);

	static void Start(string str_command);
	Conditions(){};
};

