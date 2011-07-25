#pragma once
#include <BWAPI.h>

using namespace BWAPI;

class EigenUnitData
{
public:
	EigenUnitData();

	int hitPoints;
	bool seenByEnemy;
	int seenByEnemyTime;
	Position seenByEnemyPosition;
};