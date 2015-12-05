#include "Schema.h"

const std::string Schema::INT = "INT";
const std::string Schema::CHAR = "CHAR";
const std::string Schema::FLOAT = "FLOAT";

const int Schema::INT_LENGTH = 4;
const int Schema::FLOAT_LENGTH = 8;

const std::string Schema::NOINDEX = "NOINDEX";
const std::string Schema::BTREE = "BTREE";

const std::string Schema::UNIQUE = "UNIQUE";
const std::string Schema::DUPLIC = "DUPLIC";

Attribute::Attribute(std::string name, std::list<std::string> properties){
    if(properties.size() == 4){
        attrName = name;
        std::list<std::string>::iterator iter;
        iter = properties.begin();
        type = *iter++;
        length = Schema::stringToInt(*iter++);
        index = *iter++;
        unique = !(*iter).compare(Schema::UNIQUE);
    }else{
        throw std::runtime_error("Wrong amount of properties");
    }

}

Schema::Schema(std::string tableName){
    this->tableName = tableName;
    this->primaryKey = "NULL";
}

// Schema::Schema(std::string tableName, std::map<std::string, std::list<std::string> > attrs){
//      this->tableName = tableName;
//      this->primaryKey = "NULL";
//      std::map<std::string, std::list<std::string> >::iterator iter;
//      for (iter = attrs.begin(); iter != attrs.end(); iter++)
//      {
//         std::string attrName = iter->first;
//         std::list<std::string> properties = iter->second;
//         if(attributes.count(attrName) == 1){
//             throw std::runtime_error("Duplicate column name " + attrName);
//         }else{
//             Attribute attr(attrName, properties);
//             attributes[attrName] = attr;
//             attrNames.push_back(attrName);
//             if(!(attr.index.compare(NOINDEX) == 0)){
//                 indecies[attr.index] = attrName;
//             }
//         }
//      }
// }



std::string Schema::getName(){
    return tableName;
}

std::string Schema::toString(){
    std::string str = "";
    std::string LB = "(";
    std::string RB = ")";
    std::string Q = "'";
    std::string SP = " ";
    str += "TABLE " + SP + tableName + SP + LB + "\n";
    for (int i = 0; i < attrNames.size(); i++){
        std::string attrName = attrNames[i];
        Attribute attr = attributes[attrName];
        str += " "+Q+attrName+Q+" "+attr.type+LB+intToString(attr.length)+RB+" "+attr.index;
        if(attr.unique)
            str += " " + UNIQUE;
        else
            str += " " + DUPLIC;
        str += ",\n"; 
    }
    str += " PRIMARY KEY " + SP + Q+primaryKey+Q + ",\n" + RB;
    return str;
}

void Schema::addAttribute(std::string attrName, std::string type){
    if(isAttrExists(attrName))
        throw std::runtime_error("Attribute " + attrName + "already exist");
    std::list<std::string> properties;
    properties.push_back(type);
    if (type.compare(INT) == 0)
    {
        properties.push_back(Schema::intToString(INT_LENGTH));
    }else if (type.compare(FLOAT) == 0)
    {
        properties.push_back(Schema::intToString(FLOAT_LENGTH));
    }else{
        properties.push_back("20");
    }
    properties.push_back(NOINDEX);
    properties.push_back(DUPLIC);
    Attribute attr(attrName, properties);
    attributes[attrName] = attr;
    attrNames.push_back(attrName);
}

bool Schema::isAttrExists(std::string attrName){
    return attributes.count(attrName);
}

bool Schema::isIndexExists(std::string indexName){
    return indecies.count(indexName);
}

std::string Schema::getAttrOfIndex(std::string indexName){
    if (!isIndexExists(indexName))
    {
        throw std::runtime_error("Index " + indexName + " already exists");
    }
    return indecies[indexName];
}

std::string Schema::getIndex(std::string attrName){
    if(!isAttrExists(attrName)){
        throw std::runtime_error("Attribute " + attrName + " doesn't exist");
    }
    return attributes[attrName].index;
}

std::vector<std::string> Schema::getIndecies(){
    std::vector<std::string> v;
    std::map<std::string, std::string>::iterator it;
    for(it = indecies.begin(); it != indecies.end(); it++){
        v.push_back(it->first);
    }
    return v;
}

void Schema::addIndex(std::string indexName, std::string attrName){
    if(!isAttrExists(attrName)){
        throw std::runtime_error("Attribute " + attrName + " doesn't exist");
    }
    if(isIndexExists(indexName)){
        throw std::runtime_error("Index " + indexName + " already exists");
    }
    if(attributes[attrName].index.compare(NOINDEX) == 0){
        attributes[attrName].index = indexName;
        indecies[indexName] = attrName;
    }else{
        throw std::runtime_error("Index on column " + attrName + " already exist");
    }

}

void Schema::deleteIndex(std::string indexName){
    if(isIndexExists(indexName)){
        std::string attrName = indecies[indexName];
        attributes[attrName].index = NOINDEX;
        indecies.erase(indexName);
    }else{
        throw std::runtime_error("Index " + indexName + "doesn't exist");
    }
}

void Schema::setPrimaryKey(std::string attrName){
    if(!isAttrExists(attrName))
        throw std::runtime_error("Attribute " + attrName + " doesn't exist");
    primaryKey = attrName;
    attributes[attrName].unique = true;
}

std::string Schema::getPrimaryKey(){
    return primaryKey;
}

void Schema::setUnique(std::string attrName){
    if(!isAttrExists(attrName))
        throw std::runtime_error("Attribute " + attrName + " doesn't exist");
    attributes[attrName].unique = true;
}

bool Schema::isUnique(std::string attrName){
    if(!isAttrExists(attrName))
        throw std::runtime_error("Attribute " + attrName + " doesn't exist");
    return attributes[attrName].unique;
}

void Schema::setType(std::string attrName, std::string type){
    if(!isAttrExists(attrName))
        throw std::runtime_error("Attribute " + attrName + " doesn't exist");
    attributes[attrName].type = type;
}

std::string Schema::getType(std::string attrName){
    if(!isAttrExists(attrName))
        throw std::runtime_error("Attribute " + attrName + " doesn't exist");
    return attributes[attrName].type;
}

void Schema::setLength(std::string attrName, int length){
    if(!isAttrExists(attrName))
        throw std::runtime_error("Attribute " + attrName + " doesn't exist");
    attributes[attrName].length = length;
}

int Schema::getLength(std::string attrName){
    if(!isAttrExists(attrName))
        throw std::runtime_error("Attribute " + attrName + " doesn't exist");
    return attributes[attrName].length;
}

Attribute Schema::getAttribute(std::string attrName){
    if(!isAttrExists(attrName))
        throw std::runtime_error("Attribute " + attrName + " doesn't exist");
    return attributes[attrName];
}

std::vector<std::string> Schema::getAttributes(){
    return attrNames;
}

void Schema::copyAttributes(std::vector<Attribute>& container){
    container.clear();
    for(int i = 0; i < attrNames.size(); i++){
        std::string name = attrNames[i];
        container.push_back(attributes[name]);
    }
}

Schema Schema::createSchema(std::string schString){
    std::vector<std::string> attrStrings = Split(schString, "\n");
    int size = attrStrings.size();
    std::string tname;
    tname = Split(attrStrings[0], " ")[1];
    Schema  result(tname);
    for(int i=1; i<=size-3; i++){
        std::list<std::string> attr;
        std::string attrName;
        std::string attrString = attrStrings[i].replace(attrStrings[i].find(","), 1, " ");
        std::vector<std::string> properties = Split(attrStrings[i], " ");
        attrName = Split(properties[0], "'")[0];
        attr.push_back(Split(properties[1], "(")[0]);
        attr.push_back(Split(Split(properties[1], "(")[1], ")")[0]);
        attr.push_back(properties[2]);
        attr.push_back(properties[3]);
        Attribute a(attrName, attr);
        result.attrNames.push_back(attrName);
        result.attributes[attrName] = a;
        if(!(a.index.compare(NOINDEX) == 0))
                result.indecies[a.index] = attrName;
    }
    std::string key = Split(Split(attrStrings[size-2], " ")[2], "'")[0];
    result.primaryKey = key;
    return result;
}

std::string Schema::intToString(int i){
    std::stringstream ss;
    std::string s;
    ss << i;
    ss >> s;
    return s;
}

int Schema::stringToInt(std::string str){
    return atoi(str.c_str());
}

std::vector<std::string> Schema::Split(std::string str,std::string pattern)
{
    std::vector<std::string> result;

    int i = str.find_first_not_of(pattern);
    int j;
    while(i != std::string::npos){
        j = str.find(pattern, i);
        result.push_back(str.substr(i, j-i));
        i = str.find_first_not_of(pattern, j);
    }
    return result;
}
