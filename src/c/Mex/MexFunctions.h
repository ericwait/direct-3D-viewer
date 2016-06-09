#pragma once
#include <concrt.h>
#include <vector>
#include <set>

extern HANDLE mexMessageMutex;
extern HANDLE messageLoopHandle;
extern bool registerExitFunction;
extern std::vector<VolumeTextureObject*> firstVolumeTextures;
extern std::vector<SceneNode*> hullRootNodes;
extern DWORD threadID;


void pollCommand(int nlhs, mxArray** plhs);
void initCommand(int nrhs, const mxArray** prhs);
void closeCommand();
void loadTextureCommand(const mxArray** prhs, int nrhs);
void peelUpdateCommand(int nrhs, const mxArray** prhs);
void textureLightingUpdateCommand(int nrhs, const mxArray** prhs);
void textureAttenUpdateCommand(int nrhs, const mxArray** prhs);
void segmentationLightingCommand(int nrhs, const mxArray** prhs);
void playCommand(int nrhs, const mxArray** prhs);
void rotateCommand(int nrhs, const mxArray** prhs);
void showLabelsCommand(int nrhs, const mxArray** prhs);
void resetViewCommand();
void captureSpinMovieCommand();
void transferUpdateCommand(int nrhs, int nlhs, const mxArray** prhs);
void viewTextureCommand(int nrhs, const mxArray** prhs);
void viewSegmentationCommand(int nrhs, const mxArray** prhs);
void wireframeSegmentationCommand(int nrhs, const mxArray** prhs);
void loadPolygonsCommand(int nrhs, const mxArray** prhs);
void displayPolygonsCommand(int nrhs, const mxArray** prhs);
void setFrameCommand(int nrhs, const mxArray** prhs);
void setViewOriginCommand(int nrhs, const mxArray** prhs);
void updatePolygonsCommand(int nrhs, const mxArray** prhs);
void addPolygonsCommand(int nrhs, const mxArray** prhs);
void setCapturePathCommand(int nrhs, const mxArray** prhs);
void takeControlCommand();
void releaseControlCommand();
void captureImageCommand(int nlhs, int nrhs, const mxArray** prhs, mxArray** plhs);
void removePolygonCommand(int nrhs, const mxArray** prhs);
void deleteAllPolygonsCommand();
void setBackColor(int nrhs, const mxArray** prhs);
extern "C" void exitFunc();


void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
extern "C" void exitFunc();

void setCurrentTexture(GraphicObjectTypes textureType);
void toggleSegmentationResults(bool on);
void toggleSegmentaionWireframe(bool wireframe);
void toggleSegmentaionLighting(bool lighting);
void toggleSelectedCell(std::set<int> labels);
HRESULT updatePolygons(const mxArray* hulls);
HRESULT addPolygons(const mxArray* hulls);

PolygonObject* createPolygonObject(double* faceData, size_t numFaces, double* vertData, size_t numVerts, double* normData, size_t numNormals,
	Camera* camera);
HRESULT loadPolygons(const mxArray* hulls);
HRESULT createBorder(Vec<float> &scale);
HRESULT loadVolumeTexture(unsigned char* image, Vec<size_t> dims, int numChannel, int numFrames, Vec<float> scales, GraphicObjectTypes typ);

HRESULT loadWidget(const mxArray* widget[]);

void startThread(std::string rootDir);
bool checkRenderThread();
void termThread();
void cleanUp();