#include "Condition.h"


Condition::Condition(){
	
}

Condition::~Condition(){

}

bool Condition::empty(){
	return stack.empty();
}

std::list< std::pair<std::string, std::string> > Condition::popCondition(){
	
	if (stack.empty())
		throw std::runtime_error("The condition set is empty.");
	std::list< std::pair<std::string, std::string> > ret;	
	ret.push_back(stack.front());
	stack.pop_front();
	ret.push_back(stack.front());
	stack.pop_front();
	ret.push_back(stack.front());
	stack.pop_front();
	
	return ret; 
}

void Condition::pushCondition(std::string ltype, std::string l, std::string op, std::string rtype, std::string r){		
	stack.push_back(std::pair<std::string, std::string>(ltype, l));
	stack.push_back(std::pair<std::string, std::string>(op, op));
	stack.push_back(std::pair<std::string, std::string>(rtype, r));
	
}

void Condition::test(){
	std::cout << "Condition::test()" << std::endl;
}

void Condition::print(){
	std::list< std::pair<std::string, std::string> >::iterator itr;
	for (itr=stack.begin(); itr!=stack.end(); itr++){
		std::cout << itr->first << " " << itr->second << std::endl;
	}
}