#include "LCS.h"
#include "cctype"

LCS::LCS(const std::vector<Task>& tasks)
{
	tokenize(tasks);
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