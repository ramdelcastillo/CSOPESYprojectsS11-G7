#pragma once
#include <memory>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <random>
#include <numeric>
#include <algorithm>
#include <sstream>
#include <condition_variable>
#include "CPUWorker.h"
#include "AttachedProcess.h"

class CPUScheduler {
public:
    CPUScheduler(uint32_t nCores, std::string currentSchedulingAlgo, uint32_t quantumCycles, uint32_t batchProcessFreq, uint32_t minIns, uint32_t maxIns, uint32_t delaysPerExecution, uint32_t maxOverallMemory, uint32_t memoryPerFrame, uint32_t minMemoryPerProcess, uint32_t maxMemoryPerProcess);
    ~CPUScheduler() = default;

    void addProcessToQueue(std::shared_ptr<AttachedProcess> process);
    std::atomic<bool> isScheduling();
    void start();
    void stop();
    void schedulerFlatLoop();
    void schedulerPagingLoop();
    std::vector<std::shared_ptr<AttachedProcess>> getRunningProcesses();
    std::vector<std::shared_ptr<AttachedProcess>> getFinishedProcesses();
    CPUWorker* getCPUWorker(int cpuId);
    void updateFinishedProcesses();
    void printReadyQueue();
    void printRunningAndFinishedProcesses();
    void batchProcessLoop();
    uint32_t getCPUCycles();
    void logRunningAndFinishedProcessesToFile();
    uint32_t getAmountOfRunningProcesses();
    uint32_t getAmountOfProcessesInReadyQueue();
    uint32_t getAmountOfWorkingCores();
    uint32_t getDelaysPerExecution();
    uint32_t getQuantumCycles();
    uint32_t getNumCPU();
    uint32_t getMaxOverallMemory();
    uint32_t getMemoryPerFrame();
    uint32_t getMemoryPerProcess();
    uint32_t getMinMemoryPerProcess();
    uint32_t getMaxMemoryPerProcess();
    std::mutex& getSchedulerMutex();
    std::queue<std::shared_ptr<AttachedProcess>> getReadyQueue();
    std::string getCurrentSchedulingAlgo();
    void incrementWorkingCores();
    void decrementWorkingCores();
    void incrementActiveCycles();
    void incrementInactiveCycles();
    uint32_t getActiveCycles();
    uint32_t getInactiveCycles();
 
private:
    std::string currentSchedulingAlgo;
    uint32_t quantumCycles;
    uint32_t batchProcessFreq;
    uint32_t minIns;
    uint32_t maxIns;
    uint32_t delaysPerExecution;
    uint32_t maxOverallMemory;
    uint32_t memoryPerFrame;
    uint32_t memoryPerProcess = 0;
    uint32_t minMemoryPerProcess;
    uint32_t maxMemoryPerProcess;

    uint32_t nCores;
    std::atomic<uint32_t> cpuCycles;
    std::atomic<uint32_t> cpuActiveCycles;
    std::atomic<uint32_t> cpuInactiveCycles;
    std::atomic<uint32_t> nAmountOfWorkingCores{ 0 };
    std::queue<std::shared_ptr<AttachedProcess>> readyQueue;
    std::vector<std::unique_ptr<CPUWorker>> cpuWorkers;
    std::vector<std::shared_ptr<AttachedProcess>> runningProcesses;
    std::vector<std::shared_ptr<AttachedProcess>> finishedProcesses;

    std::mutex backingStoreMutex;
    std::mutex schedulerMutex;
    std::condition_variable schedulerCV;
    std::thread cpuUsageThread;
    std::thread schedulerThread;
    std::thread batchProcessThread;
    std::atomic<bool> scheduling{ false };
};
