#ifndef _DB_H_
#define _DB_H_

#include <iostream>
#include <string>
#include <sqlite3.h>
#include "callback.h"

class TDb{
private:
	sqlite3* m_db;
	std::string m_name;
public:	
	TDb(std::string);
	TDb();
	~TDb();

	bool CreateTable(std::string);
	bool Insert(std::string);
	bool Select(std::string);
};

TDb::TDb(){

}

TDb::TDb(std::string _name){
	this->m_name = _name;
    
    int exit = 0; 
    exit = sqlite3_open(m_name.c_str(), &m_db);
  
    if(exit)
        std::cerr << "Error Create Database " << sqlite3_errmsg(m_db) << std::endl;
    else
        std::cout << "Created Database Successfully!" << std::endl;
}

bool TDb::CreateTable(std::string _sql){
	char* messaggeError; 
	int exit = sqlite3_exec(m_db, _sql.c_str(), NULL, 0, &messaggeError); 
  
    if (exit != SQLITE_OK) { 
        std::cerr << "Error Create Table" << std::endl; 
        sqlite3_free(messaggeError); 
        return false;
    } 
    else
        std::cout << "Table created Successfully" << std::endl; 

    return true;
}

bool TDb::Insert(std::string _sql){
	char* messaggeError; 
	int exit = sqlite3_exec(m_db, _sql.c_str(), NULL, 0, &messaggeError); 

    if (exit != SQLITE_OK) { 
        std::cerr << "Error Insert" << std::endl; 
        sqlite3_free(messaggeError); 
        return false;
    } 
    else
        std::cout << "Records created Successfully!" << std::endl; 

    return true;
}

bool TDb::Select(std::string _sql){
	std::string data("callback function");
	int rc = sqlite3_exec(m_db, _sql.c_str(), callback, (void*)data.c_str(), NULL);

	if (rc != SQLITE_OK){
		std::cerr << "Error SELECT\n";
		return false;
	}
	else {
		std::cout << "Operation OK!\n";
	}

	return true;
}

TDb::~TDb(){
	sqlite3_close(m_db);
}

#endif