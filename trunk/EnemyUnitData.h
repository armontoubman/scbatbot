#pragma once
#include <BWAPI.h>
class EnemyUnitData
{
public:
	EnemyUnitData();
	void update(BWAPI::Unit* u);

	BWAPI::Unit* unit;
	BWAPI::UnitType unitType;
	int hitPoints;
	BWAPI::Position position;
	int lastSeen;
};