#include "Key.h"

Key::Key(int len, unsigned char* k){
	this->len = len;
	key = new unsigned char[len];
	memcpy(key, k, len);
}

Key::~Key(){
	delete[] key;
}

Key::Key(const Key& k){
	this->len = k.len;
	if (this != &k) {
		this->key = new unsigned char[len];
		memcpy(this->key, k.key, this->len);
	}
}

Key Key::intToKey(int n){
	return Key(Schema::INT_LENGTH, (unsigned char*)&n);
}

Key Key::doubleToKey(double d){
	return Key(Schema::FLOAT_LENGTH, (unsigned char*)&d);
}

Key Key::strToKey(std::string str){
	return Key(str.length(), (unsigned char*)str.c_str());
}

unsigned char* Key::getValue(){
	return key;
}

int Key::getLength(){
	return len;
}

Key& Key::operator=(const Key& rhs){
	if (this!=&rhs){
		memcpy(this->key, rhs.key, this->len);
	}
	return *this;
}

bool Key::operator==(const Key& rhs) const{	
	for (int i=0; i<len; i++) {
		if (key[i]!=rhs.key[i]){
			return false;
		}		
	}
	return true;
}

bool Key::operator!=(const Key& rhs) const{
	return !(*this==rhs);
}

bool Key::operator<(const Key& rhs) const{
	for (int i=0; i<len; i++)
		if (key[i]>rhs.key[i])
			return false;
		else if (key[i]==rhs.key[i])
			continue;
		else
			return true;
	return false;
}

bool Key::operator>(const Key& rhs) const{
	return rhs<*this;
}

bool Key::operator<=(const Key& rhs) const{	
	return !(*this>rhs);
}

bool Key::operator>=(const Key& rhs) const{
	return !(*this<rhs);
}

void Key::print(){
	for (int i = 0; i < len; i++)
		std::cout << (int)key[i] << " ";
	std::cout << std::endl;
}