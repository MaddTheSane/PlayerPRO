//
//  embeddedPlugs.h
//  PPMacho
//
//  Created by C.W. Betts on 8/26/13.
//
//

#ifndef PPMacho_embeddedPlugs_h
#define PPMacho_embeddedPlugs_h

#ifdef __OBJC__
#import <Foundation/Foundation.h>
#endif

#define __private_extern __attribute__((visibility("hidden")))

__private_extern MADErr main669(MADFourChar, char*, MADMusic*, MADInfoRec *, MADDriverSettings *);
__private_extern MADErr mainAMF(MADFourChar, char*, MADMusic*, MADInfoRec *, MADDriverSettings *);
__private_extern MADErr mainDMF(MADFourChar, char*, MADMusic*, MADInfoRec *, MADDriverSettings *);
__private_extern MADErr mainIT(MADFourChar, char*, MADMusic*, MADInfoRec *, MADDriverSettings *);
__private_extern MADErr mainMADfg(MADFourChar, char*, MADMusic*, MADInfoRec *, MADDriverSettings *);
__private_extern MADErr mainMADH(MADFourChar, char*, MADMusic*, MADInfoRec *, MADDriverSettings *);
__private_extern MADErr mainMADI(MADFourChar, char*, MADMusic*, MADInfoRec *, MADDriverSettings *);
__private_extern MADErr mainMED(MADFourChar, char*, MADMusic*, MADInfoRec *, MADDriverSettings *);
__private_extern MADErr mainMOD(MADFourChar, char*, MADMusic*, MADInfoRec *, MADDriverSettings *);
__private_extern MADErr mainMTM(MADFourChar, char*, MADMusic*, MADInfoRec *, MADDriverSettings *);
__private_extern MADErr mainOkta(MADFourChar, char*, MADMusic*, MADInfoRec *, MADDriverSettings *);
__private_extern MADErr mainS3M(MADFourChar, char*, MADMusic*, MADInfoRec *, MADDriverSettings *);
__private_extern MADErr mainULT(MADFourChar, char*, MADMusic*, MADInfoRec *, MADDriverSettings *);
__private_extern MADErr mainUMX(MADFourChar, char*, MADMusic*, MADInfoRec *, MADDriverSettings *);
__private_extern MADErr mainXM(MADFourChar, char*, MADMusic*, MADInfoRec *, MADDriverSettings *);

#endif
