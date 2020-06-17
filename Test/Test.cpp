// Test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>

#include "App.h"


UD_USING_NAMESPACE

int main()
{
	// in this main I will write for now just the theoretically sequence of calls to initialize the engine, but eventually they will be wrapped in 1 call only
	//////////////////////////////////////////////////////////////////////////
	Settings::Instance().LoadMemorySettings();

	App demo(800, 600, "Test", VK_MAKE_VERSION(0, 0, 1));

	demo.Execute();

	
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// was used to force the library to build
	UndeadExporter exporter;
	std::cout << "Undead Engine Version: " << exporter.GetVersion();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
