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

__private_extern MADErr main669(OSType , char* , MADMusic* , PPInfoRec *, MADDriverSettings *);
__private_extern MADErr mainAMF(OSType , char* , MADMusic* , PPInfoRec *, MADDriverSettings *);
__private_extern MADErr mainDMF(OSType , char* , MADMusic* , PPInfoRec *, MADDriverSettings *);
__private_extern MADErr mainIT(OSType , char* , MADMusic* , PPInfoRec *, MADDriverSettings *);
__private_extern MADErr mainMADfg(OSType , char* , MADMusic* , PPInfoRec *, MADDriverSettings *);
__private_extern MADErr mainMADH(OSType , char* , MADMusic* , PPInfoRec *, MADDriverSettings *);
__private_extern MADErr mainMADI(OSType , char* , MADMusic* , PPInfoRec *, MADDriverSettings *);
__private_extern MADErr mainMED(OSType , char* , MADMusic* , PPInfoRec *, MADDriverSettings *);
__private_extern MADErr mainMOD(OSType , char* , MADMusic* , PPInfoRec *, MADDriverSettings *);
__private_extern MADErr mainMTM(OSType , char* , MADMusic* , PPInfoRec *, MADDriverSettings *);
__private_extern MADErr mainOkta(OSType , char* , MADMusic* , PPInfoRec *, MADDriverSettings *);
__private_extern MADErr mainS3M(OSType , char* , MADMusic* , PPInfoRec *, MADDriverSettings *);
__private_extern MADErr mainULT(OSType , char* , MADMusic* , PPInfoRec *, MADDriverSettings *);
__private_extern MADErr mainUMX(OSType , char* , MADMusic* , PPInfoRec *, MADDriverSettings *);
__private_extern MADErr mainXM(OSType , char* , MADMusic* , PPInfoRec *, MADDriverSettings *);

#endif
