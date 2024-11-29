#include "ConfigParser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

ConfigParser::ConfigParser(const std::string& configFilePath) {
    parseConfigFile(configFilePath);
}

void ConfigParser::parseConfigFile(const std::string& configFilePath) {
    std::ifstream file(configFilePath);

    if (!file.is_open()) {
        throw std::runtime_error("Unable to open config file: " + configFilePath);
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        iss >> key;

        if (key == "num-cpu") {
            iss >> nCPU;
        }
        else if (key == "scheduler") {
            iss >> currentSchedulingAlgo;
            if (currentSchedulingAlgo.front() == '"' && currentSchedulingAlgo.back() == '"') {
                currentSchedulingAlgo = currentSchedulingAlgo.substr(1, currentSchedulingAlgo.length() - 2);
            }
        }
        else if (key == "quantum-cycles") {
            iss >> quantumCycles;
        }
        else if (key == "batch-process-freq") {
            iss >> batchProcessFreq;
        }
        else if (key == "min-ins") {
            iss >> minIns;
        }
        else if (key == "max-ins") {
            iss >> maxIns;
        }
        else if (key == "delay-per-exec") {
            iss >> delaysPerExecution;
        }
        else if (key == "max-overall-mem") {
            iss >> maxOverallMemory;
        }
        else if (key == "mem-per-frame") {
            iss >> memoryPerFrame;
        }
        else if (key == "min-mem-per-proc") {
            iss >> minMemoryPerProcess;
        }
        else if (key == "max-mem-per-proc") {
            iss >> maxMemoryPerProcess;
        }
    }

    file.close();
}

uint32_t ConfigParser::getNumCPU() const {
    return nCPU;
}

std::string ConfigParser::getCurrentSchedulingAlgo() const {
    return currentSchedulingAlgo;
}

uint32_t ConfigParser::getQuantumCycles() const {
    return quantumCycles;
}

uint32_t ConfigParser::getBatchProcessFreq() const {
    return batchProcessFreq;
}

uint32_t ConfigParser::getMinIns() const {
    return minIns;
}

uint32_t ConfigParser::getMaxIns() const {
    return maxIns;
}

uint32_t ConfigParser::getDelaysPerExecution() const {
    return delaysPerExecution;
}

uint32_t ConfigParser::getMaxOverallMemory() const
{
    return maxOverallMemory;
}

uint32_t ConfigParser::getMemoryPerFrame() const
{
    return memoryPerFrame;
}

uint32_t ConfigParser::getMinMemoryPerProcess() const
{
    return minMemoryPerProcess;
}

uint32_t ConfigParser::getMaxMemoryPerProcess() const
{
    return maxMemoryPerProcess;
}

//uint32_t ConfigParser::getMemoryPerProcess() const
//{
//    return memoryPerProcess;
//}

void ConfigParser::printConfigParameters() const
{
    std::cout << "num-cpu " << nCPU << "\n";
    std::cout << "scheduler \"" << currentSchedulingAlgo << "\"\n";
    std::cout << "quantum-cycles " << quantumCycles << "\n";
    std::cout << "batch-process-freq " << batchProcessFreq << "\n";
    std::cout << "min-ins " << minIns << "\n";
    std::cout << "max-ins " << maxIns << "\n";
    std::cout << "delay-per-exec " << delaysPerExecution << "\n\n";
}

uint32_t ConfigParser::getRandomCommandListSize(uint32_t minIns, uint32_t maxIns)
{
    if (minIns == maxIns) {
        return minIns;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(minIns, maxIns);

    return distrib(gen);
}

