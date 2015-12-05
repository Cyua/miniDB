#ifndef _KEY_H
#define _KEY_H

#include <string>
#include <string.h>
#include <stdexcept>
#include "Schema.h"

class Key{
private:
	int len;
	unsigned char* key;
public:
	Key(int len, unsigned char * k);
	~Key();
	Key(const Key& k);
	static Key intToKey(int n);
	static Key doubleToKey(double d);
	static Key strToKey(std::string std);
	
	unsigned char* getValue();
	int getLength();
	Key& operator=(const Key& rhs);
	bool operator==(const Key& rhs) const;
	bool operator!=(const Key& rhs) const;
	bool operator<(const Key& rhs) const;
	bool operator>(const Key& rhs) const;
	bool operator<=(const Key& rhs) const;
	bool operator>=(const Key& rhs) const;
	
	void print();
};

#endif