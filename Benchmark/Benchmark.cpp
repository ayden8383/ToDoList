// Times the naive suffix array construction against DC3 on inputs that double in size.
// Prints CSV: input type, sequence length, naive time (ms), DC3 time (ms).
// Run in Release: Debug builds are many times slower.

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include "LCS.h"

// Builds a task list whose tokenized sequence has about n words.
static std::vector<Task> makeTasks(int n, bool worstCase, std::mt19937& rng)
{
	std::vector<Task> tasks;
	if (worstCase) {
		// One long task of the same word: every pair of suffixes shares a long start
		std::string text;
		for (int i = 0; i < n; i++) text += "a ";
		tasks.push_back(Task{ text, false });
	}
	else {
		// Realistic: tasks of 3-6 words from a 500-word vocabulary
		std::uniform_int_distribution<int> word(0, 499), len(3, 6);
		int total = 0;
		while (total < n) {
			std::string text;
			int k = len(rng);
			for (int i = 0; i < k; i++) text += "w" + std::to_string(word(rng)) + " ";
			tasks.push_back(Task{ text, false });
			total += k + 1;
		}
	}
	return tasks;
}

// Runs f several times and returns the fastest time in milliseconds
template <typename F>
static double timeMs(F f, int runs = 3)
{
	double best = 1e18;
	for (int r = 0; r < runs; r++) {
		auto start = std::chrono::steady_clock::now();
		f();
		auto end = std::chrono::steady_clock::now();
		best = std::min(best, std::chrono::duration<double, std::milli>(end - start).count());
	}
	return best;
}

int main()
{
	std::mt19937 rng(42);
	std::cout << "input,n,naive_ms,dc3_ms\n";

	for (bool worst : { false, true }) {
		for (int n = 1000; n <= 32000; n *= 2) {
			LCS lcs(makeTasks(n, worst, rng));

			lcs.buildSuffixArrayDC3();
			std::vector<int> dc3Result = lcs.getSuffixArray();
			lcs.buildSuffixArrayNaive();
			if (lcs.getSuffixArray() != dc3Result) {
				std::cerr << "Mismatch between naive and DC3 at n = " << n << "\n";
				return 1;
			}

			double naive = timeMs([&] { lcs.buildSuffixArrayNaive(); });
			double dc3 = timeMs([&] { lcs.buildSuffixArrayDC3(); });
			std::cout << (worst ? "repeated" : "random") << ","
			          << lcs.getSequence().size() << "," << naive << "," << dc3 << "\n";
		}
	}
}
