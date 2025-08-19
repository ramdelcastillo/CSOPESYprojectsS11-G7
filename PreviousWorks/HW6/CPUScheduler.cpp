#include "CPUScheduler.h"
#include "ConsoleManager.h"
#include "ProcessConsole.h"
#include "AttachedProcess.h"

CPUScheduler::CPUScheduler(int nCores, std::string currentSchedulingAlgo, int quantumCycles, int batchProcessFreq, int minIns, int maxIns, int delaysPerExecution)
    : nCores(nCores), currentSchedulingAlgo(currentSchedulingAlgo), quantumCycles(quantumCycles), batchProcessFreq(batchProcessFreq), minIns(minIns), maxIns(maxIns), delaysPerExecution(delaysPerExecution)
{
    ConsoleManager* consoleManager = ConsoleManager::getInstance();
    for (int i = 0; i < nCores; i++) {
        cpuWorkers.push_back(std::make_unique<CPUWorker>(i));
    }

    for (int i = 1; i <= 10; ++i) {
        consoleManager->addOneToMadePid();
        int processId = consoleManager->getMadePid();
        std::string screenName = "process_" + std::to_string(processId);

        std::shared_ptr<AttachedProcess> process = std::make_shared<AttachedProcess>(processId, screenName);

        std::shared_ptr<ProcessConsole> newConsole = std::make_shared<ProcessConsole>(process);

        consoleManager->addToConsoleTable(screenName, newConsole);

        addProcessToQueue(process);
    }

}

void CPUScheduler::addProcessToQueue(std::shared_ptr<AttachedProcess> process) {
    std::lock_guard<std::mutex> lock(schedulerMutex); 
    readyQueue.push(process);
}

std::atomic<bool> CPUScheduler::isScheduling() {
    return scheduling.load();
}

void CPUScheduler::start() {
    scheduling.store(true);
    schedulerThread = std::thread(&CPUScheduler::firstComeFirstServe, this);
    schedulerThread.detach();
}

void CPUScheduler::stop() {
    scheduling.store(false);
}

void CPUScheduler::firstComeFirstServe() {
    while (scheduling.load()) {


        for (int i = 0; i < nCores; ++i) { 
            CPUWorker* worker = getCPUWorker(i); 

            if (worker && worker->canAssignNewProcess()) { 
                std::shared_ptr<AttachedProcess> process; 

                {
                    std::lock_guard<std::mutex> lock(schedulerMutex); 
                    if (!readyQueue.empty()) {
                        process = readyQueue.front(); 
                        readyQueue.pop(); 
                    }
                }

                if (process) {
                    worker->assignProcess(process); 
                    worker->start();

                    if (process->getProcessState() == AttachedProcess::RUNNING) {
                        runningProcesses.push_back(process);
                    }

                }

            }
        }

        updateFinishedProcesses();
    }
  /*  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    updateFinishedProcesses();*/
}


std::vector<std::shared_ptr<AttachedProcess>> CPUScheduler::getRunningProcesses() {
    return runningProcesses;
}

std::vector<std::shared_ptr<AttachedProcess>> CPUScheduler::getFinishedProcesses() {
    return finishedProcesses;
}

CPUWorker* CPUScheduler::getCPUWorker(int cpuId) {
    for (const auto& worker : cpuWorkers) {
        if (worker->getCpuId() == cpuId && !worker->isWorking()) {
            return worker.get();  
        }
    }
    return nullptr; 
}

bool CPUScheduler::hasAvailableCPUWorker() {
    for (const auto& worker : cpuWorkers) {
        if (!worker->isWorking()) {
            return true;  
        }
    }
    return false;  
}

void CPUScheduler::updateFinishedProcesses() {
    std::lock_guard<std::mutex> lock(schedulerMutex); 

    for (auto it = runningProcesses.begin(); it != runningProcesses.end(); ) {
        if ((*it)->getProcessState() == AttachedProcess::FINISHED) {
            //std::cout << "Moving Process ID: " << (*it)->getPid() << " to finishedProcesses." << std::endl; // Log movement
            finishedProcesses.push_back(*it); 
            it = runningProcesses.erase(it); 
        }
        else {
            ++it; 
        }
    }
}

void CPUScheduler::printReadyQueue()
{
    if (readyQueue.empty()) {
        std::cout << "Ready Queue is empty." << std::endl;
        return;
    }

    std::queue<std::shared_ptr<AttachedProcess>> tempQueue = readyQueue;

    std::cout << "Ready Queue:" << std::endl;

    while (!tempQueue.empty()) {
        std::shared_ptr<AttachedProcess> process = tempQueue.front();
        std::cout << "Process ID: " << process->getPid()
            << ", Process Name: " << process->getProcessName() << std::endl;

        tempQueue.pop();
    }
    std::cout << std::endl;
}

void CPUScheduler::printRunningAndFinishedProcesses()
{
    updateFinishedProcesses();
    std::cout << "\n";
    int totalCores = nCores;
    int busyCores = 0;

    for (const auto& worker : cpuWorkers)
    {
        if (!worker->canAssignNewProcess())
        {
            busyCores++;
        }
    }

    int availableCores = totalCores - busyCores;
    int cpuUtilization = (busyCores * 100) / totalCores;

    std::cout << "CPU Utilization: " << cpuUtilization << "%\n";
    std::cout << "Cores used: " << busyCores << "\n";
    std::cout << "Cores available: " << availableCores << "\n\n";
    std::cout << "--------------------------\n";

    std::vector<std::shared_ptr<AttachedProcess>> runningProcesses = getRunningProcesses();
    std::cout << "Running processes:\n";
    for (const auto& process : runningProcesses) {
        int currentInstruction = process->getCurrentLineOfInstruction();
        int totalInstructions = process->getLinesOfCode();
        int coreID = process->getAssignedCPUID();
        std::string processName = process->getProcessName();
        std::time_t startTime = process->getStartTime();

        std::tm now;
        localtime_s(&now, &startTime);

        std::cout << std::left << std::setw(12) << processName                        
            << " (" << std::put_time(&now, "%m/%d/%Y %I:%M:%S%p") << ")    "     
            << std::setw(6) << "Core:" << std::setw(0) << coreID << "   "        
            << currentInstruction << "/" << totalInstructions << "\n";         
    }

    std::vector<std::shared_ptr<AttachedProcess>> finishedProcesses = getFinishedProcesses();
    std::cout << "\nFinished processes:\n";
    for (const auto& process : finishedProcesses) {
        int currentInstruction = process->getCurrentLineOfInstruction();
        int totalInstructions = process->getLinesOfCode();
        std::string processName = process->getProcessName();
        std::time_t startTime = process->getStartTime();

        std::tm now;
        localtime_s(&now, &startTime);

        std::cout << std::left << std::setw(12) << processName                     
            << " (" << std::put_time(&now, "%m/%d/%Y %I:%M:%S%p") << ")    "     
            << std::setw(10) << "Finished"                                    
            << currentInstruction << "/" << totalInstructions << "\n";         

    }

    std::cout << "--------------------------\n";
}




