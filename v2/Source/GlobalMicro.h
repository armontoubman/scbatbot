#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "UnitGroup.h"

void moveToNearestBase(BWAPI::Unit* unit);
bool canAttackAir(BWAPI::Unit* unit);
bool canAttackAir(std::set<BWAPI::Unit*> units);
bool canAttackGround(BWAPI::Unit* unit);
bool canAttackGround(std::set<BWAPI::Unit*> units);
BWAPI::Position moveAway(BWAPI::Unit* unit, double radius);
BWAPI::Position moveAway(BWAPI::Unit* unit);
BWAPI::Position splitUp(BWAPI::Unit* unit);