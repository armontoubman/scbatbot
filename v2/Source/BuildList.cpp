#pragma once
#include "BuildList.h"
#include "BuildItem.h"
#include <BWAPI.h>
#include <list>
#include <algorithm>
BuildList::BuildList()
{
	this->expandCounter = 0;
}

void BuildList::addItem(BuildItem b)
{
	if (this->buildlist.size()<30)
	{
		this->buildlist.push_back(b);
		this->incrementCounter(b);
	}
}

void BuildList::removeItem(BuildItem b)
{
	if(this->buildlist.size() == 0) return;
	this->buildlist.erase(find(this->buildlist.begin(), this->buildlist.end(), b));
	this->decrementCounter(b);
}

void BuildList::removeTop()
{
	if(this->buildlist.size() == 0) return;
	BuildItem b = *(this->buildlist.begin());
	this->decrementCounter(b);
	this->buildlist.erase(this->buildlist.begin());
}

void BuildList::removeSecond()
{
	if(this->buildlist.size() == 0) return;
	BuildItem b = this->top();
	this->removeTop();
	this->removeTop();
	this->addItemTop(b);
}

void BuildList::removeAll(BuildItem b)
{
	if(this->buildlist.size() == 0) return;
	std::list<BuildItem>::iterator curr = this->buildlist.begin();
	while(curr != this->buildlist.end())
	{
		if(*curr == b) {
			curr = this->buildlist.erase(curr);
			this->decrementCounter(b);
		}
		else {
			curr++;
		}
	}
}

void BuildList::clear()
{
	this->buildlist.clear();
	this->unittypeCounter.clear();
	this->techtypeCounter.clear();
	this->upgradetypeCounter.clear();
	this->expandCounter = 0;
}

int BuildList::count(BWAPI::UnitType t)
{
	return this->unittypeCounter[t];
}

int BuildList::count(BWAPI::TechType t)
{
	return this->techtypeCounter[t];
}

int BuildList::count(BWAPI::UpgradeType t)
{
	return this->upgradetypeCounter[t];
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
	return this->countUnits() > 0;
}

bool BuildList::containsExpand()
{
	return this->expandCounter > 0;
}

BuildItem BuildList::top()
{
	if(buildlist.size()==0) return BuildItem();
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
	this->incrementCounter(b);
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
		if(it->type == BuildType)
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
	for(std::map<BWAPI::UnitType, int>::iterator it=unittypeCounter.begin(); it!=unittypeCounter.end(); it++)
	{
		if(!it->first.isBuilding())
		{
			aantal++;
		}
	}
	return aantal;
}

int BuildList::countExpand()
{
	return this->expandCounter;
}

void BuildList::incrementCounter(BuildItem b)
{
	if(b.type == BuildType)
	{
		this->unittypeCounter[b.buildtype] = this->unittypeCounter[b.buildtype]+1;
	}
	if(b.type == ResearchType)
	{
		this->techtypeCounter[b.researchtype] = this->techtypeCounter[b.researchtype]+1;
	}
	if(b.type == UpgradeType)
	{
		this->upgradetypeCounter[b.upgradetype] = this->upgradetypeCounter[b.upgradetype]+1;
	}
	if(b.type = ExpandType)
	{
		this->expandCounter = this->expandCounter+1;
	}
}

void BuildList::decrementCounter(BuildItem b)
{
	if(b.type == BuildType)
	{
		this->unittypeCounter[b.buildtype] = this->unittypeCounter[b.buildtype]-1;
	}
	if(b.type == ResearchType)
	{
		this->techtypeCounter[b.researchtype] = this->techtypeCounter[b.researchtype]-1;
	}
	if(b.type == UpgradeType)
	{
		this->upgradetypeCounter[b.upgradetype] = this->upgradetypeCounter[b.upgradetype]-1;
	}
	if(b.type = ExpandType)
	{
		this->expandCounter = this->expandCounter-1;
	}
}