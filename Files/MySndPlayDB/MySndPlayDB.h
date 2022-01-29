/*
**	Apple Macintosh Developer Technical Support
**
**	Headers for soutines demonstrating how to create a function that works
**	much like the original SndPlayDoubleBuffer but is Carbon compatible
**	(which SndPlayDoubleBuffer isn't).
**
**	by Mark Cookson, Apple Developer Technical Support
**
**	File:	MySndPlayDB.h
**
**	Copyright ©1999 Apple Computer, Inc.
**	All rights reserved.
**
**	You may incorporate this sample code into your applications without
**	restriction, though the sample code has been provided "AS IS" and the
**	responsibility for its operation is 100% yours.  However, what you are
**	not permitted to do is to redistribute the source as "Apple Sample
**	Code" after having made changes. If you're going to re-distribute the
**	source, we require that you make it clear in the source that the code
**	was descended from Apple Sample Code, but that you've made changes.
*/

#pragma export on
				OSErr	MySndPlayDoubleBuffer (SndChannelPtr chan, PPSndDoubleBufferHeaderPtr theParams);
				OSErr	MySndDoImmediate (SndChannelPtr chan, SndCommand * cmd);
#pragma export off
static	pascal	void	MySndPlayDoubleBufferCleanUpProc (SndChannelPtr theChannel, SndCommand * theCallBackCmd);
static	pascal	void	MySndPlayDoubleBufferCallBackProc (SndChannelPtr theChannel, SndCommand * theCmd);
static			void	InsertSndDoCommand (SndChannelPtr chan, SndCommand * theCmd);
static	pascal	void	NMResponseProc (NMRecPtr nmReqPtr);
