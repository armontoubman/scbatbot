#pragma once
#include "Product.h"
#include <BWAPI.h>
#include <list>
class ProductList {
public:
	ProductList();
	void addProduct(Product b);
	void removeProduct(Product b);
	void removeAll(Product b);
	void removeTop();
	void removeSecond();
	void clear();
	int count(BWAPI::UnitType t);
	int count(BWAPI::TechType t);
	int count(BWAPI::UpgradeType t);
	int size();
	bool isEmpty();

	void update();

	bool containsUnits();
	bool containsExpand();

	Product top();
	Product getSecond();
	void addProductTop(Product b);

	int supplyRequiredForTopThree();
	int countUnits();
	int countExpand();

	std::list<Product> productList;
	void init();

	std::map<BWAPI::UnitType, int> unittypeCounter;
	std::map<BWAPI::TechType, int> techtypeCounter;
	std::map<BWAPI::UpgradeType, int> upgradetypeCounter;
	int expandCounter;

	void incrementCounter(Product b);
	void decrementCounter(Product b);
};