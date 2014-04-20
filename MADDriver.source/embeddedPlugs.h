//
//  embeddedPlugs.h
//  PPMacho
//
//  Created by C.W. Betts on 8/26/13.
//
//

#ifndef PPMacho_embeddedPlugs_h
#define PPMacho_embeddedPlugs_h

#define __private_extern __attribute__((visibility("hidden")))

__private_extern OSErr main669(OSType , char* , MADMusic* , PPInfoRec *, MADDriverSettings *);
__private_extern OSErr mainAMF(OSType , char* , MADMusic* , PPInfoRec *, MADDriverSettings *);
__private_extern OSErr mainDMF(OSType , char* , MADMusic* , PPInfoRec *, MADDriverSettings *);
__private_extern OSErr mainIT(OSType , char* , MADMusic* , PPInfoRec *, MADDriverSettings *);
__private_extern OSErr mainMADfg(OSType , char* , MADMusic* , PPInfoRec *, MADDriverSettings *);
__private_extern OSErr mainMADH(OSType , char* , MADMusic* , PPInfoRec *, MADDriverSettings *);
__private_extern OSErr mainMADI(OSType , char* , MADMusic* , PPInfoRec *, MADDriverSettings *);
__private_extern OSErr mainMED(OSType , char* , MADMusic* , PPInfoRec *, MADDriverSettings *);
__private_extern OSErr mainMOD(OSType , char* , MADMusic* , PPInfoRec *, MADDriverSettings *);
__private_extern OSErr mainMTM(OSType , char* , MADMusic* , PPInfoRec *, MADDriverSettings *);
__private_extern OSErr mainOkta(OSType , char* , MADMusic* , PPInfoRec *, MADDriverSettings *);
__private_extern OSErr mainS3M(OSType , char* , MADMusic* , PPInfoRec *, MADDriverSettings *);
__private_extern OSErr mainULT(OSType , char* , MADMusic* , PPInfoRec *, MADDriverSettings *);
__private_extern OSErr mainUMX(OSType , char* , MADMusic* , PPInfoRec *, MADDriverSettings *);
__private_extern OSErr mainXM(OSType , char* , MADMusic* , PPInfoRec *, MADDriverSettings *);

#endif
