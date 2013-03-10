/*
 *  testAppMath.cpp
 *  emptyExample
 *
 *  Created by Golan Levin on 7/19/10.
 *  Copyright 2010 Carnegie Mellon University. All rights reserved.
 *
 */


#include "testApp.h"


//--------------------------------------------------------------
void testApp::applyImageForces(){
	
	float imageForcePerpendicular = panel.getValueF("IMAGE_FORCE_P"); 
	float imageForceTangent		  = panel.getValueF("IMAGE_FORCE_T"); 
	
	for (int i=0; i<nParticles; i++){
		float px = positionx[i]; 
		float py = positiony[i]; 
		
		int pxi = (int)round(px);
		int pyi = (int)round(py);
		pxi = MIN(myFBOWidth-1,  MAX(0, pxi));
		pyi = MIN(myFBOHeight-1, MAX(0, pyi));
		
		int index = (ippFboPixel3S * pyi) + (pxi*3);
		float fx = (baseImageBigPixelsC3[index  ] - 128)/128.0;
		float fy = (baseImageBigPixelsC3[index+1] - 128)/128.0;
		
		velocityx[i] += imageForcePerpendicular * fy; 
		velocityy[i] -= imageForcePerpendicular * fx; 
		
		velocityx[i] += imageForceTangent * fx; 
		velocityy[i] += imageForceTangent * fy; 
		
	}
}

//--------------------------------------------------------------
void testApp::update(){
	
	
	panel.update();
	nIterations				= panel.getValueI("N_ITERATIONS"); 
	nNearestNeighborsK		= panel.getValueI("N_NEIGHBORS");
	nMaxPointsToVisit		= nNearestNeighborsK + 6;
	totalCycleDurationInIterations = nIterations * 6;
	
	if (!bPause && (iterationCount < nIterations)){
		
		int nToAddPerFrame  = panel.getValueI("ADD_PER_FRAME");
		//addPositionsProbabilisticallyBasedOnSourceImage (nToAddPerFrame);
		addPointsOnTooLongConnections();
		
		applyImageForces();
		
		IppiSize allBufRoi = {nParticles, 1};
		
		// add random forces to velocities
		unsigned int pSeed;
		float fmax = panel.getValueF("FORCE_NOISE");
		ippiAddRandUniform_Direct_32f_C1IR(velocityx, particleStep32f, allBufRoi, -fmax,fmax, &pSeed);
		ippiAddRandUniform_Direct_32f_C1IR(velocityy, particleStep32f, allBufRoi, -fmax,fmax, &pSeed);
		
		// multiply velocities by damping factor
		Ipp32f damping = panel.getValueF("DAMPING");
		Ipp32f progressiveDamping = ofMap(iterationCount, 0,nIterations, 1.0, 0.0, true);
		
		ippiMulC_32f_C1IR(progressiveDamping, velocityx, particleStep32f, allBufRoi);
		ippiMulC_32f_C1IR(progressiveDamping, velocityy, particleStep32f, allBufRoi);
		
		// add velocities to positions 
		ippiAdd_32f_C1IR(velocityx, particleStep32f, positionx, particleStep32f, allBufRoi);
		ippiAdd_32f_C1IR(velocityy, particleStep32f, positiony, particleStep32f, allBufRoi);
		

		// clamp positions to acceptable ranges
		ippiThreshold_LTValGTVal_32f_C1IR (positionx, particleStep32f, allBufRoi, bounds.x, bounds.x, bounds.x+bounds.width,  bounds.x+bounds.width);
		ippiThreshold_LTValGTVal_32f_C1IR (positiony, particleStep32f, allBufRoi, bounds.y, bounds.y, bounds.y+bounds.height, bounds.y+bounds.height);
	
		
		//----------------------------------
		updateNearestNeighbors();
		updateParticles();
		
	}
	iterationCount++;
	
	if (iterationCount == nIterations){
		saveFBO(); // DO IT!
	}
	if (bCycleAutomatically && (iterationCount > totalCycleDurationInIterations)){
		initialize(false); 
	}

	
}


//--------------------------------------------------------------
void testApp::addPointsOnTooLongConnections(){
	
	if (iterationCount > 1){
		float davg = averageInterParticleDistance;
		float dstd = stdevInterParticleDistance;
		float tooLong = davg + dstd*10.0;
		
		for (int j=0; j<1; j++){
			
			float longest = 0;
			int   longestIdA = -1;
			int   longestIdB = -1;
			for (int indexa=0; indexa < nParticles; indexa++){
				float xa = positionx[indexa];
				float ya = positiony[indexa];
				
				for (int i=0; i<nNearestNeighborsK; i++){
					int indexc = maxNParticles*i + indexa;
					int indexb = kNeighbors[indexc];
					
					if (indexb != -1) {
						float xb = positionx[indexb];
						float yb = positiony[indexb];
						
						float dist = ofDist(xa,ya,xb,yb);
						if (dist > longest){
							longest = dist;
							longestIdA = indexa;
							longestIdB = indexb;
						}
					}
				}
			}
			
			float xa = positionx[longestIdA];
			float ya = positiony[longestIdA];
			float xb = positionx[longestIdB];
			float yb = positiony[longestIdB];
			
			int nToAdd = 3;
			for (int k=1; k<nToAdd; k++){
				float frac = (float)k/(float)nToAdd;
				addIntermediatePoint (xa,ya,xb,yb, frac);
			}
		}
	
	}
}
 

//--------------------------------------------------------------
void testApp::updateNearestNeighbors(){
	
	float *ptsx = &positionx[0];
	float *ptsy = &positiony[0];
	for (int i=0; i<nParticles; i++){
		dataPts[i][0] = *ptsx++; 
		dataPts[i][1] = *ptsy++; 
	}
	
	IppiSize roiAllNeighbors = {nParticles, nNearestNeighborsK};
	ippiSet_32s_C1R(-1, kNeighbors, particleStep32s, roiAllNeighbors);
	
	annMaxPtsVisit(nMaxPointsToVisit);
	kdTree = new ANNkd_tree(
							dataPts,			// point array
							nParticles,		// number of points
							nDimensions,		// dimension of space
							nBucketSize,		// bucket size
							ANN_KD_SUGGEST); // splitting rule
	
	int   indexa, indexb, indexc;
	float xa,ya,xb,yb;
	float dist = 0;
	RS.Clear();
	
	for (indexa =0; indexa < nParticles; indexa++){
		
		queryPt = dataPts[indexa]; 
		kdTree->annkSearch(							// search
						   queryPt,					// query point
						   nNearestNeighborsK,		// number of near neighbors
						   nnIdx,					// nearest neighbors (returned)
						   dists,					// distance (returned)
						   searchError);			// error bound
		
		for (int i=1; i<nNearestNeighborsK; i++) {
			indexb = nnIdx[i];
			if (indexb > indexa){

				dist = sqrt(dists[i]);				// unsquare distance
				RS.Push(dist);
				
				indexc = maxNParticles*i + indexa;
				kNeighbors[indexc] = indexb;
				kDistances[indexc] = dist;
		
			}
		}
	}
	
	averageInterParticleDistance = RS.Mean();
	stdevInterParticleDistance   = RS.StandardDeviation();
	
	
	
	
	
	
	
	delete kdTree;
	
}


//====================================================================
void testApp::updateParticles(){
	
	
	float dif, delta;
	float DELTA_FACTOR    = panel.getValueF("DELTA_FACTOR");	// 0.01
	float AVG_DIST_FACTOR = panel.getValueF("AVG_DIST_FACTOR"); // 1.00 
	float lumSlowness	  = panel.getValueF("LUM_SLOWNESS");
	
	float davg = averageInterParticleDistance;
	float dstd = stdevInterParticleDistance;
	float tooLong = davg + dstd*2.0;
	
	
	float dx,dy;
	int   indexa, indexb, indexc;
	float xa,ya,xb,yb;
	float dist;
	
	for (indexa=0; indexa < nParticles; indexa++){
		xa = positionx[indexa];
		ya = positiony[indexa];
		
		for (int i=0; i<nNearestNeighborsK; i++){
			indexc = maxNParticles*i + indexa;
			indexb = kNeighbors[indexc];
			
			if (indexb != -1){
				xb = positionx[indexb];
				yb = positiony[indexb];
				dist = kDistances[indexc];
				

				dif = (dist - (AVG_DIST_FACTOR * averageInterParticleDistance));
				if (abs(dif) > 1.0){
					
					dx = xa-xb;
					dy = ya-yb;
					dx /= dist;
					dy /= dist;
					
				} else {
					
					/*
					float ra = ofRandom(-PI,PI);
					dx = cos(ra);
					dy = sin(ra);
					dif = 1.0;
					 */
					
					dx = xa-xb;
					dy = ya-yb;
					//dx /= (dist*dist);
					//dy /= (dist*dist);
					
				}
				
				float luminance = getSourceImageLuminance01 (xb,yb); 
				float lumFrac = ofMap(luminance, 0,1,  1.0,0.1); 
				lumFrac = (1.0-lumSlowness) + (lumSlowness)*lumFrac;
				
				
				delta = lumFrac * DELTA_FACTOR * dif;
				velocityx[indexa] -= dx * delta;
				velocityy[indexa] -= dy * delta;
				velocityx[indexb] += dx * delta;
				velocityy[indexb] += dy * delta;
				
				
								
			}
		}
	}
}


//--------------------------------------------------------------
void testApp::addIntermediatePoint (float ax, float ay, float bx, float by, float frac){
	
	if (nParticles < maxNParticles){
		float mx = ax*(1-frac) + bx*frac;
		float my = ay*(1-frac) + by*frac;
		positionx[nParticles] = mx;
		positiony[nParticles] = my;
		nParticles++;
	}
}


//--------------------------------------------------------------
void testApp::addMiddlePoint (float ax, float ay, float bx, float by){
	
	if (nParticles < maxNParticles){
		float mx = (ax+bx)/2.0;
		float my = (ay+by)/2.0;
		positionx[nParticles] = mx;
		positiony[nParticles] = my;
		nParticles++;
	}
}



//--------------------------------------------------------------
void testApp::drawDelaunay(){
	
	
	
	ofEnableAlphaBlending();
	
	/*
	 glColor4f(0,0,0, 1);
	 glDisable(GL_POINT_SMOOTH);
	 glBegin(GL_POINTS);
	 for (int i=0; i<nParticles; i++){
	 glVertex2f ( positionx[i], positiony[i]);
	 }
	 glEnd();
	 */
	
	
	//-----------------------------------
	DPVEC.clear();
	Delaunay::Point tempP;
	for (int i = 0; i < nParticles; i++){
		tempP[0] = positionx[i];
		tempP[1] = positiony[i];
		DPVEC.push_back(tempP);
	}
	
	delobject = new Delaunay(DPVEC);
	delobject->Triangulate();
	
	glColor4f(0,0,1, 0.2);
	Delaunay::fIterator fit;
	for (fit = delobject->fbegin();
		 fit != delobject->fend();
		 ++fit){
		
		for(int i=0; i<3; ++i){
			int ptb = delobject->Sym(fit,i);
			if (ptb != -1){
				int pta = delobject->Org(fit);
				float xa = positionx[pta];
				float ya = positiony[pta];
				float xb = positionx[ptb];
				float yb = positiony[ptb];
				ofLine (xa,ya, xb,yb);
			}
		}
	}
	
	delete(delobject);
	//-----------------------------------
	
}

