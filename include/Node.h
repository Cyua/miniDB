#ifndef _NODE_H
#define _NODE_H

#include <stdexcept>
// #include <conio.h>

#include "Index.h"
#include "Block.h"
#include "BufferManager.h"

class Index;
class Node {
private:
	int keyNum;
	int parent;
	const int ptr;
	Index* index;
	bool inter;
	unsigned char * content;
public:
	Node(Index *index, int ptr);
	~Node();
	void setInter(bool inter);
	int getKeyNum();
	int getPointer(int i);
	void setPointer(int i, int pointer);
	Key getKey(int i);	
	void setKey(int i, Key k);
	int getAddr();
	bool isInternal();
	int findV(Key k);
	void add(int p, Key k);
	void add(Key k, int p);
	void deletePK(Key K, int P);
	Node* split();
	void insertInLeaf(Key key, int pointer);
	void insertInParent(Key K1, Node* L1);
	void deleteEntry(Key K, int P);
	void coalesce(Node* N, Key K1);
	void redistribute(Node* N, Key K1);
	void aredistribute(Node* N, Key K1);
	void print();
	void printRecursive();
	int getParent();
};

#endif