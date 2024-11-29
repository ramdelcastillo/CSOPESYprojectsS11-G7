#pragma once
#include "ConsoleManager.h"
#include "IMemoryAllocator.h"
#include "ConsoleManager.h"

class PagingAllocator : public IMemoryAllocator
{
public:
	PagingAllocator();
	bool allocate(std::shared_ptr<AttachedProcess> process) override;
	void deallocate(int pid) override;
	void visualizeMemory() override;
	void displayStats();
	bool isProcessAllocated(std::shared_ptr<AttachedProcess> process);
	std::shared_ptr<AttachedProcess> getOldestPreemptedProcess();
	uint32_t getNumPagedIn();
	uint32_t getNumPagedOut();

private:
	std::vector<std::shared_ptr<AttachedProcess>> processList;
	std::vector<int> memoryMap;
	std::unordered_map<int, std::vector<int>> pageTable;

	uint32_t maxOverAllMemory;
	uint32_t memoryPerFrame;
	uint32_t nFrames;
	uint32_t numPagedIn = 0;
	uint32_t numPagedOut = 0;
	uint32_t usedMemory = 0;
};

