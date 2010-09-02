#pragma once
#include <BWAPI.h>
class EigenUnitData
{
public:
	EigenUnitData();

	BWAPI::Unit* unit;
	int hitPoints;
	bool seenByEnemy;
	int seenByEnemyTime;
	BWAPI::Position* seenByEnemyPosition;
};