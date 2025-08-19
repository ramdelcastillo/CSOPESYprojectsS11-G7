#include "BackingStore.h"

BackingStore::BackingStore(const std::string& storeFile, const std::string& logFile) : storeFile(storeFile), logFile(logFile)
{
    clearTextFiles();
}

void BackingStore::storeProcess(std::shared_ptr<AttachedProcess> process) {
    std::ofstream store(storeFile, std::ios::app);
    if (store.is_open()) {
        store << process->getPid() << " "
            << process->getProcessName() << " "
            << process->getMemoryFrameSize() << " "
            << process->getMemorySize() << " "
            << process->getLinesOfCode() << " "
            << process->getCurrentLineOfInstruction() << " "
            << process->getStartTime() << " "
            << process->getAssignedCPUID() << "\n";
        store.close();

        logOperation("Stored: ", process);
    }
    else {
        std::cerr << "Unable to open backing store file.\n";
    }
}

void BackingStore::clearTextFiles()
{
    std::ofstream storeFileStream;
    storeFileStream.open(storeFile, std::ofstream::out | std::ofstream::trunc);
    if (!storeFileStream) {
        std::cerr << "Error: Unable to clear store file: " << storeFile << std::endl;
    }
    storeFileStream.close();

    std::ofstream logFileStream;
    logFileStream.open(logFile, std::ofstream::out | std::ofstream::trunc);
    if (!logFileStream) {
        std::cerr << "Error: Unable to clear log file: " << logFile << std::endl;
    }
    logFileStream.close();
}

bool BackingStore::loadProcess(int pid) {
    std::ifstream store(storeFile);
    std::vector<std::string> lines;
    std::string line;

    if (store.is_open()) {
        while (std::getline(store, line)) {
            lines.push_back(line);
        }
        store.close();

        for (size_t i = 0; i < lines.size(); ++i) {
            std::istringstream ss(lines[i]);


            int storedPid, linesOfCode, currentLineOfInstruction, assignedCPUID;
            uint32_t memorySize, memoryFrameSize;
            std::string processName;
            std::time_t startTime;

            ss >> storedPid >> processName >> memoryFrameSize >> memorySize >>
                linesOfCode >> currentLineOfInstruction >> startTime >> assignedCPUID;
    
            if (storedPid == pid) {

                auto process = std::make_shared<AttachedProcess>(storedPid, processName,
                    memoryFrameSize, memorySize, linesOfCode, currentLineOfInstruction, startTime, assignedCPUID);

                std::ofstream store(storeFile, std::ios::trunc);
                if (store.is_open()) {

                    for (size_t j = 0; j < lines.size(); ++j) {
                        if (j != i) {
                            store << lines[j] << "\n";
                        }
                    }
                    store.close();
                }

  
                logOperation("Loaded: ", process);
                return true; 
            }
        }
    }
    else {
        std::cerr << "Unable to open backing store file.\n";
    }
    return false;
}

void BackingStore::logOperation(const std::string& operation, std::shared_ptr<AttachedProcess> process) {
    std::ofstream logFile("backing-store-log.txt", std::ios::app); 

    if (logFile.is_open()) {
        logFile << operation
            << "pid: " << process->getPid() << ", "
            << "processName: " << process->getProcessName() << ", "
            << "memoryFrameSize: " << process->getMemoryFrameSize() << ", "
            << "memorySize: " << process->getMemorySize() << ", "
            << "linesOfCode: " << process->getLinesOfCode() << ", "
            << "currentLineOfInstruction: " << process->getCurrentLineOfInstruction() << ", "
            << "startTime: " << process->getStartTime() << ", "
            << "assignedCPUID: " << process->getAssignedCPUID() << "\n";
        logFile.close();
    }
    else {
        std::cerr << "Unable to open log file.\n"; 
    }
}




