#pragma once

#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <utility>
#include <vector>

struct Reg{
	std::string key;
	std::string value;
	bool marked;
	int index;

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
	std::string ReadStringFromFile(std::fstream *fileToRead);
	int CreateEmptyFile();
	void CheckHeapAndWriteAtFile();
	void ResetMarkedElements();
	void MergeRuns();
	

	std::fstream m_inFile;
	std::fstream m_outFile;
	std::fstream m_currentRunFile;
	std::priority_queue <Reg> m_heap;
	std::vector<int> m_runsIds;
	int m_numberOfRuns = 0;
	int m_numberOfKeys = 0;
	bool m_someKeyAdded = false;
	std::string m_lastKeyAdded;
	std::vector<std::fstream> m_opennedFiles;


	const int MAX_HEAP = 3;
	const int REG_SIZE = sizeof(Reg);
	const int MAXNARQS = 4;

};