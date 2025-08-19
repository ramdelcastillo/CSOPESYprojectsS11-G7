#pragma once
#include <memory>
#include "AttachedProcess.h"

class IMemoryAllocator {
public:
    IMemoryAllocator();
    virtual bool allocate(std::shared_ptr<AttachedProcess> process) = 0;
    virtual void deallocate(int pid) = 0;
    virtual void visualizeMemory() = 0;
    virtual ~IMemoryAllocator() = default;  
};
