#include "masterPly.h"



/*void CSurface(const char input, const char output)
{
	thread t(Sampling,input,output);
	t.join();
}*/

void MasterPly::Sampling (char* input, char* output) {

	MyMesh m;
	MyMesh subM;
	float rad = 0.f;
	if(tri::io::ImporterPLY<MyMesh>::Open(m,input)!=0)
	{
		cout << "Error reading file  %s\n" << endl;
		return;
	}

	MyMesh vcgMesh;
    int verticeCount;
    int triangleCount;


    vcgMesh.Clear();
    verticeCount=m.VN();
    vcg::tri::Allocator<MyMesh>::AddVertices(vcgMesh,verticeCount);
    for(int i=0;i<verticeCount;i++){
        vcgMesh.vert[i].P()=vcg::Point3f(m.vert[i].P()[0],m.vert[i].P()[1],m.vert[i].P()[2]);

    }


    tri::UpdateBounding<MyMesh>::Box(vcgMesh);
	// calculate radius
	rad = tri::SurfaceSampling<MyMesh,tri::TrivialSampler<MyMesh> >::ComputePoissonDiskRadius(vcgMesh, 100000);
	tri::SurfaceSampling<MyMesh,tri::TrivialSampler<MyMesh> >::SamplingRandomGenerator().initialize((unsigned int)time(0));

	//sample point cloud
	std::vector<Point3f> sampleVec;
	tri::TrivialSampler<MyMesh> mps(sampleVec);

	// sampling
	tri::SurfaceSampling<MyMesh,tri::TrivialSampler<MyMesh> >::PoissonDiskParam pp;
	tri::SurfaceSampling<MyMesh,tri::TrivialSampler<MyMesh> >::PoissonDiskParam::Stat pds;
	pp.preGenMesh = &subM;
	//pp.pds=&pds;
	pp.bestSampleChoiceFlag=false;

	// start poisson disk prunning
	sampleVec.clear();
	tri::SurfaceSampling<MyMesh,tri::TrivialSampler<MyMesh> >::PoissonDiskPruning(mps, vcgMesh, rad, pp);
	tri::Build(subM,sampleVec);

	// sample
	std::stringstream sample;
	sample << "sampled_" << output;

	//Build surface
	vcg::tri::UpdateBounding<MyMesh>::Box(subM);
	vcg::tri::UpdateNormal<MyMesh>::PerVertex(subM);

	//Initialization
	tri::BallPivoting<MyMesh> pivot(subM);

	//the main processing
	pivot.BuildMesh();

	tri::io::ExporterPLY<MyMesh>::Save(subM,output,true);

}

void MasterPly::loadPLY(char* fileName){
	if(tri::io::ImporterPLY<MyMesh>::Open(m,fileName)!=0)
	{
		cout << "Error reading file  %s\n" << endl;
		return;
	}
}

void MasterPly::loadMesh(float* pointsX, float* pointsY, float* pointsZ, int pointsCount){

    m.Clear();
    vcg::tri::Allocator<MyMesh>::AddVertices(m,pointsCount);
    for(int i=0;i<pointsCount;i++){
        m.vert[i].P()=vcg::Point3f(pointsX[i],pointsY[i],pointsZ[i]);

    }
    tri::UpdateBounding<MyMesh>::Box(m);
}

void MasterPly::poissonDiskSampling(int sampleNum){

	float rad = 0.f;
    rad = tri::SurfaceSampling<MyMesh,tri::TrivialSampler<MyMesh> >::ComputePoissonDiskRadius(m, sampleNum);
	tri::SurfaceSampling<MyMesh,tri::TrivialSampler<MyMesh> >::SamplingRandomGenerator().initialize((unsigned int)time(0));

	//sample point cloud
	std::vector<Point3f> sampleVec;
	tri::TrivialSampler<MyMesh> mps(sampleVec);

	// sampling
	//cout << "Subsampling a PointCloud" << endl;
	tri::SurfaceSampling<MyMesh,tri::TrivialSampler<MyMesh> >::PoissonDiskParam pp;
	tri::SurfaceSampling<MyMesh,tri::TrivialSampler<MyMesh> >::PoissonDiskParam::Stat pds;
	pp.preGenMesh = &subM;
	//pp.pds=&pds;
	pp.bestSampleChoiceFlag=false;

	// start poisson disk prunning
	sampleVec.clear();
	tri::SurfaceSampling<MyMesh,tri::TrivialSampler<MyMesh> >::PoissonDiskPruning(mps, m, rad, pp);
	tri::Build(subM,sampleVec);

	vcg::tri::UpdateBounding<MyMesh>::Box(subM);
	//m.Clear();
}

void MasterPly::calculateNormalsVertex(){
    vcg::tri::UpdateNormal<MyMesh>::PerVertex(subM);
}

void MasterPly::buildMeshBallPivoting(){
	tri::BallPivoting<MyMesh> pivot(subM);
	pivot.BuildMesh();
}

void MasterPly::savePLY(char* fileName, bool binary){
    tri::io::ExporterPLY<MyMesh>::Save(subM,fileName,binary);
}

int MasterPly::totalFaces(){
    return subM.FN();
}

int MasterPly::totalVertex(){
    return subM.VN();
}

FaceStruct MasterPly::getFace(int i){
    FaceStruct f;

    f.p1[0] = m.face[i].V(0)->P()[0];
    f.p1[1] = m.face[i].V(0)->P()[1];
    f.p1[2] = m.face[i].V(0)->P()[2];

    f.p2[0] = m.face[i].V(1)->P()[0];
    f.p2[1] = m.face[i].V(1)->P()[1];
    f.p2[2] = m.face[i].V(1)->P()[2];

    f.p3[0] = m.face[i].V(2)->P()[0];
    f.p3[1] = m.face[i].V(2)->P()[1];
    f.p3[2] = m.face[i].V(2)->P()[2];

    return f;
}


FaceStruct* MasterPly::getFaces(){
    FaceStruct* f = new FaceStruct[subM.FN()];
    for (int i = 0; i< subM.FN(); i++){
        f[i].p1[0] = subM.face[i].V(0)->P()[0];
        f[i].p1[1] = subM.face[i].V(0)->P()[1];
        f[i].p1[2] = subM.face[i].V(0)->P()[2];

        f[i].p2[0] = subM.face[i].V(1)->P()[0];
        f[i].p2[1] = subM.face[i].V(1)->P()[1];
        f[i].p2[2] = subM.face[i].V(1)->P()[2];

        f[i].p3[0] = subM.face[i].V(2)->P()[0];
        f[i].p3[1] = subM.face[i].V(2)->P()[1];
        f[i].p3[2] = subM.face[i].V(2)->P()[2];
    }
    return f;
}

VertexStruct MasterPly::getFaceVertex(int faceNumber, int vertexNumber){
    VertexStruct v;

    if (vertexNumber < 3){
        v.v[0] = m.face[faceNumber].V(vertexNumber)->P()[0];
        v.v[1] = m.face[faceNumber].V(vertexNumber)->P()[1];
        v.v[2] = m.face[faceNumber].V(vertexNumber)->P()[2];
    }
    else
       cout << "Vertex doesn't exist.\n" << endl;

    return v;
}

void MasterPly::laplacianSmooth(int step){
    tri::Smooth<MyMesh>::VertexCoordLaplacian(subM, step);
}

MasterPly::MasterPly()
{
    //ctor
}

MasterPly::~MasterPly()
{
    //ctor
}

