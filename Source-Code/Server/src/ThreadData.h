#pragma once
#include <sys/time.h>

#include "ofxOpenCv.h"
//template <time_t, int> struct fm_key;

class ThreadData {
	public:
        ThreadData * sig; //Usado en el MainBuffer para concatenar los ThreadData que contengan imágenes 2D.
        std::pair <time_t, int> key;
        //fm_key<time_t, int> * key;
        timeval curTime;
        //time_t  timestamp;
        //char *  timestampStr;
        int     cliId;   // ID de la configuración de Cliente. Puede haber N
        int     camId;   // ID de la cámara en la instalación.
        ofImage img;
        void *  compImg;
        int     compSize;
        int     state; //0-No inited, 1-Only Image, 2-Only Point Cloud, 3-Ambas
        //void    updateData();
        ofShortPixels  spix;
        int nubeLength;
        int nubeW;
        int nubeH;

        int imgWidth;
        int imgHeight;
        int qfactor;
        bool compressed;
        bool released;
        bool used;
        bool deletable;

        int cameraType; //0-Unknown, 1-RGB, 2-Depth Camera
        float *         xpix;
        float *         ypix;
        float *         zpix;

        ofxCvGrayscaleImage	cvX;
        ofxCvGrayscaleImage	cvY;
        ofxCvGrayscaleImage	cvZ;
        ofxCvColorImage encodedCloud;

        ofVec4f row1;
        ofVec4f row2;
        ofVec4f row3;
        ofVec4f row4;

        ofVec4f imgrow1;
        ofVec4f imgrow2;
        ofVec4f imgrow3;
        ofVec4f imgrow4;

        //ofMatrix4x4 matrix;

        ofFloatPixels  sXpix;
        ofFloatPixels  sYpix;
        ofFloatPixels  sZpix;

        ThreadData() {
            used        = false;
            xpix        = NULL;
            ypix        = NULL;
            zpix        = NULL;
            compImg     = NULL;
            cameraType  = 0;
            sig         = NULL;
            released    = false;
            deletable   = true;
        }

        ~ThreadData() {
            releaseResources();
        }

        static ThreadData * Clone(ThreadData * oTD) {
            if(oTD == NULL) return NULL;
            ThreadData * nTD = new ThreadData();
            nTD->used        = oTD->used;
            nTD->sig         = oTD->sig;
            nTD->curTime     = oTD->curTime;
            nTD->cliId       = oTD->cliId;
            nTD->camId       = oTD->camId;
            nTD->img.clone(oTD->img);
            nTD->state       = oTD->state;
            nTD->nubeLength  = oTD->nubeLength;
            nTD->nubeW       = oTD->nubeW;
            nTD->nubeH       = oTD->nubeH;
            nTD->imgWidth    = oTD->imgWidth;
            nTD->imgHeight   = oTD->imgHeight;
            nTD->qfactor     = oTD->qfactor;
            nTD->compressed  = oTD->compressed;
            nTD->cameraType  = oTD->cameraType;
            nTD->imgrow1.set(oTD->imgrow1.x, oTD->imgrow1.y, oTD->imgrow1.z, oTD->imgrow1.w);
            nTD->imgrow2.set(oTD->imgrow2.x, oTD->imgrow2.y, oTD->imgrow2.z, oTD->imgrow2.w);
            nTD->imgrow3.set(oTD->imgrow3.x, oTD->imgrow3.y, oTD->imgrow3.z, oTD->imgrow3.w);
            nTD->imgrow4.set(oTD->imgrow4.x, oTD->imgrow4.y, oTD->imgrow4.z, oTD->imgrow4.w);
            nTD->row1.set( oTD->row1.x, oTD->row1.y, oTD->row1.z, oTD->row1.w);
            nTD->row2.set( oTD->row2.x, oTD->row2.y, oTD->row2.z, oTD->row2.w);
            nTD->row3.set( oTD->row3.x, oTD->row3.y, oTD->row3.z, oTD->row3.w);
            nTD->row4.set( oTD->row4.x, oTD->row4.y, oTD->row4.z, oTD->row4.w);
            if(oTD->state > 1) {
                if((oTD->nubeLength < 100) || ((!oTD->xpix) || (!oTD->ypix) || (!oTD->zpix))) {
                    if(nTD->state == 3) {
                        nTD->state = 1;
                    } else {
                        nTD->state = 0;
                    }
                    nTD->nubeLength = 0;
                } else {
                    nTD->xpix   = new float[oTD->nubeLength];
                    nTD->ypix   = new float[oTD->nubeLength];
                    nTD->zpix   = new float[oTD->nubeLength];
                    memcpy(nTD->xpix,  oTD->xpix,  sizeof(float) * oTD->nubeLength);
                    memcpy(nTD->ypix,  oTD->ypix,  sizeof(float) * oTD->nubeLength);
                    memcpy(nTD->zpix,  oTD->zpix,  sizeof(float) * oTD->nubeLength);
                }

            }
            return nTD;
        }

        void releaseResources() {
            if(released) return;
            if(xpix != NULL) {
                delete xpix;
                xpix = NULL;
            }

            if(ypix != NULL) {
                delete ypix;
                ypix = NULL;
            }

            if(zpix != NULL) {
                delete zpix;
                zpix = NULL;
            }

            if(compImg != NULL) {
                delete compImg;
                compImg = NULL;
            }

            img.clear();
            spix.clear();

            cvX.clear();
            cvY.clear();
            cvZ.clear();
            encodedCloud.clear();
            sXpix.clear();
            sYpix.clear();
            sZpix.clear();

            //ofLogVerbose() << "[ThreadData::~ThreadData] fin";
            released    = true;
        }

        void mergePointClouds(ThreadData * td) {
            if(td->nubeLength > 0) {
                //ofLogVerbose() << "[ThreadData::mergePointClouds] - Mergeando 2 Frames - " << nubeLength << " - " << td->nubeLength;

                int curLength   = nubeLength;

                float * tmpzPix = new float [curLength + td->nubeLength];
                float * tmpyPix = new float [curLength + td->nubeLength];
                float * tmpxPix = new float [curLength + td->nubeLength];
                char * offx    = (char *) tmpxPix + curLength * sizeof(float);
                char * offy    = (char *) tmpyPix + curLength * sizeof(float);
                char * offz    = (char *) tmpzPix + curLength * sizeof(float);

                memcpy(tmpxPix,  xpix,      sizeof(float) * curLength);
                memcpy(offx,     td->xpix,  sizeof(float) * td->nubeLength);

                memcpy(tmpyPix,  ypix,      sizeof(float) * curLength);
                memcpy(offy,     td->ypix,  sizeof(float) * td->nubeLength);

                memcpy(tmpzPix,  zpix,      sizeof(float) * curLength);
                memcpy(offz,     td->zpix,  sizeof(float) * td->nubeLength);

                int w;

                delete(xpix);
                delete(ypix);
                delete(zpix);

                xpix = (float *) tmpxPix;
                ypix = (float *) tmpyPix;
                zpix = (float *) tmpzPix;

                nubeLength = curLength + td->nubeLength;

            }
        }
};
