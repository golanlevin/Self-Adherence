/*
 *  IppLoader.cpp
 *
 *  Created by Golan Levin on 1/24/10.
 *  Copyright 2010 Carnegie Mellon University. All rights reserved.
 *
 */

#include "IppLoader.h"
#define UINT32 unsigned int

void printCpuType(IppCpuType cpuType)
{
    if(cpuType==0) {printf("ippCpuUnknown 0x00\n"); return;}
    if(cpuType==0x01) {printf("ippCpuPP 0x01 Intel Pentium processor\n");return;}
    if(cpuType==0x02) {printf("ippCpuPMX 0x02 Pentium processor with MMX technology\n");return;}
    if(cpuType==0x03) {printf("ippCpuPPR 0x03 Pentium Pro processor\n");return;}
    if(cpuType==0x04) {printf("ippCpuPII 0x04 Pentium II processor\n");return;}
    if(cpuType==0x05) {printf("ippCpuPIII 0x05 Pentium III processor and Pentium III Xeon processor\n");return;}
    if(cpuType==0x06) {printf("ippCpuP4 0x06 Pentium 4 processor and Intel Xeon processor\n");return;}
    if(cpuType==0x07) {printf("ippCpuP4HT 0x07 Pentium 4 Processor with HT Technology\n");return;}
    if(cpuType==0x08) {printf("ippCpuP4HT2 0x08 Pentium 4 processor with Streaming SIMD Extensions 3\n");return;}
    if(cpuType==0x09) {printf("ippCpuCentrino 0x09 Intel Centrino mobile technology\n");return;}
    if(cpuType==0x0a) {printf("ippCpuCoreSolo 0x0a Intel Core Solo processor\n");return;}
    if(cpuType==0x0b) {printf("ippCpuCoreDuo 0x0b Intel Core Duo processor\n");return;}
    if(cpuType==0x10) {printf("ippCpuITP 0x10 Intel Itanium processor\n");return;}
    if(cpuType==0x11) {printf("ippCpuITP2 0x11 Intel Itanium 2 processor\n");return;}
    if(cpuType==0x20) {printf("ippCpuEM64T 0x20 Intel 64 Instruction Set Architecture\n");return;}
    if(cpuType==0x21) {printf("ippCpuC2D 0x21 Intel Core 2 Duo processor\n");return;}
    if(cpuType==0x22) {printf("ippCpuC2Q 0x22 Intel Core 2 Quad processor\n");return;}
    if(cpuType==0x23) {printf("ippCpuPenryn 0x23 Intel Core 2 processor with Intel SSE4.1\n");return;}
    if(cpuType==0x24) {printf("ippCpuBonnell 0x24 Intel Atom processor\n");return;}
    if(cpuType==0x25) {printf("ippCpuNehalem 0x25\n"); return;}
    if(cpuType==0x26) {printf("ippCpuNext 0x26\n"); return;}
    if(cpuType==0x40) {printf("ippCpuSSE 0x40 Processor supports Streaming SIMD Extensions instruction set\n");return;}
    if(cpuType==0x41) {printf("ippCpuSSE2 0x41 Processor supports Streaming SIMD Extensions 2 instruction set\n");return;}
    if(cpuType==0x42) {printf("ippCpuSSE3 0x42 Processor supports Streaming SIMD Extensions 3 instruction set\n");return;}
    if(cpuType==0x43) {printf("ippCpuSSSE3 0x43 Processor supports Supplemental Streaming SIMD Extension 3 instruction set\n");return;}
    if(cpuType==0x44) {printf("ippCpuSSE41 0x44 Processor supports Streaming SIMD Extensions 4.1 instruction set\n");return;}
    if(cpuType==0x45) {printf("ippCpuSSE42 0x45 Processor supports Streaming SIMD Extensions 4.2 instruction set\n");return;}
    if(cpuType==0x60) {printf("ippCpuX8664 0x60 Processor supports 64 bit extension\n");return;}
    else printf("CPU UNKNOWN\n");
    return;
}
void printCpuCapability(Ipp64u pStatus)
{
	printf("pStatus=%d\n",(UINT32)pStatus);
	if((UINT32)pStatus & ippCPUID_MMX) printf("Intel Architecture MMX technology supported\n");
	if((UINT32)pStatus & ippCPUID_SSE) printf("Streaming SIMD Extensions\n");
	if((UINT32)pStatus & ippCPUID_SSE2) printf("Streaming SIMD Extensions 2\n");
	if((UINT32)pStatus & ippCPUID_SSE3) printf("Streaming SIMD Extensions 3\n");
	if((UINT32)pStatus & ippCPUID_SSSE3) printf("Supplemental Streaming SIMD Extensions 3\n");
	if((UINT32)pStatus & ippCPUID_MOVBE) printf("The processor supports MOVBE instruction\n");
	if((UINT32)pStatus & ippCPUID_SSE41) printf("Streaming SIMD Extensions 4.1\n");
	if((UINT32)pStatus & ippCPUID_SSE42) printf("Streaming SIMD Extensions 4.2\n");
}


//===================================================================
IppLoader::IppLoader(){
	initializeIpp();
}

//===================================================================
void IppLoader::initializeIpp(){
	bool bVerbose = true;
	
	IppStatus status = ippStaticInitCpu(ippGetCpuType());
	if (bVerbose) cout << status << " " << ippStsNoErr << " " << ippStsCpuMismatch << " " << ippStsNoOperationInDll << endl;
	
	/*   ippStsNoErr       - required target cpu library code is successfully set
	//   ippStsCpuMismatch - required target cpu library can't be set, the previous set is used
	//   ippStsNoOperationInDll 
	*/
	
	const IppLibraryVersion* lib = ippiGetLibVersion();
	if (bVerbose) printf("%s %s %d.%d.%d.%d\n", lib->Name, lib->Version,lib->major, lib->minor, lib->majorBuild, lib->build);
	
	IppCpuType cpuType;
    IppStatus pStatus;
    Ipp64u pFeatureMask;
    Ipp32u pCpuidInfoRegs[4];
	
    cpuType=ippGetCpuType();
    pStatus=ippGetCpuFeatures(&pFeatureMask, pCpuidInfoRegs);
	
    if (bVerbose) printCpuType(cpuType);
    if (bVerbose) printCpuCapability(pFeatureMask);
	
}
