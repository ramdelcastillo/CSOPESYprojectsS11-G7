#include "AConsole.h"

typedef std::string String;

AConsole::AConsole(String name) : name(name) {}

const String& AConsole::getName() const {
    return this->name;
}
