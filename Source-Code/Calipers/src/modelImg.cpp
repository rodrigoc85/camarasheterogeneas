#include "modelImg.h"
#include "FreeImage.h"
#include <vector>
#include <limits>
#include <cmath>


using namespace std;


Model_IMG::Model_IMG() {
    Id = 0;
}

void Model_IMG::MemoryLoad() {

    memoryMappedImageId.setup("ImageId", sizeof(int), false);
    isConnectedId = memoryMappedImageId.connect();
    if (isConnectedId) {
        id = memoryMappedImageId.getData();
    }
    memoryMappedImageSizeW.setup("ImagePixelsW", sizeof(int), false);
    isConnectedWPixels = memoryMappedImageSizeW.connect();
    if (isConnectedWPixels) {
        wPixels = memoryMappedImageSizeW.getData();
    }
    memoryMappedImageSizeH.setup("ImagePixelsH", sizeof(int), false);
    isConnectedHPixels = memoryMappedImageSizeH.connect();
    if (isConnectedHPixels) {
        hPixels = memoryMappedImageSizeH.getData();
    }
    memoryMappedImage.setup("ImagePixels", sizeof(char) * (*wPixels) * (*hPixels) * 3, false);
    isConnectedPixels = memoryMappedImage.connect();
    if (isConnectedPixels) {
        pixels = memoryMappedImage.getData();
    }

    if (isConnectedId && isConnectedWPixels && isConnectedHPixels && isConnectedPixels &&
        *id > 0 && *id != Id) {

        Id = *id;
        Width = *wPixels;
        Height = *hPixels;
        Pixels = new char[Width * Height * 3];
        memcpy(Pixels, pixels, sizeof(char) * Width * Height * 3);
    }
}

void Model_IMG::Load(string filename) {

    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
    FIBITMAP *dib(0);
    GLuint gl_texID;

    fif = FreeImage_GetFileType(filename.c_str(), 0);
    if(fif == FIF_UNKNOWN)
        fif = FreeImage_GetFIFFromFilename(filename.c_str());
    if(fif == FIF_UNKNOWN)
        cout << "The image does not have the correct format." << endl;
    if(FreeImage_FIFSupportsReading(fif))
        dib = FreeImage_Load(fif, filename.c_str());
    if(!dib)
        cout << "Failed to load the image." << endl;

    Pixels = (char*)FreeImage_GetBits(dib);
    Width = FreeImage_GetWidth(dib);
    Height = FreeImage_GetHeight(dib);
    if((Pixels == 0) || (Width == 0) || (Height == 0))
        cout << "The image is corrupted." << endl;
    Id++;
}
