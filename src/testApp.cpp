#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	IPPL = new IppLoader();
	maxNParticles	= 16384;
	myFBOWidth		= 1600; //1920; 
	myFBOHeight		= 900; //1080;
	bActuallyWriteFboImageFiles = false; 
	bCycleAutomatically	= true;

	
	outputW			= myFBOWidth;
	outputH			= myFBOHeight;
	outputN			= outputW*outputH*3;
	
	ippFboPixelW	= myFBOWidth;
	ippFboPixelH	= myFBOHeight;
	ippFboPixel1S	= myFBOWidth*1;
	ippFboPixel3S	= myFBOWidth*3;
	
	panel.setup("GOLAN_WI", ofGetWidth()-240-5,5, 240, 492);
	panel.addPanel(" Simulation", 1, false);
	panel.setWhichPanel(" Simulation");
	
	// addSlider(string sliderName, string xmlName, float value , float min, float max, bool isInt);
	panel.addSlider("nParticles",			"N_PARTICLES",		10000,	320,	maxNParticles,	true);
	panel.addSlider("nNearestNeighborsK",	"N_NEIGHBORS",		5,		2,		20,				true);
	panel.addSlider("deltaFactor",			"DELTA_FACTOR",		0.01,	0.000,	0.100,			false);
	panel.addSlider("averageDistFactor",	"AVG_DIST_FACTOR",	0.80,	0.500,	1.000,			false);
	panel.addSlider("damping",				"DAMPING",			0.80,	0.500,	0.999,			false);
	panel.addSlider("force noise",			"FORCE_NOISE",		0.01,	0.000,	0.100,			false);
	panel.addSlider("imageForcePerp",		"IMAGE_FORCE_P",	0.10,	0.000,	1.00,			false);
	panel.addSlider("imageForceTangent",	"IMAGE_FORCE_T",    0.00,   0.000,  2.00,			false); 
	panel.addSlider("nIterations",			"N_ITERATIONS",		100,	1,		300,			true);
	
	
	panel.addPanel(" Simulation-2", 1, false); 
	panel.setWhichPanel(" Simulation-2");
	panel.addSlider("birthProbPow",			"BIRTH_PROB_POW",	1.0,	0.25,	4.0,			false);
	panel.addSlider("birthProbMix",			"BIRTH_PROB_MIX",	0.5,	0.00,	1.0,			false);
	panel.addSlider("addRandPerFrame",		"ADD_PER_FRAME",	0,		0,		500,			true);
	
	panel.addPanel(" Graphics", 1, false); 
	panel.setWhichPanel(" Graphics");
	panel.addSlider("minLineWidth",			"MIN_LINE_WIDTH",	5.0,	1.0,	20.0,			false);
	panel.addSlider("maxLineWidth",			"MAX_LINE_WIDTH",	3.5,	1.0,	20.0,			false);
	panel.addSlider("lineWidthPow",			"LINE_WIDTH_POW",	4.0,	0.0,	8.00,			false);
	panel.addSlider("medianSize",           "MEDIAN_SIZE",      7,      3,      11,             true);
	panel.addSlider("medianPasses",         "N_MEDIAN_PASSES",  3,      0,      10,             true);
	panel.addSlider("nSourceBlurs",			"N_SOURCE_BLURS",   4,      0,      16,             true); 
	panel.addSlider("lumSlowness",			"LUM_SLOWNESS",		0.5,    0.0,    1.0,            false);
	panel.addToggle("bDrawSourceImg",		"DRAW_SOURCE_IMG",	true);
	panel.addToggle("bVaryLineWidth",		"VARY_LINE_WIDTH",	true);
	
	panel.addPanel(" Color", 1, false); 
	panel.setWhichPanel(" Color");
	panel.addSlider("handySlider1",			"HANDY_SLIDER1",	0.5,	0,		1.0,			false);
	panel.addSlider("handySlider2",			"HANDY_SLIDER2",	0.5,	0,		1.0,			false);
	panel.addSlider("handySlider3",			"HANDY_SLIDER3",	0.5,	0,		1.0,			false);
	panel.addSlider("handySlider4",			"HANDY_SLIDER4",	0.5,	0,		1.0,			false);
	panel.addSlider("handySlider5",			"HANDY_SLIDER5",	0.5,	0,		1.0,			false);
	panel.addSlider("handySlider6",			"HANDY_SLIDER6",	0.5,	0,		1.0,			false);
	panel.addSlider("handySlider7",			"HANDY_SLIDER7",	0.5,	0,		1.0,			false);
	panel.addSlider("colorGradPow",			"COLOR_GRAD_POW",	0.5,	0,		1.0,			false);
	panel.addSlider("handySlider9",			"HANDY_SLIDER9",	0.5,	0,		1.0,			false);
	
	xmlSettingsFileName = "settings/controlPanelSettings.xml";
	// panel.saveSettings(xmlSettingsFileName);
	panel.loadSettings(xmlSettingsFileName);
	
	iterationStartTime = ofGetElapsedTimeMillis();
	bInitialized		= false;
	bPause				= false;
	bDoDrawDiagnostic	= false;
	whichCurrentImage = 0;
	currentOutputFilenameIndex = 0;
	
	allocateMemory();
	initialize (false);
	bFullScreen = true;
	nMedianPassesForThisRound = 1;
	
	//--------------------------------------------------------
	myFBODisplayWidth = 768; 
	myFBODisplayHeight = (int)(myFBODisplayWidth * ((float)myFBOHeight/(float)myFBOWidth));
	
	// # program has to save .png/.tiff files with RGB/CMYK Colors and the required size of: 4080 x myFBOHeight px
	myFBO.allocate(myFBOWidth,myFBOHeight, GL_RGB);
	myImage = new ofImage();
	myImage->allocate(myFBOWidth,myFBOHeight, OF_IMAGE_COLOR);
	
	fbo1.allocate(myFBOWidth, myFBOHeight, GL_RGB);
	fbo2.allocate(myFBOWidth, myFBOHeight, GL_RGB);
	fbo1.clear(0,0,0,0);
	fbo2.clear(0,0,0,0);
	
	doShader = true;
	shaderThresh.load("shaders/noise.vert",	"shaders/noise.frag");
	shaderH.load("shaders/simpleBlurHorizontal.vert",	"shaders/simpleBlurHorizontal.frag");
	shaderV.load("shaders/simpleBlurVertical.vert",		"shaders/simpleBlurVertical.frag");
	ofBackground(255,255,255);
	
	
	ofSetVerticalSync(false);
	
	
	colorFadeOutTimingPercent = 0.7;
	colorFadeOutTimingBias    = 0.5;
	colorFadeInEndIteration   = 100 * 2; 
}




//--------------------------------------------------------------
void testApp::allocateMemory(){
	
	handyChars = new char[64];
	
	ippFboPixelsC1	     = ippiMalloc_8u_C3(ippFboPixelW, ippFboPixelH, &ippFboPixel1S);
	ippFboPixelsC1Tmp1   = ippiMalloc_8u_C3(ippFboPixelW, ippFboPixelH, &ippFboPixel1S);
	ippFboPixelsC1Tmp2	 = ippiMalloc_8u_C3(ippFboPixelW, ippFboPixelH, &ippFboPixel1S); 
	ippFboPixelsC3	     = ippiMalloc_8u_C3(ippFboPixelW, ippFboPixelH, &ippFboPixel3S);
	baseImageBigPixelsC3 = ippiMalloc_8u_C3(ippFboPixelW, ippFboPixelH, &ippFboPixel3S);
	outputPixels		 = new unsigned char[outputN];
	
	
	
	baseImageRGB.allocate		(192, 128, OF_IMAGE_COLOR);
	baseImageGray.allocate		(192, 128, OF_IMAGE_GRAYSCALE);
	baseImageGradientC3.allocate(192, 128, OF_IMAGE_COLOR);
	baseImageBigC3.allocate		(ippFboPixelW, ippFboPixelH, OF_IMAGE_COLOR);
	ippFboImageC1.allocate      (ippFboPixelW, ippFboPixelH);
	ippFboImageC3.allocate      (ippFboPixelW, ippFboPixelH);
	finalGrayscaleLineImage.allocate(ippFboPixelW, ippFboPixelH);
	
	cvImageC1R.allocate			(ippFboPixelW, ippFboPixelH);
	cvImageC1G.allocate			(ippFboPixelW, ippFboPixelH);
	cvImageC1B.allocate			(ippFboPixelW, ippFboPixelH);

	
	allocateNoiseImage();
	
	thresholder = new tmVisThresholderC1 (192, 128);
	
    
	positionx = ippiMalloc_32f_C1 (maxNParticles, 1, &particleStep32f); 
	positiony = ippiMalloc_32f_C1 (maxNParticles, 1, &particleStep32f); 
	velocityx = ippiMalloc_32f_C1 (maxNParticles, 1, &particleStep32f);
	velocityy = ippiMalloc_32f_C1 (maxNParticles, 1, &particleStep32f);
     
    /*
    positionx = new float[maxNParticles];
	positiony = new float[maxNParticles];
	velocityx = new float[maxNParticles];
	velocityy = new float[maxNParticles];
     */
	
	
	//----------------------------------------------
	
	nDimensions				= 2;
	maxNearestNeighborsK	= 20;
	averageInterParticleDistance = 0;
	
	queryPt = annAllocPt(nDimensions);					// allocate query point
	dataPts = annAllocPts(maxNParticles, nDimensions);	// allocate data points
	nnIdx = new ANNidx [maxNearestNeighborsK];			// allocate near neigh indices
	dists = new ANNdist[maxNearestNeighborsK];			// allocate near neighbor dists
	
	for (int i=0; i<maxNParticles; i++){
		dataPts[i] = new ANNcoord[nDimensions];
	}
	
	kNeighbors = ippiMalloc_32s_C1 (maxNParticles, maxNearestNeighborsK, &particleStep32s);
	kDistances = ippiMalloc_32f_C1 (maxNParticles, maxNearestNeighborsK, &particleStep32f);
	
	
	baseImageW = 192;
	baseImageH = 128;
	baseImage1S = baseImageW;
	baseImage3S = baseImageW*3;
	baseImage1N = baseImageW * baseImageH * 1;
	baseImage3N = baseImageW * baseImageH * 3;
	
	baseImageGrayscalePixels  = ippiMalloc_8u_C1(baseImageW, baseImageH, &baseImage1S);
	baseImageGrayscaleTmp1    = ippiMalloc_8u_C1(baseImageW, baseImageH, &baseImage1S);
	baseImageGrayscaleTmp2    = ippiMalloc_8u_C1(baseImageW, baseImageH, &baseImage1S);
	baseImageGrayscaleTmp3	  = ippiMalloc_8u_C1(baseImageW, baseImageH, &baseImage1S); 
	baseImageRGBPixels        = ippiMalloc_8u_C3(baseImageW, baseImageH, &baseImage3S);
	baseImageGradientPixelsC3 = ippiMalloc_8u_C3(baseImageW, baseImageH, &baseImage3S);
	base16sHoriz			  = ippiMalloc_16s_C1(baseImageW, baseImageH, &baseImage16s);
	base16sVert				  = ippiMalloc_16s_C1(baseImageW, baseImageH, &baseImage16s);
	
	nLutLevels = 256;
	pLutValues = new int[nLutLevels];
	pLutLevels = new int[nLutLevels];
	for (int i=0; i<nLutLevels; i++){
		pLutLevels[i] = i;
		float ifrac = ofClamp((i-64.0)/192.0, 0, 1);
		pLutValues[i] = (int)(255.0 * powf(ifrac, 1.5));		  
	}

}



//--------------------------------------------------------------
void testApp::initialize (bool bSameImageAsBefore){
	
	//---------------------
	bPreppedOutputImage = false;
	loadImage (bSameImageAsBefore);	
	
	
	iterationStartTime	 = ofGetElapsedTimeMillis();
	iterationElapsedTime = 0;
	iterationCount		 = 0;
	
	IppiSize ippFboRoi = {ippFboPixelW, ippFboPixelH};
	Ipp8u white[]= {255,255,255};
	Ipp8u black[]= {0,0,0};
	ippiSet_8u_C1R((Ipp8u) 255, ippFboPixelsC1,		ippFboPixel1S, ippFboRoi);
	ippiSet_8u_C1R((Ipp8u) 255, ippFboPixelsC1Tmp1,	ippFboPixel1S, ippFboRoi);
	ippiSet_8u_C3R(white,	    ippFboPixelsC3,		ippFboPixel3S, ippFboRoi);
	
	particleStep32f = maxNParticles * 4;
	IppiSize maxBufRoi = {maxNParticles, 1};
	
	// set to zero and initialize with random data.
	ippiSet_32f_C1R((Ipp32f) 0.0, positionx, particleStep32f, maxBufRoi);
	ippiSet_32f_C1R((Ipp32f) 0.0, positiony, particleStep32f, maxBufRoi);
	ippiSet_32f_C1R((Ipp32f) 0.0, velocityx, particleStep32f, maxBufRoi);
	ippiSet_32f_C1R((Ipp32f) 0.0, velocityy, particleStep32f, maxBufRoi);
     
    /*
    for (int i=0; i<maxNParticles; i++){
        positionx[i] = 0.0;
        positiony[i] = 0.0;
        velocityx[i] = 0.0;
        velocityy[i] = 0.0;
    }
    */
	
	
	
	float marginY = 20; // margin
	float marginX = 20+ (myFBOWidth-outputW)/2; // margin
	bounds = ofRectangle (marginX,marginY, myFBOWidth-2*marginX, myFBOHeight-2*marginY);
	unsigned int pSeed = (unsigned int)(ofGetElapsedTimeMillis() + (int)ofRandom(0,1000));
	
	// init randoms for all...
	ippiAddRandUniform_Direct_32f_C1IR(positionx, particleStep32f, maxBufRoi, bounds.x,bounds.x+bounds.width,  &pSeed);
	ippiAddRandUniform_Direct_32f_C1IR(positiony, particleStep32f, maxBufRoi, bounds.y,bounds.y+bounds.height, &pSeed);
    

	// use probabilistic technique for visible ones
	nParticles		= 0;
	int nToAdd		= panel.getValueI("N_PARTICLES");
	addPositionsProbabilisticallyBasedOnSourceImage (nToAdd);

	
	float fmax = 0.1;
	ippiAddRandUniform_Direct_32f_C1IR(velocityx, particleStep32f, maxBufRoi, -fmax,fmax, &pSeed);
	ippiAddRandUniform_Direct_32f_C1IR(velocityy, particleStep32f, maxBufRoi, -fmax,fmax, &pSeed);
	
	//------------------------------------------------------------------------
	nNearestNeighborsK		= 7;//4
	nMaxPointsToVisit		= nNearestNeighborsK + 6;
	nBucketSize				= 1; 
	searchError				= 0.1;
	
	cvImageC1R.set(255);
	cvImageC1G.set(255);
	cvImageC1B.set(255);
	
	nMedianPassesForThisRound = (int) ofRandom(1, 4.99);
	nMedianPassesForThisRound = (int) ofClamp(nMedianPassesForThisRound, 1, 4);
	
	//---------------------
	bInitialized = true;
	bPause = false;
	
	// RANDOMIZER FOR TAIWAN EXHIBTION
	colorFadeOutTimingPercent = ofRandom(0.65, 0.80); 
	colorFadeOutTimingBias    = ofRandom(0.45, 0.65); 
	colorFadeInEndIteration	  = (int) (nIterations * ofRandom(1.5,2)); 
	
	panel.setValueF("DELTA_FACTOR",		ofRandom(0.002, 0.040) );
	panel.setValueI("N_PARTICLES",		(int) ofRandom(1500, 8500) );
	panel.setValueF("IMAGE_FORCE_P",	powf(ofRandom(0.01, 0.99),3.0) ); 
	panel.setValueI("N_NEIGHBORS",		(int) ofRandom(3.9, 6.1)); 
	panel.setValueF("AVG_DIST_FACTOR",	ofRandom(0.80, 0.92));
	
	panel.setValueI("N_ITERATIONS",		(int) ofRandom(45,100));
	nIterations = panel.getValueI("N_ITERATIONS"); 
	
}



//--------------------------------------------------------------
void testApp::addPositionsProbabilisticallyBasedOnSourceImage (int nToAdd){
	
	float birthProbPow = panel.getValueF("BIRTH_PROB_POW");
	float birthProbMix = panel.getValueF("BIRTH_PROB_MIX");

	for (int i=0; i<nToAdd; i++){
		if (nParticles < maxNParticles){
			
			bool bAdded = false;
			while (bAdded == false) {
				float rx = ofRandom(bounds.x, bounds.x+bounds.width);
				float ry = ofRandom(bounds.y, bounds.y+bounds.height);
			
				float pixelProb = getSourceImageLuminance01 (rx,ry);
				pixelProb = powf(pixelProb, birthProbPow); 
								 
				float myProb = (1.0-birthProbMix) + (birthProbMix * ofRandom(0.0, 1.0));
				if (myProb >= pixelProb){
					bAdded = true;
					positionx[nParticles] = rx;
					positiony[nParticles] = ry;
				}
				
			}
			nParticles++;
		}
	}
	
	
}

//--------------------------------------------------------------
float testApp::getSourceImageLuminance01 (float rx, float ry){
	
	int rxi = (int)round(rx);
	int ryi = (int)round(ry);
	rxi = MIN(myFBOWidth-1,  MAX(0, rxi));
	ryi = MIN(myFBOHeight-1, MAX(0, ryi));
	
	int index = (ippFboPixel3S * ryi) + (rxi*3);
	float frac01 = baseImageBigPixelsC3[index+2]/255.0;
	return frac01;
}


void testApp::draw(){
	
	//ofEnableAlphaBlending();
	//ofBackground(255,255,255);
	
	ofxFBOTexture *src;
	ofxFBOTexture *dst;
	src = &fbo1;
	dst = &fbo2;
	
	// beginRender	
	fbo1.begin();
	ofEnableAlphaBlending();
	ofBackground(255,255,255);
	drawLinesIntoFbo();
	fbo1.end(); 
	
	float blurRange = 3.0; 
	int   nPasses = nMedianPassesForThisRound;					// nPasses = (int) ofMap(mouseY, 0,ofGetHeight(), 1,4, true); 
	float thresholds[] = {0, 0.735, 0.785, 0.814, 0.834};	
	float thresh = thresholds[nMedianPassesForThisRound];		// thresh = ofMap(mouseX,0,ofGetWidth(), 0,1,true);
	
	int fadeOutStartIter = (int) (totalCycleDurationInIterations * 0.666);
	int fadeOutEndIter   = totalCycleDurationInIterations;
	int fadeInStartIter  = 0;
	int fadeInEndIter    = nIterations / 4;
	
	if        ((iterationCount >= fadeOutStartIter) && (iterationCount <= fadeOutEndIter)){
		float factor = ofMap(iterationCount, fadeOutStartIter,fadeOutEndIter, 1,0); 
		factor = logisticSigmoid(factor, 0.55);
		thresh = thresholds[nMedianPassesForThisRound] * factor;
		
	} else if ((iterationCount >= fadeInStartIter) && (iterationCount <= fadeInEndIter)){
		float factor = ofMap(iterationCount, fadeInStartIter,fadeInEndIter, 0,1); 
		factor = powf(factor, 0.5); //logisticSigmoid(factor, 0.45);
		thresh = thresholds[nMedianPassesForThisRound] * factor;
	} 
			   

	for (int i = 0; i < nPasses; i++){
		
		//first the horizontal shader 
		shaderH.begin();
		dst->begin();
		src->draw(0, 0);
		dst->end();
		shaderH.end();
		
		//now the vertical shader
		shaderV.begin();	
		blurRange = 3.0; //(i%2 == 0) ? 2.0:3.0;
		shaderV.setUniform1f("blurRange", blurRange);
		shaderV.setUniform1f("thresh", thresh);
		shaderV.setUniform1i("bDoThresh", (i==(nPasses-1))?1:0  );
				
		src->begin();
		dst->draw(0,0);
		src->end();
		shaderV.end();
	}
	
	
	//printf("iterationCount  %d\n", iterationCount) ;
	// Draw the main (filtered) line image. 
	ofEnableAlphaBlending();
	ofSetColor(255, 255, 255);
	src->draw(0,ofGetHeight(), ofGetWidth(), 0-ofGetHeight());
	

	
	if (bPreppedOutputImage){
		// putting saveFBO() here does it every frame, but it's too expensive. 
		if (iterationCount < fadeOutStartIter) { 
			saveFBO();
		}
		ofEnableAlphaBlending();
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		huntForBlendFunc(2000, 2,5); //-1,-1);
		
		float startIteration = 0;//nIterations; 
		float endIteration   = colorFadeInEndIteration; 
		float alph01 = ofMap(iterationCount, startIteration,endIteration, 0,1); 
		alph01 = logisticSigmoid(alph01, 0.35);
		
		// fade it out at the very end. 
		int alpOutStartIter = (int) (totalCycleDurationInIterations * colorFadeOutTimingPercent);
		int alpOutEndIter   = totalCycleDurationInIterations;
		if        ((iterationCount >= alpOutStartIter) && (iterationCount <= alpOutEndIter)){
			float factor = ofMap(iterationCount, alpOutStartIter,alpOutEndIter, 1,0); 
			factor = logisticSigmoid(factor, colorFadeOutTimingBias);
			factor = powf(factor, 0.5); 
			alph01 *= factor;
		}
		
		
		
		ofSetColor(255,255,255, 255*alph01); 
		ippFboImageC3.draw	(0,0, ofGetWidth(),ofGetHeight()); // they're flipped from each other.
		glPopAttrib();
	} 
	
	
	if (bDoDrawDiagnostic){
		ofShowCursor();
		panel.draw();
		
		//------------
		// draw the color gradient band
		drawColorBand();
		
		//------------
		// draw diagnostic text
		if (iterationCount < nIterations){
			iterationElapsedTime = (ofGetElapsedTimeMillis() - iterationStartTime);
		}
		double elapsedTime = iterationElapsedTime/1000.0;
		string iterationString = ofToString(iterationCount) + "/" + ofToString(nIterations);
		iterationString += " (" + ofToString(elapsedTime) + "s); " + ofToString(nParticles);
		iterationString += "\n"; 
		iterationString += "Press 's' to render & save\n"; 
		iterationString += "Press 'i' to re-initialize\n";

		glColor3f(0,0,0);
		ofDrawBitmapString(iterationString, 5, myFBODisplayHeight+20);
	} else {
		ofHideCursor();
	}
	
}


//--------------------------------------------------------------
void testApp::huntForBlendFunc(int period, int defaultSid, int defaultDid){
	// sets all possible combinations of blend functions,
	// changing modes every [period] milliseconds. 
	
	int sfact[] = {
		GL_ZERO, 
		GL_ONE,
		GL_DST_COLOR, 
		GL_ONE_MINUS_DST_COLOR, 
		GL_SRC_ALPHA,
		GL_ONE_MINUS_SRC_ALPHA,
		GL_DST_ALPHA,
		GL_ONE_MINUS_DST_ALPHA,
		GL_SRC_ALPHA_SATURATE
	};
		
	int dfact[] = {
		GL_ZERO,
		GL_ONE,
		GL_SRC_COLOR,
		GL_ONE_MINUS_SRC_COLOR,
		GL_SRC_ALPHA,
		GL_ONE_MINUS_SRC_ALPHA,
		GL_DST_ALPHA,
		GL_ONE_MINUS_DST_ALPHA
	};
	
	glEnable(GL_BLEND);
	
	if ((defaultSid == -1) && (defaultDid == -1)) {

		int sid =  (ofGetElapsedTimeMillis()/(8*period))%9;
		int did =  (ofGetElapsedTimeMillis()/period)%8;
		glBlendFunc(sfact[sid], dfact[did]);
		printf("SRC %d	DST %d\n", sid, did);
		
	} else if (defaultDid == -1){
	
		int did =  (ofGetElapsedTimeMillis()/period)%8;
		glBlendFunc(sfact[defaultSid], dfact[did]);
		printf("SRC %d	DST %d\n", defaultSid, did);
	
	} else if (defaultSid == -1){
	
		int sid =  (ofGetElapsedTimeMillis()/(8*period))%9;
		glBlendFunc(sfact[sid], dfact[defaultDid]);
		printf("SRC %d	DST %d\n", sid, defaultDid);
		
	} else {
		
		glBlendFunc(sfact[defaultSid], dfact[defaultDid]);
	
	}
}


void testApp::update2(){
	;
}

//--------------------------------------------------------------
void testApp::draw2(){
	
	

	
	
	
	// draw the lines connecting particles into the FBO. 
	myFBO.begin();
	drawLinesIntoFbo();
	myFBO.end();
	
	
	
	ofBackground(255,255,255);
	
	
	
	
	
	shaderH.begin();
	shaderH.setUniform1f("blurAmnt", 3.0);
	
	glColor3f(1,1,1); 
	myFBO.draw (0,ofGetHeight(), ofGetWidth(),0-ofGetHeight()); 
	shaderH.end();
	
	
	
	
	
	if (bPreppedOutputImage){
		ofEnableAlphaBlending();
		float alph01 = ofMap(iterationCount, nIterations,totalCycleDurationInIterations, 0,1); 
		alph01 = powf(alph01, 0.25); 
		ofSetColor(255,255,255, 255*alph01); 
		ippFboImageC3.draw	(0,0, ofGetWidth(),ofGetHeight()); // they're flipped from each other.
	}
	
	
	if (bDoDrawDiagnostic){
		panel.draw();
		
		//------------
		// draw the color gradient band
		drawColorBand();
		
		
		//------------
		// draw diagnostic text
		if (iterationCount < nIterations){
			iterationElapsedTime = (ofGetElapsedTimeMillis() - iterationStartTime);
		}
		double elapsedTime = iterationElapsedTime/1000.0;
		string iterationString = ofToString(iterationCount) + "/" + ofToString(nIterations);
		iterationString += " (" + ofToString(elapsedTime) + "s); " + ofToString(nParticles);
		iterationString += "\n"; 
		iterationString += "Press 's' to render & save\n"; 
		iterationString += "Press 'i' to re-initialize\n";

		glColor3f(0,0,0);
		ofDrawBitmapString(iterationString, 5, myFBODisplayHeight+20);
	}
	
	

	
}


//====================================================================
void testApp::drawLinesIntoFbo(){
	
	
	
	// fill in the image with a white rect.
	//ofFill();
	//ofEnableAlphaBlending();
	//ofSetColor(255,255,255);
	//ofRect(0,0, myFBOWidth,myFBOHeight);
	
	
	if (panel.getValueB("DRAW_SOURCE_IMG")){
		glColor4f(1,1,1, 0.75);
		baseImageBigC3.draw(0,0);
	}


	glLineWidth(2.0);
	glColor4f(0,0,0, 0.4); 
	glEnable(GL_LINE_SMOOTH);
	
	float avgL = averageInterParticleDistance;
	float minL = avgL - stdevInterParticleDistance;
	float maxL = avgL + stdevInterParticleDistance;
	float minW = panel.getValueF("MIN_LINE_WIDTH");
	float maxW = panel.getValueF("MAX_LINE_WIDTH");
	float linePow = panel.getValueF("LINE_WIDTH_POW");
	
	bool bVaryLineThickness = panel.getValueB("VARY_LINE_WIDTH");
	if (bVaryLineThickness){
		
		for (int indexa=0; indexa < nParticles; indexa++){
			float xa = positionx[indexa];
			float ya = positiony[indexa];

			for (int i=0; i<nNearestNeighborsK; i++){
				int indexc = maxNParticles*i + indexa;
				int indexb = kNeighbors[indexc];
				
				if (indexb != -1) {
					float xb = positionx[indexb];
					float yb = positiony[indexb];
					
					if (bVaryLineThickness){
						float dh = ofDist(xa, ya, xb, yb);
						float th = ofMap(dh, minL, maxL, 0.0, 1.0, true);
						
						//float gr = powf(th, 0.5);
						float gr = ofMap(th, 0.0, 1.0, 0.5,0.4);
						glColor4f(0,0,0, gr); 
						
						th = 1.0 - powf(th, linePow);
						th = ofMap(th, 0.0, 1.0, minW, maxW, true);
						glLineWidth(th);
						
					}
					
					glBegin(GL_LINES);
					glVertex2f(xa,ya);
					glVertex2f(xb,yb);
					glEnd();
				}
			}
		}
		
	} else { 
		
		// don't vary line thickness
		glLineWidth(minW);
		glBegin(GL_LINES);
		
		for (int indexa=0; indexa < nParticles; indexa++){
			float xa = positionx[indexa];
			float ya = positiony[indexa];

			for (int i=0; i<nNearestNeighborsK; i++){
				int indexc = maxNParticles*i + indexa;
				int indexb = kNeighbors[indexc];
				
				if (indexb != -1) {
					float xb = positionx[indexb];
					float yb = positiony[indexb];

					glVertex2f(xa,ya);
					glVertex2f(xb,yb);
				}
			}
		}
		glEnd();
	}

	
	//ofDisableAlphaBlending();
	glLineWidth(1.0); 
	

}




//====================================================================
void testApp::drawIntoFboBogus(){
	myFBO.begin();
	glColor3f(1,1,1); 
	ofFill();
	ofRect(0,0, myFBOWidth,myFBOHeight);
	glColor3f(0.5, 0.5, 0.5);
	ofCircle(1000, 1000, 500);
	ofLine(0,0, myFBOWidth,myFBOHeight); 
	myFBO.end();
}

//------------------------------
void testApp::displayFBODiagnostic(){
	glColor3f(1,1,1); 
	myFBO.draw(5, 5+myFBODisplayHeight, myFBODisplayWidth,0-myFBODisplayHeight);
	
	bool bDrawBorder = true;
	if (bDrawBorder){
		ofNoFill();
		glLineWidth(0.5);
		glColor3f(0,0,0); 
		ofRect(5, 5, myFBODisplayWidth,myFBODisplayHeight);
		glLineWidth(1.0); 
		ofFill();
	}
}


//------------------------------
ofColor	testApp::getColorFromFrac (float frac01){
	
	float rP = panel.getValueF("HANDY_SLIDER1"); 
	float gP = panel.getValueF("HANDY_SLIDER2");
	float bP = panel.getValueF("HANDY_SLIDER3");
	rP = ofMap(rP, 0, 1, 0, 2, true);
	gP = ofMap(gP, 0, 1, 0, 2, true);
	bP = ofMap(bP, 0, 1, 0, 2, true);
	
	float rA = panel.getValueF("HANDY_SLIDER4"); 
	float gA = panel.getValueF("HANDY_SLIDER5"); 
	float bA = panel.getValueF("HANDY_SLIDER6"); 
	
	float rf = rA + (1-rA)*powf(frac01, rP);
	float colR = (255.0 * rf); 
	
	float gf = gA + (1-gA)*powf(frac01, gP);
	float colG = (255.0 * gf); 
	
	float bf = bA + (1-bA)*powf(frac01, bP);
	float colB = (255.0 * bf); 
	
	ofColor col;
	col.r = colR;
	col.g = colG;
	col.b = colB;
	return col; 
}

//------------------------------
ofColor testApp::HSL2RGB(double h, double sl, double  l) {
	
	double v;
	double r,g,b;
	
	r = l;   // default to gray
	g = l;
	b = l;
	
	v = (l <= 0.5) ? (l * (1.0 + sl)) : (l + sl - l * sl);
	if (v > 0){
		
		double m;
		double sv;
		int sextant;
		double fract, vsf, mid1, mid2;
		
		m = l + l - v;
		sv = (v - m ) / v;
		h *= 6.0;
		
		sextant = (int)h;
		fract = h - sextant;
		vsf = v * sv * fract;
		mid1 = m + vsf;
		mid2 = v - vsf;
		
		switch (sextant){
				
			case 0:
				r = v;
				g = mid1;
				b = m;
				break;
				
			case 1:
				r = mid2;
				g = v;
				b = m;
				break;
				
			case 2:
				r = m;
				g = v;
				b = mid1;
				break;
				
			case 3:
				r = m;
				g = mid2;
				b = v;
				break;
				
			case 4:
				r = mid1;
				g = m;
				b = v;
				break;
				
			case 5:
				r = v;
				g = m;
				b = mid2;
				break;	
		}	
	}
	
	ofColor rgb;
	rgb.r = (r * 255.0f);
	rgb.g = (g * 255.0f);
	rgb.b = (b * 255.0f);
	return rgb;
}

//------------------------------
void	testApp::drawColorBand(){
	
	ofDisableAlphaBlending();
	glPushMatrix();
	glTranslatef(5, myFBODisplayHeight+11, 0); 
	glLineWidth(1.0);
	
	for (int i=0; i<400; i++){
		float fraci = (float)i/400.0;
		ofColor col = getColorFromFrac(fraci);
		ofSetColor(col.r, col.g, col.b, 255);
		ofLine(i,0, i,80); 
	}
	
	glPopMatrix();
}


//------------------------------
void testApp::prepareOutputBuffer(){
	long startTime = ofGetElapsedTimeMillis();
	
	

	// copy pixels from FBO.
	// Copy from the myFBOpixels, which may not be memory-aligned, into an IPP buffer.
	unsigned char *myFBOpixels = (unsigned char *) fbo1.getPixels();
	IppiSize ippFboSize = {ippFboPixelW, ippFboPixelH};
	ippiCopy_8u_C3C1R(myFBOpixels, ippFboPixel3S, ippFboPixelsC1, ippFboPixel1S, ippFboSize);
	
	/*
	// Apply median filtering back-and-forth between ippFboPixelsC1Tmp1 and ippFboPixelsC1
	// ippiLUT_8u_C1IR (ippFboPixelsC1, ippFboPixel1S, ippFboSize, pLutValues, pLutLevels, nLutLevels);
	int nMedianPasses = panel.getValueI("N_MEDIAN_PASSES");
	int medianSize    = panel.getValueI("MEDIAN_SIZE");   
	for (int n=0; n<nMedianPasses; n++){
		int M = medianSize; 
		int offset = (ippFboPixel1S*(M/2)+ (M/2));
		IppiPoint anchor = {M/2,M/2};
		IppiSize maskSize = {M,M};
		IppiSize dstRoiSize = {ippFboPixelW-(M-1), ippFboPixelH-(M-1)};
		ippiFilterMedian_8u_C1R (ippFboPixelsC1+offset,		ippFboPixel1S,
								 ippFboPixelsC1Tmp1+offset,	ippFboPixel1S, 
								 dstRoiSize, maskSize, anchor);
		ippiCopy_8u_C1R (ippFboPixelsC1Tmp1, ippFboPixel1S, ippFboPixelsC1, ippFboPixel1S, ippFboSize);
	}
	
	// dilate ippFboPixelsC1 to thin the lines, then apply a LUT. 
	// ippFboPixelsC1 now contains the final image of the lines.
	ippiDilate3x3_8u_C1IR (ippFboPixelsC1, ippFboPixel1S, ippFboSize);
	ippiLUT_8u_C1IR (ippFboPixelsC1, ippFboPixel1S, ippFboSize, pLutValues, pLutLevels, nLutLevels);
	
	*/
	
	// binarize into ippFboPixelsC1Tmp1, then 
	// copy into ofxCVgrayscale image ippFboImageC1
	int srcIndex = 0; 
	int dstIndex = 0;
	for (int y=0; y<myFBOHeight; y++){
		srcIndex = y*ippFboPixel1S;
		dstIndex = y*myFBOWidth;
		for (int x=0; x<myFBOWidth; x++){
			Ipp8u srcVal = ippFboPixelsC1[srcIndex++];
			ippFboPixelsC1Tmp2[dstIndex] = srcVal;
			ippFboPixelsC1Tmp1[dstIndex] = (srcVal < 250) ? 0:255;
			dstIndex++;
		}
	}
	 
	ippFboImageC1.setFromPixels          (ippFboPixelsC1Tmp1, ippFboPixelW, ippFboPixelH);
	finalGrayscaleLineImage.setFromPixels(ippFboPixelsC1Tmp2, ippFboPixelW, ippFboPixelH);
	
	
	

	
	
	// remove black islands and find contours in ippFboImageC1/ippFboPixelsC1Tmp1
	ippFboImageC1.floodFill (0,0,0); // 2012
	contourFinder.findContours(ippFboImageC1, 64, 30000, 5000, true);
	
	float maxArea = 0;
	int nContours = contourFinder.blobs.size();
	for (int i = 0; i < nContours; i++){
		ofxCvBlob blob = contourFinder.blobs[i];
		if (blob.area > maxArea){
			maxArea = blob.area;
		}
	}
	

	// render the blobs into cvImageC1R/G/B
	cvImageC1R.set(255);
	cvImageC1G.set(255);
	cvImageC1B.set(255);
	unsigned char *cv1Pixels = ippFboImageC1.getPixels();
	for (int i = 0; i < nContours; i++){
		
		// obtain the blob and check if its centroid is white. 
		ofxCvBlob blob = contourFinder.blobs[i];
		ofPoint centroid = blob.centroid;
		int seedx = (int) centroid.x;
		int seedy = (int) centroid.y;
		int centroidIndex = (seedy * myFBOWidth) + (seedx);
		int centroidVal = cv1Pixels[centroidIndex];
		
		// if it's a white blob, compute a color 
		// and render this blob into the color planes.
		if (centroidVal == 255){
			
			float area = blob.area;
			float areaFrac = ofMap(area, 20, maxArea, 0.0,1.0,true); 
			
			// the "party" color is a random HSB color
			float colOffset = 0.05;
			float H = fmodf(areaFrac - colOffset, 1.0); //fmodf(ofRandom(-0.025, 0.175), 1.0);
			if ((areaFrac < 0.03) && (ofRandom(0,1) < 0.04)){
				H = ofRandom(0.45, 0.65);
			}
			float S = ofRandom(0.90, 1.00); 
			float B = 0.20 + 0.80* powf(getSourceImageLuminance01 (seedx, ippFboPixelH-seedy), 0.20);
			ofColor colParty = HSL2RGB(H,S,B);
			
			// the gradient color is set by the color sliders.
			float aP  = panel.getValueF("COLOR_GRAD_POW"); 
			float aFP = powf(areaFrac, aP); 
			ofColor colGradient = getColorFromFrac(aFP);
			
			// compute a weighted mixture of the two colors. 
			float M = powf(areaFrac, panel.getValueF("HANDY_SLIDER9"));
			M = logisticSigmoid(M, 0.85);
			float cR = (1.0-M)*colParty.r  + (M*colGradient.r);
			float cG = (1.0-M)*colParty.g  + (M*colGradient.g);
			float cB = (1.0-M)*colParty.b  + (M*colGradient.b);
			
			// draw the blob into the 3 color planes.
			cvImageC1R.drawBlobIntoMe( blob, cR );
			cvImageC1G.drawBlobIntoMe( blob, cG );	
			cvImageC1B.drawBlobIntoMe( blob, cB );
			
		}
	}
	
	// make the colored blobs bigger so they overlap their edges.
	for (int n=0; n<2; n++){
		cvImageC1R.erode_3x3();
		cvImageC1G.erode_3x3();
		cvImageC1B.erode_3x3();
	}
	
	
	// multiply the blob images by the line image. 
	// Not needed if we change the blend mode. 
	//cvImageC1R *= finalGrayscaleLineImage;
	//cvImageC1G *= finalGrayscaleLineImage;
	//cvImageC1B *= finalGrayscaleLineImage;
	
	// copy the color planes into the ippFboPixelsC3, 
	// then set the ippFboImageC3 from these pixels.
	unsigned char *pixelsR = cvImageC1R.getPixels();
	unsigned char *pixelsG = cvImageC1G.getPixels();
	unsigned char *pixelsB = cvImageC1B.getPixels();
	Ipp8u *pSrc[3] = {pixelsR, pixelsG, pixelsB};
	IppiSize fboroi = {ippFboPixelW, ippFboPixelH};
	ippiCopy_8u_P3C3R (pSrc, myFBOWidth, ippFboPixelsC3, ippFboPixel3S, fboroi);
	ippFboImageC3.setFromPixels(ippFboPixelsC3, ippFboPixelW, ippFboPixelH); //2012
	
	bPreppedOutputImage = true;
	long endTime = ofGetElapsedTimeMillis();
	printf("Elapsed = %d\n", (int)(endTime - startTime)); 
}

//------------------------------
void testApp::saveFBO(){
	
	prepareOutputBuffer();
	
	//ippFboPixelsC1 is the line image
	//ippFboPixelsC3 is the color image 	
	bool bDoColorizedOutput = true;
	if (bDoColorizedOutput){
		
		// for rendering the openCV color image instead.
		int marginX = ((myFBOWidth - outputW)/2); //8
		int srcIndex = 0; 
		int dstIndex = 0;
		unsigned char *src = ippFboImageC3.getPixels(); 
		for (int y=0; y<outputH; y++){
			srcIndex = y*ippFboPixel3S + marginX*3;
			dstIndex = y*outputW*3;
			for (int x=0; x<outputW; x++){
				outputPixels[dstIndex++] = src[srcIndex++]; 
				outputPixels[dstIndex++] = src[srcIndex++];
				outputPixels[dstIndex++] = src[srcIndex++]; 
			}
		}
	} else {
	
		// just the line image.
		int marginX = ((myFBOWidth - outputW)/2); //8
		int srcIndex = 0; 
		int dstIndex = 0;
		for (int y=0; y<outputH; y++){
			srcIndex = y*ippFboPixel1S + marginX;
			dstIndex = y*outputW*3;
			for (int x=0; x<outputW; x++){
				outputPixels[dstIndex++] = ippFboPixelsC1[srcIndex];
				outputPixels[dstIndex++] = ippFboPixelsC1[srcIndex];
				outputPixels[dstIndex++] = ippFboPixelsC1[srcIndex];
														  srcIndex++;
			}
		}
	}
	

	myImage->clear();
	myImage->setFromPixels (outputPixels, outputW, outputH, OF_IMAGE_COLOR);
	myImage->update();
	

	if (bActuallyWriteFboImageFiles){	
		sprintf(handyChars, "OUTPUT/output_%d%d.png", ofGetMinutes(), ofGetSeconds());
		printf("Saving image to %s...\n", handyChars);
		myImage->saveImage(handyChars); 
	}
	
}





//--------------------------------------------------------------
void testApp::drawParticles(){
	
	ofEnableAlphaBlending();
	glEnable(GL_POINT_SMOOTH);
	glPointSize(1.0);
	glColor4f(0,0,0, 1.0);
	glBegin(GL_POINTS);
	
	for (int i=0; i<nParticles; i++){
		float px = positionx[i];
		float py = positiony[i];
		glVertex2f ( px,py);
	}
	glEnd();
	ofDisableAlphaBlending();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	switch(key){
		case ' ':
			bPause = !bPause;
			break;
		case 'd':
		case 'D':
			bDoDrawDiagnostic = !bDoDrawDiagnostic; 
			break;
			
		case 'f':
		case 'F':
			bFullScreen = !bFullScreen;
			ofSetFullscreen(bFullScreen);
			break;
			
		case 'S':
			panel.saveSettings(xmlSettingsFileName);
			break;
			
		case 's':
			saveFBO();
			break;
		
		case 'I':
		case 'i':
			initialize(false);
			break;
			
		case 'p':
			prepareOutputBuffer();
			break;
	} 
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	if (true || bDoDrawDiagnostic) panel.mouseDragged(x,y,button);
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	if (true || bDoDrawDiagnostic) panel.mousePressed(x,y,button);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	if (true || bDoDrawDiagnostic) panel.mouseReleased();
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}


//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}