#pragma once
#include <memory>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>
#include <iomanip>
#include "CPUWorker.h"
#include "AttachedProcess.h"

class CPUScheduler {
public:
    CPUScheduler(int nCores, std::string currentSchedulingAlgo, int quantumCycles, int batchProcessFreq, int minIns, int maxIns, int delaysPerExecution);
    ~CPUScheduler() = default;

    void addProcessToQueue(std::shared_ptr<AttachedProcess> process);
    std::atomic<bool> isScheduling();
    void start();
    void stop();
    void firstComeFirstServe();
    std::vector<std::shared_ptr<AttachedProcess>> getRunningProcesses();
    std::vector<std::shared_ptr<AttachedProcess>> getFinishedProcesses();
    CPUWorker* getCPUWorker(int cpuId);
    bool hasAvailableCPUWorker();
    void updateFinishedProcesses();
    void printReadyQueue();
    void printRunningAndFinishedProcesses();
    /*  void scheduleNextProcess();*/

private:
    int nCores;
    std::string currentSchedulingAlgo;
    int quantumCycles;
    int batchProcessFreq;
    int minIns;
    int maxIns;
    int delaysPerExecution;

    std::queue<std::shared_ptr<AttachedProcess>> readyQueue;
    std::vector<std::unique_ptr<CPUWorker>> cpuWorkers;
    std::vector<std::shared_ptr<AttachedProcess>> runningProcesses;
    std::vector<std::shared_ptr<AttachedProcess>> finishedProcesses;
    std::mutex schedulerMutex;
    std::thread schedulerThread;
    std::atomic<bool> scheduling{ false };
};
