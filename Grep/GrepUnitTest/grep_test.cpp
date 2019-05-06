#include <iostream>
#include <fstream>
#include <regex>
#include "CppUnitTest.h"
#include "../Grep/Grep.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace GrepUnitTest
{		
	TEST_CLASS(UnitTest1)
	{
		/*static char * tmp;*/

	public:

		/*TEST_CLASS_INITIALIZE(class_initialize)
		{
			tmp = nullptr;
		}

		TEST_CLASS_CLEANUP(class_finalize)
		{
			if (tmp)
			{
				free(tmp);
			}
		}*/

		TEST_METHOD(test_grep_all_methods)
		{
			// TODO: Your test code here

			string commandline = "grep -c \"word\" file2.txt file3.txt";					
			char option = NULL;
			regex pattern = regex();
			vector<string> files = vector<string>();
		
			Grep::parse(commandline, files, option, pattern);
			Assert::AreEqual(option, 'c');
			Assert::AreEqual(files.size(), (size_t)2);

			int result = 0;
			result = Grep::grep(commandline);
			Assert::AreNotEqual(result, -1);
/*
			commandline = "grep -c \"test\" test.txt";
			result = Grep::grep(commandline);
			Assert::AreNotEqual(result, -1);*/

			
		}


		//TEST_METHOD(TestPath){
		//	int result = 0;


		//	string commandline = "grep -o \"test\" D:\\grep\\grep\\sub1\\sub2\\test.txt";
		//	result = Grep::grep(commandline);
		//	Assert::AreNotEqual(result, -1);

		//	commandline = "grep -c \"test\" D:\\grep\\grep\\sub1\\sub2\\test.txt ../test.txt";
		//	result = Grep::grep(commandline);
		//	Assert::AreNotEqual(result, -1);
		//}
	};

	//char * UnitTest1::tmp;
}