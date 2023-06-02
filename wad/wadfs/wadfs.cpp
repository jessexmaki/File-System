#ifndef FUSE_USE_VERSION
#define FUSE_USE_VERSION 26
#endif

#include <iostream> 
#include <fuse.h>
#include <string.h>
#include <errno.h>
#include "../libWad/Wad.h"

using namespace std;

Wad *myWad; 

// Get attribute callback function
static int getattr_callback(const char *path, struct stat *stbuf) {
    memset(stbuf, 0 ,sizeof(struct stat)); // set memory area
    string sPath(path); // convert path to string

     // Remove trailing slash from path
    if(sPath.length() >=1 && sPath.substr(sPath.length()-1 )=="/") {
        sPath = sPath.substr(0,sPath.length()-1);
    }

    // Check if path is a directory
    if(myWad->isDirectory(sPath)) {
        stbuf->st_mode = S_IFDIR | 0555; // sets permission
        stbuf->st_nlink = 2; // set hard links
        return 0;
    }

     // Check if path is a file
    if(myWad->isContent(sPath)) {
        stbuf->st_mode = S_IFREG | 0444; // set permissions
        stbuf->st_nlink = 1;  // set hard links
        stbuf->st_size = myWad->getSize(sPath); // set file size
        return 0;
    }
    return -ENOENT;
}

// Read directory callback function
static int readdir_callback(const char *path,void *buf,fuse_fill_dir_t filler,off_t offset,struct fuse_file_info *fi) {

    (void) offset; 
    (void) fi;
    filler(buf,".",NULL,0); 
    filler(buf,"..",NULL,0);
    vector<string> entries;
    string SPath(path);

        // Add trailing slash to path
    if(SPath.length() >=1 && SPath.substr(SPath.length()-1)!="/") {
        SPath = SPath + "/";
    }
    
    if(SPath.size()==0) {
        SPath = "/";
    }

    myWad->getDirectory(SPath,&entries);

    // Add directory entries to buffer
    for(string entry:entries) {
        filler(buf,entry.c_str(),NULL,0);
    }
    return 0;
}

// Open file callback function
static int open_callback(const char* path, struct fuse_file_info *fi) {
    return 0;
} 

// Open directory callback function
static int opendir_callback(const char* path, struct fuse_file_info *fi) {
    return 0;
}

// Release file callback function
static int release_callback(const char* path, struct fuse_file_info *fi) {
    return 0;
}
// Release directory callback function
static int releasedir_callback(const char* path, struct fuse_file_info *fi) {
    return 0;
}

// Read file callback function
static int read_callback(const char* path,char *buf,size_t size,off_t offset, struct fuse_file_info *fi) {
    
    if(myWad->isContent(path)) {
        return myWad->getContents(path,buf,size,offset);
    }

    return -ENOENT;
}

static struct fuse_operations myFuse;


int main(int argc, char* argv[]) { 
    // Set fuse operations
    myFuse.getattr = getattr_callback;
    myFuse.open = open_callback;
    myFuse.read = read_callback;
    myFuse.readdir = readdir_callback;
    myFuse.opendir = opendir_callback;
    myFuse.release = release_callback;
    myFuse.releasedir = releasedir_callback;

    //load WAD file
    myWad = Wad::loadWad(argv[1]);

    // Rearrange argv
    argv[1] = argv[2];

    argv[2] = NULL;
    // Start fuse
    return fuse_main(--argc,argv,&myFuse,NULL);
}



