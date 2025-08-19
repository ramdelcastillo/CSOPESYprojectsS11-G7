#include "CPUWorker.h"
#include "AttachedProcess.h"
#include "ConsoleManager.h"

CPUWorker::CPUWorker(int cpuId) : cpuId{ cpuId }, working(false) {}

CPUWorker::~CPUWorker() {
    stop();
}

int CPUWorker::getCpuId() const {
    return cpuId.load();
}

bool CPUWorker::canAssignNewProcess() const
{
    ConsoleManager* consoleManager = ConsoleManager::getInstance();
    return assignedProcess == nullptr || assignedProcess->isStateFinished() 
        || (assignedProcess->getProcessState() == AttachedProcess::WAITING && 
       consoleManager->getConfigParser()->getCurrentSchedulingAlgo() == "rr");
}

bool CPUWorker::isWorking()
{
    return working;
}

std::shared_ptr<AttachedProcess> CPUWorker::getAssignedProcess()
{
    return assignedProcess;
}

bool CPUWorker::hasAssignedProcess()
{
    return assignedProcess != nullptr;
}

void CPUWorker::setRunning()
{
    working = true;
}

void CPUWorker::assignProcess(std::shared_ptr<AttachedProcess> process) {
    process->setAssignedCPUID(getCpuId());
    this->assignedProcess = process;
}

void CPUWorker::start() {
    ConsoleManager* consoleManager = ConsoleManager::getInstance();
    if (consoleManager->getCPUScheduler()->getCurrentSchedulingAlgo() == "fcfs") {
        working = true;
        assignedProcess->setProcessState("RUNNING");
        if (working && assignedProcess != nullptr) {
            workerThread = std::thread(&CPUWorker::runFCFS, this, consoleManager->getCPUScheduler()->getDelaysPerExecution() + 1);
            workerThread.detach();
        }
    }  
    else if (consoleManager->getCPUScheduler()->getCurrentSchedulingAlgo() == "rr") {
        working = true;
        assignedProcess->setProcessState("RUNNING");
        if (working && assignedProcess != nullptr) {
            workerThread = std::thread(&CPUWorker::runRR, this, consoleManager->getCPUScheduler()->getDelaysPerExecution() + 1, consoleManager->getCPUScheduler()->getQuantumCycles(), consoleManager->getCPUScheduler()->getNumCPU());
            workerThread.detach();
        }
    }
}

void CPUWorker::stop() {
    working = false;
}

std::mutex& CPUWorker::getMutex()
{
    return printMutex;
}

void CPUWorker::runFCFS(uint32_t delayPerExecution) {
    ConsoleManager* consoleManager = ConsoleManager::getInstance();
    consoleManager->getCPUScheduler()->incrementWorkingCores();
    uint32_t currentCycleCount = consoleManager->getCPUScheduler()->getCPUCycles();
    uint32_t targetCycleCount = currentCycleCount + delayPerExecution;

    while (!assignedProcess->isFinished()) {        
        while (consoleManager->getCPUScheduler()->getCPUCycles() < targetCycleCount) {
          /*  std::this_thread::sleep_for(std::chrono::microseconds(10000)); */
        }
       
        {
            std::lock_guard<std::mutex> lock(getMutex());
            assignedProcess->executeCurrentCommand();
            assignedProcess->moveToNextLine(); 
        }

        targetCycleCount += delayPerExecution;
    }
    time_t now = time(0);
    assignedProcess->setFinishedTime(now);
    assignedProcess->setProcessState("FINISHED");
    consoleManager->getCPUScheduler()->decrementWorkingCores();
    stop();
}

void CPUWorker::runRR(uint32_t delayPerExecution, uint32_t quantumCommands, int totalCPUs) {
    ConsoleManager* consoleManager = ConsoleManager::getInstance();
    consoleManager->getCPUScheduler()->incrementWorkingCores();
    uint32_t currentCycleCount = consoleManager->getCPUScheduler()->getCPUCycles();
    uint32_t targetCycleCount = currentCycleCount + delayPerExecution;
    uint32_t commandCounter = 0;
    bool prevIsActingAsFCFS = true; 

    while (!assignedProcess->isFinished()) {
        int runningProcessesCount = consoleManager->getCPUScheduler()->getAmountOfRunningProcesses();
        int readyQueueCount = consoleManager->getCPUScheduler()->getAmountOfProcessesInReadyQueue();
        bool isActingAsFCFS = ((runningProcessesCount + readyQueueCount) <= totalCPUs);

        if (!isActingAsFCFS && commandCounter == quantumCommands) {
            commandCounter = 0;
            break;
        }

        while (consoleManager->getCPUScheduler()->getCPUCycles() < targetCycleCount) {
       /*     std::this_thread::sleep_for(std::chrono::microseconds(10000)); */
            /*     std::this_thread::sleep_for(std::chrono::milliseconds(10000)); */
        }

        {
            std::lock_guard<std::mutex> lock(getMutex());
            assignedProcess->executeCurrentCommand();
            assignedProcess->moveToNextLine();
        }

        commandCounter++;
      
        targetCycleCount += delayPerExecution;
        prevIsActingAsFCFS = isActingAsFCFS;
    }

    if (!assignedProcess->isFinished()) {
        assignedProcess->setProcessState("WAITING");
        assignedProcess->setAssignedCPUID(-1);
        consoleManager->getCPUScheduler()->addProcessToQueue(assignedProcess);
    }
    else {
        time_t now = time(0);
        assignedProcess->setFinishedTime(now);
        assignedProcess->setProcessState("FINISHED");
    }
    stop(); 
    consoleManager->getCPUScheduler()->decrementWorkingCores();
}




//DEBUGGER STATEMENT
/*                 std::cout << "Command " << assignedProcess->getCurrentLineOfInstruction()
            << " of process " << assignedProcess->getProcessName()
            << " executed at CPU cycle: " << targetCycleCount << "CPU Cycle Count: " << consoleManager->getCPUScheduler()->getCPUCycles() << std::endl;*/


