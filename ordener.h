#pragma once

#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <utility>
#include <vector>

#pragma pack(push, 1)
struct Entry{
	char key[20];
	char value[50];

};
#pragma pack(pop)

struct Reg{
	std::string key;
	std::string value;
	bool marked;


	bool operator < (const Reg& ro) const {
		return (this->marked == true && ro.marked == false) || (this->marked == ro.marked && this->key > ro.key);
	}
};


class Ordener{
public:
	Ordener(const char* fileName);
	~Ordener();

private:
	std::string m_fileName;
	bool OpenInputFile();
	bool ReadInputFile();
	std::string ReadStringFromInput();
	int CreateEmptyFile();
	void CheckHeapAndWriteAtFile();
	void ResetMarkedElements();
	

	std::fstream m_inFile;
	std::fstream m_outFile;
	std::fstream m_currentRunFile;
	std::priority_queue <Reg> m_heap;
	std::vector<int> m_runsIds;
	int m_numberOfRuns = 0;
	int m_numberOfKeys = 0;
	bool m_someKeyAdded = false;
	std::string m_lastKeyAdded;


	const int MAX_HEAP = 3;
	const int ENTRY_SIZE = sizeof(Entry);

};