#include "CppUnitTest.h"
#include "../Conditions/Conditions.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ConditionsUnitTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestFor)
		{
			// TODO: Your test code here
			string coommand = "for ((c = 1; c < 4; c++)) \n do \n ls \n cat test.txt \n done";
			Conditions::Start(coommand);
			coommand = "for ((c = 1; c -gt 4; c++)) \n do \n ls \n cat test.txt \n done";
			Conditions::Start(coommand);

			coommand = "for ((c = 1; c <= 4; c++)) \n do \n ls \n cat test.txt \n done";
			Conditions::Start(coommand);
			coommand = "for ((c = 1; c -ge 4; c++)) \n do \n ls \n cat test.txt \n done";
			Conditions::Start(coommand);

			coommand = "for ((c = 4; c > 4; c--)) \n do \n ls \n cat test.txt \n done";
			Conditions::Start(coommand);
			coommand = "for ((c = 4; c -lt 4; c--)) \n do \n ls \n cat test.txt \n done";
			Conditions::Start(coommand);

			coommand = "for ((c = 4; c >= 1; c--)) \n do \n ls \n cat test.txt \n done";
			Conditions::Start(coommand);
			coommand = "for ((c = 4; c -le 1; c--)) \n do \n ls \n cat test.txt \n done";
			Conditions::Start(coommand);

		}
		TEST_METHOD(TestIf)
		{
			// TODO: Your test code here
			string command = "c = 1 \n b = 11 \n if ($c < $b) \n then \n ls \n cat test.txt \n fi";
			Conditions::Start(command);

			command = "c = -1 \n if ($c < 10) \n then \n ls \n cat test.txt \n else \n then \n cd ./ \n ls \n fi";
			Conditions::Start(command);
			
			command = "c = 12 \n b = -10 \n if ($c < 10) \n then \n ls \n cat test.txt \n elif ($c > $b) \n then \n cd ./ \n mc test \n else \n then \n cd ./ \n ls \n fi";
			Conditions::Start(command);
			
			command = "c = 20 \n b = -11 \n if ($c < 10) \n then \n ls \n cat test.txt \n elif ($c < $b) \n then \n cd ./ \n mc test \n elif ($b < 0) \n then \n cd ./ \n else \n then \n cd ./ \n ls \n fi";
			Conditions::Start(command);
		}

		TEST_METHOD(TestWhile)
		{
			// TODO: Your test code here
			string command = "c = 15 \n b = -11 \n while ($c > 10) \n then \n ls \n cat test.txt \n ((c--)) \n done";
			Conditions::Start(command);

			command = "c = 5 \n b = 10 \n while ($c < $b) \n then \n ls \n cat test.txt \n ((c++)) \n done";
			Conditions::Start(command);

			command = "c = 0 \n b = 7 \n while ($c <= $b) \n then \n ls \n ((c++)) \n done";
			Conditions::Start(command);

			command = "c = 4 \n while ($c >= 0) \n then \n ls \n ((c--)) \n done";
			Conditions::Start(command);
		}



	};
}