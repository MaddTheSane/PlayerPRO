/* ------------------ GetFileIcon.h ------------------- */

#ifndef __GETFILEICON__
#define __GETFILEICON__

#include <Carbon/Carbon.h>

pascal OSErr GetFileIcon( FSSpec *thing, IconSelectorValue iconSelector, Handle *theSuite);
Boolean IsSuiteEmpty( Handle theSuite );

#endif // __GETFILEICON__
