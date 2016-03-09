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
        bool removeRecursively;
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
            xpix        = NULL;
            ypix        = NULL;
            zpix        = NULL;
            compImg     = NULL;
            cameraType  = 0;
            sig         = NULL;
            removeRecursively = true;
        }

        ~ThreadData() {
            if(removeRecursively && (sig != NULL)) {
                delete sig;
            }
            releaseResources();
        }

        void releaseResources() {

            if(xpix != NULL) {
                free(xpix);
                xpix = NULL;
            }

            if(ypix != NULL) {
                free(ypix);
                ypix = NULL;
            }

            if(zpix != NULL) {
                free(zpix);
                zpix = NULL;
            }

            if(compImg != NULL) {
                //cout << " eliminando compImg " << endl;
                free(compImg);
                compImg = NULL;
            }

            /*if(sig != NULL) {
                delete sig;
                //sig = NULL;
            }*/

            img.clear();
            spix.clear();

            cvX.clear();
            cvY.clear();
            cvZ.clear();
            encodedCloud.clear();
            sXpix.clear();
            sYpix.clear();
            sZpix.clear();

            ofLogVerbose() << "[ThreadData::~ThreadData] fin";
        }

        void mergePointClouds(ThreadData * td) {
            if(td->nubeLength > 0) {
                /*
                ofLogVerbose() << "[ThreadData::mergePointClouds] - Mergeando 2 Frames - " << nubeLength << " - " << td->nubeLength;

                int curLength   = nubeLength;

                char * tmpzPix = (char*) malloc ( sizeof(float) * (curLength + td->nubeLength) );
                char * tmpyPix = (char*) malloc ( sizeof(float) * (curLength + td->nubeLength) );
                char * tmpxPix = (char*) malloc ( sizeof(float) * (curLength + td->nubeLength) );
                char * offx    = tmpxPix + curLength * sizeof(float);
                char * offy    = tmpyPix + curLength * sizeof(float);
                char * offz    = tmpzPix + curLength * sizeof(float);

                memcpy(tmpxPix,  xpix,      sizeof(float) * curLength);
                memcpy(offx,     td->xpix,  sizeof(float) * td->nubeLength);

                memcpy(tmpyPix,  ypix,      sizeof(float) * curLength);
                memcpy(offy,     td->ypix,  sizeof(float) * td->nubeLength);

                memcpy(tmpzPix,  zpix,      sizeof(float) * curLength);
                memcpy(offz,     td->zpix,  sizeof(float) * td->nubeLength);

                int w;

                free(xpix);
                free(ypix);
                free(zpix);

                xpix = (float *) tmpxPix;
                ypix = (float *) tmpyPix;
                zpix = (float *) tmpzPix;

                nubeLength = curLength + td->nubeLength;
                */
            }
        }
};
