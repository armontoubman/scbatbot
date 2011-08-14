#pragma once

#include <BWAPI.h>
#include "HighCommand.h"
class HighCommand;

class OverlordMicro
{
public:
	OverlordMicro();
	OverlordMicro(HighCommand* h);
	void micro(BWAPI::Unit* unit);
private:
	HighCommand* hc;
};