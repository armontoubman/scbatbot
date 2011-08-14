#pragma once

#include "ZerglingMicro.h"
#include <BWAPI.h>
#include "Task.h"
#include "GlobalMicro.h"
#include "Util.h"

#include "ContractManager.h"

ZerglingMicro::ZerglingMicro()
{
}

ZerglingMicro::ZerglingMicro(HighCommand* h)
{
	this->hc = h;
}

void ZerglingMicro::micro(BWAPI::Unit* unit)
{
	Task task = this->hc->ta->getTaskOfUnit(unit);
	
	
}