#pragma once
#include <string>
#include <BWAPI.h>
#include <BWTA.h>

void log(const char* text, ...);
std::string intToString(int i);
std::string floatToString(float f);
double dist(int d);

#include <BWAPI.h>
#include "UnitGroup.h"
BWAPI::Unit* getNearestUnit(BWAPI::Position pos, UnitGroup ug);
BWAPI::Position getCenterPosition(std::set<BWAPI::Position> posset);
bool isMilitary(BWAPI::UnitType unittype);
std::map<BWAPI::UnitType, int> countUnitTypesInUnitGroup(std::set<BWAPI::Unit*> myUnits);