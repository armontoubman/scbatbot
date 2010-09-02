#include "EigenAssignment.h"
#include "Task.h"
#include <UnitGroup.h>
EigenAssignment::EigenAssignment()
{
}

EigenAssignment::EigenAssignment(Task* t, UnitGroup* ug)
{
	this->task = t;
	this->unitGroup = ug;
}

bool EigenAssignment::operator==(const EigenAssignment& param) const
{
	return param.task == task && param.unitGroup == unitGroup;
}