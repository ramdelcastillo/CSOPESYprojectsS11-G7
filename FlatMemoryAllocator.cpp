#include "FlatMemoryAllocator.h"

FlatMemoryAllocator::FlatMemoryAllocator()
    : IMemoryAllocator()
{
    ConsoleManager* consoleManager = ConsoleManager::getInstance();

    maxOverAllMemory = consoleManager->getCPUScheduler()->getMaxOverallMemory();

    memoryPartitions.push_back({ 1, maxOverAllMemory , true, nullptr });
}

bool FlatMemoryAllocator::allocate(std::shared_ptr<AttachedProcess> process)
{
    uint32_t processSize = process->getMemorySize();

    for (auto& partition : memoryPartitions) {
        if (partition.isAllocatable && (partition.end - partition.start + 1) >= processSize) {
            uint32_t allocatedEnd = partition.start + processSize - 1;

            partition.isAllocatable = false;
            partition.process = process;

            uint32_t originalEnd = partition.end;
            partition.end = allocatedEnd;

            uint32_t remainingStart = allocatedEnd + 1;
            if (remainingStart <= originalEnd) {
                memoryPartitions.push_back({ remainingStart, originalEnd, true, nullptr });
            }

            numPagedIn++; 
            return true;
        }
    }
    return false;
}

void FlatMemoryAllocator::deallocate(int pid)
{
    for (auto& partition : memoryPartitions) {
        if (partition.process && partition.process->getPid() == pid) {
        
            partition.isAllocatable = true;
            partition.process = nullptr;

            std::sort(memoryPartitions.begin(), memoryPartitions.end(),
                [](const MemoryPartition& a, const MemoryPartition& b) {
                    return a.start < b.start;
                });

            mergeFreeMemoryPartitions();
            numPagedOut++;
            return;
        }
    }
    std::cout << "Process with PID " << pid << " not found in memory.\n";
}

void FlatMemoryAllocator::visualizeMemory()
{
    ConsoleManager* consoleManager = ConsoleManager::getInstance();
    int busyCores = consoleManager->getCPUScheduler()->getAmountOfWorkingCores();
    int totalCores = consoleManager->getCPUScheduler()->getNumCPU();
    int availableCores = totalCores - busyCores;
    float cpuUtilization = (static_cast<float>(busyCores) * 100) / totalCores;
    std::ostringstream oss;

    uint32_t usedMemory = 0;
    uint32_t totalMemory = maxOverAllMemory;
    for (const auto& partition : memoryPartitions) {
        if (!partition.isAllocatable) {
            usedMemory += (partition.end - partition.start + 1);
        }
    }

    float memoryUsage = (static_cast<float>(usedMemory) / totalMemory) * 100;

    oss << "-------------------------------------------\n";
    oss << "| PROCESS-SMI V01.00 Driver Version: 01.00|\n";
    oss << "|          FLAT MEMORY ALLOCATOR          |\n";
    oss << "-------------------------------------------\n";
    oss << std::fixed << std::setprecision(2);
    oss << "CPU-Util: " << cpuUtilization << "%\n";
    oss << "Memory Usage: " << usedMemory << "KiB / " << totalMemory << "KiB\n";
    oss << "Memory Util: " << memoryUsage << "%\n";
    oss << "\n===========================================\n";
    oss << "Running processes and memory usage:\n";
    oss << "-------------------------------------------\n";

    std::sort(memoryPartitions.begin(), memoryPartitions.end(),
        [](const MemoryPartition& a, const MemoryPartition& b) {
            return a.start < b.start;
        });

    for (const auto& partition : memoryPartitions) {
        if (!partition.isAllocatable && partition.process) {
            oss << partition.process->getProcessName()
                << " (" << partition.process->getMemorySize() << "KiB)\n";
        }
    }
    oss << "-------------------------------------------\n";
    oss << "\n";

    std::cout << oss.str();
}

void FlatMemoryAllocator::displayStats()
{
    ConsoleManager* consoleManager = ConsoleManager::getInstance();
    uint32_t activeCycles = consoleManager->getCPUScheduler()->getActiveCycles();
    uint32_t cpuCycles = consoleManager->getCPUScheduler()->getCPUCycles();
    uint32_t InactiveCycles = consoleManager->getCPUScheduler()->getInactiveCycles();
    uint32_t usedMemory = 0;
    uint32_t totalMemory = maxOverAllMemory; 
    for (const auto& partition : memoryPartitions) {
        if (!partition.isAllocatable) {
            usedMemory += (partition.end - partition.start + 1);
        }
    }

    uint32_t freeMemory = totalMemory - usedMemory; 
    float memoryUsage = (static_cast<float>(usedMemory) / totalMemory) * 100;

    std::ostringstream oss;

    oss << "-------------------------------------------\n";
    oss << "|   VMSTAT V01.00 Driver Version: 01.00   |\n";
    oss << "|          FLAT MEMORY ALLOCATOR          |\n";
    oss << "-------------------------------------------\n";
    oss << "Total Memory: " << totalMemory << " KiB\n";
    oss << "Used Memory: " << usedMemory << " KiB\n";
    oss << "Free Memory: " << freeMemory << " KiB\n";
    oss << "Idle CPU Ticks: " << InactiveCycles << "\n";
    oss << "Active CPU Ticks: " << activeCycles << "\n";
    oss << "Total CPU Ticks: " << cpuCycles << "\n";
    oss << "Num Paged In: " << numPagedIn << "\n";
    oss << "Num Paged Out: " << numPagedOut << "\n";
    oss << "-------------------------------------------\n\n";

    std::cout << oss.str();
}

std::shared_ptr<AttachedProcess> FlatMemoryAllocator::getOldestPreemptedProcess()
{
    std::shared_ptr<AttachedProcess> oldestProcess = nullptr;

    for (auto& partition : memoryPartitions) {
        if (partition.process && partition.process->getProcessState() == AttachedProcess::WAITING) {
            if (!oldestProcess || partition.process->getStartTime() < oldestProcess->getStartTime()) {
                oldestProcess = partition.process;
            }
        }
    }

    return oldestProcess;
}

void FlatMemoryAllocator::mergeFreeMemoryPartitions()
{
    for (size_t i = 0; i < memoryPartitions.size() - 1; ++i) {
        auto& current = memoryPartitions[i];
        auto& next = memoryPartitions[i + 1];

        if (current.isAllocatable && next.isAllocatable && (current.end + 1 == next.start)) {
            current.end = next.end;
            memoryPartitions.erase(memoryPartitions.begin() + i + 1);
            --i;
        }
    }
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




