#pragma once
#include "BuildList.h"
#include "BuildItem.h"
#include <BWAPI.h>
#include <list>
#include <algorithm>
BuildList::BuildList()
{
}

void BuildList::addItem(BuildItem b)
{
	if (this->buildlist.size()<30)
	{
	this->buildlist.push_back(b);
	}
}

void BuildList::removeItem(BuildItem b)
{
	this->buildlist.erase(find(this->buildlist.begin(), this->buildlist.end(), b));
}

void BuildList::removeTop()
{
	this->buildlist.erase(this->buildlist.begin());
}

void BuildList::removeSecond()
{
	BuildItem b = this->top();
	this->removeTop();
	this->removeTop();
	this->addItemTop(b);
}

void BuildList::removeAll(BuildItem b)
{
	std::list<BuildItem>::iterator curr = this->buildlist.begin();
	while(curr != this->buildlist.end())
	{
		if(*curr == b) {
			curr = this->buildlist.erase(curr);
		}
		else {
			curr++;
		}
	}
}

void BuildList::clear()
{
	this->buildlist.clear();
}

int BuildList::count(BWAPI::UnitType t)
{
	std::list<BuildItem>::iterator it;
	int count = 0;
	for(it=this->buildlist.begin();it!=this->buildlist.end();it++)
	{
		if(it->buildtype == t)
		{
			count++;
		}
	}
	return count;
}

int BuildList::count(BWAPI::TechType t)
{
	std::list<BuildItem>::iterator it;
	int count = 0;
	for(it=this->buildlist.begin();it!=this->buildlist.end();it++)
	{
		if(it->researchtype == t)
		{
			count++;
		}
	}
	return count;
}

int BuildList::count(BWAPI::UpgradeType t)
{
	std::list<BuildItem>::iterator it;
	int count = 0;
	for(it=this->buildlist.begin();it!=this->buildlist.end();it++)
	{
		if(it->upgradetype == t)
		{
			count++;
		}
	}
	return count;
}

int BuildList::size()
{
	return this->buildlist.size();
}

void BuildList::init()
{
}

void BuildList::update()
{
}

bool BuildList::isEmpty()
{
	return this->buildlist.size() == 0;
}

bool BuildList::containsUnits()
{
	for(std::list<BuildItem>::iterator it=buildlist.begin(); it!=buildlist.end(); it++)
	{
		if(it->typenr == 1)
		{
			if(!it->buildtype.isBuilding())
			{
				return true;
			}
		}
	}
	return false;
}

bool BuildList::containsExpand()
{
	for(std::list<BuildItem>::iterator it=buildlist.begin(); it!=buildlist.end(); it++)
	{
		if(it->typenr == 4)
		{
			return true;
		}
	}
	return false;
}

BuildItem BuildList::top()
{
	return *buildlist.begin();
}

BuildItem BuildList::getSecond()
{
	std::list<BuildItem>::iterator it=buildlist.begin();
	it++;
	return *it;
}

void BuildList::addItemTop(BuildItem b)
{
	this->buildlist.push_front(b);
}

int BuildList::supplyRequiredForTopThree()
{
	int teller = 1;
	int supply = 0;
	for(std::list<BuildItem>::iterator it=buildlist.begin(); it!=buildlist.end(); it++)
	{
		if(teller == 3)
		{
			break;
		}
		if(it->typenr == 1)
		{
			supply += it->buildtype.supplyRequired();
		}
		teller++;
	}
	return supply;
}

int BuildList::countUnits()
{
	int aantal = 0;
	for(std::list<BuildItem>::iterator it=buildlist.begin(); it!=buildlist.end(); it++)
	{
		if(it->typenr == 1)
		{
			if(!it->buildtype.isBuilding())
			{
				aantal++;
			}
		}
	}
	return aantal;
}

int BuildList::countExpand()
{
	int aantal = 0;
	for(std::list<BuildItem>::iterator it=buildlist.begin(); it!=buildlist.end(); it++)
	{
		if(it->typenr == 4)
		{
				aantal++;
		}
	}
	return aantal;
}