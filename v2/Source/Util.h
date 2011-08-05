#pragma once
#include <string>
#include <BWAPI.h>
#include <BWTA.h>
#include "UnitGroup.h"

void log(const char* text, ...);
std::string intToString(int i);
std::string floatToString(float f);
int dist(int d);

BWAPI::Unit* getNearestUnit(BWAPI::Position pos, UnitGroup ug);
BWAPI::Position getCenterPosition(std::set<BWAPI::Position> posset);
bool isMilitary(BWAPI::UnitType unittype);
std::map<BWAPI::UnitType, int> countUnitTypesInUnitGroup(std::set<BWAPI::Unit*> myUnits);
UnitGroup getEnemyUnitsInRadius(BWAPI::Position pos, int radius);
UnitGroup getEigenUnitsInRadius(BWAPI::Position pos, int radius);
UnitGroup allEigenUnits();
UnitGroup allEnemyUnits();