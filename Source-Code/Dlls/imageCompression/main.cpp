#include "main.h"

#include "jpge.h"
#include "jpgd.h"
#include "timer.h"
#include <ctype.h>

void DLL_EXPORT compress_img(unsigned char ** srcBuff, int width, int height, unsigned char ** destBuff, int * comp_size, int qfactor) {
    int quality_factor              = qfactor;
    int subsampling                 = -1;
    bool optimize_huffman_tables    = true;
    int actual_comps                = 3;
    const int req_comps             = 3; // request RGB image

    jpge::params params;
    params.m_quality        = quality_factor;
    params.m_subsampling    = (subsampling < 0) ? ((actual_comps == 1) ? jpge::Y_ONLY : jpge::H2V2) : static_cast<jpge::subsampling_t>(subsampling);
    params.m_two_pass_flag  = optimize_huffman_tables;

    int orig_buf_size       = width * height * 3;
    *comp_size              = orig_buf_size;
    *destBuff               = (unsigned char *) malloc(orig_buf_size); //Remember to free this memory.

    if (!jpge::compress_image_to_jpeg_file_in_memory(*destBuff, *comp_size, width, height, req_comps, *srcBuff, params)) {
        free(*destBuff);
        *destBuff   = NULL;
        *comp_size  = -1;
    }
}

void DLL_EXPORT decompress_img(unsigned char * srcBuff, int comp_size, int * width, int * height, unsigned char ** destBuff) {
    int uncomp_actual_comps;
    int uncomp_req_comps = 3;
    *destBuff = jpgd::decompress_jpeg_image_from_memory(srcBuff, comp_size, width, height, &uncomp_actual_comps, uncomp_req_comps);
}
/*
void DLL_EXPORT compress_img_greyscale(unsigned char ** srcBuff, int width, int height, unsigned char ** destBuff, int * comp_size) {
    int quality_factor              = 90;
    bool optimize_huffman_tables    = true;
    const int req_comps             = 1; // request RGB image

    jpge::params params;
    params.m_quality        = quality_factor;
    params.m_subsampling    = jpge::Y_ONLY;
    params.m_two_pass_flag  = optimize_huffman_tables;

    int orig_buf_size       = width * height;
    *comp_size              = orig_buf_size;
    *destBuff               = (unsigned char *) malloc (orig_buf_size); //Remember to free this memory.

    if (!jpge::compress_image_to_jpeg_file_in_memory(*destBuff, *comp_size, width, height, req_comps, *srcBuff, params)) {
        free(*destBuff);
        *destBuff   = NULL;
        *comp_size  = -1;
    }
}

void DLL_EXPORT decompress_img_greyscale(unsigned char * srcBuff, int comp_size, int * width, int * height, unsigned char ** destBuff) {
    int uncomp_actual_comps;
    int uncomp_req_comps = 1;
    *destBuff = jpgd::decompress_jpeg_image_from_memory(srcBuff, comp_size, width, height, &uncomp_actual_comps, uncomp_req_comps);
}
*/

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            // attach to process
            // return FALSE to fail DLL load
            break;

        case DLL_PROCESS_DETACH:
            // detach from process
            break;

        case DLL_THREAD_ATTACH:
            // attach to thread
            break;

        case DLL_THREAD_DETACH:
            // detach from thread
            break;
    }
    return TRUE; // succesful
}
