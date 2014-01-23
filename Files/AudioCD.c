#include "Shuddup.h"
#include "MAD.h"
#include "RDriver.h"
#include "PPPlug.h"
#include <Carbon/Carbon.h>
#include <QuickTime/QuickTime.h>
#include "Undo.h"

//OSErr DoStandardOpen(FSSpec	*spec, Str255 string, OSType inType);

pascal OSErr MyProgressUPP(Movie theMovie, short message, short whatOperation, Fixed percentDone, long refcon)
{
	UpdateALLWindow();
	RollCursor();
	return noErr;
}

void ImportAudioCD()
{
	MovieImportComponent	ci;
	FSSpec			newfile;
	OSErr			iErr;
	Boolean			canceled;
	FSSpec			file;
	Movie 			theMovie;
	Track			usedTrack;
	TimeValue		addedDuration;
	long			outFlags;
	short			resRefNum, ins, samp;
	FInfo			fndrInfo;
	MovieProgressUPP	progressUPP;
	
	if (!GetIns(&ins, &samp)) {
		Erreur(13, ins);
		return;
	}
	
	progressUPP = NewMovieProgressUPP(MyProgressUPP);
	
	if (DoStandardOpen(&file, "\pAudio CD", kQTFileTypeAudioCDTrack) == noErr)
	{
		iErr = FSpGetFInfo(&file, &fndrInfo);
		
		ci = OpenDefaultComponent (MovieImportType, kQTFileTypeAudioCDTrack);
		
		if (ci)
		{
			iErr = MovieImportDoUserDialog(		ci, 
								&file, 
								NULL,
								&canceled);
			
			iErr = MovieImportSetProgressProc(	ci, progressUPP, 0);
			
			UpdateALLWindow();
			
			if (!canceled && iErr == noErr)
			{
				pStrcpy(newfile.name, file.name);
				
				iErr = FindFolder(kOnSystemDisk, kTrashFolderType, kCreateFolder, &newfile.vRefNum, &newfile.parID);	// kDesktopFolderType, kTemporaryFolderType 
				if (iErr == noErr)
				{
					/////////////////////////////////////////////////
					//		AIFF CONVERSION
					/////////////////////////////////////////////////
					
					SetCursor(&watchCrsr);
					
					resRefNum = 0;
					
					FSpDelete(&newfile);
					
					iErr = CreateMovieFile(	&newfile,
					      		       'SNPL',
					      		       smSystemScript,
					      		       createMovieFileDeleteCurFile,
					      		       &resRefNum,
					      		       &theMovie);
					if (!iErr)
					{
						usedTrack = 0;
						addedDuration = 0;
						outFlags = 0;
						
						iErr = MovieImportFile(ci, &file, theMovie, 0, &usedTrack, 0, &addedDuration, movieImportCreateTrack, &outFlags);
						
						CloseMovieFile (resRefNum);
						
						DisposeMovie(theMovie);
						
						if (!iErr)
						{
							/////////////////////////////////////////////////
							
							iErr = FSpGetFInfo(&newfile, &fndrInfo);
							fndrInfo.fdCreator = 'SNPL';
							fndrInfo.fdType = 'AIFF';
							iErr = FSpSetFInfo(&newfile, &fndrInfo);
							
							curMusic->hasChanged = true;
							
							SaveUndo(UAllSamples, 0, "\pUndo 'Import Audio CD Track'");
							
							iErr = NOpenSampleInt(ins, samp, newfile);
							MADErreur(iErr);
							
							CreateInstruList();
							DrawInfoInstrument();
							UpdateSampleWindows();
							UpdateInstruMenu();
						}
						
						FSpDelete(&newfile);
					}
					
					SetCursor(GetQDGlobalsArrow(&qdarrow));
				}
			}
			CloseComponent(ci);
		}
	}
	
	DisposeMovieProgressUPP(progressUPP);
}
/*
void ImportAudioCD()
{
	MovieImportComponent	ci;
	OSType					fileType;
	FSSpec					newfile2;
	OSErr					iErr;
	Boolean					canceled;
	FSSpec					file;
	Movie 					theMovie;
	Track					usedTrack;
	TimeValue				addedDuration;
	long					outFlags;
	short					ins, samp;
	FInfo					fndrInfo;
	
	if (!GetIns(&ins, &samp))
	{
		Erreur(13, ins);
		return;
	}
	
	if (DoStandardOpen(&file, "\pAudio CD", 'MPEG') == noErr)
	{
		ci = OpenDefaultComponent (MovieImportType, 'MPEG');
		
		if (ci)
		{
			canceled = false;
			if (!canceled && iErr == noErr)
			{
				pStrcpy(newfile2.name, file.name);
				
				iErr = FindFolder(kOnSystemDisk, kTemporaryFolderType, kCreateFolder, &newfile2.vRefNum, &newfile2.parID);	//
				if (iErr == noErr)
				{
					/////////////////////////////////////////////////
					//		AIFF CONVERSION
					/////////////////////////////////////////////////
					
					SetCursor(&watchCrsr);
					
					theMovie = NewMovie(0);
					
					if (!iErr)
					{
						usedTrack = 0;
						addedDuration = 0;
						outFlags = 0;
						
						iErr = MovieImportFile(ci, &file, theMovie, 0, &usedTrack, 0, &addedDuration, movieImportCreateTrack, &outFlags);
						
						FSpDelete(&newfile2);
						
						iErr = ConvertMovieToFile(	theMovie,
										0,
									 	&newfile2,
									 	'AIFF',
									 	'SNPL',
									 	smCurrentScript,
									 	NULL,
									 	0,
									 	0);
						
						DisposeMovie(theMovie);
						
						if (!iErr)
						{
							/////////////////////////////////////////////////
							
							iErr = FSpGetFInfo(&newfile2, &fndrInfo);
							fndrInfo.fdCreator = 'SNPL';
							fndrInfo.fdType = 'AIFF';
							iErr = FSpSetFInfo(&newfile2, &fndrInfo);
							
							curMusic->hasChanged = true;
							
							SaveUndo(UAllSamples, 0, "\pUndo 'Import Audio CD Track'");
							
							iErr = NOpenSampleInt(ins, samp, newfile2);
							MADErreur(iErr);
							
							CreateInstruList();
							DrawInfoInstrument();
							UpdateSampleWindows();
							UpdateInstruMenu();
						}
						
						FSpDelete(&newfile2);
					}
					
					SetCursor(GetQDGlobalsArrow(&qdarrow));
				}
			}
			CloseComponent(ci);
		}
	}
}*/

/*OSErr ConvertDataToAIFF(FSSpec file, FSSpec *newfile)
{
	MovieImportComponent	ci;
	OSType					fileType;
	OSErr					iErr;
	Boolean					canceled;
	Movie 					theMovie;
	Track					usedTrack;
	TimeValue				addedDuration;
	long					i, outFlags;
	short					resRefNum, ins, samp, resId, iFileRefI;
	FInfo					fndrInfo;
	Cursor					watchCrsr;
	CursHandle				myCursH;
	Str255					resName;
	Boolean					dataRefWasChanged;
	
	myCursH = GetCursor(357);
	
	if (myCursH == NULL) Debugger();
	DetachResource((Handle) myCursH);		HLock((Handle) myCursH);
	watchCrsr = **myCursH;					HUnlock((Handle) myCursH);		DisposeHandle((Handle) myCursH);
	
	ci = OpenDefaultComponent (MovieExportType, kQTFileTypeAIFF);
	
	if (ci)
	{
		resRefNum = 0;
		
		iErr = OpenMovieFile (&file, &resRefNum, fsCurPerm);
		if (iErr) DebugStr("\p1");
		
		resId = 0;
		iErr = NewMovieFromFile(&theMovie, resRefNum, &resId, resName, 0, &dataRefWasChanged);
		if (iErr) DebugStr("\p2");
		
		canceled = false;
		
		if (!canceled && iErr == noErr)
		{
			pStrcpy(newfile->name, file.name);
			
			iErr = FindFolder(kOnSystemDisk, kTemporaryFolderType, kCreateFolder, &newfile->vRefNum, &newfile->parID);
			if (iErr == noErr)
			{
				/////////////////////////////////////////////////
				//		AIFF CONVERSION
				/////////////////////////////////////////////////
				
				
				SetCursor(&watchCrsr);
				
				iErr = FSpDelete(newfile);
				
				ConvertMovieToFile(	theMovie,
							0,
						 	newfile,
						 	'AIFF',
						 	'SNPL',
						 	smCurrentScript,
						 	NULL,
						 	0,
						 	0);
				
				DisposeMovie(theMovie);
				
				CloseMovieFile(resRefNum);
				
				if (!iErr)
				{
					/////////////////////////////////////////////////
					
					iErr = FSpGetFInfo(newfile, &fndrInfo);
					fndrInfo.fdCreator = 'SNPL';
					fndrInfo.fdType = 'AIFF';
					iErr = FSpSetFInfo(newfile, &fndrInfo);
					
					curMusic->hasChanged = true;
					
					SaveUndo(UAllSamples, 0, "\pUndo 'Import Audio CD Track'");
					
					iErr = NOpenSampleInt(ins, samp, newfile);
					MADErreur(iErr);
					
					CreateInstruList();
					DrawInfoInstrument();
					UpdateSampleWindows();
					UpdateInstruMenu();
				}
				
			}
		}
		CloseComponent(ci);
	}
	else DebugStr("\p66");
	
	return iErr;
}*/
