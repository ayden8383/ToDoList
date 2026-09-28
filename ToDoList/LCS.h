#pragma once
#include <string>
#include<vector>	
#include <unordered_map>
#include "Task.h"

struct Category 
{
	std::string phrase;
	int length;
	std::vector<int> tasks;
};

class LCS
{
public:
	LCS(const std::vector<Task>& tasks);


	const std::vector<int>& getSequence() const { return sequence; }
	const std::vector<int>& getOwner() const { return taskof; }
	const std::vector<int>& getSuffixArray() const { return suffixArray; }
	const std::vector<int>& getLCP() const { return lcp; }
	std::vector<Category> findPhrases(int k) const;

private:
	void tokenize(const std::vector<Task>& tasks);
	static std::vector<std::string> splitWords(const std::string& text);

	void buildSuffixArrayNaive(); //brute force approach
	void buildLCP();


	std::unordered_map<std::string, int> wordIds; // "buy" -> 1, "milk" -> 2, ...
	std::vector<int> sequence; // every task's word IDs + separators + 0
	std::vector<int> taskof; // owner[i] = which task sequence came from
	std::vector<int> suffixArray;
	std::vector<int> lcp;
};

