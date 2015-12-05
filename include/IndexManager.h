#ifndef _INDEXMANAGER_H
#define _INDEXMANAGER_H

#include <string>
#include <stdexcept>

#include "Schema.h"
#include "Index.h"
#include "BufferManager.h"

class IndexManager{
private:
	static IndexManager* instance;
	IndexManager();
public:
	virtual ~IndexManager();
	static IndexManager* getInstance();	
	//createIndex("test", Schema::INT, Schema::INT_LENGTH);
	Index* createIndex(std::string name, std::string type, int keyLen);
	//getIndex("test", Schema::INT, Schema::INT_LENGTH);
	Index* getIndex(std::string name, std::string type, int keyLen);
	void deleteIndex(Index* index);
	void dropIndex(Index* index);
};

#endif