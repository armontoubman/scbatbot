#pragma once
#include "Task.h"
#include <UnitGroup.h>
class EigenAssignment {
public:
	EigenAssignment();
	EigenAssignment(Task* t, UnitGroup* ug);
	Task* task;
	UnitGroup* unitGroup;
	bool operator==(const EigenAssignment& param) const;
};