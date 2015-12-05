#ifndef _TABLE_H
#define _TABLE_H
#include <string>
#include <vector>


class Tuple{
public:
	std::vector<std::string> col; //each column
};


class attribute{
public:
	int datatype;				//int=0, char=1, float=2
	std::string attrName;		//the name of the attribute
	bool isPrimary;				//True if it is a primary key	
	bool isUnique;				//True if it is unique
	//bool hasIndex;				//True if it has an index	
	attribute();
	attribute(int datatype, std::string attrName, bool isPrimary=false, bool isUnique=false);
};


class Table{
private:
	std::string tableName;			//the name of table
	int attrNum;				//the number of attributes(column)
	std::vector<attribute> attrList;	//the list of attributes

public:
	std::vector<Tuple> rows;			//the data of the table	
	Table(std::string tableName);
	bool CreateTable(std::vector<attribute> attrList);
	int getAttrNum(){return attrNum;}
};

#endif