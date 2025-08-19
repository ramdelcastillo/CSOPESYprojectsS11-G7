#pragma once

#include <string>

class AConsole
{
public:
    typedef std::string String;
    AConsole(String name);
    virtual ~AConsole() = default;

    const String& getName() const;
    virtual void onEnabled() = 0;
    virtual void display() = 0;
    virtual void process() = 0;

protected:
    String name;
    friend class ConsoleManager;
};


