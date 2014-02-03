Developer Preview 7
===

Developer Preview 7 fixes an issue with the instrument list improperly refreshing the view (there may be cases where it still improperly refreshes the view). Also the application will try to get the "File Type" from the file's extension via UTI functions; it will also try to get the "File Type" by passing the actual File Type OSType to the UTI functions..

Regressions:

* The music list window won't be flushed when the app is first launched.

Other changes:

* Replaced usage of APIs that are useless or even harmful on OS X (at least, how they were used), specifically `MemFree()` and `MemFlush()`.
* 10.4 is needed for PowerPC, while 10.5 is needed for Intel.
* PowerPC is given a higher priority than Intel. This will probably be removed when I'm certain that the needed data types have been properly byte swapped.
* Replace some usage of outdated Carbon File APIs with more modern Carbon File APIs.
* Remove the Memory window source files: we don't need to worry about memory usage on OS X.
* Force the line ending of source files to UNIX in the Xcode project.
* Code refactoring and indentation.
