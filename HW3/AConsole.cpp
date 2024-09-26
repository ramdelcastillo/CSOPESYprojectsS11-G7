#include "AConsole.h"

/*
Contributors:
Del Castillo, Jose
*/

typedef std::string String;

AConsole::AConsole(String name) : name(name) {}

const String& AConsole::getName() const {
    return this->name;
}
