#pragma once
#include <BWAPI.h>
#include "HighCommand.h"
#include "Task.h"

#include <boost/unordered_map.hpp>

using namespace BWAPI;

struct ResourceData
{
	int workersOnThis;
	int owner;
};

typedef boost::unordered_map<Unit*, ResourceData> ResourceMap ;

class ResourceManager
{
public:
	ResourceManager(HighCommand* h);
	void update();
	void onStart();
	void onUnitDestroy(Unit* u);
	void onUnitMorph(Unit* u);

	std::string chat();
	std::list<Task> getTasklist(TaskType tasktype);
private:
	HighCommand* hc;
	int resourceOwner(Unit* resource);
	ResourceMap mineralmap;
	ResourceMap geysermap;
	ResourceMap extractormap;
	int countOurs(ResourceMap resources);
	std::list<Task> gathermineralstasklist;
	std::list<Task> gathergastasklist;
	void createTask(TaskType tasktype, Unit* resource);
	void clearTasklists();
	void removeMineral(Unit* u);
	void removeGeyser(Unit* u);
	void removeExtractor(Unit* u);
};