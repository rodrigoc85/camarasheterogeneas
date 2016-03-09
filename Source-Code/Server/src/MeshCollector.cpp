#include "MeshCollector.h"

// comparison, not case sensitive.
bool compare_nframe (GeneratedResult * first, GeneratedResult * second) {
    return (first->nframe < second->nframe);
}

void MeshCollector::threadedFunction() {
    outMeshId       = -1;
    ShareMesh       = (f_ShareMesh)GetProcAddress(memorySharedLibrary,          "ShareMesh");
    ShareMeshSetup  = (f_ShareMeshSetup)GetProcAddress(memorySharedLibrary,     "ShareMeshSetup");
    ShareImage      = (f_ShareImage)GetProcAddress(memorySharedLibrary,         "ShareImage");
    ShareImageSetup = (f_ShareImageSetup)GetProcAddress(memorySharedLibrary,    "ShareImageSetup");

    unsigned long long minMillis = 1000/sys_data->fps;
    unsigned long long currMill, baseMill;

    while(isThreadRunning()) {
        baseMill = ofGetElapsedTimeMillis();
        processFrame();
        currMill = ofGetElapsedTimeMillis();
        if((currMill - baseMill) < minMillis) {
            sleep(minMillis - (currMill - baseMill));
        }
    }
}

void MeshCollector::exit() {
    b_exit  = true;
}

void MeshCollector::processFrame() {
    //Ver en http://www.cplusplus.com/reference/list/list/sort/
    int i = 0;
    for(i=0; ((i<sys_data->totalFreeCores) && !b_exit); i++) {
        if(threads[i].getState() == GENERATOR_COMPLETE) {
            bool esta = false;
            for (it=list.begin(); it!=list.end(); ++it) {
                if((threads[i].result->nframe == (*it)->nframe)) esta = true;
            }
            if(!esta) {
                threads[i].setState(GENERATOR_IDLE);
                list.push_back(threads[i].result);
                list.sort(compare_nframe);
            }
        }
    }

    shareNextCompleteFrame();
}

void MeshCollector::shareNextCompleteFrame() {
    if(list.size()>0) {
        GeneratedResult * result = list.front();
        if((currFrame + 1) == result->nframe) {
            currFrame++;
            list.remove(result);
            shareFrame(result);
        }
    }
}

void MeshCollector::shareFrame(GeneratedResult * gresult) {
    int idMomento;
    unsigned char* pixels;
    int width;
    int height;

    ThreadData * iter = (ThreadData *) gresult->textures;

    if(gresult->hasDepth) {
        int numFaces = *gresult->numberFaces;
        //if(!descartado) {
            ofLogVerbose() << "--||--[MeshCollector::shareFrame] idMesh " << gresult->idMesh << ", numFaces:" << numFaces << endl;
            /*for(int i = 0; i<numFaces; i++) {
                ofLogVerbose() << "[MeshCollector::shareFrame]" << ", p1_0: " << gresult->faces[i].p1[0] << ", p1_1: " << gresult->faces[i].p1[1]  << ", p1_2: " << gresult->faces[i].p1[2] << ", p2_0: " << gresult->faces[i].p2[0] << ", p2_1: " << gresult->faces[i].p2[1]  << ", p2_2: " << gresult->faces[i].p2[2] << ", p3_0: " << gresult->faces[i].p3[0] << ", p3_1: " << gresult->faces[i].p3[1]  << ", p3_2: " << gresult->faces[i].p3[2] << endl;
            }*/
            if(outMeshId == -1) {
                ShareMeshSetup(&gresult->idMesh, &outMeshId);
                //LLAMO AL SETUP DE LA LIBRERÍA PARA LA MALLA
            }
            ShareMesh(&gresult->idMesh, &numFaces, gresult->faces, &outMeshId);
            //ShareMesh(gresult->idMesh, numFaces, gresult->faces);
            ofLogVerbose() << "--||--[MeshCollector::shareFrame] 2 ";
        //}
        ofLogVerbose() << "--||--[MeshCollector::shareFrame] 3 ";
        delete [] gresult->faces;
        ofLogVerbose() << "--||--[MeshCollector::shareFrame] 4 ";
        delete gresult->numberFaces;
        ofLogVerbose() << "--||--[MeshCollector::shareFrame] 5 ";
    }

    if(gresult->hasRGB) {
        ofLogVerbose() << "--||--[MeshCollector::shareFrame] 6 ";
        iter = (ThreadData *) gresult->textures;
        try {
            ofLogVerbose() << "--||--[MeshCollector::shareFrame] 7 ";
            int i = 0;
            do {
                if((iter->img.getWidth() > 0) && (iter->img.getHeight() > 0)) {
                    ofLogVerbose() << "--||--[MeshCollector::shareFrame]   8 ";
                    i++;
                    ofBuffer imageBuffer;
                    ofLogVerbose() << "--||--[MeshCollector::shareFrame]   9 " << iter->img.bAllocated();

                    ofSaveImage(iter->img.getPixelsRef(), imageBuffer, OF_IMAGE_FORMAT_JPEG);
                    ofLogVerbose() << "--||--[MeshCollector::shareFrame]   10 ";
                    //iter->img.saveImage("mcollector_share_img_" + ofToString(i) + ".jpg");
                    FIMEMORY* stream        = FreeImage_OpenMemory((unsigned char*) imageBuffer.getBinaryBuffer(), imageBuffer.size());
                    ofLogVerbose() << "--||--[MeshCollector::shareFrame]   11 ";
                    FREE_IMAGE_FORMAT fif   = FreeImage_GetFileTypeFromMemory( stream, 0 );
                    ofLogVerbose() << "--||--[MeshCollector::shareFrame]   12 ";
                    FIBITMAP *dib(0);
                    ofLogVerbose() << "--||--[MeshCollector::shareFrame]   13 ";
                    dib                     = FreeImage_LoadFromMemory(fif, stream);
                    ofLogVerbose() << "--||--[MeshCollector::shareFrame]   14 ";
                    pixels      = (unsigned char*)FreeImage_GetBits(dib);

                    if(pixels != NULL) {
                        ofLogVerbose() << "--||--[MeshCollector::shareFrame]   15 ";
                        width       = FreeImage_GetWidth(dib);
                        ofLogVerbose() << "--||--[MeshCollector::shareFrame]   16 ";
                        height      = FreeImage_GetHeight(dib);
                        ofLogVerbose() << "--||--[MeshCollector::shareFrame]   17 ";
                        //ofLogVerbose() << "[MeshCollector::shareFrame] dib width: " << width << ", height: " << height << ", img.width: " << iter->img.getWidth() << ", img.height: " << iter->img.getHeight() << endl;
                        idMomento   = iter->cliId;
                        idMomento   = idMomento*10 + iter->cameraType;
                        idMomento   = idMomento*10 + iter->camId;
                        idMomento   = idMomento*10000 + gresult->nframe % 10000;

                        ofLogVerbose() << "--||--[MeshCollector::shareFrame]   18 ";
                        int rgbOutId = -1;

                        for (outItRGB=outListRGB.begin(); outItRGB!=outListRGB.end(); ++outItRGB) {
                            if(!(((*outItRGB)->cliId == iter->cliId) && ((*outItRGB)->camId == iter->camId)) ) rgbOutId = (*outItRGB)->outputId;
                        }

                        if(rgbOutId == -1) {
                            ShareImageSetup(&idMomento, &width, &height, &rgbOutId);
                            //LLAMO AL SETUP DE LA LIBRERÍA PARA LA MALLA
                            RGBDataOutput * rgbDataOut = new RGBDataOutput();
                            rgbDataOut->cliId    = iter->cliId;
                            rgbDataOut->camId    = iter->camId;
                            rgbDataOut->outputId = rgbOutId;
                            outListRGB.push_back(rgbDataOut);
                        }

                        //shareImage(&idMomento, pixels, &width, &height);
                        ShareImage(&idMomento, pixels, &rgbOutId);

                        ofLogVerbose() << "--||--[MeshCollector::shareFrame]   19 ";
                        FreeImage_CloseMemory(stream);
                        ofLogVerbose() << "--||--[MeshCollector::shareFrame]   20 ";
                        imageBuffer.clear();
                        ofLogVerbose() << "--||--[MeshCollector::shareFrame]   21 ";
                        free(pixels);
                        ofLogVerbose() << "--||--[MeshCollector::shareFrame]   22 ";
                        FreeImage_Unload(dib);
                        ofLogVerbose() << "--||--[MeshCollector::shareFrame]   23 ";
                    } else {
                        return;
                    }
                } else {
                    return;
                }
                iter = iter->sig;

            } while(iter != NULL);
        } catch(const std::exception & exc) {
            ofLogVerbose() << "--||--[MeshCollector::shareFrame]   CATCH ";
        }

        ofLogVerbose() << "--||--[MeshCollector::shareFrame]   24 ";
        if(gresult->textures != NULL) {
            ofLogVerbose() << "--||--[MeshCollector::shareFrame]   25 ";
            while(gresult->textures != NULL){
                ofLogVerbose() << "--||--[MeshCollector::shareFrame]   26 ";
                ThreadData * curr = gresult->textures;
                ofLogVerbose() << "--||--[MeshCollector::shareFrame]   27 ";
                gresult->textures = gresult->textures->sig;
                ofLogVerbose() << "--||--[MeshCollector::shareFrame]   28 ";
                delete curr;
            }
            ofLogVerbose() << "--||--[MeshCollector::shareFrame]   29 ";
            gresult->textures = NULL;
        }
    }

    ofLogVerbose() << "--||--[MeshCollector::shareFrame]   30 ";
    delete gresult;
    ofLogVerbose() << "--||--[MeshCollector::shareFrame]   31 ";
}
