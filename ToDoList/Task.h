#pragma once
#include <string>
#include <vector>

struct Task
{
	std::string desctiption;
	bool done;
};

void saveTaskToFile(const std::vector<Task>& tasks, const std::string& fileName);
std::vector<Task> loadTaskFromFile(const std::string& fileName);

