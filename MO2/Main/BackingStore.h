#pragma once
#include "AttachedProcess.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

class BackingStore
{
public:
	BackingStore(const std::string& storeFile, const std::string& logFile);
	bool loadProcess(int pid);
	void storeProcess(std::shared_ptr<AttachedProcess> process);
	void clearTextFiles();

private:
	std::string storeFile;
	std::string logFile;

	void logOperation(const std::string& operation, std::shared_ptr<AttachedProcess> process);
};

