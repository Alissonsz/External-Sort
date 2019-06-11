#include "ordener.h"


Ordener::Ordener(const char* fileName) : m_fileName (fileName) {
	if(OpenInputFile()){

		ReadInputFile();

		while(!m_heap.empty()){
			std::cout<<m_heap.top()<<std::endl;
			m_heap.pop();
		}	

	} else {
		std::cout<<"Unable to open input file"<<std::endl;
	}

	
}


Ordener::~Ordener(){ }

bool Ordener::ReadInputFile(){
	Entry entry;
	int i=0;
	while(true){
		std::string readedKey = ReadStringFromInput();
		std::string readedValue = ReadStringFromInput();

		if(readedKey == "FINISHED") break;
		

		m_heap.push(readedKey);
		

		//std::cout<<readedFromIn<<std::endl;
	}


}


bool Ordener::OpenInputFile(){
	m_inFile.open(m_fileName.c_str(), std::fstream::in);

	return m_inFile.is_open();
}

std::string Ordener::ReadStringFromInput(){
	//if(m_inFile.peek() == EOF) return std::string("EOF");

	bool somethingToRead = false;
	char currentChar;
	std::string readedString;
	

	while(m_inFile.read((char*)&currentChar, sizeof(char))){
		somethingToRead = true;
		if(currentChar == ',' || currentChar == '\n') break;
		
		readedString += currentChar;
		
	}
	if(somethingToRead == false) return std::string("FINISHED");
	//if(currentChar == ',') return std::string("ENDOFKEY");
	
	return readedString;
}