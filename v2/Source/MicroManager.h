#pragma once
#include <BWAPI.h>
#include "HighCommand.h"
class HighCommand;

using namespace BWAPI;

class MicroManager
{
public:
	MicroManager(HighCommand* h);
	void update();
private:
	HighCommand* hc;
};