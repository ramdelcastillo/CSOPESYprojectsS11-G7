#include "CPUWorker.h"
#include "AttachedProcess.h"

CPUWorker::CPUWorker(int cpuId) : cpuId(cpuId), working(false) {}

CPUWorker::~CPUWorker() {
    stop();
}

int CPUWorker::getCpuId() const {
    return cpuId;
}

bool CPUWorker::canAssignNewProcess() const
{
    return assignedProcess == nullptr || assignedProcess->isStateFinished();
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

void CPUWorker::assignProcess(std::shared_ptr<AttachedProcess> process) {
    process->setAssignedCPUID(getCpuId());
    this->assignedProcess = process;
}

void CPUWorker::assignProcessWChecker(std::shared_ptr<AttachedProcess> process)
{
    if (canAssignNewProcess()) {
        this->assignedProcess = process;
        start();
    }
    else {
        std::cout << "CPU " << cpuId << " is still busy with process "
            << assignedProcess->getPid() << std::endl;
    }
}

void CPUWorker::start() {
    working = true;
    assignedProcess->setProcessState("RUNNING");
    if (working && assignedProcess != nullptr) {
        workerThread = std::thread(&CPUWorker::run, this);
        workerThread.detach();
    }
}

void CPUWorker::stop() {
    working = false;
}

std::mutex& CPUWorker::getMutex()
{
    return printMutex;
}

void CPUWorker::run() {
    while (!assignedProcess->isFinished()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // delay 
        {
            std::lock_guard<std::mutex> lock(getMutex());

            assignedProcess->setProcessState("RUNNING");
            assignedProcess->executeCurrentCommand(getCpuId(), assignedProcess->getProcessName());
            assignedProcess->moveToNextLine();
            /*std::cout << "CPU: " << cpuId << "-"<< assignedProcess->getCurrentLineOfInstruction() << "/" << assignedProcess->getLinesOfCode() << "-" << assignedProcess->getPid() << std::endl;*/
        }
    }   
    time_t now = time(0);
    assignedProcess->setFinishedTime(now);
 /*   std::cout << std::put_time(&localTime, "%a %b %d %H:%M:%S %Y") << std::endl;*/
    assignedProcess->setProcessState("FINISHED");
    //std::cout << "Process ID: " << assignedProcess->getPid() << " has finished." << std::endl; // Log completion
    stop();
}
