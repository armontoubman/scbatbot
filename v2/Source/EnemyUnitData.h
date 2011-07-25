#pragma once
#include <BWAPI.h>

using namespace BWAPI;

class EnemyUnitData
{
public:
	EnemyUnitData();
	void update(Unit* u);

	UnitType unitType;
	int hitPoints;
	Position position;
	Position lastKnownPosition;
	int lastSeen;
};