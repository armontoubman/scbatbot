#pragma once
#include "MicroManager.h"

#include <BWAPI.h>
#include "HighCommand.h"

MicroManager::MicroManager(HighCommand* h)
{
	this->hc = h;
}

void MicroManager::update()
{
}