#include "main.h"

ofxSharedMemory<int*> memoryMappedMeshId;
ofxSharedMemory<int*> memoryMappedMeshSize;
ofxSharedMemory<FaceStruct*> memoryMappedMesh;

bool isConnectedMeshId;
bool isConnectedMeshSize;
bool isConnectedMesh;

std::stringstream MeshIdKey;
std::stringstream MeshSizeKey;
std::stringstream MeshKey;


//bool firstTimeMesh = true;

MemoryMeshStruct* meshMemory = new MemoryMeshStruct[100];
int meshIndexMemory = 0;

void DLL_EXPORT ShareMeshSetup(int* meshId, int* index) {

    *index = meshIndexMemory;

    meshIndexMemory++;

    meshMemory[*index].maxNumberFaces = 0;

    meshMemory[*index].firstTimeMesh = true;

    meshMemory[*index].MeshIdKey << "MeshId" << *meshId / 10000;

    meshMemory[*index].MeshSizeKey << "MeshSize" << *meshId / 10000;

    meshMemory[*index].MeshKey << "Mesh" << *meshId / 10000;

	meshMemory[*index].memoryMappedMeshId.setup(meshMemory[*index].MeshIdKey.str(), sizeof(int), true);
    isConnectedMeshId = meshMemory[*index].memoryMappedMeshId.connect();

	meshMemory[*index].memoryMappedMeshSize.setup(meshMemory[*index].MeshSizeKey.str(), sizeof(int), true);
    isConnectedMeshSize = meshMemory[*index].memoryMappedMeshSize.connect();

    //memoryMappedMesh.setup(MeshKey.str(), sizeof(FaceStruct) * maxNumberFaces, true);
    //isConnectedMesh = memoryMappedMesh.connect();

}

void DLL_EXPORT ReadSharedMeshSetup(int* meshId, int* index) {

    meshIndexMemory++;

    *index = meshIndexMemory;

    meshMemory[*index].maxNumberFaces = 0;

    meshMemory[*index].firstTimeMesh = true;

    meshMemory[*index].MeshIdKey << "MeshId" << *meshId / 10000;

    meshMemory[*index].MeshSizeKey << "MeshSize" << *meshId / 10000;

    meshMemory[*index].MeshKey << "Mesh" << *meshId / 10000;

	meshMemory[*index].memoryMappedMeshId.setup(meshMemory[*index].MeshIdKey.str(), sizeof(int), false);
    isConnectedMeshId = meshMemory[*index].memoryMappedMeshId.connect();

	meshMemory[*index].memoryMappedMeshSize.setup(meshMemory[*index].MeshSizeKey.str(), sizeof(int), false);
    isConnectedMeshSize = meshMemory[*index].memoryMappedMeshSize.connect();

}

void DLL_EXPORT ShareMesh( int* meshId, int* numberFaces, FaceStruct* faces, int* index) {

    meshMemory[*index].memoryMappedMeshId.setData(meshId);

    /*if (*maxNumberFaces < *numberFaces){
        cout << (*numberFaces * 10) << endl;
        *maxNumberFaces = *numberFaces * 10;
        memoryMappedMesh.setup(MeshKey.str(), sizeof(FaceStruct) * 2 ***** (*maxNumberFaces), true);
        isConnectedMesh = memoryMappedMesh.connect();
    }*/
    //meshMemory[*index].maxNumberFaces = numberFaces;
    meshMemory[*index].memoryMappedMesh.setup(meshMemory[*index].MeshKey.str(), sizeof(FaceStruct) * (*numberFaces), true);
    isConnectedMesh = meshMemory[*index].memoryMappedMesh.connect();

    cout << "x1" << endl;
    if (*numberFaces > 1) {
        cout << faces[0].p1[0] << " - " << faces[0].p1[1] << " - " << faces[0].p1[2] << " - ";
        cout << faces[1].p1[0] << " - " << faces[1].p1[1] << " - " << faces[1].p1[2] << endl;

    } else {
        cout << faces[0].p1[0] << " - " << faces[0].p1[1] << " - " << faces[0].p1[2] << endl;

    }
    cout << "x2" << endl;

    meshMemory[*index].memoryMappedMesh.setData(faces);

    //int * pepe = new int;
    //*pepe = 2;
    //memoryMappedMeshSize.setData(pepe/*maxNumberFaces*/);
    meshMemory[*index].memoryMappedMeshSize.setData(numberFaces);

}

void DLL_EXPORT ReadSharedMesh(int* meshId, int* numberFaces, FaceStruct** faces, int* index) {

    *meshId = *(meshMemory[*index].memoryMappedMeshId).getData();

    *numberFaces = *(meshMemory[*index].memoryMappedMeshSize).getData();

    if (meshMemory[*index].maxNumberFaces != *numberFaces){//meshMemory[*index].maxNumberFaces < *numberFaces
        meshMemory[*index].maxNumberFaces = *numberFaces;
        //meshMemory[*index].maxNumberFaces = *numberFaces * 10;
        meshMemory[*index].memoryMappedMesh.setup(meshMemory[*index].MeshKey.str(), sizeof(FaceStruct) * (*numberFaces), false);
        isConnectedMesh = meshMemory[*index].memoryMappedMesh.connect();
    }

    *faces = meshMemory[*index].memoryMappedMesh.getData();

    cout << "x1" << endl;
    //cout << faces << endl;
    //cout << *faces << endl;
    //cout << *(*faces) << endl;
    //cout << (*faces)[0].p1[0] << endl;//faces[0][0]
    //cout << (*faces[0]).p1[0] << " - " << (*faces[0]).p1[1] << " - " << (*faces[0]).p1[2] << " - ";
    //cout << (*faces[1]).p1[0] << " + " << (*faces[1]).p1[1] << " - " << (*faces[1]).p1[2] << endl;
    cout << "x2" << endl;
}

ofxSharedMemory<int*> memoryMappedImageId;
ofxSharedMemory<int*> memoryMappedImageSizeH;
ofxSharedMemory<int*> memoryMappedImageSizeW;
ofxSharedMemory<unsigned char*> memoryMappedImagePixels;

bool isConnectedImageId;
bool isConnectedImageSizeH;
bool isConnectedImageSizeW;
bool isConnectedImagePixels;

std::stringstream ImageIdKey;
std::stringstream ImageSizeHKey;
std::stringstream ImageSizeWKey;
std::stringstream ImagePixelsKey;

//bool firstTimeImage = true;

MemoryImageStruct* imagesMemory = new MemoryImageStruct[100];
int imageIndexMemory = 0;

void DLL_EXPORT ShareImageSetup(int* imageId, int* wPixels, int* hPixels, int* index) {

    *index = imageIndexMemory;

    imageIndexMemory++;

    imagesMemory[*index].ImageIdKey << "ImageId" << *imageId / 10000;

    imagesMemory[*index].ImageSizeHKey << "ImageSizeH" << *imageId / 10000;

    imagesMemory[*index].ImageSizeWKey << "ImageSizeW" << *imageId / 10000;

    imagesMemory[*index].ImagePixelsKey << "ImagePixels" << *imageId / 10000;

    imagesMemory[*index].firstTimeImage = true;

	imagesMemory[*index].memoryMappedImageId.setup(imagesMemory[*index].ImageIdKey.str(), sizeof(int), true);
    isConnectedImageId = imagesMemory[*index].memoryMappedImageId.connect();

	imagesMemory[*index].memoryMappedImageSizeH.setup(imagesMemory[*index].ImageSizeHKey.str(), sizeof(int), true);
    isConnectedImageSizeH = imagesMemory[*index].memoryMappedImageSizeH.connect();

	imagesMemory[*index].memoryMappedImageSizeW.setup(imagesMemory[*index].ImageSizeWKey.str(), sizeof(int), true);
    isConnectedImageSizeW = imagesMemory[*index].memoryMappedImageSizeW.connect();

    imagesMemory[*index].memoryMappedImagePixels.setup(imagesMemory[*index].ImagePixelsKey.str(), (*wPixels) * (*hPixels) * 3, true);
    isConnectedImagePixels = imagesMemory[*index].memoryMappedImagePixels.connect();

    imagesMemory[*index].memoryMappedImageSizeH.setData(hPixels);

    imagesMemory[*index].memoryMappedImageSizeW.setData(wPixels);
}

void DLL_EXPORT ReadSharedImageSetup(int* imageId, int* index) {

    imageIndexMemory++;

    *index = imageIndexMemory;

    imagesMemory[*index].ImageIdKey << "ImageId" << *imageId / 10000;

    imagesMemory[*index].ImageSizeHKey << "ImageSizeH" << *imageId / 10000;

    imagesMemory[*index].ImageSizeWKey << "ImageSizeW" << *imageId / 10000;

    imagesMemory[*index].ImagePixelsKey << "ImagePixels" << *imageId / 10000;

    imagesMemory[*index].firstTimeImage = true;

	imagesMemory[*index].memoryMappedImageId.setup(imagesMemory[*index].ImageIdKey.str(), sizeof(int), false);
    isConnectedImageId = imagesMemory[*index].memoryMappedImageId.connect();

	imagesMemory[*index].memoryMappedImageSizeH.setup(imagesMemory[*index].ImageSizeHKey.str(), sizeof(int), false);
    isConnectedImageSizeH = imagesMemory[*index].memoryMappedImageSizeH.connect();

	imagesMemory[*index].memoryMappedImageSizeW.setup(imagesMemory[*index].ImageSizeWKey.str(), sizeof(int), false);
    isConnectedImageSizeW = imagesMemory[*index].memoryMappedImageSizeW.connect();

}

void DLL_EXPORT ShareImage(int* imageId, unsigned char* pixels, int* index) {

    imagesMemory[*index].memoryMappedImageId.setData(imageId);

    imagesMemory[*index].memoryMappedImagePixels.setData(pixels);
}

void DLL_EXPORT ReadSharedImage(int* Id, int* wPixels, int* hPixels , unsigned char** pixels, int* index) {

    *Id = *(imagesMemory[*index].memoryMappedImageId).getData();

    *hPixels = *(imagesMemory[*index].memoryMappedImageSizeH).getData();

    *wPixels = *(imagesMemory[*index].memoryMappedImageSizeW).getData();

    if (imagesMemory[*index].firstTimeImage){
    //cout << "Pasodll1 - "<< ImagePixelsKey.str()<< "++++" << endl;
        imagesMemory[*index].memoryMappedImagePixels.setup(imagesMemory[*index].ImagePixelsKey.str(), (*wPixels) * (*hPixels) * 3, false);
        isConnectedImagePixels = imagesMemory[*index].memoryMappedImagePixels.connect();
        imagesMemory[*index].firstTimeImage = false;
    //cout << "Pasodll2 - " << (isConnectedImagePixels ? "T" : "F") << ImagePixelsKey.str() << endl;
    }

    *pixels = imagesMemory[*index].memoryMappedImagePixels.getData();

    //cout << "Pasodll5"<< endl;
}

extern "C" DLL_EXPORT BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
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
