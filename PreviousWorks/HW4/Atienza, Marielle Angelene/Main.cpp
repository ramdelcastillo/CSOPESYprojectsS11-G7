#include <iostream>

#include "ConsoleManager.h"
#include "MainConsole.h"
#include "ProcessConsole.h"
#include "AConsole.h"

int main() {
	ConsoleManager* consoleManager = ConsoleManager::getInstance();
	MainConsole mainConsole;

	mainConsole.header();

	while (consoleManager->running()) {
		mainConsole.process();
	}

	return 0;
}