#ifndef _TABLE_H
#define _TABLE_H
#include <string>
#include <vector>
#include <set>
#include "Schema.h"



// class Attribute
// {
// private:
// 	std::string attrName;
// 	std::string type;
// 	int length;
// 	std::string index;
// 	bool unique;
//     friend class Schema;

// public:
// 	Attribute(){};
// 	Attribute(std::string name, std::list<std::string> properties);
// 	~Attribute(){};
// 	std::string getName(){return attrName;}
// 	std::string getType(){return type;}
// 	int getLength(){return length;}
// 	std::string getIndex(){return index;}
// 	bool isUnique(){return unique;}
// };
/*





*/

class Tuple{
public:
	std::vector<std::string> col; //each column
	Tuple(){}
	Tuple(std::vector<std::string> v){
		col = v;
	}
};


class Table{
private:
	std::string tableName;			//the name of table
	int attrNum;				//the number of attributes(column)
	std::vector<Attribute> attrList;	//the list of attributes
	int tailOffset;

public:
	std::vector<Tuple> rows;			//the data of the table	
	// std::set<int> deletedIndex;
	Table(std::string tableName);
	bool CreateTable(std::vector<Attribute> attrList);	//initial the table with attributes list

	int getAttrNum(){return attrNum;}					//return the number of attributes
	// int getTableLen(){return length;}
	std::string getTableName(){return tableName;}		//return the table name

	void printTable();					//print all the rows of table

	std::vector<Attribute> getAttrList(){return attrList;}	

	int getTailOffset(){return tailOffset;}
	void setTailOffset(int x){tailOffset=x;}
};

#endif