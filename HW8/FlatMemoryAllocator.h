#pragma once
#include "IMemoryAllocator.h"
#include "ConsoleManager.h"
#include <iomanip>
#include <sstream> 
#include <vector>

struct MemoryPartition {
	uint32_t slotNumber;
	bool isAllocatable;
	std::shared_ptr<AttachedProcess> process;
};

class FlatMemoryAllocator : public IMemoryAllocator
{
public:
	FlatMemoryAllocator();
	bool allocate(std::shared_ptr<AttachedProcess> process) override;
	void deallocate(std::shared_ptr<AttachedProcess> process) override;
	void visualizeMemory() override;
	void printMemory();
	void printMemoryPerCycle(uint32_t commandCounter);
	void setIsAllocatable(uint32_t slotNumber, bool value);
	bool isProcessAllocated(std::shared_ptr<AttachedProcess> process);
	uint32_t getAmountOfAllocatedSlots();
	bool hasFreeSlots();
	uint32_t getFragmentation();
private:
	std::vector<MemoryPartition> memoryPartitions;
};

