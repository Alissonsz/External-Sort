#include "ordener.h"


Ordener::Ordener(const char* fileName) : m_fileName (fileName) {
	if(OpenInputFile()){
		remove("saida.dat");
		ReadInputFile();
		
		   		   
		
		rename(std::string("run" + std::to_string(m_runsIds[0]) + ".dat").c_str(), "saida.dat");
		

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
		
		PROCESS_MEMORY_COUNTERS memCounter;
		BOOL result = GetProcessMemoryInfo(GetCurrentProcess(), &memCounter, sizeof(memCounter));

		if(memCounter.WorkingSetSize + REG_SIZE > MAXMEN){
			CheckHeapAndWriteAtFile();
		}
	}

	while(!m_heap.empty()) CheckHeapAndWriteAtFile();
	m_currentRunFile.close();

	MergeRuns();


}

void Ordener::MergeRuns(){
	int numMergedRuns = 0;
	int runsToMerge = m_runsIds.size();

	

	while(m_runsIds.size() > 1){
		
		int numFile = CreateEmptyFile();
		
		
		m_currentRunFile.open(std::string("run" + std::to_string(numFile) + ".dat").c_str(), std::fstream::in | std::fstream::out);
		

		int k = 0;
		for(int i = 0; i <  MAXNARQS-1 && i < m_runsIds.size(); i++, numMergedRuns++, k++){
			
			m_opennedFiles.push_back(std::fstream());
			m_opennedFiles[m_opennedFiles.size() - 1].open(std::string("run" + std::to_string(m_runsIds[i]) + ".dat").c_str());


			
		}
		
		

		int numOpennedFiles = m_opennedFiles.size();
		Reg reading[numOpennedFiles];

		for(int i=0; i<numOpennedFiles; i++){
			reading[i].marked = false;
			reading[i].index = -1;
		}

		
		for(int i=0; i<numOpennedFiles; i++){
			Reg auxReg;

			std::string readedKey = ReadStringFromFile(&m_opennedFiles[i]);
			if(readedKey == "FINISHED") continue;
			std::string readedValue = ReadStringFromFile(&m_opennedFiles[i]);

			auxReg.key = readedKey;
			auxReg.value = readedValue;
			auxReg.index = i;

			m_heap.push(auxReg);

		}
		
		while(!m_heap.empty()){
			Reg topReg = m_heap.top();
			m_heap.pop();
			
			m_currentRunFile << topReg.key <<"," << topReg.value <<"\n";

			int indexToRead = topReg.index;

			Reg auxReg;

			std::string readedKey = ReadStringFromFile(&m_opennedFiles[indexToRead]);
			if(readedKey == "FINISHED") continue;
			std::string readedValue = ReadStringFromFile(&m_opennedFiles[indexToRead]);
		

			auxReg.key = readedKey;
			auxReg.value = readedValue;
			auxReg.index = indexToRead;

			m_heap.push(auxReg);
		}
		m_runsIds.push_back(numFile);
		m_opennedFiles.clear();
		for(int i=0; i<k; i++){
			std::string fileToremovename = std::string("run" + std::to_string(m_runsIds[i]) + ".dat");
			remove(fileToremovename.c_str());
			//std::cout<<fileToremovename<<std::endl;
		}

		m_runsIds.erase(m_runsIds.begin(), m_runsIds.begin() + k);
		

		
		m_currentRunFile.close();
		
		
		
	//break;		

	}
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

std::string Ordener::ReadStringFromFile(std::fstream *fileToRead){
	bool somethingToRead = false;
	char currentChar;
	std::string readedString;
	

	while(fileToRead->read((char*)&currentChar, sizeof(char))){
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
		m_runsIds.push_back(fileToOpenId);
		m_currentRunFile.open(std::string("run" + std::to_string(fileToOpenId) + ".dat").c_str(), std::fstream::in | std::fstream::out);
	} else if(m_heap.top().marked == true) {
		m_currentRunFile.close();
		ResetMarkedElements();

		int fileToOpenId = CreateEmptyFile();
		m_runsIds.push_back(fileToOpenId);
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
	//m_runsIds.push_back(runId);
	newRunFile.close();

	return runId;
}