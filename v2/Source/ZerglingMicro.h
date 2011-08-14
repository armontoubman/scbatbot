#pragma once

#include <BWAPI.h>
#include "HighCommand.h"
class HighCommand;

class ZerglingMicro
{
public:
	ZerglingMicro();
	ZerglingMicro(HighCommand* h);
	void micro(Unit* unit);
private:
	HighCommand* hc;
};