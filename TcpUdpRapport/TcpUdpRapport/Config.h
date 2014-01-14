#ifndef CONFIG_H
#define CONFIG_H

#include <string>
using std::string;

struct Config
{
	Config();
	~Config();

	bool LoadFile(char* filename);
	bool CreateDefaultFile(char* filename);

	int server;
	int udp;
	unsigned short port;
	string ip;
};

#endif