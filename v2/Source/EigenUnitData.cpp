#include "EigenUnitData.h"
#include <BWAPI.h>

EigenUnitData::EigenUnitData()
{
	this->hitPoints = 0;
	this->seenByEnemy = false;
	this->seenByEnemyTime = 0;
	this->seenByEnemyPosition = Position();
}