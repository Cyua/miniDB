#include "WickyFile.h"

const int WickyFile::FILE_REDIRECT = 0;
const int WickyFile::FILE_READ = 1;
const int WickyFile::FILE_WRITE = 2;

WickyFile::WickyFile(std::string fileName, int flag)
	:fileName(fileName), flag(flag), fptr(0){
	//open file and create file if not exists
	std::string tmp;
	std::stringstream ss;
	ss << "res/";
	ss << fileName;
	ss >> tmp;
	file = fopen(tmp.c_str(), "rb+");
	if (file == NULL) {
		file = fopen(tmp.c_str(), "w");
		fclose(file);
		file = fopen(tmp.c_str(), "rb+");
	}
	if (file == NULL)
		throw std::runtime_error("file " + fileName + " create faile");
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);
}

WickyFile::~WickyFile(){		
	fclose(file);	
}

std::string WickyFile::getFileName(){
	return fileName;
}

FILE* WickyFile::getFile(){
	return file;
}

void WickyFile::setFlag(int flag){
	this->flag = flag;
}

int WickyFile::getFlag(){
	return flag;
}

int WickyFile::setFptr(int ptr){
	fptr = ptr;	
	if (fptr > size)
		size = fptr; 
}

int WickyFile::getFptr(){
	return fptr;
}

int WickyFile::getSize(){
	return size;
}