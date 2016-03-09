#include "GlobalData.h"

//--------------------------------------------------------------
int GlobalData::getFPS() {

}

bool GlobalData::getGoLive() {

}

int GlobalData::getTotal3D() {

}

int GlobalData::getTotal2D() {

}

int GlobalData::getTotalDevices() {

}

void GlobalData::loadCalibData(char * xml) {
    ofxXmlSettings settings;
    settings.loadFile(xml);
    t_camera * currCam = NULL;

    total2D     = 0;
    total3D     = 0;
    totalONI    = 0;

	if(settings.pushTag("settings")) {

	    sys_data                    = new t_data();
	    sys_data->cliId             = settings.getValue("cliId",          1);
        sys_data->cliPort           = settings.getValue("cliPort",        15000);
		sys_data->serverIp          = settings.getValue("serverIp",       "127.0.0.1");
		sys_data->serverPort        = settings.getValue("serverPort",     11969);
		sys_data->goLive            = settings.getValue("realTime",       0);
		sys_data->persistence       = settings.getValue("persistence",    1);
		sys_data->logLevel          = settings.getValue("logLevel",       0);
		sys_data->fps               = settings.getValue("fps",            10);
		sys_data->maxPackageSize    = settings.getValue("maxPackageSize", 60000);
        sys_data->allowCompression  = settings.getValue("allowCompression", 1);

        if(settings.pushTag("cameras")) {

		    int totCams = settings.getNumTags("camera");
		    for (int i = 0; i < totCams; i++) {
		        settings.pushTag("camera", i);

                if(camera == NULL) {
		            camera          = new t_camera();
		            currCam         = camera;
		        } else {
                    currCam->sig    = new t_camera();
                    currCam         = currCam->sig;
		        }

                currCam->sig                    = NULL;
                currCam->id                     = settings.getValue("id", 0);
                currCam->deviceInstance         = settings.getValue("deviceInstance", 0);
				currCam->resolutionX            = settings.getValue("resolutionX", 640);
				currCam->resolutionY            = settings.getValue("resolutionY", 480);
				int useComp                     = settings.getValue("useRGBCompression", 0);
				currCam->useCompression         = ((useComp == 0) ? false : true);
				currCam->rgbCompressionQuality  = settings.getValue("RGBCompressionQuality", 90);
				currCam->resolutionDownSample   = settings.getValue("resolutionDownSample", 1.0f);
				currCam->fps                    = settings.getValue("FPS", 24);
				currCam->use2D                  = settings.getValue("use2D", true);
				currCam->use3D                  = settings.getValue("use3D", true);
				currCam->file                   = settings.getValue("file", "");
                currCam->useONI                 = (currCam->file != "");

                if(settings.pushTag("depthSettings")) {
                    currCam->pcDownSample           = settings.getValue("pointsDownSample", 4);
                    settings.popTag();
                }

                currCam->hasCoef2D = false;
                if(settings.pushTag("coefficients2D")) {
                    currCam->hasCoef2D = true;
                    currCam->coef2Da.set(settings.getValue("k1", 1.0f),
                                        settings.getValue("k2", 1.0f),
                                        settings.getValue("k3", 1.0f),
                                        settings.getValue("k4", 1.0f));

                    currCam->coef2Db.set(settings.getValue("fx", 1.0f),
                                        settings.getValue("fy", 1.0f),
                                        settings.getValue("cx", 1.0f),
                                        settings.getValue("cy", 1.0f));
                    settings.popTag();
                }

                if(settings.pushTag("matrix3D")) {
                    currCam->row1.set(settings.getValue("m00", 1.0f), settings.getValue("m01", 1.0f), settings.getValue("m02", 1.0f), settings.getValue("m03", 1.0f));
                    currCam->row2.set(settings.getValue("m10", 1.0f), settings.getValue("m11", 1.0f), settings.getValue("m12", 1.0f), settings.getValue("m13", 1.0f));
                    currCam->row3.set(settings.getValue("m20", 1.0f), settings.getValue("m21", 1.0f), settings.getValue("m22", 1.0f), settings.getValue("m23", 1.0f));
                    currCam->row4.set(settings.getValue("m30", 1.0f), settings.getValue("m31", 1.0f), settings.getValue("m32", 1.0f), settings.getValue("m33", 1.0f));

                    settings.popTag();
                }
                if(settings.pushTag("dataContext")) {
                    settings.popTag();
                }

                if(currCam->useONI) {
                    totalONI ++;
                } else if(currCam->use3D) {
                    total3D ++;
				} else {
				    total2D ++;
				}

				settings.popTag();
			}
			settings.popTag();
		}
		settings.popTag();
	}
}
