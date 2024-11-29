#pragma once
#include "IMemoryAllocator.h"
#include "ConsoleManager.h"
#include <iomanip>
#include <memory>
#include <string>
#include <sstream> 
#include <vector>
#include <algorithm>

struct MemoryPartition {
	uint32_t start;
	uint32_t end;
	bool isAllocatable;
	std::shared_ptr<AttachedProcess> process;
};

class FlatMemoryAllocator : public IMemoryAllocator
{
public:
	FlatMemoryAllocator();
	bool allocate(std::shared_ptr<AttachedProcess> process) override;
	void deallocate(int pid) override;
	void visualizeMemory() override;
	std::shared_ptr<AttachedProcess> getOldestPreemptedProcess();
	void mergeFreeMemoryPartitions();
	bool isProcessAllocated(std::shared_ptr<AttachedProcess> process);
	void displayStats();

private:
	std::vector<MemoryPartition> memoryPartitions;
	uint32_t maxOverAllMemory;
	uint32_t numPagedIn = 0;
	uint32_t numPagedOut = 0;
};

