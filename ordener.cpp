#include "ordener.h"


Ordener::Ordener(const char* fileName) : m_fileName (fileName) {
	if(OpenInputFile()){

		ReadInputFile();

		while(!m_heap.empty()){
			std::cout<<m_heap.top().key<<" "<<m_heap.top().value<<std::endl;
			m_heap.pop();
		}	

	} else {
		std::cout<<"Unable to open input file"<<std::endl;
	}

	
}


Ordener::~Ordener(){ }

bool Ordener::ReadInputFile(){
	Reg aux;
	int i=0;
	while(true){
		std::string readedKey = ReadStringFromInput();
		std::string readedValue = ReadStringFromInput();

		aux.key = readedKey;
		aux.value = readedValue;
		aux.marked = false;

		if(readedKey == "FINISHED") break;

		if(m_someKeyAdded && aux.key < m_lastKeyAdded) aux.marked = true;

		m_heap.push(aux);
		m_numberOfKeys++;
		
		if(m_heap.size() > MAX_HEAP){
			CheckHeapAndWriteAtFile();
		}
	}

	while(!m_heap.empty()) CheckHeapAndWriteAtFile();


}


bool Ordener::OpenInputFile(){
	m_inFile.open(m_fileName.c_str(), std::fstream::in);

	return m_inFile.is_open();
}

std::string Ordener::ReadStringFromInput(){
	bool somethingToRead = false;
	char currentChar;
	std::string readedString;
	

	while(m_inFile.read((char*)&currentChar, sizeof(char))){
		somethingToRead = true;
		if(currentChar == ',' || currentChar == '\n') break;
		
		readedString += currentChar;
		
	}
	if(somethingToRead == false) return std::string("FINISHED");
	
	
	return readedString;
}

void Ordener::CheckHeapAndWriteAtFile(){
	if(m_currentRunFile.is_open() == false){
		int fileToOpenId = CreateEmptyFile();

		m_currentRunFile.open(std::string("run" + std::to_string(fileToOpenId) + ".dat").c_str(), std::fstream::in | std::fstream::out);
	} else if(m_heap.top().marked == true) {
		m_currentRunFile.close();
		ResetMarkedElements();

		int fileToOpenId = CreateEmptyFile();
		m_currentRunFile.open(std::string("run" + std::to_string(fileToOpenId) + ".dat").c_str(), std::fstream::in | std::fstream::out);
	}

	Reg regToWrite = m_heap.top();
	m_currentRunFile<<regToWrite.key<<","<<regToWrite.value+"\n";
	m_lastKeyAdded = regToWrite.key;
	m_someKeyAdded = true;

	m_heap.pop();
}

void Ordener::ResetMarkedElements(){
	std::vector<Reg> auxVector;

	while(!m_heap.empty()){
		Reg topReg = m_heap.top();
		topReg.marked = false;
		auxVector.push_back(topReg);
		m_heap.pop();
	}

	for(int i = 0; i < auxVector.size(); i++) m_heap.push(auxVector[i]);
}

int Ordener::CreateEmptyFile(){
	std::fstream newRunFile;
	
	int runId = 0;
	if(m_runsIds.size() != 0) runId = m_runsIds[m_runsIds.size() - 1] + 1;
	newRunFile.open(std::string("run" + std::to_string(runId) + ".dat").c_str(), std::fstream::out);
	m_runsIds.push_back(runId);
	newRunFile.close();

	return runId;
}