// performance_tests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Float4Test.h"

int main(int, _TCHAR*)
{
    Tests::Float4Test test;

    test.Init();
    test.Run();
    test.OutputResults();

    std::cin.get();

	return 0;
}

