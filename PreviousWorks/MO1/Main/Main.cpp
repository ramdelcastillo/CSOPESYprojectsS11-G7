#include <iostream>

#include "ConsoleManager.h"
#include "MainConsole.h"
#include "ProcessConsole.h"
#include "AConsole.h"

/* 
Developers:
S11 Group 7
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



