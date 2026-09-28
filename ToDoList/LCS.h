#pragma once
#include <string>
#include<vector>	
#include <unordered_map>
#include "Task.h"
class LCS
{
public:
	LCS(const std::vector<Task>& tasks);

	const std::vector<int>& getSequence() const { return sequence; }
	const std::vector<int>& getOwner() const { return taskof; }

private:
	void tokenize(const std::vector<Task>& tasks);
	static std::vector<std::string> splitWords(const std::string& text);

	std::unordered_map<std::string, int> wordIds; // "buy" -> 1, "milk" -> 2, ...
	std::vector<int> sequence;                    // every task's word IDs + separators + 0
	std::vector<int> taskof;                       // owner[i] = which task sequence came from
};

