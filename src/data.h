#ifndef _DATA_H_
#define _DATA_H_

#include <string>

class TData{
public:
	std::string m_name;
	int m_id;
	int m_sock;

	TData(){	};
	~TData(){	};

	void SetName(std::string);
	void SetId(int);
};

void TData::SetName(std::string _name){
	this->m_name = _name;
}

void TData::SetId(int _id){
	this->m_id = _id;
}


#endif