#include "RecordManager.h"

bool RecordManager::insertTuple(Table* table, Tuple tuple, int offset){
	int rawSize = table->rows.size();
	if(offset>rawSize){
		std::cout<<"Error: the index is wrong! Insertion failed!"<<std::endl;
		return false;
	}
	if(rawSize>0){
		table->rows.push_back(table->rows[rawSize-1]);	
		for(int i=rawSize-1; i>offset; i--){
			table->rows[i]=table->rows[i-1];
		}
		table->rows[offset]=tuple;
	}
	else{
		table->rows.push_back(tuple);
	}
	return true;
}

bool RecordManager::deleteTuple(Table* table, int offset){
	int rawSize = table->rows.size();
	if(offset>=rawSize){
		std::cout<<"Error: the index is wrong! Deletion failed!"<<std::endl;
		return false;
	}
	if(offset==rawSize-1){				//delete the last tuple
		table->rows.pop_back();
	}
	else{
		for(int i=offset; i<rawSize-1; i++){
			table->rows[i]=table->rows[i+1];
		}
		table->rows.pop_back();
	}
	return true;
}

std::vector<Tuple> RecordManager::selectTuple(Table* table, std::vector<int> offset){
	int rawSize = table->rows.size();
	std::vector<Tuple> result;
	for(int i=0; i<offset.size();i++){
		if(offset[i]<rawSize)
			result.push_back(table->rows[offset[i]]);
	}
	return result;
} 