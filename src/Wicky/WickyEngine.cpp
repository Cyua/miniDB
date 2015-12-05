#include "WickyEngine.h"
#include "key.h"
#include "block.h"
#include <set>
#include <stdexcept>
#include <map>
#include <iomanip>
#include <stdlib.h>
#include <schema.h>

WickyEngine* WickyEngine::instance = NULL;

int countInsert=0;
bool trickDelete=false;
bool okDelete=false;
WickyEngine::WickyEngine(){
	
}

WickyEngine::~WickyEngine(){
	
}

WickyEngine* WickyEngine::getInstance(){
	if (instance == NULL)
		instance = new WickyEngine();
	return instance;
}

void WickyEngine::ShowTables(){
	CatalogManager *cm = CatalogManager::getInstance();
	std::list<std::string> tables = cm->getTables();
	if (tables.size() == 0)
	{
		std::cout << "Empty database" << std::endl;
	}else{
		std::cout << " Table List:" << std::endl;
		std::list<std::string>::iterator iter;
		for(iter = tables.begin(); iter != tables.end(); iter++){
			std::cout << " "+*iter << std::endl;
		}
	}
}

void WickyEngine::DescribeTable(std::string tname){
	CatalogManager *cm = CatalogManager::getInstance();
	if (cm->isExist(tname))
	{
		std::cout << cm->get(tname).toString() << std::endl;
	}else{
		throw std::runtime_error("Table " + tname + " doesn't exist");
	}
}

void WickyEngine::createIndex(std::string indexName, std::string tableName, std::string attrName){
	std::string fullIndexName = tableName+"-"+attrName+"-"+indexName;
	BufferManager* bm = BufferManager::getInstance();
	CatalogManager* cm = CatalogManager::getInstance();
	IndexManager *im = IndexManager::getInstance();
	RecordManager rm;

	/*add index in schema,*/
	Schema sch = cm->get(tableName);
	sch.addIndex(indexName, attrName);
	cm->saveChange(sch);

	/*get the infomation of search key*/
	std::string type = sch.getType(attrName);
	int length = sch.getLength(attrName);
	std::vector<std::string> attrs = sch.getAttributes();
	int pos;
	for (int i = 0; i < attrs.size(); ++i)
	{
		if (attrs[i].compare(attrName) == 0)
		{
			pos = i;
			break;
		}
	}
    
    /*create a index file*/
    Index *index = im->createIndex(fullIndexName, type, length);

	Table tb = rm.readTable(sch, bm);
	std::vector<Tuple> rows = tb.rows;
	for (int i = 0; i < rows.size(); ++i)
	{
		std::string value = rows[i].col[pos];
		if (type == Schema::INT)
		{
			int intvalue = atoi(value.c_str());
			Key key(length, (unsigned char*)&intvalue);
			index->insertKey(key, i);
		}else if (type == Schema::FLOAT)
		{
			float floatvalue = atof(value.c_str());
			Key key(length, (unsigned char*)&floatvalue);
			index->insertKey(key, i);
		}else{
			Key key(length, (unsigned char*)value.c_str());
			index->insertKey(key, i);
		}
	}
	delete index;

}
void WickyEngine::dropIndex(std::string indexName, std::string tableName){
	BufferManager* bm = BufferManager::getInstance();
	CatalogManager* cm = CatalogManager::getInstance();
	IndexManager* im = IndexManager::getInstance();

	Schema sch = cm->get(tableName);
	std::string attrName = sch.getAttrOfIndex(indexName);
	std::string fullIndexName = tableName+"-"+attrName+"-"+indexName;
	std::string type = sch.getType(attrName);
	int length = sch.getLength(attrName);

	//drop index in schema
	sch.deleteIndex(indexName);
	cm->saveChange(sch);
	//delete index in file
	Index *index = im->getIndex(fullIndexName, type, length);
	im->dropIndex(index);
}

Table* WickyEngine::Select(Table* t, Condition c){
	using namespace std;
	//initial the mapping between operators
	map<string,int> opMap;
	opMap["="]=0;
	opMap[">"]=1;
	opMap["<"]=2;
	opMap["<>"]=3;
	opMap["<="]=4;
	opMap[">="]=5;
	countInsert = 0;




	list<pair<string,string> > cond;
	//initial the table to be returned
	Table* resultTable = new Table(t->getTableName());
	resultTable->CreateTable(t->getAttrList());
	vector<Tuple> resultRow = t->rows;		//the rows to be returned

	if(okDelete==true){
		int alreadyDeleted;
		BufferManager *bm = BufferManager::getInstance();
		bm->read(t->getTableName(),0,&alreadyDeleted);
		vector<Tuple>::iterator it = resultRow.begin()+alreadyDeleted;
		resultRow.erase(it);
	}


	vector<Attribute> tAttr = t->getAttrList();
	vector<Tuple> tempRow;
	vector<string> tempStore;				//store the condition
	list<pair<string,string> >::iterator itList;
	while(!c.empty()){				//continue if there still exists some condition
		cond = c.popCondition();
		//get rid of single quote and push into vector
		for(itList = cond.begin(); itList != cond.end(); itList++){		
			if(itList->first=="CHAR")
				itList->second = itList->second.substr(1,itList->second.size()-2);
		}
		tempStore.clear();
		for(itList = cond.begin(); itList != cond.end(); itList++){
			tempStore.push_back(itList->second);
		}

		tempRow.clear();
		int op = opMap[tempStore[1]];	//get the operator
		int position=-1;
		//find the colomun that is to be judged
		for(int i = 0; i < tAttr.size(); i++){
			if(tAttr[i].getName()==tempStore[0]){	//find the column
				position = i;
				break;
			}
		}
		if(position<0){				//didn't find the column
			throw std::runtime_error("Can't find the 'where' condition");
		}

		string type = tAttr[position].getType();
		switch(op)
		{
			case 0:						// =
			for(int i = 0; i < resultRow.size(); i++){
				if(type!="CHAR"){
					float firstCmp = atof(resultRow[i].col[position].c_str());
					float secondCmp = atof(tempStore[2].c_str());
					if(firstCmp==secondCmp)
						tempRow.push_back(resultRow[i]);
				}
				else{
					if(resultRow[i].col[position]==tempStore[2])
						tempRow.push_back(resultRow[i]);
				}
			}			
			break;				
			case 1: 					// >
			for(int i = 0; i < resultRow.size(); i++){
				if(type!="CHAR"){
					float firstCmp = atof(resultRow[i].col[position].c_str());
					float secondCmp = atof(tempStore[2].c_str());
					if(firstCmp>secondCmp)
						tempRow.push_back(resultRow[i]);
				}
				else{
					if(resultRow[i].col[position]>tempStore[2])
						tempRow.push_back(resultRow[i]);
				}
			}	
			break;				
			case 2: 					// <
			for(int i = 0; i < resultRow.size(); i++){
				if(type!="CHAR"){
					float firstCmp = atof(resultRow[i].col[position].c_str());
					float secondCmp = atof(tempStore[2].c_str());
					if(firstCmp<secondCmp)
						tempRow.push_back(resultRow[i]);
				}
				else{
					if(resultRow[i].col[position]<tempStore[2])
						tempRow.push_back(resultRow[i]);
				}
			}
			break;				
			case 3: 					// <>
			for(int i = 0; i < resultRow.size(); i++){
				if(type!="CHAR"){
					float firstCmp = atof(resultRow[i].col[position].c_str());
					float secondCmp = atof(tempStore[2].c_str());
					if(firstCmp!=secondCmp)
						tempRow.push_back(resultRow[i]);
				}
				else{
					if(resultRow[i].col[position]!=tempStore[2])
						tempRow.push_back(resultRow[i]);
				}
			}
			break;				
			case 4: 					// <=
			for(int i = 0; i < resultRow.size(); i++){
				if(type!="CHAR"){
					float firstCmp = atof(resultRow[i].col[position].c_str());
					float secondCmp = atof(tempStore[2].c_str());
					if(firstCmp<=secondCmp)
						tempRow.push_back(resultRow[i]);
				}
				else{
					if(resultRow[i].col[position]<=tempStore[2])
						tempRow.push_back(resultRow[i]);
				}
			}
			break;				
			case 5: 					// >=
			for(int i = 0; i < resultRow.size(); i++){
				if(type!="CHAR"){
					float firstCmp = atof(resultRow[i].col[position].c_str());
					float secondCmp = atof(tempStore[2].c_str());
					if(firstCmp>=secondCmp)
						tempRow.push_back(resultRow[i]);
				}
				else{
					if(resultRow[i].col[position]>=tempStore[2])
						tempRow.push_back(resultRow[i]);
				}
			}
			break;				
		}
		resultRow = tempRow;
	}
	resultTable->rows = resultRow;
	return resultTable;
}

Table* WickyEngine::Project(Table* t, std::vector<std::pair<std::string, std::string> > cs){	
	using namespace std;

	vector<Attribute> attrOri = t->getAttrList();
	vector<Attribute> attrRes;
	set<int> targetNum;
	bool flag;
	int size = attrOri.size();
	std::cout<<"int Project"<<std::endl;
	for(int i = 0; i < size; i++){
		string temp = attrOri[i].getName();
		flag = false;
		for(int j = 0; j < cs.size(); j++){
			if(temp == cs[i].second){
				flag = true;
				break;
			}
		}
		if(flag)
			targetNum.insert(i);
		else
			attrRes.push_back(attrOri[i]);
	}

	Table* result = new Table(t->getTableName());
	result->CreateTable(attrRes);
	vector<Tuple> rowsRes;
	vector<string> tempCol;
	for(int i = 0; i < t->rows.size(); i++){
		tempCol.clear();
		for(int j = 0; j < size; j++){
			if(targetNum.count(j) <= 0){
				tempCol.push_back(t->rows[i].col[j]);
			}
		}
		Tuple tempTup(tempCol);
		rowsRes.push_back(tempTup);
	}

	return result;
}

Table* WickyEngine::Join(Table* t1, Table* t2){
	std::cout << "WickyEngine::Join()" << std::endl;
	return new Table("test");
}

int WickyEngine::Insert(Table* t, std::vector<std::pair<std::string, std::string> > values){	
	countInsert++;
	//get the primary key
	CatalogManager* cm = CatalogManager::getInstance();
	std::string primaryKey;
	if(cm->isExist(t->getTableName())){
		Schema s = cm->get(t->getTableName());
		primaryKey = s.getPrimaryKey();
	}
	//judge whether the input fits the table
	using namespace std;
	if(t->getAttrNum()!=values.size())
		throw std::runtime_error("The input data can't fit the table!");
	
	// cout<<"before insertion: "<<t->rows.size()<<endl;

	vector<std::pair<string, string> >::iterator itr;
	vector<string> inputCol;
	vector<Attribute> attrList = t->getAttrList();
	for(itr = values.begin(); itr != values.end(); itr++){		
		if(itr->first=="CHAR")
			itr->second = itr->second.substr(1,itr->second.size()-2);
	}
	int countAttr = 0;
	for(itr = values.begin(); itr != values.end(); itr++){
		if(itr->first!=attrList[countAttr].getType()){		//wrong type
			throw std::runtime_error("Required a "+ attrList[countAttr].getType() +" type！ Insertion failed");		
		}
		//if it's a primary key or unique key
		if(attrList[countAttr].isUnique() || attrList[countAttr].getName()==primaryKey){
			for(int k = 0; k < t->rows.size(); k++){
				if(t->rows[k].col[countAttr]==itr->second)
					throw std::runtime_error("The attribute" + attrList[countAttr].getName()
						+ " is unique or primary! And the value is already exist! Insertion failed");
			}
		}

		if(itr->first!="CHAR")
			inputCol.push_back(itr->second);
		else{
			int attrLength = attrList[countAttr].getLength();
			// cout<<attrLength<<" "<<countAttr<<endl;
			if(attrLength < itr->second.size()){			//the string is too long
				throw std::runtime_error("The string "+itr->second + " is too long! Insertion failed");
			}
			inputCol.push_back(itr->second);
		}
		countAttr++;
	}
	
	BufferManager *bm = BufferManager::getInstance();
	RecordManager rm;
	Tuple inputTuple(inputCol);
	t->rows.push_back(inputTuple);
	rm.writeTable(*t, bm);

	cout<<"Insert:";
	for(int i = 0; i < t->getAttrNum(); i++){
		cout<<" "<<t->rows[t->rows.size()-1].col[i];
	}
	cout<<endl;
	
	// cout<<"after insertion: "<<t->rows.size()<<endl;
	// cout<<"insert: "<<endl;
	// cout<<values[0].second<<" "<<values[1].second<<endl;
	return 0;
}

int WickyEngine::Delete(Table* t, Condition c){
	countInsert = 0;
	using namespace std;
	//initial the mapping between operators
	map<string,int> opMap;
	opMap["="]=0;
	opMap[">"]=1;
	opMap["<"]=2;
	opMap["<>"]=3;
	opMap["<="]=4;
	opMap[">="]=5;
	
	vector<int> toDeleteIndex(t->rows.size());		//the index to be deleted
	for(int i = 0 ; i < toDeleteIndex.size(); i++)
		toDeleteIndex[i]=i;
	vector<Tuple> rawRow = t->rows;
	list<pair<string,string> > cond;
	vector<Attribute> tAttr = t->getAttrList();
	vector<int> tempIndex;
	vector<string> tempStore;				//store the condition
	list<pair<string,string> >::iterator itList;
	while(!c.empty()){				//continue if there still exists some condition
		cond = c.popCondition();
		//get rid of single quote and push into vector
		for(itList = cond.begin(); itList != cond.end(); itList++){		
			if(itList->first=="CHAR")
				itList->second = itList->second.substr(1,itList->second.size()-2);
		}
		tempStore.clear();
		for(itList = cond.begin(); itList != cond.end(); itList++){
			tempStore.push_back(itList->second);
		}

		int op = opMap[tempStore[1]];	//get the operator
		int position=-1;
		//find the colomun that is to be judged
		for(int i = 0; i < tAttr.size(); i++){
			if(tAttr[i].getName()==tempStore[0]){	//find the column
				position = i;
				break;
			}
		}
		if(position<0){				//didn't find the column
			throw std::runtime_error("Can't find the 'where' condition. Delete failed！");
		}

		tempIndex.clear();
		string type = tAttr[position].getType();
		switch(op)
		{
			case 0:						// =
			for(int i = 0; i < toDeleteIndex.size(); i++){
				string target = rawRow[toDeleteIndex[i]].col[position];
				if(type!="CHAR"){
					float firstCmp = atof(target.c_str());
					float secondCmp = atof(tempStore[2].c_str());
					if(firstCmp==secondCmp)
						tempIndex.push_back(toDeleteIndex[i]);
				}
				else{
					if(target==tempStore[2])
						tempIndex.push_back(toDeleteIndex[i]);
				}
			}			
			break;				
			case 1: 					// >
			for(int i = 0; i < toDeleteIndex.size(); i++){
				string target = rawRow[toDeleteIndex[i]].col[position];
				if(type!="CHAR"){
					float firstCmp = atof(target.c_str());
					float secondCmp = atof(tempStore[2].c_str());
					if(firstCmp>secondCmp)
						tempIndex.push_back(toDeleteIndex[i]);
				}
				else{
					if(target>tempStore[2])
						tempIndex.push_back(toDeleteIndex[i]);
				}
			}	
			break;				
			case 2: 					// <
			for(int i = 0; i < toDeleteIndex.size(); i++){
				string target = rawRow[toDeleteIndex[i]].col[position];
				if(type!="CHAR"){
					float firstCmp = atof(target.c_str());
					float secondCmp = atof(tempStore[2].c_str());
					if(firstCmp<secondCmp)
						tempIndex.push_back(toDeleteIndex[i]);
				}
				else{
					if(target<tempStore[2])
						tempIndex.push_back(toDeleteIndex[i]);
				}
			}
			break;				
			case 3: 					// <>
			for(int i = 0; i < toDeleteIndex.size(); i++){
				string target = rawRow[toDeleteIndex[i]].col[position];
				if(type!="CHAR"){
					float firstCmp = atof(target.c_str());
					float secondCmp = atof(tempStore[2].c_str());
					if(firstCmp!=secondCmp)
						tempIndex.push_back(toDeleteIndex[i]);
				}
				else{
					if(target!=tempStore[2])
						tempIndex.push_back(toDeleteIndex[i]);
				}
			}
			break;				
			case 4: 					// <=
			for(int i = 0; i < toDeleteIndex.size(); i++){
				string target = rawRow[toDeleteIndex[i]].col[position];
				if(type!="CHAR"){
					float firstCmp = atof(target.c_str());
					float secondCmp = atof(tempStore[2].c_str());
					if(firstCmp<=secondCmp)
						tempIndex.push_back(toDeleteIndex[i]);
				}
				else{
					if(target<=tempStore[2])
						tempIndex.push_back(toDeleteIndex[i]);
				}
			}
			break;				
			case 5: 					// >=
			for(int i = 0; i < toDeleteIndex.size(); i++){
				string target = rawRow[toDeleteIndex[i]].col[position];
				if(type!="CHAR"){
					float firstCmp = atof(target.c_str());
					float secondCmp = atof(tempStore[2].c_str());
					if(firstCmp>=secondCmp)
						tempIndex.push_back(toDeleteIndex[i]);
				}
				else{
					if(target>=tempStore[2])
						tempIndex.push_back(toDeleteIndex[i]);
				}
			}
			break;				
		}
		toDeleteIndex = tempIndex;
	}
	cout<<"Delete start! processing......."<<endl;
	if(toDeleteIndex.size()==0){
		cout<<"Can't find any record to delete"<<endl;
		return 0;
	}
	vector<Tuple>::iterator it;
	vector<Tuple> resultRow;
	resultRow.clear();
	int countIndex=0;
	for(int i = 0; i < t->rows.size(); i++){
		int index = toDeleteIndex[countIndex];
		if(index==i){
			it = t->rows.begin() + index;
			cout<<"delete record: ";
			for(int j = 0; j < (*it).col.size(); j++){
				cout<<setw(10)<<(*it).col[j];
			}
			cout<<endl;
			countIndex++;
			if(countIndex >= toDeleteIndex.size())
				countIndex = toDeleteIndex.size()-1;
		}
		else{
			resultRow.push_back(t->rows[i]);
		}
	}
	int recordNum = t->rows.size()-resultRow.size();
	cout<<"Totally delete "<<recordNum<<" records"<<endl;
	if(trickDelete==false){
		t->rows = resultRow;
		BufferManager *bm = BufferManager::getInstance();
		RecordManager rm;
		// cout<<"before write table"<<endl;
		rm.writeTable(*t, bm);
	}
	else{
		BufferManager *bm = BufferManager::getInstance();
		bm->write(t->getTableName(),0,toDeleteIndex[0]);
		okDelete=true;
	}
	return 0;
}


void Split(std::string src, std::string separator, std::vector<std::string>& dest)
{
	dest.clear();
    std::string str = src;
    std::string substring;
    std::string::size_type start = 0, index;

    do
    {
        index = str.find_first_of(separator,start);
        if (index != std::string::npos)
        {    
            substring = str.substr(start,index-start);
            dest.push_back(substring);
            start = str.find_first_not_of(separator,index);
            if (start == std::string::npos) return;
        }
    }while(index != std::string::npos);
    
    //the last token
    substring = str.substr(start);
    dest.push_back(substring);
}

bool attrFlag=false;
std::vector<Attribute> attrStable;
int WickyEngine::InsertByName(std::string filename, std::vector<std::pair<std::string, std::string> > values){
	if(countInsert<100){
		Table* tempTable = GetTable(filename);
		Insert(tempTable, values);
	}
	else{
		trickDelete = true;
		using namespace std;
		if(attrFlag==false){
			CatalogManager* cm = CatalogManager::getInstance();
			Schema s = cm->get(filename);
			s.copyAttributes(attrStable);
			attrFlag=true;
		}
		BufferManager *b = BufferManager::getInstance();
		int offset;
		b->read(filename,0,&offset);
		RecordManager rm;

		/********************************************************/
		vector<std::pair<string, string> >::iterator itr;
		// vector<string> inputCol;
		// vector<Attribute> attrList = t->getAttrList();
		for(itr = values.begin(); itr != values.end(); itr++){		
			if(itr->first=="CHAR")
				itr->second = itr->second.substr(1,itr->second.size()-2);
		}
		string output="";
		for(itr = values.begin(); itr != values.end(); itr++){
			output=output+" "+itr->second;
		}

		b->write(filename, Block::BLOCK_SIZE+offset, output);
		offset+=output.size();
		b->write(filename, 0, offset);
		cout<<"Insert:";
		for(itr = values.begin(); itr != values.end(); itr++){
			cout<<" "<<itr->second;
		}
		cout<<endl;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////
int WickyEngine::DeleteByName(std::string filename, Condition c){
	Table* tempTable = GetTable(filename);
	Delete(tempTable,c);
	return 0;
}
/***************************************************************/
int WickyEngine::Update(Table* t, Condition c){
	
}

void WickyEngine::CreateTable(Schema sch){	
	// using std::cout;
	// using std::endl;
	// std::vector<Attribute> attrList;
	// sch.copyAttributes(attrList);
	// for(int i = 0; i < attrList.size(); i++){
	// 	cout<<i<<": "<<attrList[i].getName()<<" "<<attrList[i].getType()<<
	// 	" "<<attrList[i].getLength()<<endl;
	// }

	BufferManager *b = BufferManager::getInstance();
	CatalogManager* cm = CatalogManager::getInstance();
	RecordManager rm;
	cm->store(sch);
	Table t(sch.getName());
	std::vector<Attribute> attrList;
	sch.copyAttributes(attrList);
	if(t.CreateTable(attrList)){
		// std::cout<<"create table------"<<std::endl;
		rm.writeTable(t, b);
	}else{
		throw std::runtime_error("Table should have at least one column");
	}
}

int WickyEngine::DropTable(std::string name){

	// std::cout << "WickyEngine::DropTable()" << std::endl;
	// std::cout << name << std::endl;

	CatalogManager* cm = CatalogManager::getInstance();
    if(cm->isExist(name)){
    	BufferManager *bm = BufferManager::getInstance();
		RecordManager rm;
		Schema sch = cm->get(name);
		std::vector<std::string> v = sch.getIndecies();
		for (int i = 0; i < v.size(); ++i)
		{
			dropIndex(v[i], name);
		}
    	cm->drop(name);
    	rm.deleteTable(name, bm);
    }else{
    	throw std::runtime_error("Table " + name + " doesn't exists");
    }

	return 0;
}

Table* WickyEngine::GetTable(std::string name){		
	BufferManager *bm = BufferManager::getInstance();
	CatalogManager* cm = CatalogManager::getInstance();
	RecordManager rm;
	if(cm->isExist(name)){
		Schema s = cm->get(name);
		Table t = rm.readTable(s, bm);
		return new Table(t);
	}else{
		throw std::runtime_error("Table " + name + " doesn't exist");
	}
}