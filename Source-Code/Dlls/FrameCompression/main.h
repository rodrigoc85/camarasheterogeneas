#ifndef __MAIN_H__
#define __MAIN_H__

#include <windows.h>
#include <vector>
#include <zlib.h>
#include <iostream>

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

std::vector< unsigned char > DLL_EXPORT frame_compress(const std::vector< unsigned char > & src);

std::vector< unsigned char > DLL_EXPORT frame_uncompress(const std::vector< unsigned char > & src);

#ifdef __cplusplus
}
#endif

#endif // __MAIN_H__
