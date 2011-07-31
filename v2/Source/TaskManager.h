#pragma once
#include <BWAPI.h>
#include "HighCommand.h"
class HighCommand;

#include "Task.h"

using namespace BWAPI;

class TaskManager {
public:
	TaskManager(HighCommand* h);
	void update();
	std::list<Task> getTasklist(TaskType tasktype);
	std::map<TaskType, std::list<Task>> getTasklists();
	bool existsTaskWithType(TaskType tasktype);
	std::string chat();
private:
	HighCommand* hc;
	std::map<TaskType, std::list<Task>> tasklistmap;
	void collectTasks();
	void makeScoutTasksOnBaseLocations();
};