#include "ofMain.h"
#include "Grabber.h"
#include "testApp.h"
#include "Thread3D.h"
#include "Thread2D.h"
#include "Constants.h"
#include "ofAppGlutWindow.h"
//========================================================================

int main( ) {

    ofAppGlutWindow window;
	ofSetupOpenGL(&window, 800, 600, OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofLogToFile("client_log.txt", false);
	ofRunApp( new Grabber() );
	//ofRunApp( new testApp() );
}
