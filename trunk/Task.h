#pragma once
#include <BWAPI.h>
#include <UnitGroup.h>
class Task {
public:
	Task();
	Task(int t, int p, BWAPI::Position pos, UnitGroup* ug);
	Task(int t, int p, BWAPI::Position pos, UnitGroup* ug, UnitGroup* e);
	int type;
	/*
	types:
	1 scout
	2 combat
	3 prepare
	4 detector
	*/
	int priority;
	BWAPI::Position position;
	UnitGroup* unitGroup;
	UnitGroup* enemy;
	bool operator==(const Task& param) const;
	bool operator<(const Task& rhs) const;
};