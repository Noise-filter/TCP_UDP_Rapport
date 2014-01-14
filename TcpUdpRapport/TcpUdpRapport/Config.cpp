#include "Config.h"
#include <fstream>
#include <iostream>
using std::ofstream;
using std::ifstream;
using std::cout;
using std::endl;

Config::Config()
{
	server = -1;
	udp = -1;
	port = -1;
}

Config::~Config()
{
}

bool Config::LoadFile(char* filename)
{
	ifstream inFile(filename);
	if(!inFile)
	{
		return CreateDefaultFile(filename);
	}
	string temp;
	char temp2;

	inFile >> temp >> temp2 >> server;
	inFile >> temp >> temp2 >> udp;
	inFile >> temp >> temp2 >> port;
	inFile >> temp >> temp2 >> ip;

	inFile.close();

	return true;
}

bool Config::CreateDefaultFile(char* filename)
{
	ofstream outFile(filename);
	if(!outFile)
		return false;

	server = 0;
	udp = 1;
	port = 9876;
	ip = "localhost";
	outFile << "Server\t: " << server << endl;
	outFile << "UDP\t: " << udp << endl;
	outFile << "Port\t: " << port << endl;
	outFile << "IP\t: " << ip;

	outFile.close();
	return true;
}