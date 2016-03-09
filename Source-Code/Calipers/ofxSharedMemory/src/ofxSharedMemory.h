#pragma once

#include "ofMain.h"
#include <stdio.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#include <tchar.h>
#else
#include <sys/shm.h>
#endif


/*
 ofxSharedMemory by Trent Brooks
 Openframeworks example of a memory mapped file for sharing data between multiple applications.
 - https://en.wikipedia.org/wiki/Memory-mapped_file

 Thanks to sloopi & KJ1 on the openframeworks forum for introducing me to 'memory mapped files'
 - http://forum.openframeworks.cc/index.php/topic,11730.msg55510.html#msg55510

 TODO::
 - need to allow reconnection for the client when server closes the shared memory without having to connect() in update/every frame
 - example for sharing non image data- eg. floats, ints, strings, etc.
 */


//template <typename T>
template<class T>
class ofxSharedMemory {

public:

    ofxSharedMemory();
    ~ofxSharedMemory();

    void setup(string memKey, int size, bool server=true);

	bool connect();
    void close();

    void setData(T sourceData);

    // data to share
    T getData();

protected:

    bool isServer; // only the server can 'close' the shared data
    string memoryKey;
    int memorySize;

    //unsigned char *sharedData;
    T sharedData;
	bool isReady;

	#ifdef _WIN32
	std::wstring tMemoryKey;
    HANDLE hMapFile;
	#endif
};


//--------------------------------------------------------------
/*
 Implementation
 Keeping implementation inside the .h to make the client side neater.
 Otherwise the testApp has to import ofxSharedMemory.cpp as well as ofxSharedMemory.h
 */
//--------------------------------------------------------------
template <typename T>
ofxSharedMemory<T>::ofxSharedMemory(){

    sharedData = NULL;
    isServer = false;
    memoryKey = "";
    memorySize = 0;
	isReady = false;
}

template <typename T>
ofxSharedMemory<T>::~ofxSharedMemory(){
    close();
}

template <typename T>
void ofxSharedMemory<T>::setup(string memoryKey, int memorySize, bool isServer) {

    this->memoryKey = memoryKey;
    this->memorySize = memorySize;
    this->isServer = isServer;

    #ifdef _WIN32
        tMemoryKey = std::wstring(memoryKey.begin(), memoryKey.end());
    #endif
}

template <typename T>
void ofxSharedMemory<T>::close() {

    if(isServer) {
        #ifdef _WIN32
            UnmapViewOfFile(sharedData);
            CloseHandle(hMapFile);
        #else
            munmap(sharedData, memorySize);
            shm_unlink(memoryKey.c_str());
        #endif
    }

}

template <typename T>
bool ofxSharedMemory<T>::connect() {
    #ifdef _WIN32

        if(isServer) {
            // server must use CreateFileMapping
            hMapFile = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, memorySize, tMemoryKey.c_str());
        } else {
            hMapFile = OpenFileMappingW(FILE_MAP_ALL_ACCESS, false, tMemoryKey.c_str());
        }
        if(hMapFile == NULL) {
            isReady = false;
            return false;
        }

        sharedData = (T) MapViewOfFile(hMapFile,FILE_MAP_ALL_ACCESS, 0, 0, memorySize);
        if(sharedData == NULL) {
            CloseHandle(hMapFile);
            isReady = false;
            return false;
        }
    #else

        // create/connect to shared memory from dummy file
        int descriptor = shm_open(memoryKey.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
        if(descriptor == -1) {
            isReady = false;
            return false;
        }

        // map to memory
        ftruncate(descriptor, memorySize);
        sharedData = (T) mmap(NULL, memorySize, PROT_WRITE | PROT_READ, MAP_SHARED, descriptor, 0);
        if(sharedData == NULL) {
            if(isServer) shm_unlink(memoryKey.c_str());
            isReady = false;
            return false;
        }

    #endif

        isReady = true;
        return true;
}

// copies source data to shared data
template <typename T>
void ofxSharedMemory<T>::setData(T sourceData) {
    memcpy(sharedData, sourceData, memorySize);
}

// returns shared data
template <typename T>
T ofxSharedMemory<T>::getData() {
    return sharedData;
}
