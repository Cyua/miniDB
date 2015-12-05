#include "Table.h"
#include "RecordManager.h"
#include <string>
#include <iostream>
using namespace std;
int main(void)
{
	std::vector<attribute> v;
	v.push_back(attribute(1,"name"));
	v.push_back(attribute(0,"score"));
	Table t("student");
	if(!t.CreateTable(v))
		cout<<"create failed"<<endl;

	RecordManager rm;

	Tuple tup1,tup2,tup3;
	tup1.col.push_back("jack");
	tup1.col.push_back("100");
	tup2.col.push_back("mike");
	tup2.col.push_back("99");

	rm.insertTuple(&t,tup1,0);
	rm.insertTuple(&t,tup2,1);

	int size=t.rows.size();
	for(int i=0; i<size; i++){
		int tsize=t.rows[i].col.size();
		for(int j=0; j<tsize; j++){
			cout<<t.rows[i].col[j]<<endl;
		}
	}
	// cout<<"-------------------"<<endl;
	// rm.deleteTuple(&t,0);
	// size=t.rows.size();
	// for(int i=0; i<size; i++){
	// 	int tsize=t.rows[i].col.size();
	// 	for(int j=0; j<tsize; j++){
	// 		cout<<t.rows[i].col[j]<<endl;
	// 	}
	// }
	cout<<"-------------------"<<endl;
	std::vector<int> offset;
	offset.push_back(0);
	offset.push_back(1);
	std::vector<Tuple> result = rm.selectTuple(&t,offset);
	for(int i=0; i<result.size(); i++){
		int tsize=t.rows[i].col.size();
		for(int j=0; j<tsize; j++){
			cout<<t.rows[i].col[j]<<endl;
		}
	}

	return 0;
}