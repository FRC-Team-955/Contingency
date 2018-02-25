#ifndef TEST_H
#define TEST_H

#include <vector>

class Test {
	public:
		virtual void begin () = 0;
		virtual bool update () = 0; //Returns true if the test is complete
		std::vector<Test*> subtests;
		std::string info = "";
		std::string suggestion = "";
		bool was_successful = false; 
};
#endif
