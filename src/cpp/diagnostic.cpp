#include <diagnostic.h>
#include <iostream>

void Diagnostic::push_diagnostic (Test *test) {
	queue.push_back(test);
}

void Diagnostic::start() {
	queue.back()->begin();
}

bool Diagnostic::control() {
	if (queue.size() > 0) {
		if (queue.back()->update()) {
			completed.push_back(queue.back());
			queue.pop_back();
			if (completed.back()->was_successful) {
				queue.insert(queue.end(), completed.back()->subtests.begin(), completed.back()->subtests.end());
			}
			if (queue.size() > 0) {
				queue.back()->begin();
			}
		}
		return true;
	} else {
		return false;
	}
}

void Diagnostic::results() {
	std::cout << "----------------DIAGNOSTIC RESULTS----------------" << std::endl;
	for (auto& test : completed) {
		if (test->was_successful) {
			std::cout << "SUCCESS: " << std::endl;
			std::cout << test->info << std::endl;
			std::cout << test->suggestion << std::endl;
			std::cout << "--------------------------------------------------" << std::endl;
		}
	}
	for (auto& test : completed) {
		if (!test->was_successful) {
			std::cout << "FAILURE: " << std::endl;
			std::cout << test->info << std::endl;
			std::cout << test->suggestion << std::endl;
			std::cout << "--------------------------------------------------" << std::endl;
		}
	}
}

void Diagnostic::reset() {
	queue.clear();
	for (auto& test : completed) {
		delete test; //TODO: Make sure this actually works
	}
	completed.clear();
}
