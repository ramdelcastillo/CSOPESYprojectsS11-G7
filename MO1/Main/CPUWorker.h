#pragma once
#include <thread>
#include <mutex>
#include <memory>  
#include "AttachedProcess.h"

class CPUWorker
{
public:
    CPUWorker(int cpuId);
    ~CPUWorker();

    void assignProcess(std::shared_ptr<AttachedProcess> process);
    void start();
    void stop();
    std::mutex& getMutex();
    int getCpuId() const;
    bool canAssignNewProcess() const;
    bool isWorking();
    std::shared_ptr<AttachedProcess> getAssignedProcess();
    bool hasAssignedProcess();
    void setRunning();
private:
    void runRR(uint32_t delayPerExecution, uint32_t quantumCommands, int totalCPUs);
    void runFCFS(uint32_t delayPerExecution);
    std::atomic<int> cpuId{};
    std::shared_ptr<AttachedProcess> assignedProcess = nullptr;  
    bool working;
    std::thread workerThread;
    std::mutex printMutex;
};
