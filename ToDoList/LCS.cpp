#include "LCS.h"
#include <cctype>
#include <algorithm>
#include <numeric>

LCS::LCS(const std::vector<Task>& tasks)
{
	tokenize(tasks);
	buildSuffixArrayNaive();
}

//format lists into a vector of words
std::vector<std::string> LCS::splitWords(const std::string& text)
{
	std::vector<std::string> words;
	std::string current;

	for (char c : text) {
		unsigned char uc = static_cast<unsigned char>(c);

		if (std::isalnum(uc)) {
			current += static_cast<char>(std::tolower(uc));
		}
		else if (!current.empty()) {
			words.push_back(current);
			current.clear();
		}
	}

	if (!current.empty()) {
		words.push_back(current);
	}

	return words;
}

void LCS::tokenize(const std::vector<Task> &tasks)
{
	//pass 1: split every task and give each new word the next ID, Starting at 1
	std::vector<std::vector<std::string>> taskWords;

	for (const Task& task : tasks) {
		std::vector<std::string> words = splitWords(task.desctiption);

		//check if the words is already in wordIds map. (word hasn't been seen before)
		for (const std::string& word : words) {
			if (wordIds.find(word) == wordIds.end()) {
				int nextId = static_cast<int>(wordIds.size()) + 1;
				wordIds[word] = nextId;
			}
		}

		taskWords.push_back(words);
	}

	//Word IDs are 1..w. Seperators start at W + 1
	int W = static_cast<int>(wordIds.size());

	//pass 2: for each task, build the sequence with a unique sepeartor
	for (int i = 0; i < static_cast<int>(tasks.size()); i++) {
		for (const std::string& word : taskWords[i]) {
			sequence.push_back(wordIds[word]);
			taskof.push_back(i);
		}

		sequence.push_back(W + 1 + i);
		taskof.push_back(i);
	}

	sequence.push_back(0);
	taskof.push_back(-1);
}

void LCS::buildSuffixArrayNaive() 
{
	int n = static_cast<int>(sequence.size());

	suffixArray.resize(n);
	std::iota(suffixArray.begin(), suffixArray.end(), 0); //0,1,2,...n-1

	std::sort(suffixArray.begin(), suffixArray.end(), [this](int a, int b) {
		return std::lexicographical_compare(
			sequence.begin() + a, sequence.end(),
			sequence.begin() + b, sequence.end());
		});
}