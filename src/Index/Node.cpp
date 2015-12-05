#include "Node.h"

Node::Node(Index* index, int ptr): ptr(ptr){	
	this->index = index;	
	content = new unsigned char[Block::BLOCK_SIZE];
	BufferManager* bm = BufferManager::getInstance();	
	bm->read(index->getFileName(), ptr, Block::BLOCK_SIZE, content);	
	memcpy(&parent, content, 4);
	memcpy(&keyNum, content+4, 4);
	int b;
	memcpy(&b, content+8, 4);
	if (b) 
		this->inter = true;
	else
		this->inter = false;
}

Node::~Node(){	
	BufferManager* bm = BufferManager::getInstance();
	memcpy(content, &parent, 4);	
	memcpy(content+4, &keyNum, 4);
	int b;
	if (this->inter)
		b = 1;
	else 
		b = 0;
	memcpy(content+8, &b, 4);	
	bm->write(index->getFileName(), ptr, Block::BLOCK_SIZE, content);		
	delete[] content;
}

void Node::setInter(bool inter){
	this->inter = inter;
}

int Node::getKeyNum(){
	return keyNum;
}

int Node::getPointer(int i){
	if (i > index->getMaxKeyNum()){
		std::cout << i << " " << keyNum << std::endl;
		throw std::runtime_error("Node::getPointer(i) where i is out of range.");
	}
	int ret, offset;
	offset = 4 + 4 + 4 + (index->getKeyLen() + 4) * i;
	if (offset + 4 > Block::BLOCK_SIZE || offset + 4 <0)
		throw std::runtime_error("pointer is out of range");
	memcpy(&ret, content + offset, 4); //read ret
	return ret;	
}

void Node::setPointer(int i, int pointer){
	if (i > index->getMaxKeyNum())
		throw std::runtime_error("Node::setPointer(i, ptr) where i is out of range.");
	int offset;	
	offset = 4 + 4 + 4 + (index->getKeyLen() + 4) * i;
	if (offset + 4 > Block::BLOCK_SIZE || offset + 4 <0) {
		std::cout << offset + 4 << std::endl;
		throw std::runtime_error("Node::setPointer:pointer is out of range");
	}
	memcpy(content + offset, &pointer, 4);
}

Key Node::getKey(int i){
	if (i > keyNum) {
		std::cout << "i==" << i << " keyNum =" << keyNum << std::endl; 
		throw std::runtime_error("Node::getKey(i) where i is out of range.");
	}
	int len = index->getKeyLen();
	int offset;
	unsigned char* buf = new unsigned char[len];
	offset = 4 + 4 + 4 + 4 + (index->getKeyLen() + 4) * i;
	if (offset + len > Block::BLOCK_SIZE || offset + 4 <0)
		throw std::runtime_error("Node::getKey:pointer is out of range");
	memcpy(buf, content + offset, len);
	Key key(len, buf);
	delete[] buf;	
	return key;
}

void Node::setKey(int i, Key k){
	if (i > index->getMaxKeyNum())
		throw std::runtime_error("Node::setKey(i, key) where i is out of range.");
	int len = index->getKeyLen();
	int offset;		
	offset =  4 + 4 + 4 + 4 + (index->getKeyLen() + 4) * i;
	if (offset + len > Block::BLOCK_SIZE || offset + len <0)
		throw std::runtime_error("Node::setKey:pointer is out of range");
	memcpy(content + offset, k.getValue(), len);
}

int Node::findV(Key V){		
	int ret = -1;	
	for (int i = 0; i < keyNum; i++){
		Key K = getKey(i);		
		if (V <= K){			
			return i;
		}
	}
	return ret;
}

void Node::add(int p, Key k){		
	int len = index->getKeyLen();
	int offset = 4 + 4 + 4;
	unsigned char* buf = new unsigned char[len+4];	
	for (int i=keyNum-1; i>=0; i--){
		offset = 4 + 4 + 4 + (index->getKeyLen() + 4) * i;
		if (offset + len + 4 > Block::BLOCK_SIZE || offset + len + 4 <0)
			throw std::runtime_error("Node::add(p,k)1:pointer is out of range");
		memcpy(buf, content + offset, len + 4);
		Key key(len, buf + 4);
		if (key < k) {
			offset = offset + index->getKeyLen() + 4;
			break;
		}
		if (offset + index->getKeyLen() + 4 + len + 4 > Block::BLOCK_SIZE) {
			std::cout << "p::" << this->getAddr() << std::endl;
			std::cout << "offset:" << i << std::endl;
			k.print();
			throw std::runtime_error("Node::add(p,k)3:pointer is out of range");
		}
		memcpy(content + offset + index->getKeyLen() + 4, buf, len + 4);
	}
	memcpy(buf+4, k.getValue(), len);
	memcpy(buf, &p, 4);
	if (offset + len + 4 > Block::BLOCK_SIZE)
		throw std::runtime_error("Node::add(p,k)2:pointer is out of range");
	memcpy(content + offset, buf, len + 4);	
	keyNum++;
	delete[] buf;	
}

void Node::add(Key k, int p){
	int len = index->getKeyLen();
	int offset = 4 + 4 + 4;
	unsigned char* buf = new unsigned char[len+4];
	for (int i=keyNum-1; i>=0; i--){
		offset = 4 + 4 + 4 + 4 + (index->getKeyLen() + 4) * i;
		if (offset + len + 4 > Block::BLOCK_SIZE)
			throw std::runtime_error("Node::add(k,p)1:pointer is out of range");
		memcpy(buf , content + offset, len + 4);
		Key key(len, buf);
		if (key < k) {
			offset = offset + index->getKeyLen() +4;
			break;
		}
		memcpy(content + offset + index->getKeyLen() + 4, buf, len + 4);
	}
	memcpy(buf, k.getValue(), len);
	memcpy(buf+len, &p, 4);
	if (offset + len + 4 > Block::BLOCK_SIZE) 
		throw std::runtime_error("Node::add(k,p):pointer is out of range");
	memcpy(content + offset, buf, len + 4);
	keyNum++;		
	delete[] buf;	
}

void Node::deletePK(Key K, int P){	
	int len = index->getKeyLen();
	int offset, kx, px;
	len += 4;
	for (kx = 0; kx < keyNum; kx++)
		if (getKey(kx) == K) break;
	if (kx == keyNum) 
		throw std::runtime_error("Node::deletePK():no such key");
	if (getPointer(kx) == P){		
		px = kx;
	} else if (getPointer(kx+1) == P){		
		px = kx + 1;
	} else {
		std::cout << P << std::endl;		
		K.print();
		throw std::runtime_error("Node::deletePK():no such pointer");
	}
	int add1 = 4 + 4 + 4 + 4 + (index->getKeyLen() + 4) * kx;
	int add2 = 4 + 4 + 4 + (index->getKeyLen() + 4) * px;
	int startAddr;
	if (add1 > add2)
		startAddr = add2;
	else
		startAddr = add1;
	for (; startAddr <= keyNum * (index->getKeyLen() + 4); startAddr=startAddr + index->getKeyLen() + 4){		
		if (startAddr + (index->getKeyLen() + 4) * 2 > Block::BLOCK_SIZE) {
			throw std::runtime_error("Node::deletePK() stack corruption");
		}
		memcpy(content + startAddr, content + startAddr + index->getKeyLen() + 4, index->getKeyLen() + 4);		
	}		
	keyNum--;	
}

Node* Node::split(){
	if (index->debug && 0){
		std::cout << "----------split----------" << getParent() << " " << this->getAddr() << std::endl;
		this->print();
	}
	Node* L1 = index->newNode();
	L1->setInter(this->inter);
	// L1->setPointer(index->getMaxKeyNum(), this->getPointer(index->getMaxKeyNum()));
	// this->setPointer(index->getMaxKeyNum(), L1->getAddr());
	int i, j;
	for (i=(keyNum+1)/2, j=0; i<keyNum; i++, j++){
		L1->add(this->getPointer(i), this->getKey(i));
	}
	L1->setPointer(j, this->getPointer(i));
	L1->parent = this->parent;
	if (L1->isInternal()){
		for (i = 0; i <= L1->getKeyNum(); i++){
			index->getNode(L1->getPointer(i))->parent = L1->getAddr();		
		}
	}
	keyNum = (keyNum+1)/2;
	if (this->inter)
		keyNum --;
	if (index->debug && 0){
		std::cout << "keyNum==" << keyNum << std::endl;
		std::cout << "----------this-----------" << keyNum << " " << this->getAddr() << std::endl;
		this->print();
		std::cout << "----------L1-----------" << L1->keyNum << " " << L1->getAddr() << std::endl;
		L1->print();
	}
	 
	
	return L1;
}

int Node::getAddr(){
	return ptr;
}

bool Node::isInternal(){
	return inter;
}

void Node::insertInLeaf(Key key, int pointer){	
	add(pointer, key);
}

void Node::insertInParent(Key K1, Node* L1){
	if (this == index->getRoot()){		
		Node* R = index->newNode();
		this->parent = R->getAddr();
		L1->parent = R->getAddr();				
		R->add(this->getAddr(), K1);
		R->setPointer(1, L1->getAddr());
		R->setInter(true);
		index->setRoot(R);
		return;
	}
	Node* P =index->getNode(this->parent);
	if (index->debug && 0){
		std::cout << "-----------before------------" << P->getAddr()<< "--"<< P->parent << std::endl;
		P->print();	
		K1.print();
	}
	L1->parent = this->parent;
	if (index->debug && 0){
		std::cout << L1->getAddr() << std::endl;
	}		
	if (P->getKeyNum() < index->getMaxKeyNum() - 1){
		P->add(K1, L1->getAddr());
	} else {
		P->add(K1, L1->getAddr());
		Node* P1 = P->split();
		Key K2 = P->getKey(P->getKeyNum());
		P->insertInParent(K2, P1);
	}
	if (index->debug && 0){
		std::cout << "-----------after------------" << P->getAddr() << std::endl;
		P->print();
		// getch();
	}
}

void Node::deleteEntry(Key K, int P){
	if (index->debug){
		std::cout << "---------------deleteEntry---------" << std::endl;
	}
	deletePK(K, P);
	if (!index->d)
		return;	
	if (index->getRoot() == this){
		if (keyNum == 0){
			if (!isInternal()){
				index->setRoot(NULL);
				return;
			}
			index->setRoot(index->getNode(this->getPointer(0)));
			index->deleteNode(this);	
		}
		return;
	} else if (keyNum < index->getMaxKeyNum() / 2){
		Node* Pa = index->getNode(this->parent);
		Key K1 = K;		
		Node* N1;
		int pN = Pa->findV(this->getKey(0));
		if (index->debug){
			std::cout << "pN===" << pN << std::endl;
			K.print();	
		}		
		int pN1;
		
		if (pN == -1){
			pN = Pa->keyNum;
			pN1 = pN - 1;
			K1 = Pa->getKey(Pa->keyNum-1);
		} else {
			if (Pa->getKey(pN) == this->getKey(0)) pN ++;
			if (pN == 0){
				K1 = Pa->getKey(pN);
				pN1 = pN + 1;
			} else {
				K1 = Pa->getKey(pN-1);
				pN1 = pN-1;	
			}
		}
		if (index->debug){
			std::cout << keyNum << std::endl;
			std::cout << "pa"<< pN1 << std::endl;	
		}		
		N1 = index->getNode(Pa->getPointer(pN1));		
		if (index->debug){
			std::cout << "this:" << this->getAddr() << std::endl;
			std::cout << "N1:" << N1->getAddr() << std::endl;	
		}
				
		if (this->getKeyNum() + N1->getKeyNum() < index->getMaxKeyNum()){
			if (index->debug){
				std::cout << "------------K1---------" << std::endl;			
				K1.print();
			}
	
			if (pN1 < pN){
				if (index->debug){
					std::cout << this->getAddr() << std::endl;
					Pa->print();	
				}				
				N1->coalesce(this, K1);
				Pa->deleteEntry(K1, this->getAddr());
				index->deleteNode(this);
				if (index->debug){
					std::cout << "------------N1--------"<< std::endl;
					N1->print();	
				}				
			} else  {
				if (index->debug){
					std::cout << N1->getAddr() << std::endl;
					Pa->print();	
				}
				this->coalesce(N1, K1);
				Pa->deleteEntry(K1, N1->getAddr());
				index->deleteNode(N1);
				if (index->debug){
					std::cout << "------------this-------" << std::endl;
					this->print();	
				}
				
			}
			if (index->debug){
				std::cout << "----------Pa-----------" << std::endl;
				Pa->print();	
			}			
			// getch();
		} else {			
			if (pN1 < pN){
				N1->redistribute(this, K1);
			} else {
				N1->aredistribute(this, K1);
			}	
			if (index->debug){
				Pa->print();
			}							
		}
	}		
}

void Node::coalesce(Node* N, Key K1){	
	if (N->isInternal()){		
		this->add(K1, N->getPointer(0));
		for (int i=0; i<N->getKeyNum(); i++){
			this->add(N->getKey(i), N->getPointer(i+1));
		}
	} else {		
		for (int i=0; i<N->getKeyNum(); i++){
			this->add(N->getPointer(i), N->getKey(i));
		}
		// this->setPointer(index->getMaxKeyNum(), N->getPointer(index->getMaxKeyNum()));
	}
	
}

void Node::redistribute(Node* N, Key K1){
	if (index->debug){
		std::cout << "redistribute" << std::endl;
	
		K1.print();	
	}
	
	int m = this->getKeyNum();
	Node* Pa = index->getNode(N->parent);
	if (N->isInternal()){
		int N1Pm = this->getPointer(m);	
		Key N1Kmd = this->getKey(m-1);
		this->deletePK(N1Kmd, N1Pm);
		N->add(N1Pm, K1);
		int i = Pa->findV(K1);
		Pa->setKey(i, N1Kmd);
	} else {
		int N1Pm = this->getPointer(m-1);		
		Key N1Km = this->getKey(m-1);
		this->deletePK(N1Km, N1Pm);		
		N->add(N1Pm, N1Km);
		int i = Pa->findV(K1);
		Pa->setKey(i, N1Km);
	}
}

void Node::aredistribute(Node* N, Key K1){
	if (index->debug){
		std::cout << "aredistribute" << std::endl;	
	}
	
	int m = this->getKeyNum()-1;
	Node* Pa = index->getNode(N->parent);
	if (N->isInternal()){
		int N1Pm = this->getPointer(0);		
		Key N1Kmd = this->getKey(0);
		this->deletePK(N1Kmd, N1Pm);
		N->add(N1Pm, K1);
		int i = Pa->findV(K1);
		Pa->setKey(i, N1Kmd);
	} else {
		int N1Pm = this->getPointer(0);
		Key N1Km = this->getKey(0);		
		this->deletePK(N1Km, N1Pm);		
		N->add(N1Pm, N1Km);
		int i = Pa->findV(K1);
		Pa->setKey(i, N1Km);
	}
}

void Node::print(){	
	if (!index->debug) return;
	for (int i=0; i<this->getKeyNum(); i++){
		std::cout << i << " pointer:" << this->getPointer(i) << std::endl;
		std::cout << i << " key:";
		this->getKey(i).print();
	}
	if (inter) {
		std::cout << getKeyNum() << " pointer:" << this->getPointer(getKeyNum()) << std::endl;
	}
}

void Node::printRecursive(){
	if (!index->debug) return;
	std::cout << getAddr() << "-----------" << getParent() << std::endl;
	print();
	if (this->inter){
		for (int i=0; i<=this->getKeyNum(); i++){
			std::cout << i << std::endl;
			index->getNode(this->getPointer(i))->printRecursive();
		}	
	};
}

int Node::getParent(){
	return parent;
}