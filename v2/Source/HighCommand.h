#pragma once
#include <BWAPI.h>

#include "EigenUnitDataManager.h"
class EigenUnitDataManager;
#include "EnemyUnitDataManager.h"
class EnemyUnitDataManager;
#include "MicroManager.h"
class MicroManager;
#include "EigenUnitGroupManager.h"
class EigenUnitGroupManager;
#include "TaskManager.h"
class TaskManager;
#include "TaskAssigner.h"
class TaskAssigner;
#include "WantBuildManager.h"
class WantBuildManager;
#include "ResourceManager.h"
class ResourceManager;

#include <time.h>

using namespace BWAPI;

class HighCommand
{
public:

	HighCommand();
	~HighCommand();
	void update();
	void onStart();
	void onUnitDiscover(Unit* u);
	void onUnitEvade(Unit* u);
	void onUnitShow(Unit* u);
	void onUnitHide(Unit* u);
	void onUnitCreate(Unit* u);
	void onUnitDestroy(Unit* u);
	void onUnitMorph(Unit* u);
	void onUnitRenegade(Unit* u);

	std::string chat();

	EigenUnitDataManager* eiudm;
	EnemyUnitDataManager* eudm;
	MicroManager* mm;
	EigenUnitGroupManager* eiugm;
	TaskManager* tm;
	TaskAssigner* ta;
	WantBuildManager* wbm;
	ResourceManager* rm;

private:
	void startLog();

	void tic();
	float toc();
	float c;
	std::map<int, float> times;
	void logtimes();

	void drawFPS();
	void drawTasks();
	void drawUnits();
};