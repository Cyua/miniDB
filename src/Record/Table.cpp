#include <Table.h>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

Table::Table(std::string name){
	tableName = name;
	attrNum=0;
	tailOffset=0;
}

bool Table::CreateTable(std::vector<Attribute> v){
	attrList = v;
	if(attrList.size()==0)
		return false;
	attrNum = attrList.size();
	return true;
}

void Table::printTable(){
	for(int i=0; i<attrList.size(); i++){
		std::cout<<std::setw(12)<<attrList[i].getName();
	}
	std::cout<<std::endl;
	for(int i=0; i<rows.size(); i++){
		for(int j=0; j<rows[i].col.size(); j++){
			std::cout<<std::setw(12)<<rows[i].col[j];
		}
		std::cout<<std::endl;
	}
}