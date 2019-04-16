#include <iostream>
#include "db.h"

int main(int argc, char const *argv[]){
	std::string name = "db/first-database.db";
	TDatabase *db = new TDatabase(name);

	std::string sql = "CREATE TABLE PERSON("
                          "ID       INT PRIMARY KEY     NOT NULL, "
                          "NAME     TEXT                NOT NULL, "
                          "SURNAME  TEXT                NOT NULL, "
                          "AGE      INT                 NOT NULL, "
                          "ADDRESS  CHAR(50), "
                          "SALARY   REAL "
                      ");";

	// db->CreateTable(sql);

	sql = std::string("INSERT INTO PERSON VALUES(1, 'STEVE', 'GATES', 30, 'PALO ALTO', 1000.0);"
					  "INSERT INTO PERSON VALUES(2, 'BILL', 'ALLEN', 20, 'SEATTLE', 300.22);"
					  "INSERT INTO PERSON VALUES(3, 'PAUL', 'JOBS', 24, 'SEATTLE', 9900.0);");

	db->Insert(sql);

	sql = "select * from PERSON;";
	db->Select(sql);

	delete db;
	return 0;
}