/* ------------------ GetFileIcon.h ------------------- */

#ifndef __GETFILEICON__
#define __GETFILEICON__

#include <Icons.h>
#include <Finder.h>

pascal OSErr GetFileIcon( FSSpec *thing, IconSelectorValue iconSelector, Handle *theSuite);
Boolean IsSuiteEmpty( Handle theSuite );

#endif // __GETFILEICON__
