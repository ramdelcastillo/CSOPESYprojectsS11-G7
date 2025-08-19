#pragma once
#include <string>
#include <iostream>

class ICommand
{
public:
    enum CommandType
    {
        UNDEFINED,
        PRINT,
        IO
    };

    ICommand() : commandType(UNDEFINED) {}
    virtual void execute() = 0;

protected:
    CommandType commandType;
};


