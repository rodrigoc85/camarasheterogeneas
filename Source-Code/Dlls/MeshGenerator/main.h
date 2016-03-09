#ifndef __MAIN_H__
#define __MAIN_H__

#include <windows.h>
#include "masterPly.h"

/*  To use this exported function of dll, include this header
 *  in your project.
 */

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif


#ifdef __cplusplus
extern "C"
{
#endif

struct PointsCloud{
    float* x;
    float* y;
    float* z;
    int length;
};

void DLL_EXPORT meshGenerate(PointsCloud* nbIN, FaceStruct** faces, int* numberFaces, int nroFrame);
void DLL_EXPORT meshGenerateToCaliper(PointsCloud* nbIN, FaceStruct** faces, int* numberFaces);
#ifdef __cplusplus
}
#endif

#endif // __MAIN_H__
