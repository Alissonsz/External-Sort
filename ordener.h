#pragma once

#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <utility>
#include <vector>
#include <functional>

#pragma pack(push, 1)
struct Entry{
	char key[20];
	char value[50]; 
};
#pragma pack(pop)




class Ordener{
public:
	Ordener(const char* fileName);
	~Ordener();

private:
	std::string m_fileName;
	bool OpenInputFile();
	bool ReadInputFile();
	std::string ReadStringFromInput();
	bool Compare(std::pair<std::string, bool> a, std::pair<std::string, bool> b) { 
		return a.second == false && b.second == true || a.first < b.first;
	}
	

	std::fstream m_inFile;
	std::fstream m_outFile;
	std::priority_queue <std::string, std::vector<std::string>> m_heap;


	const int MAX_HEAP = 3;
	const int ENTRY_SIZE = sizeof(Entry);

};