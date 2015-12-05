#ifndef _SCHEMA_H
#define _SCHEMA_H

#include <map>
#include <list>
#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

class Attribute
{
private:
	/**
	* the value of variable can be: type{INT, CHAR, FLOAT}, index{NOINDEX or the index name}
	*/
	std::string attrName;
	std::string type;
	int length;
	std::string index;
	bool unique;
    friend class Schema;

public:
	Attribute(){};
	/**
	* @param properties is a list of length 4
	* if there are too much or too little property, it will throw a runtime error
	*/
	Attribute(std::string name, std::list<std::string> properties);
	~Attribute(){};
	std::string getName(){return attrName;}
	std::string getType(){return type;}
	int getLength(){return length;}
	std::string getIndex(){return index;}
	bool isUnique(){return unique;}
};

class Schema {
private:
	std::string tableName;
	std::vector<std::string> attrNames;
	std::map<std::string, Attribute> attributes;
	std::map<std::string, std::string> indecies;
	std::string primaryKey;

	static Schema createSchema(std::string schString); //recover a schema from a string
	friend class CatalogManager;

public:
	Schema(){};
	// Schema(std::string tableName, std::map<std::string, std::list<std::string> > attrs);
	Schema(std::string tableName);
	~Schema(){};

	std::string getName(); //return table name
	std::string toString(); //convert the schema to a string for the convenience of print or store
	/**
	* add a new attribute to schema 
	* the default value is: length = "20", index = NOINDEX, isunique = DUPLIC
	* the funciton will throw a runtime error is the attribute is exist
	*/
	void addAttribute(std::string attrName, std::string type);
	bool isAttrExists(std::string attrName);
	bool isIndexExists(std::string indexName);
	std::string getAttrOfIndex(std::string indexName); //get the attribute has this index
	/**
	* return NOINDEX, if there is no index on attribute
	* return the index name if the index is exist
	*/
	std::string getIndex(std::string attrName); 
	std::vector<std::string> getIndecies();
	/**
	* add b+ Tree index to the attribute
	* if the index is exist, it will throw a runtime error
	*/
	void addIndex(std::string indexName, std::string attrName); 
	/**
	* delete index
	* if the index is not exist, it will throw a runtime error
	*/
	void deleteIndex(std::string indexName);
	void setPrimaryKey(std::string attrName); //set the attribute to be unique at the same time
	std::string getPrimaryKey(); //returns "NULL" if is not defined
	void setUnique(std::string attrName);
	bool isUnique(std::string attrName);
	void setType(std::string attrName, std::string type); 
	std::string getType(std::string attrName);
	void setLength(std::string attrName, int length);
	int getLength(std::string attrName);
	Attribute getAttribute(std::string attrName);
	std::vector<std::string> getAttributes(); //return a list of all attributes' name
	void copyAttributes(std::vector<Attribute>& container);

	static std::string intToString(int i);
	static int stringToInt(std::string str);
    static std::vector<std::string> Split(std::string str,std::string pattern);


	const static std::string INT;
	const static std::string CHAR;
	const static std::string FLOAT;
	
	const static int INT_LENGTH;
	const static int FLOAT_LENGTH;

	const static std::string NOINDEX;
	const static std::string BTREE;
	
	const static std::string UNIQUE;
	const static std::string DUPLIC;

};

#endif