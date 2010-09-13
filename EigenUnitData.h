#pragma once
#include <BWAPI.h>
class EigenUnitData
{
public:
	EigenUnitData();

	BWAPI::Unit* unit;
	BWAPI::UnitType type;
	int hitPoints;
	bool seenByEnemy;
	int seenByEnemyTime;
	BWAPI::Position* seenByEnemyPosition;
};