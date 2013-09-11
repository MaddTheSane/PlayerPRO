//
//  MADTypeConversions.h
//  PPMacho
//
//  Created by C.W. Betts on 9/11/13.
//
//

#ifndef PPMacho_MADTypeConversions_h
#define PPMacho_MADTypeConversions_h

#include "RDriver.h"

OSErr ConvertPlugInfoToUnicode(PPInfoRec infoIn, PPInfoRecU *infoOut);
OSErr ConvertMusicToUnicode(MADMusic *musIn, MADMusicUnicode *musOut);
OSErr ConvertMusicFromUnicode(MADMusicUnicode *musIn, MADMusic *musOut);

#endif
