#include <wx/log.h>   // temporary, for the radixPass test
#include "LCS.h"
#include <cctype>
#include <algorithm>
#include <numeric>
#include <map>
#include <set>

LCS::LCS(const std::vector<Task>& tasks)
{
	tokenize(tasks);
	buildSuffixArrayNaive();
	buildLCP();

	// TEMPORARY dc3 test: "banana" with b=2, a=1, n=3
	std::vector<int> s = { 2, 1, 3, 1, 3, 1, 0, 0, 0 };
	std::vector<int> SA(6);
	dc3(s, SA, 6, 3);

}

// One pass of stable counting sort, used by DC3.
// Sorts the first n positions in `in` by keys[pos + offset] (keys range 0..K) into `out`.
// Stable: positions with equal keys keep their order from `in`.
void LCS::radixPass(const std::vector<int>& in, std::vector<int>& out,
	const std::vector<int>& keys, int offset, int n, int K)
{
	// Count how many positions have each key
	std::vector<int> count(K + 1, 0);
	for (int i = 0; i < n; i++) {
		count[keys[in[i] + offset]]++;
	}

	// Prefix sum: count[key] becomes where the first position with that key goes
	int sum = 0;
	for (int key = 0; key <= K; key++) {
		int keyCount = count[key];
		count[key] = sum;
		sum += keyCount;
	}

	// Place each position in its slot, going through `in` in order to keep it stable
	for (int i = 0; i < n; i++) {
		out[count[keys[in[i] + offset]]++] = in[i];
	}
}

// DC3 (skew) suffix array construction, O(n).
//  values 1..K, followed by three 0s of padding (s.size() == n + 3).
// : output, the n suffix start positions in sorted order.
void LCS::dc3(const std::vector<int>& s, std::vector<int>& SA, int n, int K)
{
	int n0 = (n + 2) / 3;  // positions with i mod 3 == 0
	int n1 = (n + 1) / 3;  // i mod 3 == 1
	int n2 = n / 3;        // i mod 3 == 2
	int n02 = n0 + n2;     // sample size (includes a dummy mod 1 position when n0 > n1)

	std::vector<int> s12(n02 + 3, 0);
	std::vector<int> SA12(n02 + 3, 0);

	// (i mod 3 != 0).
	// n0 - n1 adds the dummy position n when n mod 3 == 1.
	for (int i = 0, j = 0; i < n + (n0 - n1); i++) {
		if (i % 3 != 0) {
			s12[j++] = i;
		}
	}

	// Sort the sample positions by their first 3 values, last value first
	radixPass(s12, SA12, s, 2, n02, K);
	radixPass(SA12, s12, s, 1, n02, K);
	radixPass(s12, SA12, s, 0, n02, K);

	// TEMPORARY part 2 test
	wxString line;
	for (int i = 0; i < n02; i++) line << SA12[i] << " ";
	wxLogDebug("SA12: %s", line);
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

// Kasai's algorithm: builds the LCP array in O(n).
// Visits suffixes in text order, so each LCP starts from the previous one minus 1
void LCS::buildLCP()
{
	int n = static_cast<int>(sequence.size());
	lcp.assign(n, 0);

	// rank[pos] = where the suffix starting at pos sits in sorted order
	std::vector<int> rank(n);
	for (int i = 0; i < n; i++) {
		rank[suffixArray[i]] = i;
	}

	int h = 0;
	for (int pos = 0; pos < n; pos++) {
		// The first suffix in sorted order doesnt have a neighbor before it
		if (rank[pos] == 0) {
			h = 0;
			continue;
		}

		int prev = suffixArray[rank[pos] - 1];

		// Extend the match from h instead of from 0
		while (pos + h < n && prev + h < n && sequence[pos + h] == sequence[prev + h]) {
			h++;
		}

		lcp[rank[pos]] = h;

		// The next suffix (pos + 1) shares at least h - 1 words with its neighbor
		if (h > 0) {
			h--;
		}
	}
}

// Finds phrases of  k words shared by 2 or more tasks.
// A run of lcp >= k means those neighboring suffixes all start with the same phrase.
std::vector<Category> LCS::findPhrases(int k) const
{
	int n = static_cast<int>(sequence.size());

	// Reverse of wordIds, so a phrase's IDs can be turned back into words
	std::vector<std::string> idToWord(wordIds.size() + 1);
	for (const auto& entry : wordIds) {
		idToWord[entry.second] = entry.first;
	}

	std::vector<Category> categories;
	std::map<std::vector<int>, int> categoryOf; 

	int i = 1;
	while (i < n) {
		if (lcp[i] < k) {
			i++;
			continue;
		}

		// The run covers suffixArray[start] up to suffixArray[i - 1]
		int start = i - 1;
		int length = lcp[i];
		while (i < n && lcp[i] >= k) {
			length = std::min(length, lcp[i]);
			i++;
		}

		std::set<int> taskSet;
		for (int j = start; j < i; j++) {
			taskSet.insert(taskof[suffixArray[j]]);
		}

		if (taskSet.size() < 2) {
			continue; 
		}

		// Turn the shared word IDs back into text
		int pos = suffixArray[start];
		std::string phrase;
		for (int w = 0; w < length; w++) {
			if (w > 0) phrase += ' ';
			phrase += idToWord[sequence[pos + w]];
		}

		std::vector<int> tasks(taskSet.begin(), taskSet.end());

		// One category per set of tasks, named after the longest phrase
		auto found = categoryOf.find(tasks);
		if (found == categoryOf.end()) {
			categoryOf[tasks] = static_cast<int>(categories.size());
			categories.push_back(Category{ phrase, length, tasks });
		}
		else if (length > categories[found->second].length) {
			categories[found->second].phrase = phrase;
			categories[found->second].length = length;
		}
	}

	return categories;
}