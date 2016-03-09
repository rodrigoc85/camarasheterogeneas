#ifndef __MAIN_H__
#define __MAIN_H__

#include <windows.h>

#include "jpge.h"
#include "jpgd.h"
#include "timer.h"
#include <ctype.h>
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

void DLL_EXPORT compress_img(unsigned char **, int, int, unsigned char **, int *, int);
void DLL_EXPORT decompress_img(unsigned char *, int, int*, int*, unsigned char **);
//void DLL_EXPORT compress_img_greyscale(unsigned char *, int, int*, int*, unsigned char **);
//void DLL_EXPORT decompress_img_greyscale(unsigned char *, int, int*, int*, unsigned char **);

#ifdef __cplusplus
}
#endif

#endif // __MAIN_H__
