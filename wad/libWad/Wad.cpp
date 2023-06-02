#include "Wad.h"
#include <fstream>

//constructor
Descriptor::Descriptor(uint32_t eOffset,uint32_t length,string name,string filePath) {
this->offset = eOffset; //set the offset of this Descriptor object
this->length = length; //set the length of this Descriptor object
this->name = name; // set the name of this Descriptor object
this->path = filePath; // set the path of this Descriptor object
} 

// Deconstructor
Descriptor::~Descriptor(){}

// functions below gets the offset, lengt, name, and path of descriptor. 

uint32_t Descriptor::getOffset() {
    return this->offset;
    }

uint32_t Descriptor::getLength() {
    return this->length;
    }
string Descriptor::getName() {
    return this->name;
    }

string Descriptor::getPath() {
    return this->path;
    }

// Constructor for WAD class
Wad::Wad(uint8_t *pData) {

    // initialize variables
    magic = "";
    int i;
    
    // loops through first 4 bytes and add to the magic string. 
    for (i=0 ; i < 4;i++) {
        magic+=pData[i];
        
    }
    
    numD = 0; // initialize number of descriptors to 0
    
    // loops through the first 4 bytes and calcuates number of descriptors
    for (i=0 ; i < 4;i++) {
        numD+=(pData[i+4]<<(8*i));
    }
        
    dOffset = 0; // initialize descriptor offset to 0
    
    // loops through first 4 bytes and calculates descriptor offset
    for (i=0 ; i < 4;i++) {
        dOffset+=(pData[i+8]<<(8*i));
    }
    
    int count = dOffset; 

    std::string currFolder = ""; 
    int em = 0;

    // loop through all the descriptors
    for(int k = 0 ; k < numD ; k++) {
        uint32_t element = 0;
        
        // loop through the next 4 bytes and calculates element length
        for (i=0 ; i < 4;i++) {
            element+=(pData[i+count]<<(8*i));
        }
        
        uint32_t elementL = 0;

        // loop through the next 4 bytes and calculate element length
        for (i=0 ; i < 4;i++) {
            elementL+=(pData[i+count+4]<<(8*i));
        }

        char* eName = new char[9]; // new character array size of 9

        // loop through the next 8 bytes
        for(i=0;i<8;i++) {
            eName[i]=pData[(i+8+count)];
        }


        eName[8]= '\0'; //set the last character of the character array to null
        string sName(eName); // new string from the character array
        delete [] eName; // delete the character array

        // initialize strings start + end and file path to empty string
        string start= "_START";
        string end = "_END";
        string filePath = "";
        bool deleteEnd = true;

        if(elementL==0)  { // a directyory
        
            // if last 6 characters of the names are "_Start"
            if(sName.length() >= start.length() && 0 == sName.compare(sName.length() - start.length(),start.length(),start)) {
                currFolder += "/" + sName.substr(0,sName.length()-6);
            }

             //if the last 4 characters of the name are "_END"
            else if(sName.length() >= end.length() && 0 == sName.compare(sName.length() - end.length(),start.length(),end)) {
                currFolder =currFolder.substr(0,currFolder.length()-sName.length()+3);
                deleteEnd = false;
            }

            else {
                currFolder += "/" + sName; //append the name to the current folder string
                em = 10;
            }
            filePath = currFolder;
        }
        // //if the element length is not equal to 0 (file)
        else {
            filePath = currFolder + "/" + sName; //append the name to the current folder string and set the file path to the result

            // if em count is greater than 0
            if(em>0) {
                em--;
            // if em is equal to 0
            if(em == 0) {
                currFolder = currFolder.substr(0,currFolder.length()-5);
            }
        }
    }

    // if the delete end flag is true, create new descriptor and add descriptor to list
    if(deleteEnd) {
        Descriptor fileDes = Descriptor(element,elementL,sName,filePath);
        disList.push_back(fileDes);
    }

       // if the delete end flag is false
    else {
        deleteEnd = true; //set the delete end flag to true
    }

    count +=16; //increment the count variable by 16 (the size of each descriptor)
    this->pData = pData;//set the pData variable to the argument passed in to the constructor
    }
}

// load a WAD file from the given path
Wad* Wad::loadWad(const string &path) {
    ifstream wadFile(path,std::ios::binary | std::ios::ate); //create a new input file stream for the file at the given path in binary mode
    streamsize size = wadFile.tellg(); //initialize the size variable to the size of the file
    uint8_t *pData = new uint8_t[(unsigned int)size]; //create a new uint8_t array with size equal to the size of the file
    wadFile.seekg(0,std::ios::beg); //set the position of the file pointer to the beginning of the file
    wadFile.read((char*) pData,size); //read the entire file into the uint8_t array
    return new Wad(pData); //create a new Wad object from the uint8_t array and return it
}

// get the magic string
char* Wad::getMagic(){
    return (char *) magic.c_str();
}

bool Wad::isContent(const string &path) {

    //loop through all descriptors
    for(int i = 0 ; i< disList.size();i++) {

         //if the path matches the path of the current descriptor
        if(path==disList[i].getPath()){
             // if the length of the current descriptor is greaster than 0
            if(disList[i].getLength() >0) {
                return true;
            }

            return false;
        }
    }
    return false; //if no matching descriptor is found, return false
}

//check if the given path is a directory
bool Wad::isDirectory(const string &path) { 
    // if path is happy
    if(path.size()==0)
    return true;


    
    for(int i = 0 ; i < disList.size();i++) {

          if(path==disList[i].getPath()) {
            
            if(disList[i].getLength() ==0) {
                return true;
            }
            return false;
        }
    }
    return false; //if no matching descriptor is found, return false
}

//get the size of the file at the given path
int Wad::getSize(const string &path) {
    
    for (int i = 0 ; i < disList.size();i++) {

          
          if(path==disList[i].getPath()) {
            
            if(disList[i].getLength() >0) {
                return disList[i].getLength();
            }
            return -1;
        }
    }
    return -1;  //if no matching descriptor is found, return -1
}

//get the contents of the file at the given path
int Wad::getContents(const string &path, char *buffer, int length, int offset) {
    for(int i = 0 ; i < disList.size() ;  i++) {

        if(path == disList[i].getPath()) {

            if(disList[i].getLength()>0) {

                if(disList[i].getLength() < offset * length) {
                    length = disList[i].getLength() - offset;
                }

                for(int j = 0 ; j < length ; j++) {
                    buffer[j] = pData[disList[i].getOffset() +j+offset];
                }
                return length;
            }
            return -1;
        }
    }
  return -1;
}


//get the contents of the directory at the given path
int Wad::getDirectory(const string &path, vector<string> *directory) {

    //initialize
    string start= "_START";
    string end = "_END";
    
    // if the given path is a directory
    if(isDirectory(path.substr(0,path.length()-1))) { 

        // get path of current descriptor
        for(int i = 0 ; i < disList.size();i++) {
            string tempPath = disList[i].getPath();

            if(path.substr(0,path.length()-1)== tempPath.substr(0,path.length()-1)) {
                if(tempPath.size()>(path.length()-1) && tempPath.substr(path.length()).find("/") == std::string::npos) {
                 string fileName = disList[i].getName();

                        //if the last 6 characters of the name are "_START"
                    if(fileName.length() >= start.length() && 0 == fileName.compare(fileName.length()-start.length(),start.length(),start)) {
                        fileName = fileName.substr(0,fileName.length()-6);
                    }
                    //if the last 4 characters of the name are "_END"
                    else if(fileName.length() >= end.length() && 0 == fileName.compare(fileName.length()-end.length(),end.length(),end)) {
                        fileName = fileName.substr(0,fileName.length()-6);
                    }
                    string pushEntry = tempPath.substr(tempPath.length() - fileName.length());

                    //if the last 6 characters of the entry are "_START"
                    if(pushEntry.length() >= start.length() && 0 == pushEntry.compare ( pushEntry.length() - start.length(),start.length(),start)) {
                        pushEntry = pushEntry.substr(0,pushEntry.length()-6);
                    }

                    //if the last 4 characters of the entry are "_END"
                    else if(pushEntry.length() >= end.length() && 0 == pushEntry.compare ( pushEntry.length() - end.length(),end.length(),end)) {
                        pushEntry = pushEntry.substr(0,pushEntry.length()-4);
                    }
                    directory->push_back(pushEntry);
                }
            }
        }
        return directory->size(); 
    }
    return -1;
}

