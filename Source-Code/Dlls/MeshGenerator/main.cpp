#include "main.h"

void DLL_EXPORT meshGenerate(PointsCloud* nbIN, FaceStruct** faces, int* numberFaces, int nroFrame)
{
        MasterPly* mply = new MasterPly();

        mply->loadMesh(nbIN->x,nbIN->y,nbIN->z,nbIN->length);//Load Mesh in VCG Lib structure

        mply->poissonDiskSampling(25000);//Apply Poisson Disk Sampling to reduce the number of points

        mply->buildMeshBallPivoting();//Apply Ball Pivoting to build the mesh

        mply->laplacianSmooth(3);//Apply Laplacian Smooth to smoothe the mesh

        ///Load the outputs
        *numberFaces = mply->totalFaces();
        *faces = mply->getFaces();

        delete mply;
}

void DLL_EXPORT meshGenerateToCaliper(PointsCloud* nbIN, FaceStruct** faces, int* numberFaces)
{
        MasterPly* mply = new MasterPly();

        mply->loadMesh(nbIN->x,nbIN->y,nbIN->z,nbIN->length);//Load Mesh in VCG Lib structure

        mply->poissonDiskSampling(50000);//Apply Poisson Disk Sampling to reduce the number of points

        mply->buildMeshBallPivoting();//Apply Ball Pivoting to build the mesh

        mply->laplacianSmooth(3);//Apply Laplacian Smooth to smoothe the mesh

        ///Load the outputs
        *numberFaces = mply->totalFaces();
        *faces = mply->getFaces();

        ///Save the mesh in disk
        mply->savePLY("joinMesh.ply",true);

        delete mply;
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
