#pragma once
#include <BWAPI.h>
#include "UnitGroup.h"
class HighCommand;

class DroneMicro;
class OverlordMicro;
class ZerglingMicro;

using namespace BWAPI;

class MicroManager
{
public:
	MicroManager(HighCommand* h);
	void update();
	bool isUnitMicrod(BWAPI::Unit* unit);
	void unitIsMicrod(BWAPI::Unit* unit);
private:
	HighCommand* hc;

	UnitGroup microdUnits;

	DroneMicro* droneMicro;
	OverlordMicro* overlordMicro;
	ZerglingMicro* zerglingMicro;

	void selectMicro(BWAPI::Unit* unit);
};