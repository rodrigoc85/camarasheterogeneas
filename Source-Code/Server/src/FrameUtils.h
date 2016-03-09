#pragma once

#include <stdio.h>
#include <locale.h>
#include <time.h>
#include <string>
#include <iostream>
#include <sys/time.h>
#include <sstream>
#include "ofxCvShortImage.h"

#include <windows.h>

typedef void (*f_compress_img) (void ** srcBuff, int width, int height, void ** destBuff, int * comp_size, int qfactor);
typedef void (*f_decompress_img) (void * srcBuff, int comp_size, int* width, int* height, void ** destBuff);

class FrameUtils {

	public:

    static void init() {

    }

    static ThreadData * getDummyFrame() {

        ThreadData * tData  = new ThreadData[2];
        tData[0].cliId      = 519; // ID que identificará a esta instalación de cliente en el servidor.
        tData[0].camId      = 2; // ID que identifica esta cámara dentro de la instalación cliente.

        gettimeofday(&tData[0].curTime, NULL);

        tData[0].img.loadImage("foto0.png");

        ofxCvShortImage img;
        img.allocate(100, 100);
        tData[0].spix       = img.getShortPixelsRef();
        tData[0].nubeW      = img.getWidth();
        tData[0].nubeH      = img.getHeight();
        tData[0].nubeLength = img.getWidth() * img.getHeight();
        tData[0].xpix       = (float*) malloc (sizeof(float) *tData[0].nubeLength);
        tData[0].ypix       = (float*) malloc (sizeof(float) *tData[0].nubeLength);
        tData[0].zpix       = (float*) malloc (sizeof(float) *tData[0].nubeLength);
        tData[0].state      = 3; //0-No inited, 1-Only Image, 2-Only Point Cloud, 3-Ambas

        tData[0].row1.set(1, 1, 1, 1);
        tData[0].row2.set(1, 1, 1, 1);
        tData[0].row3.set(1, 1, 1, 1);
        tData[0].row4.set(1, 1, 1, 1);

        tData[0].imgrow1.set(1, 1, 1, 1);
        tData[0].imgrow2.set(1, 1, 1, 1);
        tData[0].imgrow3.set(1, 1, 1, 1);
        tData[0].imgrow4.set(1, 1, 1, 1);

        tData[1].cliId      = 1; // ID que identificará a esta instalación de cliente en el servidor.
        tData[1].camId      = 3; // ID que identifica esta cámara dentro de la instalación cliente.
        gettimeofday(&tData[1].curTime, NULL);
        //tData[1].timestamp  = ofGetUnixTime();
        ofxCvShortImage img2;
        img2.allocate(100, 200);

        tData[1].img.loadImage("foto1.png");

        tData[1].spix       = img2.getShortPixelsRef();
        tData[1].nubeW      = img2.getWidth();
        tData[1].nubeH      = img2.getHeight();
        tData[1].nubeLength = img2.getWidth() * img2.getHeight();
        tData[1].xpix       = (float*) malloc (sizeof(float) *tData[1].nubeLength);
        tData[1].ypix       = (float*) malloc (sizeof(float) *tData[1].nubeLength);
        tData[1].zpix       = (float*) malloc (sizeof(float) *tData[1].nubeLength);
        tData[1].state      = 3; //0-No inited, 1-Only Image, 2-Only Point Cloud, 3-Ambas

        tData[1].row1.set(1, 1, 1, 1);
        tData[1].row2.set(1, 1, 1, 1);
        tData[1].row3.set(1, 1, 1, 1);
        tData[1].row4.set(1, 1, 1, 1);

        tData[1].imgrow1.set(1, 1, 1, 1);
        tData[1].imgrow2.set(1, 1, 1, 1);
        tData[1].imgrow3.set(1, 1, 1, 1);
        tData[1].imgrow4.set(1, 1, 1, 1);

        int w;
        for(w=0; w<tData[1].nubeLength; w++) {
            tData[1].xpix[w] = 0.0f;
            tData[1].ypix[w] = 0.0f;
            tData[1].zpix[w] = 0.0f;
            //cout << tData[1].xpix[w] << " " << tData[1].ypix[w] << " " << tData[1].zpix[w] << endl;
        }
        for(w=0; w<tData[0].nubeLength; w++) {
            tData[0].xpix[w] = 1.0f;
            tData[0].ypix[w] = 1.0f;
            tData[0].zpix[w] = 1.0f;
            //cout << tData[0].xpix[w] << " " << tData[0].ypix[w] << " " << tData[0].zpix[w] << endl;
        }

        return tData;
    }

    static int getFrameSize(ThreadData * tData, int totalCams) {
            /*
            Aquí tengo que calcular el tamaño total del gran bytearray.
            Cada ThreadData lo voy a transformar en un elemento bien controlado en tamaño para poder castearlo sin problema del otro lado.
            Dicho elemento va a estar compuesto de:
            p/c/thread data
             - (int) cliId; // ID de la configuración de Cliente. Puede haber N.
             - (int) camId; // ID de la cámara en la instalación.
             - (int) state  //0-No inited, 1-Only Image, 2-Only Point Cloud, 3-Ambas
             - (struct timeval) curTime
             - (int) imgWidth
             - (int) imgHeight
             - (unsigned char *) imagebytearray
             - (int) pcWidth
             - (int) pcHeight
             - (ofFloatPixels *) sXpix
             - (ofFloatPixels *) sYpix
             - (ofFloatPixels *) sZpix
            */

        try {
            int i=0;
            int totSize = sizeof(int); // totalCams
            for(i=0; i<totalCams; i++) {
                totSize += sizeof(int);     //(int) cliId
                totSize += sizeof(int);     //(int) camId
                totSize += sizeof(int);     //(int) state
                totSize += sizeof(int);     //(int) cameraType
                totSize += sizeof(timeval); //(struct timeval) [long + long]

                //Si (tData[i].state > 0) = Está inicializado
                if(tData[i].state > 0) {
                    //Si tiene imágen;
                    if((tData[i].state == 1) || (tData[i].state == 3)) {
                        totSize += sizeof(int);     //(int) camBArrSize
                        //totSize += sizeof(int);     //(int) camHeight
                        totSize += sizeof(float)*16; //(int) transformación matriz
                        //Reservo lugar para la imágen.

                        //ofPixels p = tData[i].img.getPixelsRef();
                        //totSize += p.size();
                        ofLogVerbose() << ">>>>[FrameUtils::getFrameSize] sumando el size " << tData[i].compSize << endl;
                        totSize += tData[i].compSize;

                        totSize += sizeof(int)*3; //imgWidth, imgHeight, compressed
                    }

                    //Si tiene nube;
                    if((tData[i].state == 2) || (tData[i].state == 3)) {
                        totSize += sizeof(int);     //(int) pcWidth
                        totSize += sizeof(int);     //(int) pcHeight
                        totSize += sizeof(int);     //(int) pcLength
                        //Reservo lugar para la nube.

                        totSize += (sizeof(float)) * tData[i].nubeLength * 3;
                    }
                }
            }
            return totSize;

        } catch (...) {
          ofLogWarning() << ">>>>[FrameUtils::getFrameSize] - Exception occurred.";
        }

        return -1;
    }

    static int getTotalCamerasFromByteArray(char * bytearray) {
        int totCameras; // = (int) *bytearray;
        try {
            memcpy(&(totCameras), (bytearray),     sizeof(int));
        } catch (...) {
            ofLogWarning() << ">>>>[FrameUtils::getTotalCamerasFromByteArray] - Exception occurred.";
        }
        return NULL;
    }

    static std::pair <int, ThreadData *> getThreadDataFromByteArray(char * bytearray) {
        try {
            int totCameras;
            memcpy(&(totCameras), (bytearray),     sizeof(int));
            //ofLogVerbose() << ">>>>[FrameUtils::getThreadDataFromByteArray] - totCameras " << totCameras;
            if(totCameras > 0) {
                ThreadData * tData  = new ThreadData[totCameras];
                int i;
                char * start = bytearray + sizeof(int);
                char* off_cliId;
                char* off_camId;
                char* off_state;
                char* off_cameraType;
                char* off_curTime;
                char* off_imgBArrSize;
                char* off_imgWidth;
                char* off_imgHeight;
                char* off_imgXYZ;
                char* off_imagebytearray;
                char* off_pcWidth;
                char* off_pcHeight;
                char* off_pcLength;
                char* off_nubeByteArray;

                for(i=0; i<totCameras; i++) {
                    if(i>0) {
                        tData[i-1].sig = &tData[i];
                    }
                    tData[i].sig = NULL;
                    /*
                     - (int) cliId; // ID de la configuración de Cliente. Puede haber N.
                     - (int) camId; // ID de la cámara en la instalación.
                     - (int) state  //0-No inited, 1-Only Image, 2-Only Point Cloud, 3-Ambas
                     - (timeval) curTime
                     - (int) imgWidth
                     - (int) imgHeight
                     - (unsigned char *) imagebytearray
                     - (int) pcWidth
                     - (int) pcHeight
                     - ofFloatPixels  sXpix;
                     - ofFloatPixels  sYpix;
                     - ofFloatPixels  sZpix;
                    */
                    off_cliId           = start;
                    off_camId           = off_cliId         + sizeof(int);
                    off_state           = off_camId         + sizeof(int);
                    off_cameraType      = off_state         + sizeof(int);
                    off_curTime         = off_cameraType    + sizeof(int);

                    memcpy(&(tData[i].cliId),       (off_cliId),      sizeof(int));
                    memcpy(&(tData[i].camId),       (off_camId),      sizeof(int));
                    memcpy(&(tData[i].state),       (off_state),      sizeof(int));
                    memcpy(&(tData[i].cameraType),  (off_cameraType), sizeof(int));
                    memcpy(&(tData[i].curTime),     (off_curTime),    sizeof(timeval));

                    //ofLogVerbose() << ">>>>[FrameUtils::getThreadDataFromByteArray] - recuperado - cliId: " << tData[i].cliId << ", camId: " << tData[i].camId << ", state: " << tData[i].state  << ", cameraType: " << tData[i].cameraType;

                    start = off_curTime   + sizeof(timeval);

                    //Si (tData[i].state > 0) = Está inicializado
                    if(tData[i].state > 0) {
                        //Si tiene imágen;
                        if((tData[i].state == 1) || (tData[i].state == 3)) {

                            //Reservo lugar para la imágen.
                            off_imgBArrSize      = start;
                            //off_imgWidth         = start;
                            //off_imgHeight        = off_imgWidth  + sizeof(int);
                            off_imgXYZ           = off_imgBArrSize + sizeof(int);
                            off_imagebytearray   = off_imgXYZ    + sizeof(float)*6;

                            int w;
                            int h;

                            memcpy(&(tData[i].compSize),     (off_imgBArrSize),     sizeof(int));

                            //ofLogVerbose() << ">>>>[FrameUtils::getThreadDataFromByteArray] - Recibida imagen de size: " << tData[i].compSize;

                            memcpy(&(tData[i].imgrow1.x),   (off_imgXYZ),                        sizeof(float));
                            memcpy(&(tData[i].imgrow1.y),   (off_imgXYZ + sizeof(float)),        sizeof(float));
                            memcpy(&(tData[i].imgrow1.z),   (off_imgXYZ + sizeof(float) * 2),    sizeof(float));
                            memcpy(&(tData[i].imgrow1.w),   (off_imgXYZ + sizeof(float) * 3),    sizeof(float));

                            memcpy(&(tData[i].imgrow2.x),   (off_imgXYZ + sizeof(float) * 4),    sizeof(float));
                            memcpy(&(tData[i].imgrow2.y),   (off_imgXYZ + sizeof(float) * 5),    sizeof(float));
                            memcpy(&(tData[i].imgrow2.z),   (off_imgXYZ + sizeof(float) * 6),    sizeof(float));
                            memcpy(&(tData[i].imgrow2.w),   (off_imgXYZ + sizeof(float) * 7),    sizeof(float));

                            memcpy(&(tData[i].imgrow3.x),   (off_imgXYZ + sizeof(float) * 8),    sizeof(float));
                            memcpy(&(tData[i].imgrow3.y),   (off_imgXYZ + sizeof(float) * 9),    sizeof(float));
                            memcpy(&(tData[i].imgrow3.z),   (off_imgXYZ + sizeof(float) * 10),   sizeof(float));
                            memcpy(&(tData[i].imgrow3.w),   (off_imgXYZ + sizeof(float) * 11),   sizeof(float));

                            memcpy(&(tData[i].imgrow4.x),   (off_imgXYZ + sizeof(float) * 12),   sizeof(float));
                            memcpy(&(tData[i].imgrow4.y),   (off_imgXYZ + sizeof(float) * 13),   sizeof(float));
                            memcpy(&(tData[i].imgrow4.z),   (off_imgXYZ + sizeof(float) * 14),   sizeof(float));
                            memcpy(&(tData[i].imgrow4.w),   (off_imgXYZ + sizeof(float) * 15),   sizeof(float));

                            /*
                            ofLogVerbose() << ">>>>[FrameUtils::getFrameByteArray] - imgrow1.x: " << tData[i].imgrow1.x << ", imgrow1.y: " << tData[i].imgrow1.y << ", imgrow1.z: " << tData[i].imgrow1.z << ", imgrow1.w: " << tData[i].imgrow1.w;
                            ofLogVerbose() << ">>>>[FrameUtils::getFrameByteArray] - imgrow2.x: " << tData[i].imgrow2.x << ", imgrow2.y: " << tData[i].imgrow2.y << ", imgrow2.z: " << tData[i].imgrow2.z << ", imgrow2.w: " << tData[i].imgrow2.w;
                            ofLogVerbose() << ">>>>[FrameUtils::getFrameByteArray] - imgrow3.x: " << tData[i].imgrow3.x << ", imgrow3.y: " << tData[i].imgrow3.y << ", imgrow3.z: " << tData[i].imgrow3.z << ", imgrow3.w: " << tData[i].imgrow3.w;
                            ofLogVerbose() << ">>>>[FrameUtils::getFrameByteArray] - imgrow4.x: " << tData[i].imgrow4.x << ", imgrow4.y: " << tData[i].imgrow4.y << ", imgrow4.z: " << tData[i].imgrow4.z << ", imgrow4.w: " << tData[i].imgrow4.w;
                            */

                            tData[i].compImg    = new char[tData[i].compSize];
                            memcpy((tData[i].compImg), ((unsigned char *) off_imagebytearray), tData[i].compSize);

                            off_imagebytearray  = off_imagebytearray + tData[i].compSize;
                            memcpy(&(tData[i].imgWidth),     (off_imagebytearray),     sizeof(int));

                            off_imagebytearray  = off_imagebytearray + sizeof(int);
                            memcpy(&(tData[i].imgHeight),     (off_imagebytearray),     sizeof(int));

                            int compressed = 0;
                            off_imagebytearray  = off_imagebytearray + sizeof(int);
                            memcpy(&compressed,     (off_imagebytearray),     sizeof(int));

                            tData[i].compressed = (compressed == 1);

                            start    = off_imagebytearray + sizeof(int);
                        }

                        off_pcWidth = start;

                        //Si tiene nube;
                        if((tData[i].state == 2) || (tData[i].state == 3)) {

                            off_pcHeight         = off_pcWidth  + sizeof(int);
                            off_pcLength         = off_pcHeight + sizeof(int);
                            off_nubeByteArray    = off_pcLength + sizeof(int);

                            int w                = (int) *off_pcWidth;
                            int h                = (int) *off_pcHeight;
                            int nubeLength       = (int) *off_pcLength;

                            memcpy(&(w),            (off_pcWidth),     sizeof(int));
                            memcpy(&(h),            (off_pcHeight),    sizeof(int));
                            memcpy(&(nubeLength),   (off_pcLength),    sizeof(int));

                            ofLogVerbose() << "Recibida nube de: " << w << "x" << h << " - total puntos: " << nubeLength;

                            tData[i].nubeW  = w;
                            tData[i].nubeH  = h;
                            tData[i].nubeLength = nubeLength;

                            if(nubeLength > 0) {
                                tData[i].xpix   = new float[nubeLength];
                                tData[i].ypix   = new float[nubeLength];
                                tData[i].zpix   = new float[nubeLength];

                                memcpy((tData[i].xpix),     (off_nubeByteArray),     sizeof(float)*nubeLength);
                                off_nubeByteArray   = off_nubeByteArray + nubeLength*sizeof(float);

                                memcpy((tData[i].ypix),     (off_nubeByteArray),     sizeof(float)*nubeLength);
                                off_nubeByteArray   = off_nubeByteArray + nubeLength*sizeof(float);

                                memcpy((tData[i].zpix),     (off_nubeByteArray),     sizeof(float)*nubeLength);
                                off_nubeByteArray   = off_nubeByteArray + nubeLength*sizeof(float);
                            }
                            //Reservo lugar para la nube.
                            start = off_nubeByteArray;
                        }
                    }
                }
                std::pair <int, ThreadData *> retVal;
                tData[totCameras-1].sig = NULL;
                retVal.first    = totCameras;
                retVal.second   = tData;
                return retVal;
            }
        } catch (...) {
            ofLogWarning() << ">>>>[FrameUtils::getThreadDataFromByteArray] - Exception occurred.";
        }
        std::pair <int, ThreadData *> retVal;
        retVal.first    = 0;
        retVal.second   = NULL;
        return retVal;
    }

    static char * getFrameByteArray(ThreadData * tData, int totalCams, int size) {
        /*
        Aquí tengo que formar el gran bytearray del frame (el tamaño ya está calculado en size).
        Cada ThreadData lo voy a transformar en un elemento bien controlado en tamaño para poder castearlo sin problema del otro lado.
        Dicho elemento va a estar compuesto de:
        p/c/thread data
         - (int) cliId; // ID de la configuración de Cliente. Puede haber N.
         - (int) camId; // ID de la cámara en la instalación.
         - (int) state  //0-No inited, 1-Only Image, 2-Only Point Cloud, 3-Ambas
         - (timeval) curTime
         - (unsigned char *) imagebytearray
         - ofFloatPixels  sXpix;
         - ofFloatPixels  sYpix;
         - ofFloatPixels  sZpix;
        */
        char * combined = NULL;
        try {
            combined        = (char*) malloc (size);
            memcpy(combined,       &totalCams,     sizeof(int));
            char* start     = combined + sizeof(int);
            char* off_cliId;
            char* off_camId;
            char* off_state;
            char* off_cameraType;
            char* off_curTime;
            char* off_imgBArrSize;
            char* off_imgXYZ;
            char* off_imagebytearray;
            char* off_pcWidth;
            char* off_pcHeight;
            char* off_pcLength;
            char* off_nubeByteArray;

            int i=0, totSize = 0;
            for(i=0; i<totalCams; i++) {
                //ofLogVerbose() << "off_imagebytearray size " << tData[i].compSize << endl;

                off_cliId           = start;
                off_camId           = off_cliId      + sizeof(int);
                off_state           = off_camId      + sizeof(int);
                off_cameraType      = off_state      + sizeof(int);
                off_curTime         = off_cameraType + sizeof(int);

                memcpy(off_cliId,           &tData[i].cliId,        sizeof(int));
                memcpy(off_camId,           &tData[i].camId,        sizeof(int));
                memcpy(off_state,           &tData[i].state,        sizeof(int));
                memcpy(off_cameraType,      &tData[i].cameraType,   sizeof(int));
                memcpy(off_curTime,         &tData[i].curTime,      sizeof(timeval));

                ofLogVerbose() << ">>>>[FrameUtils::getThreadDataFromByteArray] - guardado - cliId: " << tData[i].cliId << ", camId: " << tData[i].camId << ", state: " << tData[i].state  << ", cameraType: " << tData[i].cameraType;

                start = off_curTime + sizeof(timeval);

                //Si (tData[i].state > 0) = Está inicializado
                if(tData[i].state > 0) {
                    //Si tiene imágen;

                    if((tData[i].state == 1) || (tData[i].state == 3)) {
                        //Reservo lugar para la imágen.
                        off_imgBArrSize      = start;
                        off_imgXYZ           = off_imgBArrSize + sizeof(int);
                        off_imagebytearray   = off_imgXYZ    + sizeof(float)*6;

                        int w = tData[i].img.getWidth();
                        int h = tData[i].img.getHeight();

                        ofLogVerbose() << ">>>>[FrameUtils::getFrameByteArray] - guardado - w: " << w;
                        ofLogVerbose() << ">>>>[FrameUtils::getFrameByteArray] - guardado - h: " << h;

                        memcpy(off_imgBArrSize,    &tData[i].compSize,    sizeof(int));

                        ofLogVerbose() << ">>>>[FrameUtils::getFrameByteArray] - Peso de la imagen guardada: " << tData[i].compSize;

                        memcpy(off_imgXYZ,                    &tData[i].imgrow1.x,    sizeof(float));
                        memcpy(off_imgXYZ + sizeof(float),    &tData[i].imgrow1.y,    sizeof(float));
                        memcpy(off_imgXYZ + sizeof(float)*2,  &tData[i].imgrow1.z,    sizeof(float));
                        memcpy(off_imgXYZ + sizeof(float)*3,  &tData[i].imgrow1.w,    sizeof(float));

                        memcpy(off_imgXYZ + sizeof(float)*4,  &tData[i].imgrow2.x,    sizeof(float));
                        memcpy(off_imgXYZ + sizeof(float)*5,  &tData[i].imgrow2.y,    sizeof(float));
                        memcpy(off_imgXYZ + sizeof(float)*6,  &tData[i].imgrow2.z,    sizeof(float));
                        memcpy(off_imgXYZ + sizeof(float)*7,  &tData[i].imgrow2.w,    sizeof(float));

                        memcpy(off_imgXYZ + sizeof(float)*8,  &tData[i].imgrow3.x,    sizeof(float));
                        memcpy(off_imgXYZ + sizeof(float)*9,  &tData[i].imgrow3.y,    sizeof(float));
                        memcpy(off_imgXYZ + sizeof(float)*10, &tData[i].imgrow3.z,    sizeof(float));
                        memcpy(off_imgXYZ + sizeof(float)*11, &tData[i].imgrow3.w,    sizeof(float));

                        memcpy(off_imgXYZ + sizeof(float)*12, &tData[i].imgrow4.x,    sizeof(float));
                        memcpy(off_imgXYZ + sizeof(float)*13, &tData[i].imgrow4.y,    sizeof(float));
                        memcpy(off_imgXYZ + sizeof(float)*14, &tData[i].imgrow4.z,    sizeof(float));
                        memcpy(off_imgXYZ + sizeof(float)*15, &tData[i].imgrow4.w,    sizeof(float));

                        ofLogVerbose() << ">>>>[FrameUtils::getFrameByteArray] - row1.x: " << tData[i].imgrow1.x << ", row1.y: " << tData[i].imgrow1.y << ", row1.z: " << tData[i].imgrow1.z << ", row1.w: " << tData[i].imgrow1.w;
                        ofLogVerbose() << ">>>>[FrameUtils::getFrameByteArray] - row2.x: " << tData[i].imgrow2.x << ", row2.y: " << tData[i].imgrow2.y << ", row2.z: " << tData[i].imgrow2.z << ", row2.w: " << tData[i].imgrow2.w;
                        ofLogVerbose() << ">>>>[FrameUtils::getFrameByteArray] - row3.x: " << tData[i].imgrow3.x << ", row3.y: " << tData[i].imgrow3.y << ", row3.z: " << tData[i].imgrow3.z << ", row3.w: " << tData[i].imgrow3.w;
                        ofLogVerbose() << ">>>>[FrameUtils::getFrameByteArray] - row4.x: " << tData[i].imgrow4.x << ", row4.y: " << tData[i].imgrow4.y << ", row4.z: " << tData[i].imgrow4.z << ", row4.w: " << tData[i].imgrow4.w;

                        memcpy(off_imagebytearray, tData[i].compImg, tData[i].compSize);

                        off_imagebytearray  = off_imagebytearray + tData[i].compSize;
                        memcpy(off_imagebytearray,     (&tData[i].imgWidth),     sizeof(int));

                        off_imagebytearray  = off_imagebytearray + sizeof(int);
                        memcpy(off_imagebytearray,     (&tData[i].imgHeight),     sizeof(int));

                        int compressed = ((tData[i].compressed) ? 1 : 0);
                        off_imagebytearray  = off_imagebytearray + sizeof(int);
                        memcpy(off_imagebytearray,     (&compressed),     sizeof(int));

                        start    = off_imagebytearray + sizeof(int);
                    }

                    off_pcWidth = start; //off_curTime + sizeof(timeval);
                    //Si tiene nube;
                    if((tData[i].state == 2) || (tData[i].state == 3)) {

                        off_pcHeight         = off_pcWidth  + sizeof(int);
                        off_pcLength         = off_pcHeight + sizeof(int);
                        off_nubeByteArray    = off_pcLength + sizeof(int);

                        int w           = tData[i].nubeW;
                        int h           = tData[i].nubeH;
                        int nubeLength  = tData[i].nubeLength;

                        ofLogVerbose() << ">>>>[FrameUtils::getFrameByteArray] - nube guardado - w: " << tData[i].nubeW;
                        ofLogVerbose() << ">>>>[FrameUtils::getFrameByteArray] - nube guardado - h: " << tData[i].nubeH;
                        ofLogVerbose() << ">>>>[FrameUtils::getFrameByteArray] - nube guardado - nubeLength: " << nubeLength;

                        memcpy(off_pcWidth,       &w,           sizeof(int));
                        memcpy(off_pcHeight,      &h,           sizeof(int));
                        memcpy(off_pcLength,      &nubeLength,  sizeof(int));

                        if(nubeLength > 0) {
                            memcpy(off_nubeByteArray,   tData[i].xpix, (sizeof(float) * nubeLength));
                            off_nubeByteArray = off_nubeByteArray + (sizeof(float)) * nubeLength;

                            memcpy(off_nubeByteArray,   tData[i].ypix, (sizeof(float)) * nubeLength);
                            off_nubeByteArray = off_nubeByteArray + (sizeof(float)) * nubeLength;

                            memcpy(off_nubeByteArray,   tData[i].zpix, (sizeof(float)) * nubeLength);
                            off_nubeByteArray = off_nubeByteArray + (sizeof(float)) * nubeLength;
                        }

                        start = off_nubeByteArray;
                    }

                }
            }

        } catch (...) {
            ofLogWarning() << ">>>>[FrameUtils::getFrameByteArray] - Exception occurred.";
        }

        return combined;
    }

    static char * addToBytearray(char * receiveBytes, int numBytes, char * currBytearray, int currTotal) {
        try {
            if(numBytes <= 0) return currBytearray;
            char * retBytearray = new char[currTotal + numBytes];
            char * pointer;
            if(currBytearray != NULL) {
                memcpy(retBytearray, currBytearray, currTotal);
                delete currBytearray;
                pointer = retBytearray + currTotal;
                memcpy(pointer, receiveBytes, numBytes);
            } else {
                memcpy(retBytearray, receiveBytes, numBytes);
            }
            return retBytearray;
        } catch (...) {
            ofLogWarning() << ">>>>[FrameUtils::addToBytearray] - Exception occurred.";
        }
        return NULL;
    }

    /**
    * Recorre los totalCams ThreadData y comprime todas las imágenes.
    */
    static void compressImages(ThreadData * tData, int totalCams, f_compress_img compress_img) {
        try {
            void * srcBuff;
            int i;
            for(i=0; i<totalCams; i++) {
                if(tData[i].state > 0) {
                    if((tData[i].state == 1) || (tData[i].state == 3)) {
                        srcBuff           = (void *) tData[i].img.getPixels();
                        if(!tData[i].compressed) {
                            int orig_buf_size       = tData[i].img.getWidth() * tData[i].img.getHeight() * 3;
                            tData[i].compImg        = malloc(orig_buf_size);
                            memcpy(tData[i].compImg, srcBuff, orig_buf_size);
                            tData[i].compSize       = orig_buf_size;
                        } else {
                            compress_img(&srcBuff, tData[i].img.getWidth(), tData[i].img.getHeight(), &tData[i].compImg, &tData[i].compSize, tData[i].qfactor);
                            ofLogWarning() << ">>>>[FrameUtils::compressImages] - tData[i].compSize " << tData[i].compSize;
                        }
                    }
                    if((tData[i].state == 2) || (tData[i].state == 3)) {
                        //@TODO: Acá falta integrar la compresión de la nube.
                    }
                }
            }
        } catch (...) {
            ofLogWarning() << ">>>>[FrameUtils::compressImages] - Exception occurred.";
        }
    }

    /*static void compressSample() {

        HINSTANCE hGetProcIDDLL;

        hGetProcIDDLL =  LoadLibraryA("imageCompression.dll");

        if (!hGetProcIDDLL) {
            std::cout << "No se pudo cargar la libreria: " << std::endl;
        } else {
            f_compress_img      compress_img    = (f_compress_img)   GetProcAddress(hGetProcIDDLL, "compress_img");
            f_decompress_img    decompress_img  = (f_decompress_img) GetProcAddress(hGetProcIDDLL, "decompress_img");

            ofImage img;
            img.loadImage("img1.jpg");

            void * srcBuff           = (void *) img.getPixels();
            void * destBuff          = NULL;
            int width                = img.getWidth();
            int height               = img.getHeight();
            int comp_size;

            compress_img(&srcBuff, width, height, &destBuff, &comp_size);

            if(comp_size == -1) {
                std::cout << "Error al comprimir la imagen. " << std::endl;
            } else {
                int unc_width   = 0;
                int unc_height  = 0;

                const unsigned char * unc_Buff = NULL;
                decompress_img(destBuff, comp_size, &unc_width, &unc_height, (void **)&unc_Buff);

                ofImage img2;
                img2.setFromPixels(unc_Buff, unc_width, unc_height, OF_IMAGE_COLOR, true);
                img2.saveImage("salida_decompress_img.jpg");

            }
        }
    }*/

    /**
    * Recorre los totalCams ThreadData y descomprime todas las imágenes.
    */
    static void decompressImages(ThreadData * tData, int totalCams, f_decompress_img decompress_img) {
//        HINSTANCE hGetProcIDDLL;
//        //HINSTANCE hGetProcPCIDDLL;
//
//        hGetProcIDDLL                    =  LoadLibraryA("imageCompression.dll");
//        //hGetProcPCIDDLL                     =  LoadLibraryA("pointCloudCompression.dll");
//
//        f_decompress_img    decompress_img  = (f_decompress_img) GetProcAddress(hGetProcIDDLL,      "decompress_img");
        //f_decompress_pc     decompress_pc   = (f_decompress_img) GetProcAddress(hGetProcPCIDDLL,    "decompress_pc");

        try {
            int i;
            for(i=0; i<totalCams; i++) {
                if(tData[i].state > 0) {
                    if((tData[i].state == 1) || (tData[i].state == 3)) {
                        if(tData[i].compSize == -1) {
                            std::cout << "Error al comprimir la imagen. " << std::endl;
                        } else {
                            if(tData[i].compressed) {
                                int unc_width   = 0;
                                int unc_height  = 0;

                                const unsigned char * unc_Buff = NULL;
                                decompress_img(tData[i].compImg, tData[i].compSize, &unc_width, &unc_height, (void **)&unc_Buff);
                                tData[i].img.setFromPixels(unc_Buff, unc_width, unc_height, OF_IMAGE_COLOR, true);
                                //tData[i].img.saveImage("decompress_debug.jpg");
                            } else {
                                tData[i].img.setFromPixels((unsigned char *) tData[i].compImg, tData[i].imgWidth, tData[i].imgHeight, OF_IMAGE_COLOR, true);
                                //tData[i].img.setFromPixels(tData[i].compImg, tData[i]., unc_height, OF_IMAGE_COLOR, true);
                            }
                        }
                    }
                    if((tData[i].state == 2) || (tData[i].state == 3)) {
                        //@TODO: Acá falta integrar la compresion de la nube.

                    }
                }
            }
        } catch (...) {
            ofLogWarning() << ">>>>[FrameUtils::decompressImages] - Exception occurred.";
        }
    }

    /**
    * Retorna un pair con <bytearray, size>
    */
    static std::pair <char *, int> compressData(char * data, int size) {

    }

    /**
    * Retorna un pair con <bytearray, size>
    */
    static std::pair <char *, int> decompressData(char * data, int size) {

    }
};
