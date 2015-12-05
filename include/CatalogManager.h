#ifndef _CATALOGMANAGER_H
#define _CATALOGMANAGER_H

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <stdexcept>

#include "Schema.h"
#include "BufferManager.h"

class Schema;

class CatalogManager{

public:
	
	static CatalogManager* getInstance();
	void store(Schema sch);
	void saveChange(Schema sch);
	void drop(std::string tableName);
	Schema get(std::string name);
	std::list<std::string> getTables();
	bool isExist(std::string name);
	const static std::string CATALOG;
	const static int ENDFILE;
	virtual ~CatalogManager();
private:
	static CatalogManager* instance;
	std::map<std::string, Schema> schemaQueue;
	void writeBack(std::map<std::string, Schema> schemaQueue); 
	CatalogManager();			
};

#endif