#ifndef _BLOCK_H
#define _BLOCK_H

#include "WickyFile.h"
#include "BufferManager.h"

class Block {
public:
	Block(WickyFile* wf, int index);
	~Block();
	const static int BLOCK_SIZE;
	int getStart();
	int getIndex();
	WickyFile* getFile();	
private:
	friend class BufferManager;
	int start, index;	
	WickyFile* wf;
	unsigned char* mem;
	int write(int position, int len, unsigned char* buf);
	int read(int position, int len, unsigned char* buf);
};

#endif