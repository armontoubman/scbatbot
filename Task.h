#pragma once
#include <BWAPI.h>
#include <UnitGroup.h>
class Task {
public:
	Task();
	Task(int t, BWAPI::Position pos);
	Task(int t, BWAPI::Position pos, bool eCA, bool eCG, int eS, int eMS);
	int type;
	/*
	types:
	1 scout
	2 combat
	3 prepare
	4 detector
	5 defend
	*/
	bool enemyContainsAir;
	bool enemyContainsGround;
	int enemySize;
	int enemyMilitarySize;
	BWAPI::Position position;
	bool operator==(const Task& param) const;
	bool operator<(const Task& rhs) const;
};