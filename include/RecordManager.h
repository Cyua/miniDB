#include "Table.h"
#include "BufferManager.h"
#include "block.h"

#include <vector>
#include <iostream>
#include <stdio.h>

#ifndef _RECORDMANAGER_H
#define _RECORDMANGAER_H


class RecordManager{
public:
	//if offset is 2, then insert **before** the tuple on index 2, index starts from zero
	bool insertTuple(Table* table, Tuple tuple, int offset);	//return true if insertion successed

	//if offset is 2, then delete the tuple on index 2
	bool deleteTuple(Table* table, int offset);					//return true if deletion successe
	//return a vector containing the tuples indicated by offsets
	std::vector<Tuple> selectTuple(Table* table, std::vector<int> offset);  

	Table readTable(Schema s, BufferManager *b);
	bool writeTable(Table table, BufferManager *b);
	void deleteTable(std::string tableName, BufferManager *b);	//drop table
	void triWrite(std::string tableName, BufferManager *b , int key);
	void Split(std::string src, std::string separator, std::vector<std::string>& v);
};

#endif