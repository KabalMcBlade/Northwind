// Test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>

#include "App.h"


NW_USING_NAMESPACE

static constexpr uint32 kWidth = 1024;
static constexpr uint32 kHeight = 768;

int main(int argc, char **argv)
{
	Settings::Instance().LoadMemorySettings();

	CommandLineParser cmdLine;
	cmdLine.AddWithValueAndDefault("-width", false, kWidth);
	cmdLine.AddWithValueAndDefault("-height", false, kHeight);
	cmdLine.AddWithValueAndDefault("-fullscreen", false, false);
	cmdLine.AddWithValueAndDefault("-showcursor", false, true);
	cmdLine.Parse(argc, argv);

	FileSystem fileSystem("Assets", "Shaders", "Textures", "Models");

	App demo("Test", VK_MAKE_VERSION(0, 0, 1), cmdLine, fileSystem);

	demo.Execute();

	return 0;
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
