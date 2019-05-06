#pragma once

#include <string>
#include <vector>
#include <regex>
#include <boost\filesystem.hpp>
#include <boost\filesystem\path.hpp>
#include <boost\filesystem\fstream.hpp>

using namespace std;

/*!
* \file
* \author Khodjabekova Sh.B. (hodjabekova@gmail.com)
* \date   May, 2019
* \brief  Grep class
*
* \section DESCRIPTION_GREP
*
* Grep - contains cache server running functions.
* 
*/

//--------------------------------------------------
/// \brief  Grep class
///
/// This class 


class Grep{

public:
	//! Parse given line 
	/// \param[in] commandLine
	/// \param[out]  files vector of files or file path if option is -r
	/// \param[out]  option 
	/// \param[out]  pattern 
	static void parse(const string &commandLine, vector<string> &files, char &option, regex &pattern);

	//! For option 'o' prints matched pattern
	/// param [in] str string where pattern searched
	/// param [in] reg regular expression, pattern 
	static void PrintMatches(string str, regex reg);

	//! For option 'c' prints count of matched pattern
	/// param [in] str string where pattern searched
	/// param [in] reg regular expression, pattern 
	/// param [in] matchC count of matched pattern
	static void CountMatches(string str, regex reg, int &matchC);

	//! For option 'n; prints number of line where matched pattern was find
	/// param [in] str string where pattern searched
	/// param [in] reg regular expression, pattern 
	/// param [in] line current line number 
	static void PrintMatchesLineNum(string str, regex reg, int line);
	
	Grep(){};

	//! Pare given line 
	/// \param[in] command - это строка, которая 
	/// \return result 
	/*!

	*/
	static int grep(string &command);
};