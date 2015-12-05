#include "BufferManager.h"

BufferManager* BufferManager::instance = NULL;
const int BufferManager::READ_ALL = -1;
const int BufferManager::MEM_LIMIT = 64*1024;

BufferManager::BufferManager():
	mem_size(0), block_load(0), block_dump(0){
		
}

BufferManager::~BufferManager(){
	// CatalogManager* cm = CatalogManager::getInstance();
	// delete cm;
	std::map<WickyFile*, std::map<int, Block*>* >::iterator fileItr;
		
	for (fileItr = blockIndex.begin(); fileItr != blockIndex.end(); fileItr++){
		std::map<int, Block*>* eachFile = fileItr->second;
		std::map<int, Block*>::iterator blockItr;		
		for (blockItr = eachFile->begin(); blockItr != eachFile->end(); blockItr++){			
			delete blockItr->second;			
		}
		delete eachFile;
	}
	std::map<std::string, WickyFile*>::iterator itr;
	for (itr = filePile.begin(); itr != filePile.end(); itr++){
		delete itr->second;
	}
	std::cout << "block load time:" << block_load << std::endl;
	std::cout << "block dump time:" << block_dump << std::endl; 
}

BufferManager* BufferManager::getInstance(){
	if (instance == NULL)
		instance = new BufferManager();
	return instance;
}

Block* BufferManager::getBlock(WickyFile* wf, int n){
	std::map<int, Block*>* fileBlockIndex;
	std::map<WickyFile*, std::map<int, Block*>* >::iterator itr;
	itr = blockIndex.find(wf);
	if (itr == blockIndex.end()){
		fileBlockIndex = new std::map<int, Block*>;
		blockIndex.insert(std::map<WickyFile*, std::map<int, Block*>* >::value_type(wf, fileBlockIndex));
	} else {
		fileBlockIndex = itr->second;
	}		
	
	Block* block;
	std::map<int, Block*>::iterator blockItr = fileBlockIndex->find(n);
	if (blockItr == fileBlockIndex->end()){
		block = new Block(wf, n);		
		fileBlockIndex->insert(std::map<int, Block*>::value_type(n, block));
		mem_size += 1;
		buffer.push_back(block);
	} else {
		block = blockItr->second;
	}
	
	return block;
}

void BufferManager::sweep(){
	while (mem_size > MEM_LIMIT) {
		Block* block = buffer.front();
		buffer.pop_front();
		mem_size -= 1;
		std::map<int, Block*>* fileBlockIndex = blockIndex.find(block->getFile())->second;
		fileBlockIndex->erase(block->getIndex());
		delete block;
	}
}

void BufferManager::writeDisk(WickyFile* wf, int offset, int len, unsigned char* buf){	
	block_dump += 1;
	fseek(wf->getFile(), offset, SEEK_SET);
	fwrite(buf, len, 1, wf->getFile());	
}

void BufferManager::readDisk(WickyFile* wf, int offset, int len, unsigned char* buf){
	block_load += 1;
	fseek(wf->getFile(), offset, SEEK_SET);	
	fread(buf, len, 1, wf->getFile());	
}

void BufferManager::redirect(std::string name, int offset){
	WickyFile* wf = getFile(name);
	wf->setFptr(offset);
}

void BufferManager::removeFile(std::string name){
	if (!isFileExists(name)) {
		throw std::runtime_error("file " + name + " doesn't exist");
	}
	std::map<std::string, WickyFile*>::iterator itr = filePile.find(name);
	
	WickyFile* wf;	
	if (itr != filePile.end()){		
		wf = itr->second;
		filePile.erase(itr);
		std::map<WickyFile*, std::map<int, Block*>* >::iterator fileItr;
		fileItr = blockIndex.find(wf);
		blockIndex.erase(fileItr);
		std::map<int, Block*>* eachFile = fileItr->second;
		std::map<int, Block*>::iterator blockItr;
		for (blockItr = eachFile->begin(); blockItr != eachFile->end(); blockItr++){
			delete blockItr->second;
		}		
		delete wf;
	}
	std::string tmp;
	std::stringstream ss;
	ss << "res/";
	ss << name;
	ss >> tmp;
	remove(tmp.c_str());
}

int BufferManager::eof(std::string name){
	WickyFile* wf = getFile(name);
	return wf->getSize() == wf->getFptr();
}

int BufferManager::readAll(std::string name, int offset, unsigned char* buf){
	if (!isFileExists(name))
		throw std::runtime_error("file " + name + " doesn't exist");
	WickyFile* wf = getFile(name);	
	wf->setFptr(offset);
	return read(name, offset, wf->getSize() - offset, buf);	
}

int BufferManager::write(std::string name, int offset, int len, unsigned char* buf){
	WickyFile* wf = getFile(name, WickyFile::FILE_WRITE);
	int ret=0;
	if (len < 0)
		throw std::runtime_error("write file " + name + " , index is out of range");
	int position = offset;
	while (len > 0){
		Block* block = getBlock(wf, position / Block::BLOCK_SIZE);
		sweep();
		int tmp = block->write(position, len, buf+ret);
		position += tmp;
		len -= tmp;
		ret += tmp;
	}
	return ret;
}

int BufferManager::read(std::string name, int offset, int len, unsigned char* buf){	
	WickyFile* wf = getFile(name, WickyFile::FILE_READ);	 
	int ret=0;
	if (!isFileExists(name))
		throw std::runtime_error("file " + name + " doesn't exist");	
	if (len < 0)
		throw std::runtime_error("write file " + name + " , index is out of range");			
	int position = offset;
	while (len > 0){
		Block* block = getBlock(wf, position / Block::BLOCK_SIZE);
		sweep();
		int tmp = block->read(position, len, buf+ret);
		
		if (tmp == 0) break;
		position += tmp;
		len -= tmp;
		ret += tmp;
	}
	return ret;
}

int BufferManager::write(std::string name, int len, unsigned char* buf){
	WickyFile* wf = getFile(name, WickyFile::FILE_WRITE);	
	if (len < 0)
		throw std::runtime_error("write file " + name + " , length is out of range");
	return write(name, wf->getFptr(), len, buf);
}

int BufferManager::read(std::string name, int len, unsigned char* buf){
	WickyFile* wf = getFile(name, WickyFile::FILE_WRITE);
	if (!isFileExists(name))
		throw std::runtime_error("file " + name + " doesn't exist");
	if (len < 0)
		throw std::runtime_error("write file " + name + " , length is out of range");
	return read(name, wf->getFptr(), len, buf);
}

WickyFile* BufferManager::getFile(std::string name, int flag){
	std::map<std::string, WickyFile*>::iterator itr = filePile.find(name);
	WickyFile* wf;
	if (itr == filePile.end()){
		wf = new WickyFile(name, flag);
		filePile.insert(std::map<std::string, WickyFile*>::value_type(name, wf));
		return wf;
	}
	wf = itr->second;
	if (flag != WickyFile::FILE_REDIRECT)
		if (flag != wf->getFlag()){
			wf->setFlag(flag);			
			fseek(wf->getFile(), SEEK_SET, ftell(wf->getFile()));
		}
	return wf;
}

bool BufferManager::isFileExists(std::string name){
	std::string tmp;
	std::stringstream ss;
	ss << "res/";
	ss << name;
	ss >> tmp;
	return access(tmp.c_str(), 0)==0;
}

int BufferManager::write(std::string name, int offset, int n){
	unsigned char buf[Schema::INT_LENGTH];
	intToBytes(n, buf);
	write(name, offset, Schema::INT_LENGTH, buf);
}

int BufferManager::read(std::string name, int offset, int *n){
	read(name, offset, Schema::INT_LENGTH, (unsigned char *)n);
}

int BufferManager::write(std::string name, int n){
	unsigned char buf[Schema::INT_LENGTH];
	intToBytes(n, buf);
	write(name, Schema::INT_LENGTH, buf);
}

int BufferManager::read(std::string name, int *n){	
	read(name, Schema::INT_LENGTH, (unsigned char *)n);	
}

int BufferManager::write(std::string name, int offset, double n){
	unsigned char buf[Schema::FLOAT_LENGTH];
	doubleToBytes(n, buf);
	write(name, offset, Schema::FLOAT_LENGTH, buf);
}

int BufferManager::read(std::string name, int offset, double *n){
	read(name, offset,Schema::FLOAT_LENGTH, (unsigned char *)n);
}

int BufferManager::write(std::string name, double n){
	unsigned char buf[Schema::FLOAT_LENGTH];
	doubleToBytes(n, buf);
	write(name, Schema::FLOAT_LENGTH, buf);
}

int BufferManager::read(std::string name, double *n){
	read(name, Schema::FLOAT_LENGTH, (unsigned char *)n);
}

int BufferManager::write(std::string name, int offset, std::string str){
	write(name, offset, str.length(), (unsigned char*)str.c_str());
}

int BufferManager::read(std::string name, int offset, std::string *str, int len){
	if (len < 0)
		throw std::runtime_error("write str into file " + name + " , whose length is out of range");	
	unsigned char *buf = new unsigned char[len+1];
	read(name, offset, len, buf);
	buf[len] = 0;
	*str = (char *)buf;
	delete[] buf;
}

int BufferManager::write(std::string name, std::string str){
	write(name, str.length(), (unsigned char*)str.c_str());
}

int BufferManager::read(std::string name, std::string *str, int len){
	if (len < 0)
		throw std::runtime_error("write file " + name + " , length is out of range");
	unsigned char *buf = new unsigned char[len+1];
	read(name, len, buf);
	buf[len] = 0;
	*str = (char *)buf;
	delete[] buf;
}

/*
@n: input integer
@bytes: destination pointer
@return: bytes length
*/
void BufferManager::intToBytes(int n, unsigned char* bytes){
	memcpy(bytes, &n, Schema::INT_LENGTH);	
}

/*
@n: input float
@bytes: destination pointer
@return: bytes length
*/	
void BufferManager::doubleToBytes(double n, unsigned char* bytes){
	memcpy(bytes, &n, Schema::FLOAT_LENGTH);	
}


void BufferManager::stringToBytes(std::string str, unsigned char* bytes){
	int len = str.length();
	memcpy(bytes, str.c_str(), len);	
}