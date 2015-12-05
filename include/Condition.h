#ifndef _CONDITION_H
#define _CONDITION_H

#include <list>
#include <string>
#include <iostream>
#include <stdexcept>

class Condition{
private:
	std::list< std::pair<std::string, std::string> > stack;
public:

	Condition();
	~Condition();
	/*
		get one signle condition that must be ensured.
		the return value is in the format like that {l, op, r}
	*/
	bool empty();
	std::list< std::pair<std::string, std::string> > popCondition();
	void pushCondition(std::string ltype, std::string l, std::string op, std::string rtype, std::string r);
	void test();
	
	void print();
};

#endif