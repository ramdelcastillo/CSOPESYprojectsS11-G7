#include "CPUScheduler.h"
#include "ConsoleManager.h"
#include "ProcessConsole.h"
#include "AttachedProcess.h"

CPUScheduler::CPUScheduler(uint32_t nCores, std::string currentSchedulingAlgo, uint32_t quantumCycles, uint32_t batchProcessFreq, uint32_t minIns, uint32_t maxIns, uint32_t delaysPerExecution, uint32_t maxOverallMemory, uint32_t memoryPerFrame, uint32_t minMemoryPerProcess, uint32_t maxMemoryPerProcess)
    : nCores(nCores), currentSchedulingAlgo(currentSchedulingAlgo), quantumCycles(quantumCycles), batchProcessFreq(batchProcessFreq), minIns(minIns), maxIns(maxIns), delaysPerExecution(delaysPerExecution), nAmountOfWorkingCores(0), maxOverallMemory(maxOverallMemory), memoryPerFrame(memoryPerFrame), minMemoryPerProcess(minMemoryPerProcess), maxMemoryPerProcess(maxMemoryPerProcess)
{
    ConsoleManager* consoleManager = ConsoleManager::getInstance();
    for (uint32_t i = 0; i < nCores; i++) {
        cpuWorkers.push_back(std::make_unique<CPUWorker>(i));
    }

    if (consoleManager->getConfigParser()->getMaxOverallMemory() == consoleManager->getConfigParser()->getMemoryPerFrame()) {
        std::thread schedulerThread(&CPUScheduler::schedulerFlatLoop, this);
        schedulerThread.detach();
    }
    else {
        std::thread schedulerThread(&CPUScheduler::schedulerPagingLoop, this);
        schedulerThread.detach();
    }

}

void CPUScheduler::addProcessToQueue(std::shared_ptr<AttachedProcess> process) {
    {
        std::lock_guard<std::mutex> lock(schedulerMutex);
        readyQueue.push(process); 
    }
}

std::atomic<bool> CPUScheduler::isScheduling() {
    return scheduling.load();
}

void CPUScheduler::start() {
    if (!scheduling.load()) {
        scheduling.store(true);
        batchProcessThread = std::thread(&CPUScheduler::batchProcessLoop, this);
        batchProcessThread.detach();
        std::cout << "Scheduler has started.\n" << std::endl;
    }
    else {
        std::cout << "Scheduler has already started.\n" << std::endl;
    }
    
}

void CPUScheduler::stop() {
    if (scheduling.load()) {
        scheduling.store(false);
        std::cout << "Scheduler has stopped.\n" << std::endl;
    }
    else {
        std::cout << "Scheduler has not started.\n" << std::endl;
    }
    
}

uint32_t CPUScheduler::getCPUCycles()
{
    return cpuCycles.load();
}

uint32_t CPUScheduler::getAmountOfRunningProcesses()
{
    return (uint32_t) runningProcesses.size();
}

uint32_t CPUScheduler::getAmountOfProcessesInReadyQueue()
{
    return (uint32_t) readyQueue.size();
}

uint32_t CPUScheduler::getAmountOfWorkingCores()
{
    return nAmountOfWorkingCores.load();
}

uint32_t CPUScheduler::getDelaysPerExecution()
{
    return delaysPerExecution;
}

uint32_t CPUScheduler::getQuantumCycles()
{
    return quantumCycles;
}

uint32_t CPUScheduler::getNumCPU()
{
    return nCores;
}

uint32_t CPUScheduler::getMaxOverallMemory()
{
    return maxOverallMemory;
}

uint32_t CPUScheduler::getMemoryPerFrame()
{
    return memoryPerFrame;
}

uint32_t CPUScheduler::getMemoryPerProcess()
{
    return memoryPerProcess;
}

uint32_t CPUScheduler::getMinMemoryPerProcess()
{
    return minMemoryPerProcess;
}

uint32_t CPUScheduler::getMaxMemoryPerProcess()
{
    return maxMemoryPerProcess;
}

std::mutex& CPUScheduler::getSchedulerMutex()
{
    return schedulerMutex;
}

std::queue<std::shared_ptr<AttachedProcess>> CPUScheduler::getReadyQueue()
{
    return readyQueue;
}

std::string CPUScheduler::getCurrentSchedulingAlgo()
{
    return currentSchedulingAlgo;
}

void CPUScheduler::incrementWorkingCores()
{
    nAmountOfWorkingCores.fetch_add(1);
}

void CPUScheduler::decrementWorkingCores()
{
    nAmountOfWorkingCores.fetch_sub(1);
}

void CPUScheduler::incrementActiveCycles()
{
    cpuActiveCycles++;
}

void CPUScheduler::incrementInactiveCycles()
{
    cpuInactiveCycles++;
}

uint32_t CPUScheduler::getActiveCycles()
{
    return cpuActiveCycles.load();
}

uint32_t CPUScheduler::getInactiveCycles()
{
    return cpuInactiveCycles.load();
}

void CPUScheduler::batchProcessLoop() {
    ConsoleManager* consoleManager = ConsoleManager::getInstance();
    uint32_t lastCycleProcessed = 0;

    while (scheduling.load()) {

        uint32_t currentCycle = getCPUCycles();

        if (currentCycle % batchProcessFreq == 0 && currentCycle != lastCycleProcessed) {
            lastCycleProcessed = currentCycle;

            ConsoleManager* consoleManager = ConsoleManager::getInstance();
            consoleManager->addOneToMadePid();
            int processId = consoleManager->getMadePid();
            std::string processName = "process_" + std::to_string(processId);

            std::shared_ptr<AttachedProcess> process = std::make_shared<AttachedProcess>(processId, processName, memoryPerFrame, minMemoryPerProcess, maxMemoryPerProcess);
            std::shared_ptr<ProcessConsole> newConsole = std::make_shared<ProcessConsole>(process);

            consoleManager->addToConsoleTable(processName, newConsole);
            addProcessToQueue(process);
        }
    }
}

void CPUScheduler::schedulerFlatLoop() {
    ConsoleManager* consoleManager = ConsoleManager::getInstance();
    std::vector<uint32_t> coreIndices(nCores);
    std::iota(coreIndices.begin(), coreIndices.end(), 0);
    auto baseAllocator = consoleManager->getMemoryAllocator(); 
    auto flatMemoryAllocator = std::dynamic_pointer_cast<FlatMemoryAllocator>(baseAllocator);
    while (true) {
        cpuCycles++;

        if (getAmountOfWorkingCores() == 0) {
            incrementInactiveCycles();
        }

        if (getAmountOfWorkingCores() > 0) {
            incrementActiveCycles();
        }

        std::vector<uint32_t> availableWorkers;
        for (uint32_t i = 0; i < nCores; ++i) {
            CPUWorker* worker = getCPUWorker(i);
            if (worker && worker->canAssignNewProcess()) {
                availableWorkers.push_back(i);
            }
        }

        std::shuffle(availableWorkers.begin(), availableWorkers.end(), std::mt19937{ std::random_device{}() });

        if (!readyQueue.empty() && !availableWorkers.empty()) {
            for (const auto& coreIndex : availableWorkers) {
                CPUWorker* worker = getCPUWorker(coreIndex);
                std::shared_ptr<AttachedProcess> process;

                {
                    std::lock_guard<std::mutex> lock(schedulerMutex);
                    if (!readyQueue.empty()) {
                        process = readyQueue.front();
                        readyQueue.pop();
                    }
                }
                if (process) { //processes in ready queue
                    if (!flatMemoryAllocator->isProcessAllocated(process)) {

                        {
                            std::lock_guard<std::mutex> lock(backingStoreMutex);
                            bool check = consoleManager->getBackingStore()->loadProcess(process->getPid()); //REMOVE FROM BACKING STORE IF INSIDE BACKING STORE
                        }
                        
                        bool isAllocated = flatMemoryAllocator->allocate(process); //TRY TO ALLOCATE PROCESS

                        if (isAllocated) {                  //IF ALLOCATED
                            worker->assignProcess(process); //ASSIGN TO WORKER
                            worker->start();                //START THE WORKER
                            if (process->getProcessState() == AttachedProcess::RUNNING) {
                                //AS SOON AS CPU WORKER STARTS THE PROCESS, PROCESS STATE BECOMES RUNNING
                                runningProcesses.push_back(process);
                            }
                        }

                        if (!isAllocated) {                                                      // IF NOT ALLOCATED

                            auto processToBS = flatMemoryAllocator->getOldestPreemptedProcess(); //GET OLDEST PREEMPTED PROCCESS

                            if (!processToBS) { // IF PROCESS TO BACKING STORE IS NULL
                                std::lock_guard<std::mutex> lock(schedulerMutex);
                                readyQueue.push(process); // QUEUE THE PROCESS AGAIN
                                continue;  
                            }

                            flatMemoryAllocator->deallocate(processToBS->getPid());              //DEALLOCATE OLDEST PREEMPTED PROCESS
                            consoleManager->getBackingStore()->storeProcess(processToBS);        //STORE TO BACKING STORE
                           
                            bool is = flatMemoryAllocator->allocate(process); 
                            if (is) {
                                worker->assignProcess(process);                                     //ASSIGN TO WORKER
                                worker->start();
                                if (process->getProcessState() == AttachedProcess::RUNNING) {
                                    //AS SOON AS CPU WORKER STARTS THE PROCESS, PROCESS STATE BECOMES RUNNING
                                    runningProcesses.push_back(process);
                                }
                            }
                        }
                    }
                }
            }
        }
        updateFinishedProcesses();
    }
}

void CPUScheduler::schedulerPagingLoop()
{
    ConsoleManager* consoleManager = ConsoleManager::getInstance();
    std::vector<uint32_t> coreIndices(nCores);
    std::iota(coreIndices.begin(), coreIndices.end(), 0);
    auto baseAllocator = consoleManager->getMemoryAllocator(); 
    auto pagingAllocator = std::dynamic_pointer_cast<PagingAllocator>(baseAllocator);
    while (true) {
        cpuCycles++;

        if (getAmountOfWorkingCores() == 0) {
            incrementInactiveCycles();
        }

        if (getAmountOfWorkingCores() > 0) {
            incrementActiveCycles();
        }

        std::vector<uint32_t> availableWorkers;
        for (uint32_t i = 0; i < nCores; ++i) {
            CPUWorker* worker = getCPUWorker(i);
            if (worker && worker->canAssignNewProcess()) {
                availableWorkers.push_back(i);
            }
        }

        std::shuffle(availableWorkers.begin(), availableWorkers.end(), std::mt19937{ std::random_device{}() });

        if (!readyQueue.empty() && !availableWorkers.empty()) {
            for (const auto& coreIndex : availableWorkers) {
                CPUWorker* worker = getCPUWorker(coreIndex);
                std::shared_ptr<AttachedProcess> process;

                {
                    std::lock_guard<std::mutex> lock(schedulerMutex);
                    if (!readyQueue.empty()) {
                        process = readyQueue.front();
                        readyQueue.pop();
                    }
                }
                if (process) { //processes in ready queue
                    if (!pagingAllocator->isProcessAllocated(process)) {

                        {
                            std::lock_guard<std::mutex> lock(backingStoreMutex);
                            bool check = consoleManager->getBackingStore()->loadProcess(process->getPid()); //REMOVE FROM BACKING STORE IF INSIDE BACKING STOR
                        }

                        bool isAllocated = pagingAllocator->allocate(process); //TRY TO ALLOCATE PROCESS

                        if (isAllocated) {                  //IF ALLOCATED
                            worker->assignProcess(process); //ASSIGN TO WORKER
                            worker->start();                //START THE WORKER
                            if (process->getProcessState() == AttachedProcess::RUNNING) {
                                //AS SOON AS CPU WORKER STARTS THE PROCESS, PROCESS STATE BECOMES RUNNING
                                runningProcesses.push_back(process);
                            }
                        }

                        if (!isAllocated) {                                                      // IF NOT ALLOCATED

                            auto processToBS = pagingAllocator->getOldestPreemptedProcess(); //GET OLDEST PREEMPTED PROCCESS

                            if (!processToBS) { // IF PROCESS TO BACKING STORE IS NULL
                                std::lock_guard<std::mutex> lock(schedulerMutex);
                                readyQueue.push(process); // QUEUE THE PROCESS AGAIN
                                continue;
                            }

                            pagingAllocator->deallocate(processToBS->getPid());              //DEALLOCATE OLDEST PREEMPTED PROCESS
                            consoleManager->getBackingStore()->storeProcess(processToBS);        //STORE TO BACKING STORE

                            bool is = pagingAllocator->allocate(process);
                            if (is) {
                                worker->assignProcess(process);                                     //ASSIGN TO WORKER
                                worker->start();
                                if (process->getProcessState() == AttachedProcess::RUNNING) {
                                    //AS SOON AS CPU WORKER STARTS THE PROCESS, PROCESS STATE BECOMES RUNNING
                                    runningProcesses.push_back(process);
                                }
                            }
                        }
                    }
                }
            }
        }
        updateFinishedProcesses();
    }
}

void CPUScheduler::updateFinishedProcesses() {
    std::lock_guard<std::mutex> lock(schedulerMutex);

    auto it = std::remove_if(runningProcesses.begin(), runningProcesses.end(),
        [this](const std::shared_ptr<AttachedProcess>& process) {
            if (process->getProcessState() == AttachedProcess::FINISHED) {
                ConsoleManager* consoleManager = ConsoleManager::getInstance();
                consoleManager->getMemoryAllocator()->deallocate(process->getPid()); 
                finishedProcesses.push_back(process);
                return true;
            }
            return false;
        });

    runningProcesses.erase(it, runningProcesses.end());
}

//std::this_thread::sleep_for(std::chrono::nanoseconds(230)); // for 6 7
       /* std::this_thread::sleep_for(std::chrono::microseconds(500)); */
        //std::this_thread::sleep_for(std::chrono::nanoseconds(100));

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

void CPUScheduler::printReadyQueue()
{
    std::ostringstream buffer;

    if (readyQueue.empty()) {
        buffer << "Ready Queue is empty." << std::endl;
    }
    else {
        std::queue<std::shared_ptr<AttachedProcess>> tempQueue = readyQueue;

        buffer << "Ready Queue:" << std::endl;

        while (!tempQueue.empty()) {
            std::shared_ptr<AttachedProcess> process = tempQueue.front();
            buffer << "Process ID: " << process->getPid()
                << ", Process Name: " << process->getProcessName() << std::endl;

            tempQueue.pop();
        }
    }

    buffer << std::endl;
    std::cout << buffer.str();
}

void CPUScheduler::printRunningAndFinishedProcesses() {
    int busyCores = getAmountOfWorkingCores();
  /*  updateFinishedProcesses();*/
    std::vector<std::shared_ptr<AttachedProcess>> runningProcesses = getRunningProcesses();
    std::vector<std::shared_ptr<AttachedProcess>> finishedProcesses = getFinishedProcesses();
    int totalCores = nCores;
    int availableCores = totalCores - busyCores;
    float cpuUtilization = (static_cast<float>(busyCores) * 100) / totalCores;

    std::ostringstream outputBuffer;
    outputBuffer << std::fixed << std::setprecision(2);
    outputBuffer << "\nCPU Utilization: " << cpuUtilization << "%\n";
    outputBuffer << "Cores used: " << busyCores << "\n";
    outputBuffer << "Cores available: " << availableCores << "\n\n";
    outputBuffer << "--------------------------\n";

    std::sort(runningProcesses.begin(), runningProcesses.end(),
        [](const std::shared_ptr<AttachedProcess>& a, const std::shared_ptr<AttachedProcess>& b) {
            return a->getAssignedCPUID() > b->getAssignedCPUID();
        });

    outputBuffer << "Running processes:\n";
    for (const auto& process : runningProcesses) {
            int currentInstruction = process->getCurrentLineOfInstruction();
            int totalInstructions = process->getLinesOfCode();
            int coreID = process->getAssignedCPUID();
            if (coreID == -1) {
                continue; 
            }
            std::string processName = process->getProcessName();
            std::time_t startTime = process->getStartTime();

            std::tm now;
            localtime_s(&now, &startTime);

            outputBuffer << std::left << std::setw(12) << processName
                << " (" << std::put_time(&now, "%m/%d/%Y %I:%M:%S%p") << ")    "
                << std::setw(6) << "Core:" << std::setw(0) << coreID << "   "
                << currentInstruction << "/" << totalInstructions << "\n";
    }

    outputBuffer << "\nFinished processes:\n";
    for (const auto& process : finishedProcesses) {
        int currentInstruction = process->getCurrentLineOfInstruction();
        int totalInstructions = process->getLinesOfCode();
        std::string processName = process->getProcessName();
        std::time_t startTime = process->getStartTime();

        std::tm now;
        localtime_s(&now, &startTime);

        outputBuffer << std::left << std::setw(12) << processName
            << " (" << std::put_time(&now, "%m/%d/%Y %I:%M:%S%p") << ")    "
            << std::setw(10) << "Finished"
            << currentInstruction << "/" << totalInstructions << "\n";
    }

    outputBuffer << "--------------------------\n";

    std::cout << outputBuffer.str();
}

void CPUScheduler::logRunningAndFinishedProcessesToFile() {
    int busyCores = getAmountOfWorkingCores();
   /* updateFinishedProcesses();*/
    std::vector<std::shared_ptr<AttachedProcess>> runningProcesses = getRunningProcesses();
    std::vector<std::shared_ptr<AttachedProcess>> finishedProcesses = getFinishedProcesses();
    std::ofstream logFile("csopesy-log.txt");

    if (!logFile.is_open()) {
        std::cerr << "Error: Unable to open log file.\n";
        return;
    }
    
    std::ostringstream buffer;
    
    std::sort(runningProcesses.begin(), runningProcesses.end(),
        [](const std::shared_ptr<AttachedProcess>& a, const std::shared_ptr<AttachedProcess>& b) {
            return a->getAssignedCPUID() > b->getAssignedCPUID();
        });

    int totalCores = nCores;
    int availableCores = totalCores - busyCores;
    float cpuUtilization = (static_cast<float>(busyCores) * 100) / totalCores;
    buffer << std::fixed << std::setprecision(2);
    buffer << "CPU Utilization: " << cpuUtilization << "%\n";
    buffer << "Cores used: " << busyCores << "\n";
    buffer << "Cores available: " << availableCores << "\n\n";
    buffer << "--------------------------\n";

    buffer << "Running processes:\n";
    for (const auto& process : runningProcesses) {
            int currentInstruction = process->getCurrentLineOfInstruction();
            int totalInstructions = process->getLinesOfCode();
            int coreID = process->getAssignedCPUID();
            if (coreID == -1) {
                continue;
            }
            std::string processName = process->getProcessName();
            std::time_t startTime = process->getStartTime();

            std::tm now;
            localtime_s(&now, &startTime);

            buffer << std::left << std::setw(12) << processName
                << " (" << std::put_time(&now, "%m/%d/%Y %I:%M:%S%p") << ")    "
                << std::setw(6) << "Core:" << std::setw(0) << coreID << "   "
                << currentInstruction << "/" << totalInstructions << "\n";
    }

    buffer << "\nFinished processes:\n";
    for (const auto& process : finishedProcesses) {
        int currentInstruction = process->getCurrentLineOfInstruction();
        int totalInstructions = process->getLinesOfCode();
        std::string processName = process->getProcessName();
        std::time_t startTime = process->getStartTime();

        std::tm now;
        localtime_s(&now, &startTime);

        buffer << std::left << std::setw(12) << processName
            << " (" << std::put_time(&now, "%m/%d/%Y %I:%M:%S%p") << ")    "
            << std::setw(10) << "Finished"
            << currentInstruction << "/" << totalInstructions << "\n";
    }

    buffer << "--------------------------\n";

    logFile << buffer.str();
    logFile.close();

    std::filesystem::path currentPath = std::filesystem::current_path();
    std::filesystem::path filePath = currentPath / "csopesy-log.txt";
    std::cout << "Report generated at: " << filePath.string() << std::endl << std::endl;
}




