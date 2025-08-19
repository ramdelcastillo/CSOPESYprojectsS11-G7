#include "PagingAllocator.h"

PagingAllocator::PagingAllocator()
{
	ConsoleManager* consoleManager = ConsoleManager::getInstance();
	maxOverAllMemory = consoleManager->getCPUScheduler()->getMaxOverallMemory();
	memoryPerFrame = consoleManager->getCPUScheduler()->getMemoryPerFrame();
	nFrames = maxOverAllMemory / memoryPerFrame;
    memoryMap.resize(nFrames, -1);
}

bool PagingAllocator::allocate(std::shared_ptr<AttachedProcess> process)
{
    uint32_t requiredFrames = process->getNPages(); 

    if (requiredFrames > nFrames) {
        return false;
    }

    for (uint32_t i = 0; i < nFrames - requiredFrames + 1; ++i) {
        bool canAllocate = true;

        for (uint32_t j = 0; j < requiredFrames; ++j) {
            if (memoryMap[i + j] != -1) {  
                canAllocate = false;
                break;
            }
        }

        if (canAllocate) {
            for (uint32_t j = 0; j < requiredFrames; ++j) {
                memoryMap[i + j] = process->getPid(); 
            }

            processList.push_back(process);

            numPagedIn += requiredFrames;  
            usedMemory += requiredFrames * memoryPerFrame;  

    /*        std::cout << "Process " << process->getPid() << " allocated " << requiredFrames << " pages (" << requiredFrames * memoryPerFrame << " KiB) successfully.\n";*/
            return true;
        }
    }

    //std::cout << "Failed to allocate memory for Process " << process->getPid() << ".\n";
    return false;
}


void PagingAllocator::deallocate(int pid)
{
    uint32_t removedPages = 0;
    for (uint32_t i = 0; i < nFrames; ++i) {
        if (memoryMap[i] == pid) {
            memoryMap[i] = -1;  
            removedPages++;
        }
    }

    uint32_t memoryFreed = removedPages * memoryPerFrame; 
    numPagedOut += removedPages; 
    usedMemory -= memoryFreed;

    auto it = std::remove_if(processList.begin(), processList.end(),
        [pid](const std::shared_ptr<AttachedProcess>& proc) { return proc->getPid() == pid; });
    processList.erase(it, processList.end()); 

    //std::cout << "Process " << pid << " deallocated successfully.\n";
}

void PagingAllocator::visualizeMemory()
{
    ConsoleManager* consoleManager = ConsoleManager::getInstance();
    int busyCores = consoleManager->getCPUScheduler()->getAmountOfWorkingCores();
    int totalCores = consoleManager->getCPUScheduler()->getNumCPU();
    float cpuUtilization = (static_cast<float>(busyCores) * 100) / totalCores;

    std::ostringstream oss;

    uint32_t usedMemory = 0;
    uint32_t totalMemory = maxOverAllMemory;

    std::unordered_map<int, uint32_t> memoryUsagePerProcess;
    std::unordered_map<int, std::string> processNameMap;

    for (uint32_t i = 0; i < nFrames; ++i) {
        if (memoryMap[i] != -1) {
            int pid = memoryMap[i];
            memoryUsagePerProcess[pid] += memoryPerFrame;
            usedMemory += memoryPerFrame;
        }
    }

    for (const auto& process : processList) {
        if (process) {
            processNameMap[process->getPid()] = process->getProcessName();
        }
    }

    float memoryUsage = (static_cast<float>(usedMemory) / totalMemory) * 100;

    // Header
    oss << "-------------------------------------------\n";
    oss << "| PROCESS-SMI V01.00 Driver Version: 01.00|\n";
    oss << "|             PAGING ALLOCATOR            |\n";
    oss << "-------------------------------------------\n";
    oss << std::fixed << std::setprecision(2);
    oss << "CPU-Util: " << cpuUtilization << "%\n";
    oss << "Memory Usage: " << usedMemory << "KiB / " << totalMemory << "KiB\n";
    oss << "Memory Util: " << memoryUsage << "%\n";
    oss << "\n===========================================\n";
    oss << "Running processes and memory usage:\n";
    oss << "-------------------------------------------\n";

    for (const auto& entry : memoryUsagePerProcess) {
        int pid = entry.first;
        uint32_t memoryUsed = entry.second;
        std::string processName = processNameMap[pid];
        oss << processName << " (" << memoryUsed << "KiB)\n";
    }

    oss << "-------------------------------------------\n";
    oss << "\n";

    std::cout << oss.str();
}

void PagingAllocator::displayStats()
{
    ConsoleManager* consoleManager = ConsoleManager::getInstance();
    uint32_t activeCycles = consoleManager->getCPUScheduler()->getActiveCycles();
    uint32_t cpuCycles = consoleManager->getCPUScheduler()->getCPUCycles();
    uint32_t InactiveCycles = consoleManager->getCPUScheduler()->getInactiveCycles();
    uint32_t freeMemory = maxOverAllMemory - usedMemory; 
    float memoryUsage = (static_cast<float>(usedMemory) / maxOverAllMemory) * 100;

    std::ostringstream oss;

    oss << "-------------------------------------------\n";
    oss << "|   VMSTAT V01.00 Driver Version: 01.00   |\n";
    oss << "|             PAGING ALLOCATOR            |\n";
    oss << "-------------------------------------------\n";
    oss << "Total Memory: " << maxOverAllMemory << " KiB\n";
    oss << "Used Memory: " << usedMemory << " KiB\n";
    oss << "Free Memory: " << freeMemory << " KiB\n";
    oss << "Idle CPU Ticks: " << InactiveCycles << "\n";
    oss << "Active CPU Ticks: " << activeCycles << "\n";
    oss << "Total CPU Ticks: " << cpuCycles << "\n";
    oss << "Num Paged In: " << numPagedIn << " \n";
    oss << "Num Paged Out: " << numPagedOut << " \n";
    oss << "-------------------------------------------\n\n";

    std::cout << oss.str();
}

bool PagingAllocator::isProcessAllocated(std::shared_ptr<AttachedProcess> process)
{
    for (const auto& allocatedProcess : processList) {
        if (allocatedProcess == process) {  
            return true;
        }
    }
    return false;
}

std::shared_ptr<AttachedProcess> PagingAllocator::getOldestPreemptedProcess()
{
    std::shared_ptr<AttachedProcess> oldestProcess = nullptr;

    for (auto& process : processList) {
        if (process && process->getProcessState() == AttachedProcess::WAITING) {

            if (!oldestProcess || process->getStartTime() < oldestProcess->getStartTime()) {
                oldestProcess = process;
            }
        }
    }

    return oldestProcess;
}

uint32_t PagingAllocator::getNumPagedIn()
{
	return numPagedIn;
}

uint32_t PagingAllocator::getNumPagedOut()
{
	return numPagedOut;
}
