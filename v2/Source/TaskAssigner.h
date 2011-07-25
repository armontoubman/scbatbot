#pragma once
#include <BWAPI.h>
#include "HighCommand.h"

using namespace BWAPI;

class TaskAssigner
{
public:
	TaskAssigner(HighCommand* h);
	void update();
private:
	HighCommand* hc;
};