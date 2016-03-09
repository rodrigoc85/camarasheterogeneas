#include "modelImg.h"
#include <vector>
#include <limits>
#include <cmath>


using namespace std;
int masterImageIndex = 0;

typedef void (*f_ReadSharedImage)(int* Id, int* Width, int* Height, unsigned char** Pixels);
typedef void (*f_MemoryCheck)(int* Id);

Model_IMG::Model_IMG() {

    char* dllName = "SharedMemory.dll";
    shareImageLibrary =  LoadLibraryA(dllName);
    if (!shareImageLibrary) {
        std::cout << "Failed to load the library" << std::endl;
    }
    primeraVez = true;
}

bool Model_IMG::MemoryLoad() {

    std::stringstream key1;

    key1 << "ImageId" << Id / 10000;

    f_ReadSharedImage readImage = (f_ReadSharedImage)GetProcAddress(shareImageLibrary, "ReadSharedImage");
    int* wPixels = new int;
    int* hPixels = new int;
    *wPixels = 0;
    *hPixels = 0;

    int idAux = Id;
    readImage(&Id, wPixels, hPixels, &pixels);

    if (Id >= 0){
        if (*wPixels > 0 && *hPixels > 0) {
            char* nombre = new char[20];
            sprintf(nombre,"imagen%d.png",Id);

            ofImage image;
            image.setFromPixels(pixels,*wPixels,*hPixels,OF_IMAGE_COLOR);
            //image.saveImage(nombre);

            Width = *wPixels;
            Height = *hPixels;
            delete wPixels;
            delete hPixels;
            unsigned char* pixelsAux;
            pixelsAux = Pixels;
            Pixels = new unsigned char[Width * Height * 3];
            memcpy(Pixels, pixels, sizeof(unsigned char) * Width * Height * 3);
            //delete [] pixelsAux;
            return true;
        }
        else
            return false;
    }
    else{
        Id = idAux;
        return false;
    }
}

bool Model_IMG::MemoryCheck() {
    f_MemoryCheck memoryCheck = (f_MemoryCheck)GetProcAddress(shareImageLibrary, "MemoryCheck");
    int* idAux = new int;
    *idAux = Id;
    memoryCheck(idAux);
    bool newData = *idAux > Id;
    delete idAux;
    return newData;
}

void Model_IMG::Load(string filename) {

    if (primeraVez){
        ofImage auxImg;
        auxImg.loadImage(filename);

        ofBuffer imageBuffer;
        ofSaveImage(auxImg.getPixelsRef(), imageBuffer, OF_IMAGE_FORMAT_JPEG);

        FIMEMORY* stream = FreeImage_OpenMemory((unsigned char*) imageBuffer.getBinaryBuffer(), imageBuffer.size());

        FREE_IMAGE_FORMAT fif   = FreeImage_GetFileTypeFromMemory( stream, 0 );

        FIBITMAP *dib(0);
        dib = FreeImage_LoadFromMemory(fif, stream);

        Pixels = (unsigned char*)FreeImage_GetBits(dib);

        Width = FreeImage_GetWidth(dib);
        Height = FreeImage_GetHeight(dib);
        Id++;
        primeraVez = false;
    }
}

int sizeToPowerTwoLessInt(int n) {
    int res = 1;
    while (res < n) {
        res *= 2;
    }
    return res / 2;
}
int sizeToPowerTwoMoreInt(int n) {
    int res = 1;
    while (res < n) {
        res *= 2;
    }
    return res;
}

int sizeToPowerFourLessInt(int n) {
    int res = n;
    while (res % 4 != 0) {
        res -= 2;
    }
    return res;
}
int sizeToPowerFourMoreInt(int n) {
    int res = n;
    while (res % 4 != 0) {
        res += 2;
    }
    return res;
}
