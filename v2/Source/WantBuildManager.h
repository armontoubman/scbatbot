#pragma once
#include <BWAPI.h>
#include "HighCommand.h"
class HighCommand;

using namespace BWAPI;

class WantBuildManager
{
public:
	WantBuildManager(HighCommand* h);
	void update();
private:
	HighCommand* hc;
};