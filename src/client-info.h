#ifndef _CLIENT_INFO_H
#define _CLIENT_INFO_H

#include <string>

class TClientInfo{
public:
	std::string m_name;
	int m_id;
	int m_sock;

	TClientInfo(){	};
	~TClientInfo(){	};

	void SetName(std::string);
	void SetId(int);
};

void TClientInfo::SetName(std::string _name){
	this->m_name = _name;
}

void TClientInfo::SetId(int _id){
	this->m_id = _id;
}


#endif