#ifndef _BUFFERMANAGER_H
#define _BUFFERMANAGER_H

#include <string>
#include <iostream>
#include <map>
#include <list>
#include <stdio.h>
#include <string.h>
#include <io.h>

#include "WickyFile.h"
#include "Schema.h"
#include "Block.h"

class Block;
class BufferManager{
private:
	static BufferManager* instance;
	int mem_size;
	int block_load, block_dump;
	std::map<std::string, WickyFile*> filePile;
	friend class Block;
	std::list<Block*> buffer;
	std::map<WickyFile*, std::map<int, Block*>* > blockIndex;
	Block* getBlock(WickyFile* wf, int n);
	BufferManager();
	WickyFile* getFile(std::string name, int flag = WickyFile::FILE_REDIRECT);
	void writeDisk(WickyFile* wf, int offset, int len, unsigned char* buf);
	void readDisk(WickyFile* wf, int offset, int len, unsigned char* buf);
	void sweep();
	
	const static int MEM_LIMIT;
	
public:
	virtual ~BufferManager();
	//single instance mode making sure only one buffer exists among global
	static BufferManager* getInstance();
	bool isFileExists(std::string name);
	void redirect(std::string name, int offset=0);
	void removeFile(std::string name);
	
	int eof(std::string name);
	
	int readAll(std::string name, int offset, unsigned char* buf);
	
	//all parameters are pointer
	//and the data specified by wickypointer would be operated, onece
	int write(std::string name, int offset, int len, unsigned char* buf);
	int read(std::string name, int offset, int len, unsigned char* buf);
	//write and read sequentially, append to last operate
	int write(std::string name, int len, unsigned char* buf);
	int read(std::string name, int len, unsigned char* buf);	
	
	int write(std::string name, int offset, int n);
	int read(std::string name, int offset, int *n);
	
	int write(std::string name, int n);
	int read(std::string name, int *n);
	
	int write(std::string name, int offset, double n);
	int read(std::string name, int offset, double *n);
	
	int write(std::string name, double n);
	int read(std::string name, double *n);
	
	int write(std::string name, int offset, std::string str);
	int read(std::string name, int offset, std::string *str, int len);
	
	int write(std::string name, std::string str);
	int read(std::string name, std::string *str, int len);
	
	/*
	@n: input integer
	@bytes: destination pointer
	@return: bytes length
	*/
	static void intToBytes(int n, unsigned char* bytes);
	/*
	@n: input float
	@bytes: destination pointer
	@return: bytes length
	*/	
	static void doubleToBytes(double n, unsigned char* bytes);
	/*
	plase remember that the length would be plused one because of the last zero
	@str: input string
	@bytes: destination pointer
	@return: bytes length
	*/
	static void stringToBytes(std::string str, unsigned char* bytes);
	
	const static int READ_ALL;
};

#endif