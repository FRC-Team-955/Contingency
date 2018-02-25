#include <diagnostic.h>

void Diagnostic::push_diagnostic (Test *test) {
	queue.push_back(test);
}

void Diagnostic::start() {
	std::cout << "---------------- BEGIN DIAGNOSTIC ----------------" << std::endl;
	queue.back()->begin();
}

bool Diagnostic::control() {
	if (queue.size() > 0) {
		//Are we still waiting for a test to finish?
		if (queue.back()->update()) {
			//Push a complete test to the stack
			completed.push_back(queue.back());
			//Pop it off of the queued tests
			queue.pop_back();
			if (completed.back()->was_successful) {
				//If the last one was successful, add it's subtests to the queue
				queue.insert(queue.end(), completed.back()->subtests.begin(), completed.back()->subtests.end());
			}
			if (queue.size() > 0) {
				//Begin the next one if we have any left
				queue.back()->begin();
			}
		}
		return true;
	} else {
		return false;
	}
}

void Diagnostic::results() {
	//Print successes before failures, because failures will be more visible at the bottom
	//TODO: Make this shorter...
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
	for (auto& test : completed) {
		delete test; //TODO: Make sure this actually works
	}
	for (auto& test : queue) {
		delete test; //TODO: Make sure this actually works
	}
	queue.clear();
	completed.clear();
}
