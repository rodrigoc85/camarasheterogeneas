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

#ifdef __cplusplus
extern "C"
{
#endif

void DLL_EXPORT OcclusionCulling(int textureIndex,int TotalFaces, float* Faces_Triangles, int*** faces);

#ifdef __cplusplus
}
#endif

#endif // __MAIN_H__
