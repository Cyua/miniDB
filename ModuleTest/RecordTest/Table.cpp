#include "Table.h"
#include <string>
#include <vector>

attribute::attribute(int datatype, std::string attrName, bool isPrimary, bool isUnique){
	this->datatype = datatype;
	this->attrName = attrName;
	this->isPrimary = isPrimary;
	this->isUnique = isUnique;
}
attribute::attribute(){
	this->datatype = 0;
	this->attrName = "NULL";
	this->isPrimary = false;
	this->isUnique = false;
}

Table::Table(std::string name){
	tableName = name;
	attrNum=0;
}

bool Table::CreateTable(std::vector<attribute> v){
	attrList = v;
	if(attrList.size()==0)
		return false;
	attrNum = attrList.size();
	return true;
}