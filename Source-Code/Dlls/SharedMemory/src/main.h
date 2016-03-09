#ifndef __MAIN_H__
#define __MAIN_H__

#include <windows.h>
#include "ofxSharedMemory.h"

/*  To use this exported function of dll, include this header
 *  in your project.
 */

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif


struct FaceStruct
{
    float p1[3];
    float p2[3];
    float p3[3];
};

struct MemoryImageStruct
{
    ofxSharedMemory<int*> memoryMappedImageId;
    ofxSharedMemory<int*> memoryMappedImageSizeH;
    ofxSharedMemory<int*> memoryMappedImageSizeW;
    ofxSharedMemory<unsigned char*> memoryMappedImagePixels;
    std::stringstream ImageIdKey;
    std::stringstream ImageSizeHKey;
    std::stringstream ImageSizeWKey;
    std::stringstream ImagePixelsKey;
    bool firstTimeImage;
};

struct MemoryMeshStruct
{
    ofxSharedMemory<int*> memoryMappedMeshId;
    ofxSharedMemory<int*> memoryMappedMeshSize;
    ofxSharedMemory<FaceStruct*> memoryMappedMesh;
    std::stringstream MeshIdKey;
    std::stringstream MeshSizeKey;
    std::stringstream MeshKey;
    bool firstTimeMesh;
    int maxNumberFaces;

};

#ifdef __cplusplus
extern "C"
{
#endif

//MESH
void DLL_EXPORT ShareMeshSetup(int* meshId, int* index);

void DLL_EXPORT ReadSharedMeshSetup(int* meshId, int* index);

void DLL_EXPORT ShareMesh( int* meshId, int* numberFaces, FaceStruct* faces, int* index);

void DLL_EXPORT ReadSharedMesh(int* meshId, int* numberFaces, FaceStruct** faces, int* index);

//IMAGE
void DLL_EXPORT ShareImageSetup(int* imageId, int* wPixels, int* hPixels, int* index);

void DLL_EXPORT ReadSharedImageSetup(int* imageId, int* index);

void DLL_EXPORT ShareImage(int* imageId, unsigned char* pixels, int* index);

void DLL_EXPORT ReadSharedImage(int* Id, int* wPixels, int* hPixels , unsigned char** pixels, int* index) ;

#ifdef __cplusplus
}
#endif

#endif // __MAIN_H__
