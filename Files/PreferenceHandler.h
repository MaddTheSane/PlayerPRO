//
//  PreferenceHandler.h
//  PPMacho
//
//  Created by C.W. Betts on 1/19/14.
//
//

#ifndef PPMacho_PreferenceHandler_h
#define PPMacho_PreferenceHandler_h

#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CoreFoundation.h>

#ifdef __cplusplus
extern "C" {
#endif

void RegisterCFDefaults();
void ResetCFPreferences();

void ReadCFPreferences();
void WriteCFPreferences();

#ifdef __cplusplus
}
#endif

#endif
