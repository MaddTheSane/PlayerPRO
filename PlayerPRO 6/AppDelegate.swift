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
import SwiftAdditions

@inline(__always) private func makeNSRGB(red: UInt16, green: UInt16, blue:UInt16) -> NSColor {
	return NSColor(calibratedRed: CGFloat(red) / CGFloat(UInt16.max), green: CGFloat(green) / CGFloat(UInt16.max), blue: CGFloat(blue) / CGFloat(UInt16.max), alpha: 1)
}

private func CocoaDebugStr(line: Int16, file: UnsafePointer<Int8>, text: UnsafePointer<Int8>) {
	let swiftFile = NSFileManager.defaultManager().stringWithFileSystemRepresentation(file, length: Int(strlen(file)))
	let swiftText = String.fromCString(text)!
	println("\(swiftFile):\(line), error text: \(swiftText)")
	let errStr = NSLocalizedString("MyDebugStr_Error", comment: "Error")
	let mainStr = String(format: NSLocalizedString("MyDebugStr_MainText", comment: "The Main text to display"), text)
	let quitStr = NSLocalizedString("MyDebugStr_Quit", comment: "Quit")
	let contStr = NSLocalizedString("MyDebugStr_Continue", comment: "Continue")
	let debuStr = NSLocalizedString("MyDebugStr_Debug", comment: "Debug")

	let alert = PPRunCriticalAlertPanel(errStr, message: mainStr, defaultButton: quitStr, alternateButton: contStr, otherButton: debuStr)
	switch (alert) {
	case NSAlertAlternateReturn:
		break
		
	case NSAlertOtherReturn:
		assert(false, "Chose to go to debugger.")
		
	case NSAlertDefaultReturn:
		println("Choosing to fail!")
		fallthrough
	default:
		abort()
	}
}

internal var globalMadLib: PPLibrary {
	return (NSApplication.sharedApplication().delegate as! AppDelegate).madLib
}

@NSApplicationMain
class AppDelegate: NSDocumentController, NSApplicationDelegate, ExportObjectDelegate {
	private var exportObjects = [ExportObject]()
	var plugInInfos = [PlugInInfo]()
	let madLib = PPLibrary()!
	let instrumentPlugHandler = PPInstrumentPlugHandler()
	let digitalHandler = DigitalPlugHandler()
	let filterHandler = PPFilterPlugHandler()
	let complexImport = ComplexImportPlugHandler()
	let samplesHandler = SamplePlugHandler()
	let preferences = Preferences.newPreferenceController()
	var thePPColors = [NSColor]()
	
	@IBOutlet weak var musicExportMenu:			NSMenu!
	@IBOutlet weak var aboutPlugInMenu:			NSMenu!
	@IBOutlet weak var instrumentExportMenu:	NSMenu!
	@IBOutlet weak var newInstrumentMenu:		NSMenu!
	@IBOutlet weak var exportStatusPanel:		NSPanel!
	
	private(set) lazy var trackerDict: [String: [String]] = {
		let localMADKName = NSLocalizedString("PPMADKFile", tableName: "InfoPlist", value: "MADK Tracker", comment: "MADK Tracker")
		let localGenericMADName = NSLocalizedString("Generic MAD tracker", comment: "Generic MAD tracker")
		var tmpTrackerDict = [localMADKName: [MADNativeUTI], localGenericMADName: [MADGenericUTI]] as [String: [String]]
		
		for obj in self.madLib {
			tmpTrackerDict[obj.menuName] = obj.UTITypes
		}
		
		return tmpTrackerDict
		}()
	
	private(set) lazy var importDict: [String: [String]] = {
		var tmpTrackerDict = self.trackerDict
		
		for obj in self.complexImport {
			tmpTrackerDict[obj.menuName] = (obj.UTITypes) as? [String]
		}
		
		return tmpTrackerDict
		}()

	
	private(set) lazy var trackerUTIs: [String] = {
		let arrayOfUTIs = self.trackerDict.values
		var toAddUTI = [String]()
		for anArray in arrayOfUTIs {
			toAddUTI += anArray
		}
		return toAddUTI
		}()
	
	class func globalMADLibrary() -> PPLibrary {
		return globalMadLib
	}
	
	@IBAction func showPreferences(sender: AnyObject?) {
		preferences.window!.center()
		preferences.showWindow(sender)
	}

	@IBAction func showPlugInInfo(sender: AnyObject?) {
		let tag = (sender as! NSMenuItem).tag
		if (tag < 0 || tag >= plugInInfos.count) {
			return;
		}
		let inf = plugInInfos[tag]
		
		let infoCont = PlugInInfoController.windowControllerFromInfo(inf)
		infoCont.window!.center()
		NSApplication.sharedApplication().runModalForWindow(infoCont.window!)
	}

	func updatePlugInInfoMenu() {
		let trackerPlugName = NSLocalizedString("TrackerPlugName", comment: "Tracker plug-in name")
		let complexTrackerPlugName = NSLocalizedString("ComplexTrackerPlugName", comment: "Tracker plug-in name")
		let instrumentPlugName = NSLocalizedString("InstrumentPlugName", comment: "Instrument plug-in name")
		let digitalPlugName = NSLocalizedString("DigitalPlugName", comment: "Digital plug-in name")
		let filterPlugName = NSLocalizedString("FilterPlugName", comment: "Filter plug-in name")
		let samplePlugName = NSLocalizedString("SamplePlugName", comment: "Sample plug-in name")
		
		for obj in madLib {
			let tmpInfo = PlugInInfo(plugName: obj.menuName, author: obj.authorString, plugType: trackerPlugName, plugURL: obj.bundle.bundleURL)
			if !contains(plugInInfos, tmpInfo) {
				plugInInfos.append(tmpInfo)
			}
		}
		
		for obj in instrumentPlugHandler {
			let tmpInfo = PlugInInfo(plugName: obj.menuName, author: obj.authorString, plugType: instrumentPlugName, plugURL: obj.file.bundleURL)
			if !contains(plugInInfos, tmpInfo) {
				plugInInfos.append(tmpInfo)
			}
		}
		
		for obj in samplesHandler {
			let tmpInfo = PlugInInfo(plugName: obj.menuName, author: obj.authorString, plugType: instrumentPlugName, plugURL: obj.file.bundleURL)
			if !contains(plugInInfos, tmpInfo) {
				plugInInfos.append(tmpInfo)
			}
		}
		
		for obj in digitalHandler {
			let tmpInfo = PlugInInfo(plugName: obj.menuName, author: obj.authorString, plugType: digitalPlugName, plugURL: obj.file.bundleURL)
			if !contains(plugInInfos, tmpInfo) {
				plugInInfos.append(tmpInfo)
			}
		}
		
		for obj in filterHandler {
			let tmpInfo = PlugInInfo(plugName: obj.menuName, author: obj.authorString, plugType: filterPlugName, plugURL: obj.file.bundleURL)
			if !contains(plugInInfos, tmpInfo) {
				plugInInfos.append(tmpInfo)
			}
		}
		
		for obj in complexImport {
			let tmpInfo = PlugInInfo(plugName: obj.menuName, author: obj.authorString, plugType: complexTrackerPlugName, plugURL: obj.file.bundleURL)
			if !contains(plugInInfos, tmpInfo) {
				plugInInfos.append(tmpInfo)
			}
		}
		
		plugInInfos.sort({ (obj1, obj2) -> Bool in
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
		var tooLargeDict: [String: AnyObject] = [PPSoundDriver: Int(MADSoundOutput.CoreAudioDriver.rawValue)]
		
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
			PPDEMarkerColorPref: makeNSRGB(0xFFFF, 0xFFFF, 0x9999).PPencodeColor(),
			
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
		
		let colorDefaults = [
			PPCColor1: makeNSRGB(0xEEEE, 0, 0).PPencodeColor(),
			PPCColor2: makeNSRGB(0x8C8C, 0xBCBC, 0x1C1C).PPencodeColor(),
			PPCColor3: makeNSRGB(0x3333, 0xFFFF, 0xFFFF).PPencodeColor(),
			PPCColor4: NSColor.yellowColor().PPencodeColor(),
			PPCColor5: makeNSRGB(0x6060, 0xCACA, 36494).PPencodeColor(),
			PPCColor6: makeNSRGB(0x3333, 0xFFFF, 0xFFFF).PPencodeColor(),
			PPCColor7: makeNSRGB(0x6666, 0xCCCC, 0xFFFF).PPencodeColor(),
			PPCColor8: makeNSRGB(0xFFFF, 0x6354, 0xFFFF).PPencodeColor(),
			PPCColor9: makeNSRGB(0xCCCC, 0x9999, 0x6666).PPencodeColor(),
			PPCColor10: makeNSRGB(0x3333, 0x6666, 0x3333).PPencodeColor(),
			PPCColor11: makeNSRGB(0xFFFF, 0x6666, 0xFFFF).PPencodeColor(),
			PPCColor12: makeNSRGB(0xFFFF, 0x9999, 0).PPencodeColor(),
			PPCColor13: makeNSRGB(0xCCCC, 0xFFFF, 0xCCCC).PPencodeColor(),
			PPCColor14: makeNSRGB(0x9999, 0, 0x6666).PPencodeColor(),
			PPCColor15: makeNSRGB(0x6666, 0xCCCC, 0).PPencodeColor(),
			PPCColor16: makeNSRGB(0xFFFF, 0x9999, 0xFFFF).PPencodeColor(),
			PPCColor17: makeNSRGB(0xCCCC, 0xFFFF, 0x6666).PPencodeColor(),
			PPCColor18: makeNSRGB(0x6666, 0, 0x9999).PPencodeColor(),
			PPCColor19: makeNSRGB(0xCCCC, 0, 0x9999).PPencodeColor(),
			PPCColor20: makeNSRGB(0xCCCC, 0x9999, 0xFFFF).PPencodeColor(),
			PPCColor21: makeNSRGB(0x6666, 0xCCCC, 0x6666).PPencodeColor(),
			PPCColor22: makeNSRGB(0xCCCC, 0xCCCC, 0x3333).PPencodeColor(),
			PPCColor23: makeNSRGB(0x9999, 0xCCCC, 0xCCCC).PPencodeColor(),
			PPCColor24: makeNSRGB(0x6666, 13109, 0x6666).PPencodeColor(),
			PPCColor25: makeNSRGB(0xCCCC, 0x9999, 0xCCCC).PPencodeColor(),
			PPCColor26: makeNSRGB(0x3333, 0xFFFF, 0xFFFF).PPencodeColor(),
			PPCColor27: makeNSRGB(0x3333, 0xFFFF, 0x6666).PPencodeColor(),
			PPCColor28: makeNSRGB(0x6666, 0xFFFF, 0x3333).PPencodeColor(),
			PPCColor29: makeNSRGB(0xCCCC, 0x9999, 0xCCCC).PPencodeColor(),
			PPCColor30: makeNSRGB(0xCCCC, 0x9999, 0x3333).PPencodeColor(),
			PPCColor31: makeNSRGB(0xCCCC, 0xCCCC, 0x3333).PPencodeColor(),
			PPCColor32: makeNSRGB(0xFFFF, 0xCCCC, 0).PPencodeColor(),
			PPCColor33: makeNSRGB(0xEEEE, 0, 0).PPencodeColor(),
			PPCColor34: makeNSRGB(0, 0xFFFF, 0x6666).PPencodeColor(),
			PPCColor35: NSColor.cyanColor().PPencodeColor(),
			PPCColor36: NSColor.yellowColor().PPencodeColor(),
			PPCColor37: NSColor.greenColor().PPencodeColor(),
			PPCColor38: makeNSRGB(0x3333, 0xCCCC, 0xFFFF).PPencodeColor(),
			PPCColor39: makeNSRGB(0x9999, 0xCCCC, 0x9999).PPencodeColor(),
			PPCColor40: makeNSRGB(0xFFFF, 0x9999, 0x6666).PPencodeColor(),
			PPCColor41: makeNSRGB(0x9999, 0xFFFF, 0xFFFF).PPencodeColor(),
			PPCColor42: makeNSRGB(0x6666, 0xFFFF, 0xFFFF).PPencodeColor(),
			PPCColor43: makeNSRGB(0xFFFF, 0x9999, 0xFFFF).PPencodeColor(),
			PPCColor44: makeNSRGB(0, 0x5555, 0).PPencodeColor(),
			PPCColor45: makeNSRGB(0xCCCC, 0xFFFF, 0xCCCC).PPencodeColor(),
			PPCColor46: makeNSRGB(0x6666, 0, 0).PPencodeColor(),
			PPCColor47: makeNSRGB(0x3333, 0xFFFF, 0x6666).PPencodeColor(),
			PPCColor48: makeNSRGB(0xFFFF, 0xFFFF, 0x6666).PPencodeColor(),
			PPCColor49: makeNSRGB(0xCCCC, 0, 0).PPencodeColor(),
			PPCColor50: makeNSRGB(0x9999, 0, 0x3333).PPencodeColor(),
			PPCColor51: makeNSRGB(0x9999, 0xFFFF, 0x9999).PPencodeColor(),
			PPCColor52: makeNSRGB(0x9999, 0xCCCC, 0xCCCC).PPencodeColor(),
			PPCColor53: makeNSRGB(0xCCCC, 0xCCCC, 0).PPencodeColor(),
			PPCColor54: makeNSRGB(0xCCCC, 0xCCCC, 0x3333).PPencodeColor(),
			PPCColor55: makeNSRGB(0xFFFF, 0x9999, 0xFFFF).PPencodeColor(),
			PPCColor56: NSColor.redColor().PPencodeColor(),
			PPCColor57: makeNSRGB(0xFFFF, 0, 0x6666).PPencodeColor(),
			PPCColor58: makeNSRGB(0x6666, 0xFFFF, 0xFFFF).PPencodeColor(),
			PPCColor59: makeNSRGB(0x6666, 0xFFFF, 0xCCCC).PPencodeColor(),
			PPCColor60: makeNSRGB(0x9999, 0xCCCC, 0x6666).PPencodeColor(),
			PPCColor61: makeNSRGB(0x9999, 0xCCCC, 0x9999).PPencodeColor(),
			PPCColor62: makeNSRGB(0x9999, 0xCCCC, 0x6666).PPencodeColor(),
			PPCColor63: makeNSRGB(0xCCCC, 0xCCCC, 0x6666).PPencodeColor(),
			PPCColor64: makeNSRGB(0xCCCC, 0xCCCC, 0x9999).PPencodeColor(),
			PPCColor65: makeNSRGB(0xEEEE, 0, 0).PPencodeColor(),
			PPCColor66: makeNSRGB(0, 0xFFFF, 0x6666).PPencodeColor(),
			PPCColor67: makeNSRGB(0, 0xFFFF, 0xFFFF).PPencodeColor(),
			PPCColor68: makeNSRGB(0xFFFF, 0xFFFF, 0).PPencodeColor(),
			PPCColor69: NSColor.greenColor().PPencodeColor(),
			PPCColor70: makeNSRGB(0x3333, 0xCCCC, 0xFFFF).PPencodeColor(),
			PPCColor71: makeNSRGB(0x9999, 0xCCCC, 0x9999).PPencodeColor(),
			PPCColor72: makeNSRGB(0xFFFF, 0x9999, 0x6666).PPencodeColor(),
			PPCColor73: makeNSRGB(0x9999, 0xFFFF, 0xFFFF).PPencodeColor(),
			PPCColor74: makeNSRGB(0x6666, 0xFFFF, 0xFFFF).PPencodeColor(),
			PPCColor75: makeNSRGB(0xFFFF, 0x9999, 0xFFFF).PPencodeColor(),
			PPCColor76: makeNSRGB(0, 0x5555, 0).PPencodeColor(),
			PPCColor77: makeNSRGB(0xCCCC, 0xFFFF, 0xCCCC).PPencodeColor(),
			PPCColor78: makeNSRGB(0x6666, 0, 0).PPencodeColor(),
			PPCColor79: makeNSRGB(0x3333, 0xFFFF, 0x6666).PPencodeColor(),
			PPCColor80: makeNSRGB(0xFFFF, 0xFFFF, 0x6666).PPencodeColor(),
			PPCColor81: makeNSRGB(0xCCCC, 0, 0).PPencodeColor(),
			PPCColor82: makeNSRGB(0x9999, 0, 0x3333).PPencodeColor(),
			PPCColor83: makeNSRGB(0x9999, 0xFFFF, 0x9999).PPencodeColor(),
			PPCColor84: makeNSRGB(0x9999, 0xCCCC, 0xCCCC).PPencodeColor(),
			PPCColor85: makeNSRGB(0xCCCC, 0xCCCC, 0).PPencodeColor(),
			PPCColor86: makeNSRGB(0xCCCC, 0xCCCC, 0x3333).PPencodeColor(),
			PPCColor87: makeNSRGB(0xFFFF, 0x9999, 0xFFFF).PPencodeColor(),
			PPCColor88: NSColor.redColor().PPencodeColor(),
			PPCColor89: makeNSRGB(0x9999, 0, 0x6666).PPencodeColor(),
			PPCColor90: makeNSRGB(0x6666, 0xFFFF, 0xFFFF).PPencodeColor(),
			PPCColor91: makeNSRGB(0x6666, 0xFFFF, 0xCCCC).PPencodeColor(),
			PPCColor92: makeNSRGB(0x9999, 0xCCCC, 0x6666).PPencodeColor(),
			PPCColor93: makeNSRGB(0x9999, 0xCCCC, 0x9999).PPencodeColor(),
			PPCColor94: makeNSRGB(0x9999, 0xCCCC, 0x6666).PPencodeColor(),
			PPCColor95: makeNSRGB(0xCCCC, 0xCCCC, 0x6666).PPencodeColor(),
			PPCColor96: makeNSRGB(0xCCCC, 0xCCCC, 0x9999).PPencodeColor()]
		
		tooLargeDict += defaults1
		tooLargeDict += defaults2
		tooLargeDict += defaults3
		tooLargeDict += (colorDefaults as [String: AnyObject])
		
		NSUserDefaults.standardUserDefaults().registerDefaults(tooLargeDict)
	}
	
	override init() {
		super.init()
		registerDefaults()
	}
	
	func handleFile(theURL1: NSURL, ofType theUTI: String) -> Bool {
		let sharedWorkspace = NSWorkspace.sharedWorkspace()
		var theURL = theURL1
		if sharedWorkspace.type(theUTI, conformsToType: MADNativeUTI) {
			// Document controller should automatically handle this.
			// But just in case...
			if let aDoc = documentForURL(theURL1) as? NSDocument {
				return true
			} else {
				openDocumentWithContentsOfURL(theURL1, display: true, completionHandler: { (_, alreadyOpen, error) -> Void in
					
					if alreadyOpen {
						println("\(theURL1) is already open? How did we not catch this?")
					}
					
					if let aErr = error {
						let alertErr = NSAlert(error: aErr)
						dispatch_async(dispatch_get_main_queue()) {
							alertErr.runModal()
							
							return
						}
					}
				})
			}
			return true
		} else if (theUTI  == MADGenericUTI) {
			let invExt = NSLocalizedString("Invalid Extension", comment: "Invalid Extension")
			let invExtDes = NSLocalizedString("The file %@ is identified as as a generic MAD tracker, and not a specific one. Renaming it will fix this. Do you want to rename the file extension?", comment: "Invalid extension description")
			let renameFile =  NSLocalizedString("Rename", comment: "rename file")
			let openFile = NSLocalizedString("Open", comment: "Open a file")
			let cancelOp = NSLocalizedString("Cancel", comment: "Cancel")
			let unwrapped = String(format: invExtDes, theURL.lastPathComponent!)
			
			let retVal = PPRunInformationalAlertPanel(NSLocalizedString("Invalid Extension", comment: "Invalid extension"), message: unwrapped, defaultButton: NSLocalizedString("Rename", comment: "rename file"), alternateButton: NSLocalizedString("Open", comment:"Open a file"), otherButton: NSLocalizedString("Cancel", comment: "Cancel"));
			switch (retVal) {
			case NSAlertDefaultReturn:
				
				let identRet = madLib.identifyFile(URL: theURL)
				switch identRet {
				case .Failure(_):
					PPRunCriticalAlertPanel(NSLocalizedString("Unknown File", comment: "unknown file"), message: NSLocalizedString("The file type could not be identified.", comment: "Unidentified file"))
					return false
					
				case .Success(let madSubtype):
					let infoRet = madLib.informationFromFile(URL: theURL, type: madSubtype)
					switch infoRet {
					case .Failure(_):
						PPRunCriticalAlertPanel(NSLocalizedString("Unknown File", comment: "unknown file"), message: NSLocalizedString("The file type could not be identified.", comment: "Unidentified file"))
						return false
						
					case .Success(let info):
						let tmpURL = theURL.URLByDeletingPathExtension!.URLByAppendingPathExtension(info.signature.lowercaseString)
						
						var err: NSError?
						if !NSFileManager.defaultManager().moveItemAtURL(theURL, toURL: tmpURL, error: &err) {
							println("Could not move file, error \(err!)")
							let couldNotRenameStr = String(format: NSLocalizedString("The file could not be renamed to \"%@\".\n\nThe music file \"%@\" will still be loaded.", comment: "Could not rename file"), tmpURL.lastPathComponent!, theURL.lastPathComponent!)
							PPRunInformationalAlertPanel(NSLocalizedString("Rename Error", comment: "Rename Error"), message: couldNotRenameStr)
						} else {
							theURL = tmpURL
							//TODO: regenerate the UTI
						}
					}
				}
				
			case NSAlertAlternateReturn:
				break;
				
				//case NSAlertOtherReturn:
			default:
				return false;
			}
		}
		if (sharedWorkspace.type(theUTI, conformsToType:PPPCMDUTI)) {
			var theOSErr = importPcmdFromURL(theURL)
			if (theOSErr != MADErr.NoErr) {
				let theErr = createErrorFromMADErrorType(theOSErr)!
				NSAlert(error: theErr).runModal()
				return false;
			}
			return true;
		} else if (sharedWorkspace.type(theUTI, conformsToType:PPInstrumentListUTI)) {
			var err: NSError? = nil
			if (!importInstrumentListFromURL(theURL, error:&err)) {
				NSAlert(error: err!).runModal()
			} else {
				return true;
			}
		} //else
		
		for obj in complexImport {
			for aUTI in obj.UTITypes as! [String] {
				if sharedWorkspace.type(theUTI, conformsToType: aUTI) {
					var aErr: NSError? = nil
					let canImport = obj.canImportURL(theURL1, error: &aErr)
					if canImport {
						obj.beginImportOfURL(theURL1, withHandler: { (ourObject, anErr) -> Void in
							if anErr == .NoErr {
								let aPPDoc = PPDocument(music: ourObject!)
								
								self.addDocument(aPPDoc)
								aPPDoc.makeWindowControllers()
								aPPDoc.showWindows()
								aPPDoc.setDisplayName(theURL1.lastPathComponent!.stringByDeletingPathExtension)
							} else {
								let nsErr = createErrorFromMADErrorType(anErr)!
								if PPErrorIsUserCancelled(nsErr) == false {
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

		
		//TODO: check for valid extension.
		for aUTI in trackerUTIs {
			if sharedWorkspace.type(theUTI, conformsToType:aUTI) {
				let aType = madLib.typeFromUTI(theUTI)!
				if let theWrap = PPMusicObject(URL: theURL1, stringType: aType, library: madLib, error: nil) {
					let aDoc = PPDocument(music: theWrap)
					
					addDocument(aDoc)
					aDoc.makeWindowControllers()
					aDoc.showWindows()
					aDoc.setDisplayName(theURL1.lastPathComponent!.stringByDeletingPathExtension)
					return true;
				} else {
					return false
				}
			}
		}
		
		for obj in instrumentPlugHandler {
			for aUTI in obj.UTITypes as! [String] {
				if sharedWorkspace.type(theUTI, conformsToType:aUTI) {
					var theErr: NSError? = nil
					if (!importInstrument(URL: theURL, makeUserSelectInstrument: true, error:&theErr)) {
						NSAlert(error: theErr!).runModal()
						return false;
					}
					return true;
				}
			}
		}
		
		for obj in samplesHandler {
			for aUTI in obj.UTITypes as! [String] {
				if sharedWorkspace.type(theUTI, conformsToType:aUTI) {
					var theErr: NSError? = nil
					if (!importSample(URL: theURL, makeUserSelectSample: true, error:&theErr)) {
						NSAlert(error: theErr!).runModal()
						return false;
					}
					return true;
				}
			}
		}
		
		return false;
	}
	
	func importSample(URL theURL: NSURL, makeUserSelectSample: Bool = false, error: NSErrorPointer = nil) -> Bool {
		if error != nil {
			error.memory = createErrorFromMADErrorType(.OrderNotImplemented)!
		}
		
		return false
	}
	
	func importInstrument(URL theURL: NSURL, makeUserSelectInstrument: Bool = false, error: NSErrorPointer = nil) -> Bool {
		if error != nil {
			error.memory = createErrorFromMADErrorType(.OrderNotImplemented)!
		}
		
		return false
	}

	func importPcmdFromURL(url: NSURL) -> MADErr {
		return .OrderNotImplemented
	}
	
	func importInstrumentListFromURL(url: NSURL, error: NSErrorPointer) -> Bool {
		if error != nil {
			error.memory = createErrorFromMADErrorType(.OrderNotImplemented)!
		}
		
		return false
	}
	
	required init?(coder: NSCoder) {
		super.init(coder: coder)
		registerDefaults()
	}
	
	@objc(PPExportAddObject:) func addExportObject(expObj: ExportObject) {
		exportObjects.append(expObj);
		expObj.run()
	}
	
	func applicationDidFinishLaunching(notification: NSNotification) {
		PPLibrary.registerDebugBlock(CocoaDebugStr)
		let defaults = NSUserDefaults.standardUserDefaults()
		
		for (i, obj) in enumerate(instrumentPlugHandler) {
			if (obj.mode == MADPlugModes.ImportExport || obj.mode == MADPlugModes.Export) {
				let mi = NSMenuItem(title: obj.menuName, action: "exportInstrument:", keyEquivalent: "")
				mi.tag = i;
				mi.target = nil
				instrumentExportMenu.addItem(mi);
			}
		}
		
		for (i, obj) in enumerate(madLib) {
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
	
	override func makeUntitledDocumentOfType(typeName: String, error outError: NSErrorPointer) -> AnyObject? {
		assert(typeName == MADNativeUTI, "Unknown type passed to \(__FUNCTION__): \(typeName)")
		let theDoc = PPDocument(music: PPMusicObject())

		if outError != nil {
			outError.memory = nil
		}
		
		return theDoc
	}
	
	@IBAction func openFile(sender: AnyObject?) {
		let panel = NSOpenPanel();
		let otherDict: [String : [String]]  = ["PCMD": [PPPCMDUTI], "Instrument List": [PPInstrumentListUTI]];
		var samplesDict = [String: [String]]()
		for obj in instrumentPlugHandler {
			if (obj.mode == .Import || obj.mode == .ImportExport) {
				samplesDict[obj.menuName] = (obj.UTITypes as! [String]);
			}
		}
		
		for obj in samplesHandler {
			if obj.canImport {
				samplesDict[obj.menuName] = (obj.UTITypes as! [String])
			}
		}
		
		if let av = OpenPanelViewController(openPanel: panel, trackerDictionary: importDict, instrumentDictionary: samplesDict, additionalDictionary: otherDict) {
			av.setupDefaults()
			av.beginWithCompletionHandler { (retval) -> Void in
				if retval == NSFileHandlingPanelOKButton {
					let panelURL = panel.URL!
					let filename = panelURL.path!
					var err: NSError? = nil
					let utiFile = NSWorkspace.sharedWorkspace().typeOfFile(filename, error: &err)
					if err != nil {
						PPRunAlertPanel("Error opening file", message: String(format:"Unable to open %@: %@", filename.lastPathComponent, err!.localizedFailureReason!))
						return
					}
					self.handleFile(panelURL, ofType: utiFile!)
				}
			}
		}
	}
	
	func application(theApplication: NSApplication, openFile filename: String) -> Bool {
		var err: NSError? = nil
		let utiFile = NSWorkspace.sharedWorkspace().typeOfFile(filename, error:&err)
		if (err != nil) {
			PPRunAlertPanel("Error opening file", message: String(format:"Unable to open %@: %@", filename.lastPathComponent, err!.localizedFailureReason!))
			return false
		}
		return handleFile(NSURL(fileURLWithPath: filename)!, ofType: utiFile!)
	}
	
	@objc(PPExportObjectDidFinish:) func exportObjectDidFinish(theObj: ExportObject) {
		
	}
	
	@objc(PPExportObjectEncounteredError:errorCode:errorString:) func exportObjectEncounteredError(theObj: ExportObject, errorCode errCode: MADErr, errorString errStr: String?) {
		
	}
}
