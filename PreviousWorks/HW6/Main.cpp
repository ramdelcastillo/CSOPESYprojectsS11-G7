#include <iostream>

#include "ConsoleManager.h"
#include "MainConsole.h"
#include "ProcessConsole.h"
#include "AConsole.h"

/*
Developers:
S11
Atienza, Angelene
Del Castillo, Jose
Padua, Alexandra
Rivera, Jose
*/

int main() {
	ConsoleManager* consoleManager = ConsoleManager::getInstance();
	MainConsole mainConsole;

	
	mainConsole.header();

	while (consoleManager->running()) {
		mainConsole.process();
	}
  
   return 0;
}


//consoleManager->createConsoleWOswitch("Salary-2019.exe");
	//consoleManager->createConsoleWOswitch("thequickbrownfoxjumpsoverthelazydog");
	//consoleManager->createConsoleWOswitch("Word.exe");
	//consoleManager->createConsoleWOswitch("thisisaverylongconsolenameeeeeeeeeeeeeeeeeeeeeeeeeeeeee");
	//consoleManager->createConsoleWOswitch("opesy.exe");
