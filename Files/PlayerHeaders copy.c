/*
 *	MacHeaders.c
 *
 *	Script to generate the 'MacHeaders<xxx>' precompiled header for Metrowerks C/C++.
 *  Copyright © 1993 metrowerks inc.  All rights reserved.
 */

/*
 *	Required for c-style toolbox glue function: c2pstr and p2cstr
 *	the inverse operation (pointers_in_A0) is performed at the end ...
 */

#if !(powerc || __CFM68K__)
 #pragma d0_pointers on
#endif

/*
 *	To allow the use of ToolBox calls which have now become obsolete on PowerPC, but
 *	which are still needed for System 6 applications, we need to #define OBSOLETE.  If
 *	your application will never use these calls then you can comment out this #define.
 *	NB: This is only for 68K ...
 */

#if !defined(powerc) && !defined(OBSOLETE)
 #define OBSOLETE	1
#endif

#define OLDROUTINENAMES 1

/*
 *	Metrowerks-specific definitions
 *
 *	These definitions are commonly used but not in Apple's headers. We define
 *	them in our precompiled header so we can use the Apple headers without modification.
 */


#define topLeft(r)	(((Point *) &(r))[0])
#define botRight(r)	(((Point *) &(r))[1])

#define TRUE		true
#define FALSE		false

#ifndef powerc
 #include <MixedMode.h>
 long GetCurrentA5(void)
  ONEWORDINLINE(0x200D);
#endif

/*
 *	Apple #include files
 *
 *	Uncomment any additional #includes you want to add to MacHeaders.
 */

	#include <Movies.h>
	#include <AEObjects.h>
	#include <AEPackObject.h>
	#include <AERegistry.h>
	#include <AEUserTermTypes.h>
	#include <AIFF.h>
	#include <Aliases.h>
	#include <AppleEvents.h>

//	#include <AppleGuide.h>
	#include <AppleScript.h>
//	#include <AppleTalk.h>
//	#include <ASDebugging.h>
//	#include <ASRegistry.h>
//	#include <Balloons.h>
//	#include <CMApplication.h>
//	#include <CMComponent.h>
//	#include <CodeFragments.h>
	#include <ColorPicker.h>
//	#include <CommResources.h>
//	#include <Components.h>
	#include <ConditionalMacros.h>
//	#include <Connections.h>
//	#include <ConnectionTools.h>
	#include <Controls.h>
	#include <ControlDefinitions.h>
//	#include <ControlStrip.h>
//	#include <CRMSerialDevices.h>
//	#include <CTBUtilities.h>
//	#include <CursorCtl.h>
//	#include <CursorDevices.h>
//	#include <DatabaseAccess.h>
//	#include <DeskBus.h>
	#include <Devices.h>
	#include <Dialogs.h>
//	#include <Dictionary.h>
//	#include <DisAsmLookup.h>
//	#include <Disassembler.h>
	#include <DiskInit.h>
//	#include <Disks.h>
//	#include <Displays.h>
	#include <Drag.h>
//	#include <Editions.h>
//	#include <ENET.h>
//	#include <EPPC.h>
//	#include <ErrMgr.h>
	#include <Errors.h>
	#include <Events.h>
//	#include <fenv.h>
	#include <Files.h>
//	#include <FileTransfers.h>
//	#include <FileTransferTools.h>
	#include <FileTypesAndCreators.h>
//	#include <Finder.h>
//	#include <FixMath.h>
	#include <Folders.h>
	#include <Fonts.h>
//	#include <fp.h>
//	#include <FragLoad.h>
//	#include <FSM.h>
	#include <Gestalt.h>
//	#include <HyperXCmd.h>
//	#include <Icons.h>
//	#include <ImageCodec.h>
//	#include <ImageCompression.h>
//	#include <IntlResources.h>
//	#include <Language.h>
	#include <Lists.h>
	#include <LowMem.h>
//	#include <MachineExceptions.h>
//	#include <MacTCP.h>
//	#include <MediaHandlers.h>
	#include <Memory.h>
	#include <Menus.h>
//	#include <MIDI.h>
	#include <MixedMode.h>
//	#include <Movies.h>
//	#include <MoviesFormat.h>
//	#include <Notification.h>
//	#include <OSA.h>
//	#include <OSAComp.h>
//	#include <OSAGeneric.h>
	#include <OSUtils.h>
	#include <Packages.h>
//	#include <Palettes.h>
//	#include <Picker.h>
//	#include <PictUtil.h>
//	#include <PictUtils.h>

	#include <PLStringFuncs.h>
//	#include <Power.h>
//	#include <PPCToolbox.h>
	#include <Printing.h>
	#include <Processes.h>
//	#include <QDOffscreen.h>
	#include <Quickdraw.h>
//	#include <QuickdrawText.h>
//	#include <QuickTimeComponents.h>
	#include <Resources.h>
//	#include <Retrace.h>
//	#include <ROMDefs.h>

	#include <Scrap.h>
	#include <Script.h>
//	#include <SCSI.h>
	#include <SegLoad.h>
//	#include <Serial.h>
//	#include <ShutDown.h>
//	#include <Slots.h>
	#include <Sound.h>
	#include <SoundComponents.h>
	#include <SoundInput.h>
//	#include <Speech.h>
	#include <StandardFile.h>
//	#include <Start.h>
	#include <Strings.h>
//	#include <Terminals.h>
//	#include <TerminalTools.h>
	#include <TextEdit.h>
//	#include <TextServices.h>
	#include <TextUtils.h>
//	#include <Threads.h>
//	#include <Timer.h>
	#include <ToolUtils.h>
//	#include <Translation.h>
//	#include <TranslationExtensions.h>
	#include <Traps.h>
//	#include <TSMTE.h>
	#include <Types.h>
//	#include <Unmangler.h>
//	#include <Video.h>
	#include <Windows.h>
//	#include <WorldScript.h>


	#include "Drag.h"


	#include "navigation.h"
	#include "string.h"
	#include "stdlib.h"
	#include "stdio.h"

/*
 *	required for c-style toolbox glue function: c2pstr and p2cstr
 *	(match the inverse operation at the start of the file ...
 */

#if !(powerc || __CFM68K__)
 #pragma d0_pointers reset
#endif

void			MyDisposPtr( Ptr*);
void			MyDisposHandle( Handle*);
Handle			MyNewHandle( long);
Ptr 			MyNewPtr( long);



/***************/

#include "shuddup.h"
#include "MAD.h"
#include "RDriver.h"
#include "RDriverInt.h"
#include "Undo.h"
#include "PrivateList.h"
#include "PPPlug.h"
#include "VA.h"
#include "MIDI.h"

/**/

#include "3D.proto.h"
#include "About.proto.h"
#include "ActiveHelp.proto.h"
#include "Adaptators.proto.h"
#include "AEvent.proto.h"
#include "Check.proto.h"
#include "ClassicalP.proto.h"
#include "Command Period.proto.h"
#include "Command.proto.h"
#include "DataFiles.proto.h"
#include "DigitalInfo.proto.h"
#include "DragManager.proto.h"
#include "Editor.proto.h"
#include "EtoileAsm.proto.h"
#include "fade.proto.h"
#include "FFT.proto.h"
#include "FileUtils.proto.h"
#include "FillPoly.proto.h"
#include "Fourier.proto.h"
#include "FourierInt.proto.h"
#include "FourierNEW.proto.h"
#include "Functions.proto.h"
#include "GetFileIcon.proto.h"
#include "Help.proto.h"
#include "HelpOnline.proto.h"
#include "InstruList.proto.h"
#include "InstruView.proto.h"
#include "LDEF.proto.h"
#include "Liste.proto.h"
#include "Main.proto.h"
#include "Main3D.proto.h"
#include "MapPoly.proto.h"
#include "Memory.proto.h"
#include "MIDI-GM.proto.h"
#include "MIDI-Hardware.proto.h"
#include "MIDI.proto.h"
#include "MODList.proto.h"
#include "Mozart.proto.h"
#include "Oscillo.proto.h"
#include "Partition.proto.h"
#include "Pattern.proto.h"
#include "Piano.proto.h"
#include "PlugImport.proto.h"
#include "PlugsInit.proto.h"
#include "PlugsPPDGInit.proto.h"
#include "Print.proto.h"
#include "PrintTEHandle.proto.h"
#include "PrivateList.proto.h"
#include "Progress.proto.h"
#include "ProgressBar.proto.h"
#include "PubWindow.proto.h"
#include "Quicktime.proto.h"
#include "Record.proto.h"
#include "Registration.proto.h"
#include "Samples.proto.h"
#include "SoundInput.proto.h"
#include "Spectrum.proto.h"
#include "StaffEditor.proto.h"
#include "TextScroll.proto.h"
#include "ThreadSystem.proto.h"
#include "Tools.proto.h"
#include "TrackView.proto.h"
#include "Undo.proto.h"
#include "Utils.proto.h"
#include "VAHeart.proto.h"
#include "WaveEditor.proto.h"
#include "ZoomRect.proto.h"
#include "QuicktimeWindow.proto.h"
#include "Equalizer.proto.h"
/**************/
