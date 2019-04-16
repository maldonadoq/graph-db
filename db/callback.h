#ifndef _CALLBACK_H_
#define _CALLBACK_H_

#include <iostream>
#include <string>

static int callback_header(int argc, char** azColName){
	int i;
	for (i = 0; i < argc; i++) {
		printf("%s\t", azColName[i]);
	}
	printf("\n");
	return 0;
}

static int callback_body(int argc, char** argv){
	int i;
	for (i = 0; i < argc; i++) {
		printf("%s\t", argv[i] ? argv[i] : "NULL");
	}

	printf("\n");
	return 0;
}

static int callback(void* data, int argc, char** argv, char** azColName)
{
	int i;
	fprintf(stderr, "%s: ", (const char*)data);

	for (i = 0; i < argc; i++) {
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}

	printf("\n");
	return 0;
}

#endif