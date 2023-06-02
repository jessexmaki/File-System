#ifndef WAD_h
#define WAD_h

#include <iostream>
#include <vector>
using namespace std;


class Descriptor{
public:
Descriptor();
Descriptor(uint32_t eOffset,uint32_t length,string name,string filePath);
uint32_t offset;
uint32_t length;
string name;
string path;

uint32_t getOffset();
uint32_t getLength();
string getName();
string getPath();

~Descriptor();

};

class Wad{

public :
string magic;
int numD;
int dOffset;
vector<Descriptor> disList;
uint8_t *pData;

static Wad* loadWad(const string &path);
char* getMagic();
bool isContent(const string &path);
bool isDirectory(const string &path);
int getSize(const string &path);
int getContents(const string &path, char *buffer, int length, int offset = 0);
int getDirectory(const string &path, vector<string> *directory);

void testPath(const string &path){}

friend std::ostream & operator <<( std::ostream &os, const string & c )
   {
      return ( os );
   }
Wad(uint8_t *pData);

};


#endif
