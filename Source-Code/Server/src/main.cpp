#include <stdio.h>
#include <locale.h>
#include <time.h>
#include <string>
#include <iostream>

#include <sstream>

#include "ofMain.h"
#include "ofxOpenNI.h"
#include "XnCppWrapper.h"
#include "Server.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main( ) {

    ofAppGlutWindow window;
	ofSetupOpenGL(&window, 800, 600, OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( new Server());

}
