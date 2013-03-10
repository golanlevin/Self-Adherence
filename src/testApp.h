#pragma once

/* 
 
 Required addons and other libraries:
 ofxFBOTexture
 ipp
 ofxOpenCV
 ofxDirList
 ofxXmlSettings
 ofxControlPanel
 ANN
 perlin
 triangle++
 
 */



#include "ofMain.h"

#define B_COMMAND_LINE_DRIVEN	false  

#include "ofxOpenCv.h"
#include "ofxCvContourFinder.h"
 
#include "IppLoader.h"
#include "ofxControlPanel.h"
#include "ofxFBOTexture.h"

#include "RunningStat.h"
#include "tmVisThresholderC1.h"
#include "perlin.h"

//----------------------
// For Delaunay/Triangle++
#include <vector>
#include <iostream>
#include <fstream>
#include <del_interface.hpp>


#include "ANN.h"

using namespace tpp;

struct Particle {
    float px;
	float py; 
	float vx;
	float vy;
	
    int binx;
	int biny;
};


class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void update2();
		void draw();
		void draw2();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
	
	
		IppLoader *IPPL;
		ofxControlPanel panel;
		string xmlSettingsFileName;
		ofxCvContourFinder 	contourFinder;
	
		std::vector<string> requestedOutputFileNames;
		int  currentOutputFilenameIndex;
	
	
		ofShader shaderH;
		ofShader shaderV;
		ofShader shaderThresh;
		bool doShader;

		int maxNParticles;
		int nParticles;
		int particleStep32f;
		int particleStep16u;
		int particleStep32s;
		int particleStep8u;
		ofRectangle bounds;
	
		int nDesiredBinBits;
		int maxOrderX;
		int maxOrderY;
	
		float pixelsPerBinX; 
		float pixelsPerBinY;
		int nBinsX;
		int nBinsY; 
		int nBins;
	
	vector<int> intVec;
	vector<int> *intVecArray1D;
	vector<int> **intVecArray2D;
	vector<int> idVec;

		Ipp32f	*positionx;
		Ipp32f	*positiony;
		Ipp32f	*velocityx;
		Ipp32f	*velocityy;
		bool	*pointValid;
	
		Ipp32s	*kNeighbors;
		Ipp32f	*kDistances;
	
		Ipp8u	*positionx8u; 
		Ipp8u	*positiony8u;
		Ipp16u  *positionx16u;
		Ipp16u	*positiony16u;
		Ipp32f	*positionx32f;
		Ipp32f	*positiony32f; // copy
	
	//----------------------
	void drawDelaunay();
	Delaunay *delobject;
	vector< Delaunay::Point > DPVEC;
	//----------------------
	
	ANNpointArray	dataPts;	// data points
	ANNpoint		queryPt;	// query point
	ANNidxArray		nnIdx;		// near neighbor indices
	ANNdistArray	dists;		// near neighbor distances
	ANNkd_tree*		kdTree;		// search structure
	int				maxNearestNeighborsK;
	int				nNearestNeighborsK;
	int				nMaxPointsToVisit;
	int				nDimensions;
	int				nBucketSize;
	float			searchError;
	vector<int>		*idsOfNearestNeighbors;
	
	bool bDoDrawDiagnostic;
	bool bPause;
	bool bInitialized;
	bool bFullScreen; 
	
	int  nMedianPassesForThisRound; 
	
	void allocateMemory();
	void initialize(bool bSameImageAsBefore);
	void updateNearestNeighbors();	// in testAppMath.cpp
	void updateParticles();			// in testAppMath.cpp
	void applyImageForces();
	void addPositionsProbabilisticallyBasedOnSourceImage (int nToAdd);
	float getSourceImageLuminance01 (float rx, float ry);
	
	void addMiddlePoint (float ax, float ay, float bx, float by);
	void addIntermediatePoint (float ax, float ay, float bx, float by, float frac);
	
	int	 nIterations;
	int  iterationCount;
	long iterationStartTime;
	long iterationElapsedTime;
	float averageInterParticleDistance;
	float stdevInterParticleDistance;
	void drawParticles();
	
	RunningStat RS;
	
	ofColor	getColorFromFrac (float frac01);
	void	drawColorBand();
	ofColor HSL2RGB(double h, double sl, double  l);
	
	//-----------------------------
	ofxFBOTexture myFBO;
	
	ofxFBOTexture fbo1;
	ofxFBOTexture fbo2;
	
	
	
	ofImage *myImage;
	void drawIntoFboBogus();
	void drawLinesIntoFbo(); 
	void displayFBODiagnostic();
	void saveFBO();
	void prepareOutputBuffer();
	int myFBOWidth;
	int myFBOHeight;
	int myFBODisplayWidth; 
	int myFBODisplayHeight; 
	bool bActuallyWriteFboImageFiles;
	
	int outputW;
	int outputH;
	int outputN;
	unsigned char *outputPixels;
	char *handyChars;
	
	int	ippFboPixelW;
	int	ippFboPixelH;
	int	ippFboPixel1S;
	int	ippFboPixel3S;
	Ipp8u	*ippFboPixelsC1;
	Ipp8u	*ippFboPixelsC1Tmp1;
	Ipp8u	*ippFboPixelsC1Tmp2;
	Ipp8u	*ippFboPixelsC3;
	Ipp8u	*ippFboPixelsC3Tmp1;
	void	addPointsOnTooLongConnections();
	
	int nLutLevels;
	Ipp32s* pLutValues; 
	Ipp32s* pLutLevels;
	
	//-----------------------------
	void	loadImage (bool bSameImageAsBefore);
	tmVisThresholderC1 *thresholder;
	int		whichCurrentImage;
	
	ofImage	baseImageRGB;
	ofImage baseImageGray;
	ofImage	baseImageGradientC3;
	ofImage baseImageBigC3;
	ofxCvGrayscaleImage ippFboImageC1;
	ofxCvGrayscaleImage finalGrayscaleLineImage;
	bool bFoundContours;
	bool bPreppedOutputImage;
	
	bool bCycleAutomatically;
	int  totalCycleDurationInIterations;
	
	ofxCvColorImage     ippFboImageC3;
	ofxCvGrayscaleImage	cvImageC1R;
	ofxCvGrayscaleImage	cvImageC1G;
	ofxCvGrayscaleImage	cvImageC1B;
	
	Perlin	*perlin;
	ofImage	noiseImageSmall;
	Ipp8u	*noisePixelsSmall_C1;
	void	allocateNoiseImage();
	void	generateNoiseImage();
	void	displayNoiseImage();
	float	doubleExponentialSigmoid (float x, float a);
	float	doubleExponentialSeat (float x, float a);
	float	logisticSigmoid (float x, float a);
	
	Ipp8u	*baseImageGrayscalePixels;
	Ipp8u	*baseImageGrayscaleTmp1;
	Ipp8u	*baseImageGrayscaleTmp2;
	Ipp8u	*baseImageGrayscaleTmp3;
	Ipp8u	*baseImageGradientPixelsC3;
	Ipp8u	*baseImageBigPixelsC3;
	Ipp8u	*baseImageRGBPixels;
	Ipp16s  *base16sHoriz;
	Ipp16s  *base16sVert;
	
	int		baseImageW; 
	int		baseImageH;
	int		baseImage1S;
	int		baseImage3S;
	int		baseImage16s; 
	
	int		baseImage3N;
	int		baseImage1N; 
	
	void huntForBlendFunc(int period, int defaultSid, int defaultDid);
	
	
	float colorFadeOutTimingPercent; 
	float colorFadeOutTimingBias; 
	int   colorFadeInEndIteration; 
		
};
