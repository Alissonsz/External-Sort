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
	
	//loop to read input file
	while(true){
		std::string readedKey = ReadStringFromInput();
		std::string readedValue = ReadStringFromInput();

		aux.key = readedKey;
		aux.value = readedValue;
		aux.marked = false;

		//ReadStringFromInput() return a string "FINISHED" if there's no more entries
		if(readedKey == "FINISHED") break;

		//marked says if the new readed entry is lower than the last entrie added on heap
		if(m_someKeyAdded && aux.key < m_lastKeyAdded) aux.marked = true;

		m_heap.push(aux);
		m_numberOfKeys++;
		



		//if we can't add a new entry on heap, write the entrie there's on the top of the heap at the current run
		if(m_heap.size() * REG_SIZE + REG_SIZE > MAXMEN){
			CheckHeapAndWriteAtFile();
		}
	}

	while(!m_heap.empty()) CheckHeapAndWriteAtFile();
	m_currentRunFile.close();

	//intercalate runs
	MergeRuns();


}

void Ordener::CheckHeapAndWriteAtFile(){
	//if there's no open run or all the elements int the current run are markeds, create a new run and unmark all elements
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


	//write the top element
	Reg regToWrite = m_heap.top();
	m_currentRunFile<<regToWrite.key<<","<<regToWrite.value+"\n";
	m_lastKeyAdded = regToWrite.key;
	m_someKeyAdded = true;

	m_heap.pop();
}

void Ordener::ResetMarkedElements(){
	std::vector<Reg> auxVector;

	//to do --------- find a better way to do that
	while(!m_heap.empty()){
		Reg topReg = m_heap.top();
		topReg.marked = false;
		auxVector.push_back(topReg);
		m_heap.pop();
	}

	for(int i = 0; i < auxVector.size(); i++) m_heap.push(auxVector[i]);
}

void Ordener::MergeRuns(){
	int numMergedRuns = 0;

	//while we have at least 2 runs to intercalate
	while(m_runsIds.size() > 1){
		//create a new file to save the merge's result
		int numFile = CreateEmptyFile();
		
		
		m_currentRunFile.open(std::string("run" + std::to_string(numFile) + ".dat").c_str(), std::fstream::in | std::fstream::out);
		

		int k = 0;
		//open MAXNARQS-1 runss (-1 because we must have one file to write the merge result)
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

		//read one entry from each run (we assume that we can hold at least MAXNARQS-1 entries at memory)
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
		
		//write the top's entry and read a new entry from the run where the entry we write were
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
		//delete processed runs
		for(int i=0; i<k; i++){
			std::string fileToremovename = std::string("run" + std::to_string(m_runsIds[i]) + ".dat");
			remove(fileToremovename.c_str());
		
		}

		//att run ids vector
		m_runsIds.erase(m_runsIds.begin(), m_runsIds.begin() + k);
		

		
		m_currentRunFile.close();
				

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



int Ordener::CreateEmptyFile(){
	std::fstream newRunFile;
	
	int runId = 0;
	if(m_runsIds.size() != 0) runId = m_runsIds[m_runsIds.size() - 1] + 1;
	newRunFile.open(std::string("run" + std::to_string(runId) + ".dat").c_str(), std::fstream::out);
	//m_runsIds.push_back(runId);
	newRunFile.close();

	return runId;
}