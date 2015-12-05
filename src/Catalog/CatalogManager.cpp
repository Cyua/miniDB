#include "CatalogManager.h"

CatalogManager* CatalogManager::instance = NULL;
const std::string CatalogManager::CATALOG = "WICKY_CATALOG.wk";
const int CatalogManager::ENDFILE = 10241024;

CatalogManager::CatalogManager(){
	int strLength;
	std::string schString;
	BufferManager* bm = BufferManager::getInstance();
	if(bm->isFileExists(CATALOG)){
		bm->redirect(CATALOG);
		bm->read(CATALOG, &strLength);
		while(strLength != ENDFILE){
			bm->read(CATALOG, &schString, strLength);
			Schema sch = Schema::createSchema(schString);
			schemaQueue[sch.getName()] = sch;
			bm->read(CATALOG, &strLength);
		}	
	}
}

CatalogManager::~CatalogManager(){
	writeBack(schemaQueue);
}

CatalogManager* CatalogManager::getInstance(){
	if (instance == NULL)
		instance = new CatalogManager();
	return instance;
}

void CatalogManager::store(Schema sch){
	if (isExist(sch.getName()))
		throw std::runtime_error("Table " + sch.getName() + " already exists");
	if (sch.attributes.size() == 0)
		throw std::runtime_error("A table should have at least one attribute");
	schemaQueue[sch.getName()] = sch;
}

void CatalogManager::saveChange(Schema sch){
	if (!isExist(sch.getName()))
	{
		throw std::runtime_error("Use CatalogManager::store() to save a new schema");
	}
	schemaQueue[sch.getName()] = sch;
}

void CatalogManager::drop(std::string tableName){
	if (!isExist(tableName))
		throw std::runtime_error("table " + tableName + " doesn't exists");
	schemaQueue.erase(tableName);
}

Schema CatalogManager::get(std::string name){
	if (!isExist(name))
		throw std::runtime_error("table " + name + " doesn't exists");
	return schemaQueue.find(name)->second;
}

std::list<std::string> CatalogManager::getTables(){
	std::map<std::string, Schema>::iterator itr;
	std::list<std::string> ret;
	for (itr = schemaQueue.begin(); itr != schemaQueue.end(); itr++){
		ret.push_back(itr->first);
	}
	return ret;
}

bool CatalogManager::isExist(std::string name){
	return schemaQueue.find(name)!=schemaQueue.end();
}

void CatalogManager::writeBack(std::map<std::string, Schema> schemaQueue){
    //write back the schemas to file when destructor is executing
    int strLength;
    std::string schString;
    BufferManager* bm = BufferManager::getInstance();
    if (bm->isFileExists(CATALOG))
    {
	    bm->removeFile(CATALOG);
    }
    bm->redirect(CATALOG);
    std::map<std::string, Schema>::iterator iter;
    for(iter = schemaQueue.begin();iter != schemaQueue.end(); iter++){
    	Schema sch = iter->second;
    	schString = sch.toString();
    	strLength = schString.length();
    	bm->write(CATALOG, strLength);
    	bm->write(CATALOG, schString);
    }
    bm->write(CATALOG, ENDFILE);
}