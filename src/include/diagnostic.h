#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H

#include <WPILib.h>
#include <ctre/Phoenix.h>
#include <vector>
#include <test.h>

class Diagnostic {
	private:
		std::vector<Test*> queue;
		std::vector<Test*> completed;
	public:
		void start();
		void push_diagnostic (Test* test);
		bool control(); //Returns true if there are tests left
		void reset();
		void results();
};

#endif
