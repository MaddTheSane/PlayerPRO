//
//  AppDelegate.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/18/14.
//
//

import Cocoa
import PlayerPROCore
import PlayerPROKit

private func makeNSRGB(red: UInt16, green: UInt16, blue:UInt16) -> NSColor {
	return NSColor(calibratedRed: CGFloat(red) / CGFloat(UInt16.max), green: CGFloat(green) / CGFloat(UInt16.max), blue: CGFloat(blue) / CGFloat(UInt16.max), alpha: 1)
}

func CocoaDebugStr (line: Int16, file: UnsafePointer<Int8>, text: UnsafePointer<Int8>) {
	let swiftFile: String = NSString(UTF8String: file)
	let swiftText: String = NSString(UTF8String: text)
	NSLog("\(swiftFile):\(line), error text: \(swiftText)")
	let errStr = NSLocalizedString("MyDebugStr_Error", comment: "Error")
	let mainStr = NSLocalizedString("MyDebugStr_MainText", comment: "The Main text to display")
	let quitStr = NSLocalizedString("MyDebugStr_Quit", comment: "Quit")
	let contStr = NSLocalizedString("MyDebugStr_Continue", comment: "Continue")
	let debuStr = NSLocalizedString("MyDebugStr_Debug", comment: "Debug")
	//NSLog("%s:%u error text:%s!", file, line, text);

	let alert = PPRunCriticalAlertPanel(errStr, message: mainStr, defaultButton: quitStr, alternateButton: contStr, otherButton: debuStr, args: swiftText)
	switch (alert) {
	case NSAlertAlternateReturn:
		break;
		
	case NSAlertOtherReturn:
		assert(false, "Chose to go to debugger.");
		break;
		
	case NSAlertDefaultReturn:
		NSLog("Choosing to fail!");
		fallthrough
	default:
		abort();
		break;
	}
}

internal var globalMadLib: PPLibrary {
	get {
		return ((NSApp as NSApplication).delegate as AppDelegate).madLib
	}
}

class AppDelegate: NSDocumentController, NSApplicationDelegate, ExportObjectDelegate {
	private var exportObjects = [ExportObject]()
	private var _trackerDict = [String: [String]]()
	private var _trackerUTIs = [String]()
	var plugInInfos = [PlugInInfo]()
	let madLib = PPLibrary()
	let instrumentPlugHandler = PPInstrumentPlugHandler()
	let digitalHandler = DigitalPlugHandler()
	let filterHandler = PPFilterPlugHandler()
	let complexImport = ComplexImportPlugHandler()
	let preferences = Preferences.newPreferenceController()
	var thePPColors = [NSColor]()
	
	@IBOutlet weak var musicExportMenu:			NSMenu!
	@IBOutlet weak var aboutPlugInMenu:			NSMenu!
	@IBOutlet weak var instrumentExportMenu:	NSMenu!
	@IBOutlet weak var newInstrumentMenu:		NSMenu!
	@IBOutlet weak var exportStatusPanel:		NSPanel!
	
	var trackerDict: [String: [String]] { get {
		if _trackerDict.isEmpty || _trackerDict.count != Int(madLib.pluginCount) + 2 {
			let localMADKName = NSLocalizedString("PPMADKFile", tableName: "InfoPlist", comment: "MADK Tracker")
			let localGenericMADName = NSLocalizedString("Generic MAD tracker", comment: "Generic MAD tracker")
			var tmpTrackerDict = [localMADKName: [MADNativeUTI], localGenericMADName: [MADGenericUTI]] as [String: [String]]
			
			for objRaw in madLib {
				let obj = objRaw as PPLibraryObject
				tmpTrackerDict[obj.menuName] = (obj.UTItypes) as? [String]
			}
			
			for obj in complexImport {
				tmpTrackerDict[obj.menuName] = (obj.UTITypes) as? [String]
			}
			
			_trackerDict = tmpTrackerDict
		}
		return _trackerDict
		}}
	
	var trackerUTIs: [String] { get {
		if _trackerUTIs.isEmpty {
			let arrayOfUTIs = trackerDict.values
			var toAddUTI = [String]()
			for anArray in arrayOfUTIs {
				toAddUTI += anArray
			}
			_trackerUTIs = toAddUTI
		}
		return _trackerUTIs
		}}

	@IBAction func showPreferences(sender: AnyObject?) {
		preferences.window.center()
		preferences.showWindow(sender)
	}

	@IBAction func showPlugInInfo(sender: AnyObject?) {
		let tag = (sender as NSMenuItem).tag;
		if (tag < 0 || tag >= plugInInfos.count) {
			return;
		}
		let inf = plugInInfos[tag];
		
		let infoCont = PlugInInfoController.windowControllerFromInfo(inf)
		infoCont.window.center()
		NSApplication.sharedApplication().runModalForWindow(infoCont.window)
		//[infoCont showWindow:sender];
	}

	func updatePlugInInfoMenu() {
		let trackerPlugName = NSLocalizedString("TrackerPlugName", comment: "Tracker plug-in name")
		let complexTrackerPlugName = NSLocalizedString("ComplexTrackerPlugName", comment: "Tracker plug-in name")
		let instrumentPlugName = NSLocalizedString("InstrumentPlugName", comment: "Instrument plug-in name")
		let digitalPlugName = NSLocalizedString("DigitalPlugName", comment: "Digital plug-in name")
		let filterPlugName = NSLocalizedString("FilterPlugName", comment: "Filter plug-in name")
		
		for rawObj in madLib {
			let obj = rawObj as PPLibraryObject
			let tmpInfo = PlugInInfo(plugName: obj.menuName, author: obj.menuName, plugType: trackerPlugName, plugURL: obj.plugFile.bundleURL)
			let infoArray = plugInInfos.filter({ (hi) -> Bool in
				return hi == tmpInfo
			})
			if infoArray.count == 0 {
				plugInInfos.append(tmpInfo)
			}
		}
		
		for obj in instrumentPlugHandler {
			let tmpInfo = PlugInInfo(plugName: obj.menuName, author: obj.authorString, plugType: instrumentPlugName, plugURL: obj.file.bundleURL)
			let infoArray = plugInInfos.filter({ (hi) -> Bool in
				return hi == tmpInfo
			})
			if infoArray.count == 0 {
				plugInInfos.append(tmpInfo)
			}
		}
		
		for obj in digitalHandler {
			let tmpInfo = PlugInInfo(plugName: obj.menuName, author: obj.authorString, plugType: digitalPlugName, plugURL: obj.file.bundleURL)
			let infoArray = plugInInfos.filter({ (hi) -> Bool in
				return hi == tmpInfo
			})
			if infoArray.count == 0 {
				plugInInfos.append(tmpInfo)
			}
		}
		
		for obj in filterHandler {
			let tmpInfo = PlugInInfo(plugName: obj.menuName, author: obj.authorString, plugType: filterPlugName, plugURL: obj.file.bundleURL)
			let infoArray = plugInInfos.filter({ (hi) -> Bool in
				return hi == tmpInfo
			})
			if infoArray.count == 0 {
				plugInInfos.append(tmpInfo)
			}
		}
		
		for obj in complexImport {
			let tmpInfo = PlugInInfo(plugName: obj.menuName, author: obj.authorString, plugType: complexTrackerPlugName, plugURL: obj.file.bundleURL)
			let infoArray = plugInInfos.filter({ (hi) -> Bool in
				return hi == tmpInfo
			})
			if infoArray.count == 0 {
				plugInInfos.append(tmpInfo)
			}
		}
		
		plugInInfos.sort({ (obj1:PlugInInfo, obj2:PlugInInfo) -> Bool in
			let menuNam1 = obj1.plugName
			let menuNam2 = obj2.plugName
			let res = menuNam1.localizedStandardCompare(menuNam2)
			return res == NSComparisonResult.OrderedAscending;
			
		})
		
		aboutPlugInMenu.removeAllItems()
		
		for (i, pi) in enumerate(plugInInfos) {
			let mi = NSMenuItem(title: pi.plugName, action: "showPlugInInfo:", keyEquivalent: "")
			mi.tag = i
			mi.target = self
			aboutPlugInMenu.addItem(mi)
		}
	}
	
	private func registerDefaults() {
		let tooLargeDict: [String: AnyObject] = [PPSoundDriver: Int(MADSoundOutput.CoreAudioDriver.toRaw())]
		
		let defaults1: [String: AnyObject]  = [PPSoundOutBits: 16,
			PPSoundOutRate: 44100,
			PPStereoDelayToggle: true,
			PPReverbToggle: false,
			PPSurroundToggle: false,
			PPOversamplingToggle: false,
			PPStereoDelayAmount: 30,
			PPReverbAmount: 25,
			PPReverbStrength: 30,
			PPOversamplingAmount: 1]
		
		let defaults2: [String: AnyObject]  = [PPDEShowInstrument: true,
			PPDEShowNote: true,
			PPDEShowEffect: true,
			PPDEShowArgument: true,
			PPDEShowVolume: true,
			PPDEShowMarkers: true,
			PPDEMarkerOffsetPref: 0,
			PPDEMarkerLoopPref: 3,
			PPDEMouseClickControlPref: true,
			PPDEMouseClickShiftPref: false,
			PPDEMouseClickCommandPref: false,
			PPDEMouseClickOptionPref: false,
			PPDELineHeightNormal: true,
			PPDEMusicTraceOn: true,
			PPDEPatternWrappingPartition: true,
			PPDEDragAsPcmd: true]
		
		let defaults3: [String: AnyObject]  = [PPBEMarkersEnabled: true,
			PPBEMarkersOffset: 0,
			PPBEMarkersLoop: 3,
			PPBEOctaveMarkers: true,
			PPBENotesProjection: false,
			PPDEMarkerColorPref: makeNSRGB(65535, 65535, 39321).PPencodeColor(),
			
			PPMAddExtension: true,
			PPMMadCompression: true,
			PPMNoLoadMixerFromFiles: false,
			PPMOscilloscopeDrawLines: true,
			
			PPCEShowNotesLen: false,
			PPCEShowMarkers: true,
			PPCEMarkerOffset: 0,
			PPCEMarkerLoop: 3,
			PPCETempoNum: 4,
			PPCETempoUnit: 4,
			PPCETrackHeight: 130]
		
		let defaults4: [String: AnyObject]  = [
			PPCColor1: makeNSRGB(61166, 0, 0).PPencodeColor(),
			PPCColor2: makeNSRGB(35980, 48316, 7196).PPencodeColor(),
			PPCColor3: makeNSRGB(13107, 65535, 65535).PPencodeColor(),
			PPCColor4: NSColor.yellowColor().PPencodeColor(),
			PPCColor5: makeNSRGB(24672, 51914, 36494).PPencodeColor(),
			PPCColor6: makeNSRGB(13107, 65535, 65535).PPencodeColor(),
			PPCColor7: makeNSRGB(26214, 52428, 65535).PPencodeColor(),
			PPCColor8: makeNSRGB(65535, 25428, 65535).PPencodeColor(),
			PPCColor9: makeNSRGB(52428, 39321, 26214).PPencodeColor(),
			PPCColor10: makeNSRGB(13107, 26214, 13107).PPencodeColor(),
			PPCColor11: makeNSRGB(65535, 26214, 65535).PPencodeColor(),
			PPCColor12: makeNSRGB(65535, 39321, 0).PPencodeColor(),
			PPCColor13: makeNSRGB(52428, 65535, 52428).PPencodeColor(),
			PPCColor14: makeNSRGB(39321, 0, 26214).PPencodeColor(),
			PPCColor15: makeNSRGB(26214, 52428, 0).PPencodeColor(),
			PPCColor16: makeNSRGB(65535, 39321, 65535).PPencodeColor(),
			PPCColor17: makeNSRGB(52428, 65535, 26214).PPencodeColor(),
			PPCColor18: makeNSRGB(26214, 0, 39321).PPencodeColor(),
			PPCColor19: makeNSRGB(52428, 0, 39321).PPencodeColor(),
			PPCColor20: makeNSRGB(52428, 39321, 65535).PPencodeColor(),
			PPCColor21: makeNSRGB(26214, 52428, 26214).PPencodeColor(),
			PPCColor22: makeNSRGB(52428, 52428, 13107).PPencodeColor(),
			PPCColor23: makeNSRGB(39321, 52428, 52428).PPencodeColor(),
			PPCColor24: makeNSRGB(26214, 13109, 26214).PPencodeColor(),
			PPCColor25: makeNSRGB(52428, 39321, 52428).PPencodeColor(),
			PPCColor26: makeNSRGB(13107, 65535, 65535).PPencodeColor(),
			PPCColor27: makeNSRGB(13107, 65535, 26214).PPencodeColor(),
			PPCColor28: makeNSRGB(26214, 65535, 13107).PPencodeColor(),
			PPCColor29: makeNSRGB(52428, 39321, 52428).PPencodeColor(),
			PPCColor30: makeNSRGB(52428, 39321, 13107).PPencodeColor(),
			PPCColor31: makeNSRGB(52428, 52428, 13107).PPencodeColor(),
			PPCColor32: makeNSRGB(65535, 52428, 0).PPencodeColor(),
			PPCColor33: makeNSRGB(61166, 0, 0).PPencodeColor(),
			PPCColor34: makeNSRGB(0, 65535, 26214).PPencodeColor(),
			PPCColor35: NSColor.cyanColor().PPencodeColor(),
			PPCColor36: NSColor.yellowColor().PPencodeColor(),
			PPCColor37: NSColor.greenColor().PPencodeColor(),
			PPCColor38: makeNSRGB(13107, 52428, 65535).PPencodeColor(),
			PPCColor39: makeNSRGB(39321, 52428, 39321).PPencodeColor(),
			PPCColor40: makeNSRGB(65535, 39321, 26214).PPencodeColor(),
			PPCColor41: makeNSRGB(39321, 65535, 65535).PPencodeColor(),
			PPCColor42: makeNSRGB(26214, 65535, 65535).PPencodeColor(),
			PPCColor43: makeNSRGB(65535, 39321, 65535).PPencodeColor(),
			PPCColor44: makeNSRGB(0, 21845, 0).PPencodeColor(),
			PPCColor45: makeNSRGB(52428, 65535, 52428).PPencodeColor(),
			PPCColor46: makeNSRGB(26214, 0, 0).PPencodeColor(),
			PPCColor47: makeNSRGB(13107, 65535, 26214).PPencodeColor(),
			PPCColor48: makeNSRGB(65535, 65535, 26214).PPencodeColor(),
			PPCColor49: makeNSRGB(52428, 0, 0).PPencodeColor(),
			PPCColor50: makeNSRGB(39321, 0, 13107).PPencodeColor(),
			PPCColor51: makeNSRGB(39321, 65535, 39321).PPencodeColor(),
			PPCColor52: makeNSRGB(39321, 52428, 52428).PPencodeColor(),
			PPCColor53: makeNSRGB(52428, 52428, 0).PPencodeColor(),
			PPCColor54: makeNSRGB(52428, 52428, 13107).PPencodeColor(),
			PPCColor55: makeNSRGB(65535, 39321, 65535).PPencodeColor(),
			PPCColor56: NSColor.redColor().PPencodeColor(),
			PPCColor57: makeNSRGB(65535, 0, 26214).PPencodeColor(),
			PPCColor58: makeNSRGB(26214, 65535, 65535).PPencodeColor(),
			PPCColor59: makeNSRGB(26214, 65535, 52428).PPencodeColor(),
			PPCColor60: makeNSRGB(39321, 52428, 26214).PPencodeColor(),
			PPCColor61: makeNSRGB(39321, 52428, 39321).PPencodeColor(),
			PPCColor62: makeNSRGB(39321, 52428, 26214).PPencodeColor(),
			PPCColor63: makeNSRGB(52428, 52428, 26214).PPencodeColor(),
			PPCColor64: makeNSRGB(52428, 52428, 39321).PPencodeColor(),
			PPCColor65: makeNSRGB(61166, 0, 0).PPencodeColor(),
			PPCColor66: makeNSRGB(0, 65535, 26214).PPencodeColor(),
			PPCColor67: makeNSRGB(0, 65535, 65535).PPencodeColor(),
			PPCColor68: makeNSRGB(65535, 65535, 0).PPencodeColor(),
			PPCColor69: NSColor.greenColor().PPencodeColor(),
			PPCColor70: makeNSRGB(13107, 52428, 65535).PPencodeColor(),
			PPCColor71: makeNSRGB(39321, 52428, 39321).PPencodeColor(),
			PPCColor72: makeNSRGB(65535, 39321, 26214).PPencodeColor(),
			PPCColor73: makeNSRGB(39321, 65535, 65535).PPencodeColor(),
			PPCColor74: makeNSRGB(26214, 65535, 65535).PPencodeColor(),
			PPCColor75: makeNSRGB(65535, 39321, 65535).PPencodeColor(),
			PPCColor76: makeNSRGB(0, 21845, 0).PPencodeColor(),
			PPCColor77: makeNSRGB(52428, 65535, 52428).PPencodeColor(),
			PPCColor78: makeNSRGB(26214, 0, 0).PPencodeColor(),
			PPCColor79: makeNSRGB(13107, 65535, 26214).PPencodeColor(),
			PPCColor80: makeNSRGB(65535, 65535, 26214).PPencodeColor(),
			PPCColor81: makeNSRGB(52428, 0, 0).PPencodeColor(),
			PPCColor82: makeNSRGB(39321, 0, 13107).PPencodeColor(),
			PPCColor83: makeNSRGB(39321, 65535, 39321).PPencodeColor(),
			PPCColor84: makeNSRGB(39321, 52428, 52428).PPencodeColor(),
			PPCColor85: makeNSRGB(52428, 52428, 0).PPencodeColor(),
			PPCColor86: makeNSRGB(52428, 52428, 13107).PPencodeColor(),
			PPCColor87: makeNSRGB(65535, 39321, 65535).PPencodeColor(),
			PPCColor88: NSColor.redColor().PPencodeColor(),
			PPCColor89: makeNSRGB(39321, 0, 26214).PPencodeColor(),
			PPCColor90: makeNSRGB(26214, 65535, 65535).PPencodeColor(),
			PPCColor91: makeNSRGB(26214, 65535, 52428).PPencodeColor(),
			PPCColor92: makeNSRGB(39321, 52428, 26214).PPencodeColor(),
			PPCColor93: makeNSRGB(39321, 52428, 39321).PPencodeColor(),
			PPCColor94: makeNSRGB(39321, 52428, 26214).PPencodeColor(),
			PPCColor95: makeNSRGB(52428, 52428, 26214).PPencodeColor(),
			PPCColor96: makeNSRGB(52428, 52428, 39321).PPencodeColor()]
		
		let alltogether = NSMutableDictionary(dictionary: tooLargeDict)
		alltogether.addEntriesFromDictionary(defaults1)
		alltogether.addEntriesFromDictionary(defaults2)
		alltogether.addEntriesFromDictionary(defaults3)
		alltogether.addEntriesFromDictionary(defaults4)
		
		NSUserDefaults.standardUserDefaults().registerDefaults(alltogether)
	}
	
	override init() {
		
		
		super.init()
		registerDefaults()
	}
	
	func handleFile(theURL1: NSURL!, ofType theUTI: String) -> Bool {
		let sharedWorkspace = NSWorkspace.sharedWorkspace()
		var theURL = theURL1
		if sharedWorkspace.type(theUTI, conformsToType: MADNativeUTI) {
			// Document controller should automatically handle this.
			return true
		} else if (theUTI  == MADGenericUTI) {
			let invExt = NSLocalizedString("Invalid Extension", comment: "Invalid Extension")
			let invExtDes = NSLocalizedString("The file %@ is identified as as a generic MAD tracker, and not a specific one. Renaming it will fix this. Do you want to rename the file extension?", comment: "Invalid extension description")
			let renameFile =  NSLocalizedString("Rename", comment: "rename file")
			let openFile = NSLocalizedString("Open", comment: "Open a file")
			let cancelOp = NSLocalizedString("Cancel", comment: "Cancel")
			let retVal = PPRunInformationalAlertPanel(NSLocalizedString("Invalid Extension", comment: "Invalid extension"), message: NSLocalizedString("The file %@ is identified as as a generic MAD tracker, and not a specific one. Renaming it will fix this. Do you want to rename the file extension?", comment: "Invalid extension description"), defaultButton: NSLocalizedString("Rename", comment: "rename file"), alternateButton: NSLocalizedString("Open", comment:"Open a file"), otherButton: NSLocalizedString("Cancel", comment: "Cancel"), args: theURL.lastPathComponent);
			switch (retVal) {
			case NSAlertDefaultReturn:
				
				var rec: NSDictionary? = nil
				var ostype = [Int8](count: 5, repeatedValue: 0)
				
				let identified = madLib.identifyFileAtURL(theURL, type: &ostype)
				
				if (madLib.identifyFileAtURL(theURL, type: &ostype) != MADErr.NoErr) || madLib.getInformationFromFileAtURL(theURL, type: &ostype, infoDictionary: &rec) != MADErr.NoErr {
					PPRunAlertPanel(NSLocalizedString("Unknown File", comment: "unknown file"), message: NSLocalizedString("The file type could not be identified.", comment: "Unidentified file"));
					return false;
				}
				let sigVala: AnyObject = rec![kPPSignature] ?? NSNumber(unsignedInt: StringToOSType("madk"))
				let sigValb: MADFourChar = (sigVala as NSNumber).unsignedIntValue
				let sigVal = OSTypeToString(sigValb)
				
				
				let tmpURL = theURL.URLByDeletingPathExtension!.URLByAppendingPathExtension(sigVal.lowercaseString);
				var err: NSError? = nil
				if (NSFileManager.defaultManager().moveItemAtURL(theURL, toURL:tmpURL, error:&err) == false) {
					println("Could not move file, error: \(err!)");
					PPRunInformationalAlertPanel(NSLocalizedString("Rename Error", comment: "Rename Error"), message: NSLocalizedString("The file could not be renamed to \"%@\".\n\nThe music file \"%@\" will still be loaded.", comment: "Could not rename file"), args: tmpURL.lastPathComponent, theURL.lastPathComponent);
				} else {
					theURL = tmpURL;
					//TODO: regenerate the UTI
				}
				
				break;
				
			case NSAlertAlternateReturn:
				break;
				
				//case NSAlertOtherReturn:
			default:
				return false;
			}
		}
		#if false
			if (sharedWorkspace.type(theUTI, conformsToType:PPPCMDUTI)) {
				var theOSErr = patternHandler.importPcmdFromURL(theURL)
				if (theOSErr != MADErr.NoErr) {
					let theErr = CreateErrorFromMADErrorType(theOSErr);
					NSAlert(error: theErr).runModal()
					return false;
				}
				return true;
			} else if (sharedWorkspace.type(theUTI, conformsToType:PPInstrumentListUTI)) {
				var err: NSError? = nil
				if (!instrumentController.importInstrumentListFromURL(theURL, error:&err)) {
					NSAlert(error: err!).runModal()
				} else {
					return true;
				}
			} //else
		#endif

		//TODO: check for valid extension.
		for aUTI in trackerUTIs {
			if sharedWorkspace.type(theUTI, conformsToType:aUTI) {
				let theWrap = PPMusicObject(URL: theURL1, library: madLib)
				let theDoc = PPDocument(music: theWrap)
				
				self.addDocument(theDoc)
				return true;
			}
		}
		
		for obj in instrumentPlugHandler {
			for aUTI in obj.UTITypes as [String] {
				if sharedWorkspace.type(theUTI, conformsToType:aUTI) {
					#if false
						if (instrumentController.isWindowLoaded()) {
							var theErr:NSError? = nil
							if (!instrumentPlugHandler.importSampleFromURL(theURL, makeUserSelectInstrument: true, error:&theErr)) {
								NSAlert(error: err!).runModal()
								return false;
							}
							return true;
						} else {
							return false;
						}
					#endif
				}
			}
		}
		
		for obj in complexImport {
			for aUTI in obj.UTITypes as [String] {
				if sharedWorkspace.type(theUTI, conformsToType: aUTI) {
					var aErr: NSError? = nil
					let canImport = obj.canImportURL(theURL1, error: &aErr)
					if canImport {
						obj.beginImportOfURL(theURL1, withHandler: { (ourObject, anErr) -> Void in
							if anErr == .NoErr {
								let aPPDoc = PPDocument(music: ourObject)
								
								self.addDocument(aPPDoc)
							} else {
								let nsErr = CreateErrorFromMADErrorType(anErr)!
								if ErrorIsUserCancelled(nsErr) == false {
									NSAlert(error: nsErr).runModal()
								} else {
									NSBeep()
								}
							}
						})
						return true
					}
				}
			}
		}

		return false;
	}
	
	required init(coder: NSCoder!) {
		
		
		super.init(coder: coder)
		registerDefaults()
	}
	
	func addExportObject(expObj: ExportObject) {
		exportObjects.append(expObj);
		expObj.run()
	}
	
	func applicationDidFinishLaunching(notification: NSNotification!) {
		MADRegisterDebugBlock(CocoaDebugStr)
		let defaults = NSUserDefaults.standardUserDefaults()
		
		for (i, obj) in enumerate(instrumentPlugHandler) {
			if (obj.mode == MADPlugModes.ImportExport || obj.mode == MADPlugModes.Export) {
				let mi = NSMenuItem(title: obj.menuName, action: "exportInstrument:", keyEquivalent: "")
				mi.tag = i;
				mi.target = nil
				instrumentExportMenu.addItem(mi);
			}
		}
		
		for (i, rawObj) in enumerate(madLib) {
			let obj = rawObj as PPLibraryObject;
			if (obj.canExport) {
				let mi = NSMenuItem(title: "\(obj.menuName)…", action: "exportMusicAs:", keyEquivalent: "")
				mi.tag = i
				mi.target = nil
				musicExportMenu.addItem(mi)
			}
		}
		
		for i in 1 ... 96 {
			let tmpColor = NSColor.PPDecodeColorWithData(defaults.dataForKey("PPColor \(i)"))
			thePPColors.append(tmpColor!)
		}
		
		updatePlugInInfoMenu()
	}
	
	override func makeUntitledDocumentOfType(typeName: String!, error outError: NSErrorPointer) -> AnyObject! {
		assert(typeName == MADNativeUTI, "Unknown type passed to \(__FUNCTION__): \(typeName)")
		let theDoc = PPDocument(music: PPMusicObject())

		return theDoc
	}
	
	@IBAction func openFile(sender: AnyObject?) {
		let panel = NSOpenPanel();
		let otherDict: [String : [String]]  = ["PCMD": [PPPCMDUTI], "Instrument List": [PPInstrumentListUTI]];
		var samplesDict = [String: [String]]()
		for obj in instrumentPlugHandler {
			if (obj.mode == .Import || obj.mode == .ImportExport) {
				samplesDict[obj.menuName] = (obj.UTITypes as [String]);
			}
		}
		
		let av = OpenPanelViewController(openPanel: panel, trackerDictionary: trackerDict, instrumentDictionary: samplesDict, additionalDictionary: otherDict)
		av.setupDefaults()
		panel.beginWithCompletionHandler { (retval) -> Void in
			if retval == NSFileHandlingPanelOKButton {
				let panelURL = panel.URL
				let filename = panelURL.path
				var err: NSError? = nil
				let utiFile = NSWorkspace.sharedWorkspace().typeOfFile(filename, error: &err)
				if err != nil {
					PPRunAlertPanel("Error opening file", message: "Unable to open %@: %@", args: filename!.lastPathComponent, err!.localizedFailureReason!);
					return
				}
				self.handleFile(panelURL, ofType: utiFile)
			}
		}
	}
	
	func application(theApplication: NSApplication, openFile filename: String) -> Bool {
		var err: NSError? = nil;
		let utiFile = NSWorkspace.sharedWorkspace().typeOfFile(filename, error:&err)
		if (err != nil) {
			PPRunAlertPanel("Error opening file", message: "Unable to open %@: %@", args: filename.lastPathComponent, err!.localizedFailureReason!);
			return false;
		}
		return handleFile(NSURL(fileURLWithPath: filename), ofType: utiFile) ;
	}
	
	func exportObjectDidFinish(theObj: ExportObject) {
		
	}
	
	func exportObjectEncounteredError(theObj: ExportObject, errorCode errCode: MADErr, errorString errStr: NSString?) {
		
	}
}
