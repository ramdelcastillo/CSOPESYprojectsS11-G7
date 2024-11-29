#pragma once
#include <string>
#include <iomanip>
#include <random>

class ConfigParser {
public:
    ConfigParser(const std::string& configFilePath);  

    uint32_t getNumCPU() const;
    std::string getCurrentSchedulingAlgo() const;
    uint32_t getQuantumCycles() const;
    uint32_t getBatchProcessFreq() const;
    uint32_t getMinIns() const;
    uint32_t getMaxIns() const;
    uint32_t getDelaysPerExecution() const;
    uint32_t getMaxOverallMemory() const;
    uint32_t getMemoryPerFrame() const;
    uint32_t getMinMemoryPerProcess() const;
    uint32_t getMaxMemoryPerProcess() const;
    void printConfigParameters() const;
    uint32_t getRandomCommandListSize(uint32_t minIns, uint32_t maxIns);
private:
    uint32_t nCPU;
    std::string currentSchedulingAlgo;
    uint32_t quantumCycles;
    uint32_t batchProcessFreq;
    uint32_t minIns;
    uint32_t maxIns;
    uint32_t delaysPerExecution;
    uint32_t maxOverallMemory;
    uint32_t memoryPerFrame;
    uint32_t minMemoryPerProcess;
    uint32_t maxMemoryPerProcess;
    void parseConfigFile(const std::string& configFilePath);
};
