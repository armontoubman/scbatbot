#pragma once

#include <BWAPI.h>
#include "HighCommand.h"
class HighCommand;

class DroneMicro
{
public:
	DroneMicro();
	DroneMicro(HighCommand* h);
	void micro(BWAPI::Unit* unit);
private:
	HighCommand* hc;
};