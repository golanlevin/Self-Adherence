/*
 *  testAppImaging.cpp
 *  emptyExample
 *
 *  Created by Golan Levin on 7/26/10.
 *  Copyright 2010 Carnegie Mellon University. All rights reserved.
 *
 */

#include "testApp.h"

//--------------------------------------------------------------
void testApp::allocateNoiseImage(){
	noiseImageSmall.allocate (192, 128, OF_IMAGE_GRAYSCALE);
	noisePixelsSmall_C1 = ippiMalloc_8u_C1(192, 168, &baseImage1S);
	
	int   octaves    = 3;	// The first parameter 1-16 is the number of octaves. Recommended: 4-8.
	float frequency  = 4.0; // The second parameter is the noise frequency. Values betwen 1 and 8 are reasonable here. 
	float amplitude  = 1.0; // Amplitude; a value of 1 will return randomized samples between -1 and +1. 
	int   randomSeed = (int)(ofRandom(0,1000000.0));  // The last parameter is the random number seed.
	perlin = new Perlin	(octaves,frequency,amplitude,randomSeed);
}

//--------------------------------------------------------------
void testApp::generateNoiseImage(){
	
	int randomSeed = (int)(ofRandom(0,1000000.0));
	perlin->setSeed (randomSeed);
	
	
	float yoffset1 = ofRandom(0,1);
	float xoffset1 = ofRandom(0,1);
	float xscale1  = 0.005; //0.0015; 
	float yscale1  = xscale1 * 1.5;
	
	float yoffset2 = ofRandom(0,1);
	float xoffset2 = ofRandom(0,1);
	float xscale2  = 0.0012;
	float yscale2  = xscale2 * 1.25;
	
	float yoffset3 = ofRandom(0,1);
	float xoffset3 = ofRandom(0,1);
	float xscale3  = 0.005; //0.0015; 
	float yscale3  = xscale3 * 1.66;
	
	
	for (int y=0; y<128; y++){
		for (int x=0; x<192; x++){
			int index = y*baseImage1S + x;
			
			
			float yf1 = yoffset1 + y*yscale1; 
			float xf1 = xoffset1 + x*xscale1 * yf1;  // bends here
			float noiseValf1 = perlin->Get(xf1, yf1); 
			noiseValf1 = 0.5 + 0.5*noiseValf1;
			noiseValf1 = powf(noiseValf1, 0.70); 
			noiseValf1 = logisticSigmoid(noiseValf1, 0.90);
			
			float yf2 = yoffset2 + y*yscale2; 
			float xf2 = xoffset2 + x*xscale2; 
			float noiseValf2 = perlin->Get(xf2, yf2);
			noiseValf2 = 0.5 + 0.5*noiseValf2;
			noiseValf2 = doubleExponentialSigmoid (noiseValf2, 0.86);
			noiseValf2 = powf(noiseValf2, 0.33); 
			
			
			float noiseVal = 255.0 * (0.5*noiseValf1 + 0.5*noiseValf2); 
			Ipp8u noiseVal8u = (Ipp8u)(round(noiseVal));
			noisePixelsSmall_C1[index] = noiseVal8u;
		}
	}
	
	noiseImageSmall.setFromPixels(noisePixelsSmall_C1, 192, 128, OF_IMAGE_GRAYSCALE);
}



//--------------------------------------------------------------
void testApp::displayNoiseImage(){
	noiseImageSmall.draw(300, myFBODisplayHeight+10+128, 196, -128); 
}


//--------------------------------------------------------------
void testApp::loadImage (bool bSameImageAsBefore){
	
	IppiSize baseImageRoi = {baseImageW, baseImageH};
	
	bool bUseGeneratedNoiseImage = true;
	if (bUseGeneratedNoiseImage){
		
		if (bSameImageAsBefore == false){
			generateNoiseImage();
		}
		ippiCopy_8u_C1R(noisePixelsSmall_C1, baseImage1S, baseImageGrayscalePixels, baseImage1S, baseImageRoi);
		
	} else {
		
		int whichImageId = whichCurrentImage;
		if (bSameImageAsBefore == false){
			whichImageId = (int) ofRandom(0, 106);
		}
		
		char filenameChars[64];
		sprintf(filenameChars, "base_images/thumb_%.3d.png", whichImageId); //
		printf("Loading %s\n", filenameChars); 
		baseImageRGB.loadImage(filenameChars);
		
		unsigned char *loadedPixelsRGB = baseImageRGB.getPixels();
		int indexS = 0; 
		int indexD = 0; 
		for (int y=0; y<baseImageH; y++){
			for (int x=0; x<baseImageW; x++){
				baseImageRGBPixels[indexD++] = loadedPixelsRGB[indexS++]; 
				baseImageRGBPixels[indexD++] = loadedPixelsRGB[indexS++]; 
				baseImageRGBPixels[indexD++] = loadedPixelsRGB[indexS++]; 
				indexS++;
			}
		}
		
		// compute grayscale version
		ippiRGBToGray_8u_C3C1R (baseImageRGBPixels,		  baseImage3S,
								baseImageGrayscalePixels, baseImage1S, baseImageRoi);
		ippiMirror_8u_C1IR	   (baseImageGrayscalePixels, baseImage1S, baseImageRoi, ippAxsHorizontal);
	}
	
	
	//--------------------------
	// blur grayscale image some number of times.
	
	int nSourceBlurs = panel.getValueI("N_SOURCE_BLURS");
	for (int i=0; i<nSourceBlurs; i++){
		bool bBlackEdges = false;
		if (bBlackEdges){
			IppiSize oneRowRoi = {baseImageW, 1};
			ippiSet_8u_C1R(0, baseImageGrayscalePixels, baseImage1S, oneRowRoi);
			ippiSet_8u_C1R(0, baseImageGrayscalePixels+baseImage1S*(baseImageH-1), baseImage1S, oneRowRoi);
		}
		
		int blurOffset     = 0;
		IppiSize blurRoi   = {baseImageW, baseImageH};
		ippiFilterGauss_8u_C1R (baseImageGrayscalePixels+blurOffset, baseImage1S,
								baseImageGrayscaleTmp1  +blurOffset, baseImage1S, blurRoi, ippMskSize5x5);
		ippiFilterGauss_8u_C1R (baseImageGrayscaleTmp1  +blurOffset, baseImage1S,
								baseImageGrayscalePixels+blurOffset, baseImage1S, blurRoi, ippMskSize5x5);
	}
	
	//--------------------------
	// compute histogram equalized version.
	thresholder->modImageByHistogram(baseImageGrayscalePixels, baseImageGrayscaleTmp1, 1, 1.0);
	ippiCopy_8u_C1R(baseImageGrayscaleTmp1, baseImage1S, 
					baseImageGrayscalePixels,  baseImage1S, baseImageRoi);
	baseImageGray.setFromPixels(baseImageGrayscalePixels, baseImageW, baseImageH, OF_IMAGE_GRAYSCALE);
	
	//--------------------------
	// compute gradients.
	
	IppiSize sobelRoi  = {baseImageW-2, baseImageH-2};
	int sobelOffset8u  = baseImage1S  +1;
	int sobelOffset16s = baseImage16s +1;
	
	ippiFilterSobelHoriz_8u16s_C1R (baseImageGrayscalePixels+sobelOffset8u,    baseImage1S, 
									base16sHoriz+sobelOffset16s,			   baseImage16s, sobelRoi, ippMskSize5x5);
	ippiFilterSobelVert_8u16s_C1R  (baseImageGrayscalePixels+sobelOffset8u,    baseImage1S, 
									base16sVert +sobelOffset16s,			   baseImage16s, sobelRoi, ippMskSize5x5);
	
	ippiDivC_16s_C1IRSfs (48, base16sHoriz,   baseImage16s, baseImageRoi, 0); //48
	ippiDivC_16s_C1IRSfs (48, base16sVert ,   baseImage16s, baseImageRoi, 0);
	ippiAddC_16s_C1IRSfs (128, base16sHoriz, baseImage16s, baseImageRoi, 0);
	ippiAddC_16s_C1IRSfs (128, base16sVert , baseImage16s, baseImageRoi, 0);
	ippiConvert_16s8u_C1R (base16sHoriz, baseImage16s, baseImageGrayscaleTmp1, baseImage1S, baseImageRoi);
	ippiConvert_16s8u_C1R (base16sVert , baseImage16s, baseImageGrayscaleTmp2, baseImage1S, baseImageRoi);
	ippiSet_8u_C1R(128, baseImageGrayscaleTmp3, baseImage1S, baseImageRoi);
	
	//--------------------------
	// compile gradients into RGB image.
	ippiCopy_8u_C1C3R (baseImageGrayscaleTmp1,				baseImage1S,
				       baseImageGradientPixelsC3,			baseImage3S, baseImageRoi);
	ippiCopy_8u_C1C3R (baseImageGrayscaleTmp2,				baseImage1S,
				       baseImageGradientPixelsC3 + 1,		baseImage3S, baseImageRoi);
	ippiCopy_8u_C1C3R (baseImageGrayscalePixels,			baseImage1S, 
					   baseImageGradientPixelsC3 + 2,		baseImage3S, baseImageRoi);
	baseImageGradientC3.setFromPixels (baseImageGradientPixelsC3, baseImageW, baseImageH, OF_IMAGE_COLOR);
	
	//--------------------------
	// resize for big version, baseImageBigC3, used for forces.
	IppiRect srcRect = {0,0, baseImageW-1,baseImageH-1};
	IppiSize srcSize = {baseImageW,   baseImageH};
	IppiSize dstSize = {ippFboPixelW, ippFboPixelH};
	double xFactor = (float)ippFboPixelW / (float)baseImageW;
	double yFactor = (float)ippFboPixelH / (float)baseImageH;
	ippiResize_8u_C3R(
					  baseImageGradientPixelsC3, srcSize, baseImage3S,   srcRect, 
					  baseImageBigPixelsC3,			    ippFboPixel3S, dstSize, 
					  xFactor, yFactor, IPPI_INTER_CUBIC);
	baseImageBigC3.setFromPixels(baseImageBigPixelsC3, ippFboPixelW, ippFboPixelH, OF_IMAGE_COLOR);
}



//--------------------------------------------------------------
float testApp::doubleExponentialSigmoid (float x, float a){
	
	float epsilon = 0.000001;
	float min_param_a = 0.0 + epsilon;
	float max_param_a = 1.0 - epsilon;
	a = MIN(max_param_a, MAX(min_param_a, a));
	a = 1.0-a; // for sensible results
	
	float y = 0;
	if (x<=0.5){
		y = (powf(2.0*x, 1.0/a))/2.0;
	} else {
		y = 1.0 - (powf(2.0*(1.0-x), 1.0/a))/2.0;
	}
	return y;
}

float testApp::doubleExponentialSeat (float x, float a){
	
	float epsilon = 0.00001;
	float min_param_a = 0.0 + epsilon;
	float max_param_a = 1.0 - epsilon;
	a = MIN(max_param_a, MAX(min_param_a, a)); 
	
	float y = 0;
	if (x<=0.5){
		y = (powf(2.0*x, 1.0-a))/2.0;
	} else {
		y = 1.0 - (powf(2.0*(1.0-x), 1.0-a))/2.0;
	}
	return y;
}


//---------------------------------------
float testApp::logisticSigmoid (float x, float a){
	// n.b.: this Logistic Sigmoid has been normalized.
	
	float epsilon = 0.0001;
	float min_param_a = 0.0 + epsilon;
	float max_param_a = 1.0 - epsilon;
	a = MAX(min_param_a, MIN(max_param_a, a));
	a = (1.0/(1.0-a) - 1.0);
	
	float A = 1.0 / (1.0 + expf(0.0 -((x-0.5)*a*2.0)));
	float B = 1.0 / (1.0 + expf(a));
	float C = 1.0 / (1.0 + expf(0.0-a)); 
	float y = (A-B)/(C-B);
	return y;
}


