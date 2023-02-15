//
//  PlugVST.h
//  PPMacho
//
//  Created by C.W. Betts on 2/15/23.
//

#ifndef PlugVST_h
#define PlugVST_h

#include <Carbon/Carbon.h>
#include "RDriver.h"

extern void VSTEditorDoItemPress(short itemHit, DialogPtr aDia);
extern void FillVSTEffects(void);
extern Boolean VSTEditorClose(DialogPtr aDia, short itemHit);
extern VSTEffect* CreateVSTEffect(short effectID);
extern void DisposeVSTEffect(VSTEffect *myEffect);
extern void ProcessVSTPlug(MADDriverRec *intDriver, long *data, long datasize, short channel);
extern Boolean IsVSTChanEffect(MADDriverRec *intDriver, short channel);
extern void CheckVSTEditor(VSTEffect *ce);
extern void ApplyVSTSets(VSTEffect* myEffect, FXSets* set);
extern short ConvertUniqueIDToIndex(long);

#endif /* PlugVST_h */
