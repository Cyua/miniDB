#include <iostream>
#include <fstream>
#include <string>
#include "WickyEngine.h"
#include "CatalogManager.h"
#include "IndexManager.h"
#include "Parser.h"

int main(int argc, char* argv[]){	
	Parser parser;	
	BufferManager* bm = BufferManager::getInstance();
	CatalogManager* cm = CatalogManager::getInstance();
	IndexManager* im = IndexManager::getInstance();
    
	std::cout << "Welcome to the WicyDB monitor. Commands end with ;" << std::endl;	
	std::cout << "This is our team work. The team compose of Hai Jiewen Zhang Haiwei Yu Qiubin and Xiao Shaobin." << std::endl;
	  		
	while (true){
		parser.setNewSmt(true);
		parser.printHead();
		int no=parser.parse("");
		if (no==Parser::EXIT) break;
		if (no==Parser::EXEC){
			parser.setEcho(false);
			parser.parse(parser.sqlFileName);
			parser.setEcho(true);	
		}
		parser.setErr(true);
	}
		
	delete im;
	delete cm;	
	delete bm;	
}