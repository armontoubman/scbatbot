#pragma once
#include <BWAPI.h>
#include "UnitGroup.h"

using namespace BWAPI;

enum TaskType
{
	NoTask,
	ScoutTask,
	CombatTask,
	PrepareTask,
	DetectorTask,
	DefendTask,
	GatherMineralsTask,
	GatherGasTask
};

	/*
	uitleg voor oude code:
	types:
	1 scout
	2 combat
	3 prepare
	4 detector
	5 defend
	*/

class Task {
public:
	Task();
	Task(TaskType t, Position pos);
	Task(TaskType t, Position pos, Unit* target);
	bool operator==(const Task& param) const;
	bool operator<(const Task& rhs) const;
	TaskType getType();
	Position getPosition();
	Unit* getTarget();
	std::string getTypeName();
	std::string getTypeName(TaskType t);
private:
	TaskType type;
	Position position;
	Unit* target;
};