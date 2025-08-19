#include "FlatMemoryAllocator.h"

FlatMemoryAllocator::FlatMemoryAllocator()
    : IMemoryAllocator()
{
    ConsoleManager* consoleManager = ConsoleManager::getInstance();

    uint32_t partitionSize = consoleManager->getCPUScheduler()->getMemoryPerProcess();

    for (uint32_t i = 0; i < 4; ++i) {
        memoryPartitions.push_back({ i, true, nullptr });
    }
}

bool FlatMemoryAllocator::allocate(std::shared_ptr<AttachedProcess> process)
{
    for (const auto& partition : memoryPartitions) {
        if (partition.process == process) {
            return false;  
        }
    }

    for (auto& partition : memoryPartitions) {
        if (partition.isAllocatable) {
            partition.process = process;
            partition.isAllocatable = false;
            return true;  
        }
    }

    return false;
}


void FlatMemoryAllocator::deallocate(std::shared_ptr<AttachedProcess> process)
{
    for (auto& partition : memoryPartitions) {
        if (partition.process == process) {
            partition.process.reset();
            partition.isAllocatable = true;

            //std::cout << "Process " << process->getProcessName()
            //    << " deallocated from slot " << partition.slotNumber << "\n";
            //return;  // Deallocation done, exit the function
        }
    }
}

void FlatMemoryAllocator::visualizeMemory()
{
    ConsoleManager* consoleManager = ConsoleManager::getInstance();
    uint32_t totalMemory = consoleManager->getCPUScheduler()->getMaxOverallMemory();
    uint32_t slotSize = consoleManager->getCPUScheduler()->getMemoryPerProcess();

    std::ostringstream outputBuffer;  

    uint32_t currentMemory = totalMemory;
    uint32_t numProcessesInMemory = getAmountOfAllocatedSlots();
    uint32_t totalExternalFragmentation = 0;

    time_t now = time(0);
    struct tm localTime;
    localtime_s(&localTime, &now);
    outputBuffer << "\nTimestamp: (" << std::put_time(&localTime, "%m/%d/%Y %I:%M:%S%p") << ")\n";

    outputBuffer << "Number of processes in memory: " << numProcessesInMemory << "\n";
    outputBuffer << "Total external fragmentation in KB: " << getFragmentation() << "\n";

    outputBuffer << "----end---- = " << totalMemory << "\n";

    for (size_t i = 0; i < memoryPartitions.size(); ++i) {
        const auto& partition = memoryPartitions[i];
        if (partition.isAllocatable) {
            outputBuffer << "\n";
            currentMemory -= slotSize;
        }
        else if (partition.process) {
            std::string processName = partition.process->getProcessName();


            if (partition.process->getProcessState() == AttachedProcess::WAITING) {
                processName += " *";
            }

            outputBuffer << processName << "\n";
            currentMemory -= slotSize;  
            numProcessesInMemory++; 
        }

        if (i < memoryPartitions.size() - 1) {
            outputBuffer << currentMemory << "\n";
        }
    }

    totalExternalFragmentation = currentMemory;

    outputBuffer << "----start---- = 0\n\n";

    std::cout << outputBuffer.str();
}

void FlatMemoryAllocator::printMemory()
{
    ConsoleManager* consoleManager = ConsoleManager::getInstance();
    uint32_t totalMemory = consoleManager->getCPUScheduler()->getMaxOverallMemory();
    uint32_t slotSize = consoleManager->getCPUScheduler()->getMemoryPerProcess();

    std::ostringstream buffer;

    uint32_t currentMemory = totalMemory;
    uint32_t numProcessesInMemory = getAmountOfAllocatedSlots();
    uint32_t totalExternalFragmentation = 0;

    time_t now = time(0);
    struct tm localTime;
    localtime_s(&localTime, &now);

    buffer << "\nTimestamp: (" << std::put_time(&localTime, "%m/%d/%Y %I:%M:%S%p") << ")\n";
    buffer << "Number of processes in memory: " << numProcessesInMemory << "\n";
    buffer << "Total external fragmentation in KB: " << getFragmentation() << "\n"; 

    buffer << "----end---- = " << totalMemory << "\n";

    for (size_t i = 0; i < memoryPartitions.size(); ++i) {
        const auto& partition = memoryPartitions[i];
        if (partition.isAllocatable) {
            buffer << "\n";
            currentMemory -= slotSize;
        }
        else if (partition.process) {
            std::string processName = partition.process->getProcessName();
            buffer << processName << "\n";
            currentMemory -= slotSize;
            numProcessesInMemory++;
        }

        if (i < memoryPartitions.size() - 1) {
            buffer << currentMemory << "\n";
        }
    }

    totalExternalFragmentation = currentMemory;

    buffer << "----start---- = 0\n\n";

    std::ofstream logFile("memory_log.txt", std::ios::trunc);

    if (!logFile.is_open()) {
        std::cerr << "Error: Unable to open memory log file.\n";
        return;
    }

    logFile << buffer.str();
    logFile.close();

    std::filesystem::path currentPath = std::filesystem::current_path();
    std::filesystem::path filePath = currentPath / "memory_log.txt";
    std::cout << "Memory report generated at: " << filePath.string() << std::endl << std::endl;
}

void FlatMemoryAllocator::printMemoryPerCycle(uint32_t commandCounter)
{
    ConsoleManager* consoleManager = ConsoleManager::getInstance();
    uint32_t totalMemory = consoleManager->getCPUScheduler()->getMaxOverallMemory();
    uint32_t slotSize = consoleManager->getCPUScheduler()->getMemoryPerProcess();

    std::ostringstream buffer;

    uint32_t currentMemory = totalMemory;
    uint32_t numProcessesInMemory = getAmountOfAllocatedSlots();
    uint32_t totalExternalFragmentation = 0;

    time_t now = time(0);
    struct tm localTime;
    localtime_s(&localTime, &now);

    buffer << "\nTimestamp: (" << std::put_time(&localTime, "%m/%d/%Y %I:%M:%S%p") << ")\n";
    buffer << "Number of processes in memory: " << numProcessesInMemory << "\n";
    buffer << "Total external fragmentation in KB: " << getFragmentation() << "\n";

    buffer << "----end---- = " << totalMemory << "\n";

    for (size_t i = 0; i < memoryPartitions.size(); ++i) {
        const auto& partition = memoryPartitions[i];
        if (partition.isAllocatable) {
            buffer << "\n";
            currentMemory -= slotSize;
        }
        else if (partition.process) {
            std::string processName = partition.process->getProcessName();
            buffer << processName << "\n";
            currentMemory -= slotSize;
            numProcessesInMemory++;
        }

        if (i < memoryPartitions.size() - 1) {
            buffer << currentMemory << "\n";
        }
    }

    totalExternalFragmentation = currentMemory;

    buffer << "----start---- = 0\n\n";

    std::ostringstream filename;
    filename << "memory_stamp_" << commandCounter << ".txt";


    std::filesystem::path logDirectory = std::filesystem::current_path() / "MemoryLogs";
    std::filesystem::path filePath = logDirectory / filename.str();

    int fileIndex = 1;
    while (std::filesystem::exists(filePath)) {
        filename.str(""); 
        filename << "memory_stamp_" << commandCounter << " (" << fileIndex << ").txt";
        filePath = logDirectory / filename.str();
        fileIndex++;
    }

    std::filesystem::create_directories(logDirectory);

    std::ofstream logFile(filePath, std::ios::trunc);

    if (!logFile.is_open()) {
        std::cerr << "Error: Unable to open memory log file.\n";
        return;
    }

    logFile << buffer.str();
    logFile.close();

    //std::cout << "Memory report generated at: " << filePath.string() << std::endl << std::endl;
}


void FlatMemoryAllocator::setIsAllocatable(uint32_t slotNumber, bool value)
{
    memoryPartitions[slotNumber].isAllocatable = value;
}

bool FlatMemoryAllocator::isProcessAllocated(std::shared_ptr<AttachedProcess> process)
{
    for (const auto& partition : memoryPartitions) {
        if (partition.process == process) {
            return true;  
        }
    }
    return false; 
}

uint32_t FlatMemoryAllocator::getAmountOfAllocatedSlots()
{
    uint32_t allocatedSlots = 0;

    for (const auto& partition : memoryPartitions) {
        if (!partition.isAllocatable) {
            allocatedSlots++; 
        }
    }

    return allocatedSlots;
}

bool FlatMemoryAllocator::hasFreeSlots()
{
    for (const auto& partition : memoryPartitions) {
        if (partition.isAllocatable) {
            return true;
        }
    }
    return false;
}

uint32_t FlatMemoryAllocator::getFragmentation()
{
    ConsoleManager* consoleManager = ConsoleManager::getInstance();
    uint32_t fragmentation = 0;
    uint32_t previousUpperBound = 0; 

    std::vector<MemoryPartition> allocatedPartitions;
    for (const auto& partition : memoryPartitions) {
        if (!partition.isAllocatable && partition.process) {
            allocatedPartitions.push_back(partition);
        }
    }
    std::sort(allocatedPartitions.begin(), allocatedPartitions.end(),
        [](const MemoryPartition& a, const MemoryPartition& b) {
            return a.slotNumber < b.slotNumber;
        });

    for (const auto& partition : allocatedPartitions) {
        uint32_t lowerBound = partition.slotNumber * 4096;
        uint32_t upperBound = lowerBound + 4096;

        if (previousUpperBound < lowerBound) {
            fragmentation += lowerBound - previousUpperBound;
        }
        previousUpperBound = upperBound;
    }

    if (previousUpperBound < consoleManager->getCPUScheduler()->getMaxOverallMemory()) {
        fragmentation += consoleManager->getCPUScheduler()->getMaxOverallMemory() - previousUpperBound;
    }

    return fragmentation;
}

