#pragma once
#include "ProductList.h"
#include "Product.h"
#include <BWAPI.h>
#include <list>
#include <algorithm>
ProductList::ProductList()
{
	this->expandCounter = 0;
}

void ProductList::addProduct(Product b)
{
	if (this->productList.size()<30)
	{
		this->productList.push_back(b);
		this->incrementCounter(b);
	}
}

void ProductList::removeProduct(Product b)
{
	if(this->productList.size() == 0) return;
	this->productList.erase(find(this->productList.begin(), this->productList.end(), b));
	this->decrementCounter(b);
}

void ProductList::removeTop()
{
	if(this->productList.size() == 0) return;
	Product b = *(this->productList.begin());
	this->decrementCounter(b);
	this->productList.erase(this->productList.begin());
}

void ProductList::removeSecond()
{
	if(this->productList.size() == 0) return;
	Product b = this->top();
	this->removeTop();
	this->removeTop();
	this->addProductTop(b);
}

void ProductList::removeAll(Product b)
{
	if(this->productList.size() == 0) return;
	std::list<Product>::iterator curr = this->productList.begin();
	while(curr != this->productList.end())
	{
		if(*curr == b) {
			curr = this->productList.erase(curr);
			this->decrementCounter(b);
		}
		else {
			curr++;
		}
	}
}

void ProductList::clear()
{
	this->productList.clear();
	this->unittypeCounter.clear();
	this->techtypeCounter.clear();
	this->upgradetypeCounter.clear();
	this->expandCounter = 0;
}

int ProductList::count(BWAPI::UnitType t)
{
	return this->unittypeCounter[t];
}

int ProductList::count(BWAPI::TechType t)
{
	return this->techtypeCounter[t];
}

int ProductList::count(BWAPI::UpgradeType t)
{
	return this->upgradetypeCounter[t];
}

int ProductList::size()
{
	return this->productList.size();
}

void ProductList::init()
{
}

void ProductList::update()
{
}

bool ProductList::isEmpty()
{
	return this->productList.size() == 0;
}

bool ProductList::containsUnits()
{
	return this->countUnits() > 0;
}

bool ProductList::containsExpand()
{
	return this->expandCounter > 0;
}

Product ProductList::top()
{
	if(this->productList.size()==0) return Product();
	return *(this->productList.begin());
}

Product ProductList::getSecond()
{
	std::list<Product>::iterator it=this->productList.begin();
	it++;
	return *it;
}

void ProductList::addProductTop(Product b)
{
	this->productList.push_front(b);
	this->incrementCounter(b);
}

int ProductList::supplyRequiredForTopThree()
{
	int teller = 1;
	int supply = 0;
	for(std::list<Product>::iterator it=this->productList.begin(); it!=this->productList.end(); it++)
	{
		if(teller == 3)
		{
			break;
		}
		if(it->type == BuildProduct)
		{
			supply += it->buildtype.supplyRequired();
		}
		teller++;
	}
	return supply;
}

int ProductList::countUnits()
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

int ProductList::countExpand()
{
	return this->expandCounter;
}

void ProductList::incrementCounter(Product b)
{
	if(b.type == BuildProduct)
	{
		this->unittypeCounter[b.buildtype] = this->unittypeCounter[b.buildtype]+1;
	}
	if(b.type == TechProduct)
	{
		this->techtypeCounter[b.techtype] = this->techtypeCounter[b.techtype]+1;
	}
	if(b.type == UpgradeProduct)
	{
		this->upgradetypeCounter[b.upgradetype] = this->upgradetypeCounter[b.upgradetype]+1;
	}
	if(b.type = ExpandProduct)
	{
		this->expandCounter = this->expandCounter+1;
	}
}

void ProductList::decrementCounter(Product b)
{
	if(b.type == BuildProduct)
	{
		this->unittypeCounter[b.buildtype] = this->unittypeCounter[b.buildtype]-1;
	}
	if(b.type == TechProduct)
	{
		this->techtypeCounter[b.techtype] = this->techtypeCounter[b.techtype]-1;
	}
	if(b.type == UpgradeProduct)
	{
		this->upgradetypeCounter[b.upgradetype] = this->upgradetypeCounter[b.upgradetype]-1;
	}
	if(b.type = ExpandProduct)
	{
		this->expandCounter = this->expandCounter-1;
	}
}