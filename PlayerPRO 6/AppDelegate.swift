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

@inline(__always) private func makeNSRGB(_ red: UInt16, _ green: UInt16, _ blue:UInt16) -> NSColor {
	return NSColor(calibratedRed: CGFloat(red) / CGFloat(UInt16.max), green: CGFloat(green) / CGFloat(UInt16.max), blue: CGFloat(blue) / CGFloat(UInt16.max), alpha: 1)
}

private func CocoaDebugStr(_ line: Int16, file: UnsafePointer<Int8>?, text: UnsafePointer<Int8>?) {
	let swiftFile = FileManager.default.string(withFileSystemRepresentation: file!, length: Int(strlen(file)))
	let swiftText = String(cString: text!)
	print("\(swiftFile):\(line), error text: \(swiftText)")
	let errStr = NSLocalizedString("MyDebugStr_Error", comment: "Error")
	let mainStr = String(format: NSLocalizedString("MyDebugStr_MainText", comment: "The Main text to display"), text!)
	let quitStr = NSLocalizedString("MyDebugStr_Quit", comment: "Quit")
	let contStr = NSLocalizedString("MyDebugStr_Continue", comment: "Continue")
	let debuStr = NSLocalizedString("MyDebugStr_Debug", comment: "Debug")

	let alert = PPRunCriticalAlertPanel(title: errStr, message: mainStr, defaultButton: quitStr, alternateButton: contStr, otherButton: debuStr)
	switch (alert) {
	case NSApplication.ModalResponse.alertSecondButtonReturn:
		break
		
	case NSApplication.ModalResponse.alertThirdButtonReturn:
		assert(false, "Chose to go to debugger.")
		
	case NSApplication.ModalResponse.alertFirstButtonReturn:
		print("Choosing to fail!")
		fallthrough
	default:
		abort()
	}
}

internal let globalMadLib = try! PPLibrary()

@NSApplicationMain
class AppDelegate: NSDocumentController, NSApplicationDelegate {
	fileprivate var exportObjects = [ExportObject]()
	var plugInInfos = [PlugInInfo]()
	var madLib: PPLibrary {
		return globalMadLib
	}
	let instrumentPlugHandler = PPInstrumentPlugHandler()
	let digitalHandler = DigitalPlugHandler()
	let filterHandler = FilterPlugHandler()
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
		var tmpTrackerDict: [String: [String]] = [localMADKName: [MADNativeUTI], localGenericMADName: [MADGenericUTI]]
		
		for obj in self.madLib {
			tmpTrackerDict[obj.menuName] = obj.UTITypes
		}
		
		return tmpTrackerDict
		}()
	
	private(set) lazy var importDict: [String: [String]] = {
		var tmpTrackerDict = self.trackerDict
		
		for obj in self.complexImport {
			tmpTrackerDict[obj.menuName] = obj.utiTypes
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
	
	@IBAction func showPreferences(_ sender: AnyObject?) {
		preferences.window!.center()
		preferences.showWindow(sender)
	}

	@IBAction func showPlugInInfo(_ sender: AnyObject?) {
		let tag = (sender as? NSMenuItem)?.tag ?? -1
		if tag < 0 || tag >= plugInInfos.count {
			return;
		}
		let inf = plugInInfos[tag]
		
		let infoCont = PlugInInfoController.windowController(from: inf)
		infoCont.window!.center()
		NSApplication.shared.runModal(for: infoCont.window!)
	}

	func updatePlugInInfoMenu() {
		let trackerPlugName = NSLocalizedString("TrackerPlugName", comment: "Tracker plug-in name")
		let complexTrackerPlugName = NSLocalizedString("ComplexTrackerPlugName", comment: "Tracker plug-in name")
		let instrumentPlugName = NSLocalizedString("InstrumentPlugName", comment: "Instrument plug-in name")
		let digitalPlugName = NSLocalizedString("DigitalPlugName", comment: "Digital plug-in name")
		let filterPlugName = NSLocalizedString("FilterPlugName", comment: "Filter plug-in name")
		//let samplePlugName = NSLocalizedString("SamplePlugName", comment: "Sample plug-in name")
		
		for obj in madLib {
			let tmpInfo = PlugInInfo(plugName: obj.menuName, author: obj.authorString, plugType: trackerPlugName, plugURL: obj.bundle.bundleURL)
			if !plugInInfos.contains(tmpInfo) {
				plugInInfos.append(tmpInfo)
			}
		}
		
		for obj in instrumentPlugHandler {
			let tmpInfo = PlugInInfo(plugName: obj.menuName, author: obj.authorString, plugType: instrumentPlugName, plugURL: obj.file.bundleURL)
			if !plugInInfos.contains(tmpInfo) {
				plugInInfos.append(tmpInfo)
			}
		}
		
		for obj in samplesHandler {
			let tmpInfo = PlugInInfo(plugName: obj.menuName, author: obj.authorString, plugType: instrumentPlugName, plugURL: obj.file.bundleURL)
			if !plugInInfos.contains(tmpInfo) {
				plugInInfos.append(tmpInfo)
			}
		}
		
		for obj in digitalHandler {
			let tmpInfo = PlugInInfo(plugName: obj.menuName, author: obj.authorString, plugType: digitalPlugName, plugURL: obj.file.bundleURL)
			if !plugInInfos.contains(tmpInfo) {
				plugInInfos.append(tmpInfo)
			}
		}
		
		for obj in filterHandler {
			let tmpInfo = PlugInInfo(plugName: obj.menuName, author: obj.authorString, plugType: filterPlugName, plugURL: obj.file.bundleURL)
			if !plugInInfos.contains(tmpInfo) {
				plugInInfos.append(tmpInfo)
			}
		}
		
		for obj in complexImport {
			let tmpInfo = PlugInInfo(plugName: obj.menuName, author: obj.authorString, plugType: complexTrackerPlugName, plugURL: obj.file.bundleURL)
			if !plugInInfos.contains(tmpInfo) {
				plugInInfos.append(tmpInfo)
			}
		}
		
		plugInInfos.sort(by: { (obj1, obj2) -> Bool in
			let menuNam1 = obj1.plugName
			let menuNam2 = obj2.plugName
			let res = menuNam1.localizedStandardCompare(menuNam2)
			return res == .orderedAscending
		})
		
		aboutPlugInMenu.removeAllItems()
		
		for (i, pi) in plugInInfos.enumerated() {
			let mi = NSMenuItem(title: pi.plugName, action: #selector(AppDelegate.showPlugInInfo(_:)), keyEquivalent: "")
			mi.tag = i
			mi.target = self
			aboutPlugInMenu.addItem(mi)
		}
	}
	
	private func registerDefaults() {
		var tooLargeDict: [String: Any] = [PPSoundDriver: Int(MADSoundOutput.CoreAudioDriver.rawValue)]
		
		let defaults1: [String: Any]  = [PPSoundOutBits: 16,
			PPSoundOutRate: 44100,
			PPStereoDelayToggle: true,
			PPReverbToggle: false,
			PPSurroundToggle: false,
			PPOversamplingToggle: false,
			PPStereoDelayAmount: 30,
			PPReverbAmount: 25,
			PPReverbStrength: 30,
			PPOversamplingAmount: 1]
		
		let defaults2: [String: Any]  = [PPDEShowInstrument: true,
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
		
		let defaults3: [String: Any]  = [PPBEMarkersEnabled: true,
			PPBEMarkersOffset: 0,
			PPBEMarkersLoop: 3,
			PPBEOctaveMarkers: true,
			PPBENotesProjection: false,
			PPDEMarkerColorPref: makeNSRGB(0xFFFF, 0xFFFF, 0x9999).encodeColor(),
			
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
		
		let colorDefaults: [String: Data] = [
			PPCColor1: makeNSRGB(0xEEEE, 0, 0).encodeColor(),
			PPCColor2: makeNSRGB(0x8C8C, 0xBCBC, 0x1C1C).encodeColor(),
			PPCColor3: makeNSRGB(0x3333, 0xFFFF, 0xFFFF).encodeColor(),
			PPCColor4: NSColor.yellow.encodeColor(),
			PPCColor5: makeNSRGB(0x6060, 0xCACA, 36494).encodeColor(),
			PPCColor6: makeNSRGB(0x3333, 0xFFFF, 0xFFFF).encodeColor(),
			PPCColor7: makeNSRGB(0x6666, 0xCCCC, 0xFFFF).encodeColor(),
			PPCColor8: makeNSRGB(0xFFFF, 0x6354, 0xFFFF).encodeColor(),
			PPCColor9: makeNSRGB(0xCCCC, 0x9999, 0x6666).encodeColor(),
			PPCColor10: makeNSRGB(0x3333, 0x6666, 0x3333).encodeColor(),
			PPCColor11: makeNSRGB(0xFFFF, 0x6666, 0xFFFF).encodeColor(),
			PPCColor12: makeNSRGB(0xFFFF, 0x9999, 0).encodeColor(),
			PPCColor13: makeNSRGB(0xCCCC, 0xFFFF, 0xCCCC).encodeColor(),
			PPCColor14: makeNSRGB(0x9999, 0, 0x6666).encodeColor(),
			PPCColor15: makeNSRGB(0x6666, 0xCCCC, 0).encodeColor(),
			PPCColor16: makeNSRGB(0xFFFF, 0x9999, 0xFFFF).encodeColor(),
			PPCColor17: makeNSRGB(0xCCCC, 0xFFFF, 0x6666).encodeColor(),
			PPCColor18: makeNSRGB(0x6666, 0, 0x9999).encodeColor(),
			PPCColor19: makeNSRGB(0xCCCC, 0, 0x9999).encodeColor(),
			PPCColor20: makeNSRGB(0xCCCC, 0x9999, 0xFFFF).encodeColor(),
			PPCColor21: makeNSRGB(0x6666, 0xCCCC, 0x6666).encodeColor(),
			PPCColor22: makeNSRGB(0xCCCC, 0xCCCC, 0x3333).encodeColor(),
			PPCColor23: makeNSRGB(0x9999, 0xCCCC, 0xCCCC).encodeColor(),
			PPCColor24: makeNSRGB(0x6666, 13109, 0x6666).encodeColor(),
			PPCColor25: makeNSRGB(0xCCCC, 0x9999, 0xCCCC).encodeColor(),
			PPCColor26: makeNSRGB(0x3333, 0xFFFF, 0xFFFF).encodeColor(),
			PPCColor27: makeNSRGB(0x3333, 0xFFFF, 0x6666).encodeColor(),
			PPCColor28: makeNSRGB(0x6666, 0xFFFF, 0x3333).encodeColor(),
			PPCColor29: makeNSRGB(0xCCCC, 0x9999, 0xCCCC).encodeColor(),
			PPCColor30: makeNSRGB(0xCCCC, 0x9999, 0x3333).encodeColor(),
			PPCColor31: makeNSRGB(0xCCCC, 0xCCCC, 0x3333).encodeColor(),
			PPCColor32: makeNSRGB(0xFFFF, 0xCCCC, 0).encodeColor(),
			PPCColor33: makeNSRGB(0xEEEE, 0, 0).encodeColor(),
			PPCColor34: makeNSRGB(0, 0xFFFF, 0x6666).encodeColor(),
			PPCColor35: NSColor.cyan.encodeColor(),
			PPCColor36: NSColor.yellow.encodeColor(),
			PPCColor37: NSColor.green.encodeColor(),
			PPCColor38: makeNSRGB(0x3333, 0xCCCC, 0xFFFF).encodeColor(),
			PPCColor39: makeNSRGB(0x9999, 0xCCCC, 0x9999).encodeColor(),
			PPCColor40: makeNSRGB(0xFFFF, 0x9999, 0x6666).encodeColor(),
			PPCColor41: makeNSRGB(0x9999, 0xFFFF, 0xFFFF).encodeColor(),
			PPCColor42: makeNSRGB(0x6666, 0xFFFF, 0xFFFF).encodeColor(),
			PPCColor43: makeNSRGB(0xFFFF, 0x9999, 0xFFFF).encodeColor(),
			PPCColor44: makeNSRGB(0, 0x5555, 0).encodeColor(),
			PPCColor45: makeNSRGB(0xCCCC, 0xFFFF, 0xCCCC).encodeColor(),
			PPCColor46: makeNSRGB(0x6666, 0, 0).encodeColor(),
			PPCColor47: makeNSRGB(0x3333, 0xFFFF, 0x6666).encodeColor(),
			PPCColor48: makeNSRGB(0xFFFF, 0xFFFF, 0x6666).encodeColor(),
			PPCColor49: makeNSRGB(0xCCCC, 0, 0).encodeColor(),
			PPCColor50: makeNSRGB(0x9999, 0, 0x3333).encodeColor(),
			PPCColor51: makeNSRGB(0x9999, 0xFFFF, 0x9999).encodeColor(),
			PPCColor52: makeNSRGB(0x9999, 0xCCCC, 0xCCCC).encodeColor(),
			PPCColor53: makeNSRGB(0xCCCC, 0xCCCC, 0).encodeColor(),
			PPCColor54: makeNSRGB(0xCCCC, 0xCCCC, 0x3333).encodeColor(),
			PPCColor55: makeNSRGB(0xFFFF, 0x9999, 0xFFFF).encodeColor(),
			PPCColor56: NSColor.red.encodeColor(),
			PPCColor57: makeNSRGB(0xFFFF, 0, 0x6666).encodeColor(),
			PPCColor58: makeNSRGB(0x6666, 0xFFFF, 0xFFFF).encodeColor(),
			PPCColor59: makeNSRGB(0x6666, 0xFFFF, 0xCCCC).encodeColor(),
			PPCColor60: makeNSRGB(0x9999, 0xCCCC, 0x6666).encodeColor(),
			PPCColor61: makeNSRGB(0x9999, 0xCCCC, 0x9999).encodeColor(),
			PPCColor62: makeNSRGB(0x9999, 0xCCCC, 0x6666).encodeColor(),
			PPCColor63: makeNSRGB(0xCCCC, 0xCCCC, 0x6666).encodeColor(),
			PPCColor64: makeNSRGB(0xCCCC, 0xCCCC, 0x9999).encodeColor(),
			PPCColor65: makeNSRGB(0xEEEE, 0, 0).encodeColor(),
			PPCColor66: makeNSRGB(0, 0xFFFF, 0x6666).encodeColor(),
			PPCColor67: makeNSRGB(0, 0xFFFF, 0xFFFF).encodeColor(),
			PPCColor68: makeNSRGB(0xFFFF, 0xFFFF, 0).encodeColor(),
			PPCColor69: NSColor.green.encodeColor(),
			PPCColor70: makeNSRGB(0x3333, 0xCCCC, 0xFFFF).encodeColor(),
			PPCColor71: makeNSRGB(0x9999, 0xCCCC, 0x9999).encodeColor(),
			PPCColor72: makeNSRGB(0xFFFF, 0x9999, 0x6666).encodeColor(),
			PPCColor73: makeNSRGB(0x9999, 0xFFFF, 0xFFFF).encodeColor(),
			PPCColor74: makeNSRGB(0x6666, 0xFFFF, 0xFFFF).encodeColor(),
			PPCColor75: makeNSRGB(0xFFFF, 0x9999, 0xFFFF).encodeColor(),
			PPCColor76: makeNSRGB(0, 0x5555, 0).encodeColor(),
			PPCColor77: makeNSRGB(0xCCCC, 0xFFFF, 0xCCCC).encodeColor(),
			PPCColor78: makeNSRGB(0x6666, 0, 0).encodeColor(),
			PPCColor79: makeNSRGB(0x3333, 0xFFFF, 0x6666).encodeColor(),
			PPCColor80: makeNSRGB(0xFFFF, 0xFFFF, 0x6666).encodeColor(),
			PPCColor81: makeNSRGB(0xCCCC, 0, 0).encodeColor(),
			PPCColor82: makeNSRGB(0x9999, 0, 0x3333).encodeColor(),
			PPCColor83: makeNSRGB(0x9999, 0xFFFF, 0x9999).encodeColor(),
			PPCColor84: makeNSRGB(0x9999, 0xCCCC, 0xCCCC).encodeColor(),
			PPCColor85: makeNSRGB(0xCCCC, 0xCCCC, 0).encodeColor(),
			PPCColor86: makeNSRGB(0xCCCC, 0xCCCC, 0x3333).encodeColor(),
			PPCColor87: makeNSRGB(0xFFFF, 0x9999, 0xFFFF).encodeColor(),
			PPCColor88: NSColor.red.encodeColor(),
			PPCColor89: makeNSRGB(0x9999, 0, 0x6666).encodeColor(),
			PPCColor90: makeNSRGB(0x6666, 0xFFFF, 0xFFFF).encodeColor(),
			PPCColor91: makeNSRGB(0x6666, 0xFFFF, 0xCCCC).encodeColor(),
			PPCColor92: makeNSRGB(0x9999, 0xCCCC, 0x6666).encodeColor(),
			PPCColor93: makeNSRGB(0x9999, 0xCCCC, 0x9999).encodeColor(),
			PPCColor94: makeNSRGB(0x9999, 0xCCCC, 0x6666).encodeColor(),
			PPCColor95: makeNSRGB(0xCCCC, 0xCCCC, 0x6666).encodeColor(),
			PPCColor96: makeNSRGB(0xCCCC, 0xCCCC, 0x9999).encodeColor()]
		
		tooLargeDict += defaults1
		tooLargeDict += defaults2
		tooLargeDict += defaults3
		tooLargeDict += colorDefaults
		
		UserDefaults.standard.register(defaults: tooLargeDict)
	}
	
	override init() {
		super.init()
		registerDefaults()
	}
	
	@discardableResult
	func handleFile(_ theURL1: URL, ofType theUTI: String) -> Bool {
		let sharedWorkspace = NSWorkspace.shared
		var theURL = theURL1
		if sharedWorkspace.type(theUTI, conformsToType: MADNativeUTI) {
			// Document controller should automatically handle this.
			// But just in case...
			if let _ = document(for: theURL1) {
				return true
			} else {
				openDocument(withContentsOf: theURL1, display: true, completionHandler: { (_, alreadyOpen, error) -> Void in
					
					if alreadyOpen {
						print("\(theURL1) is already open? How did we not catch this?")
					}
					
					if let aErr = error {
						let alertErr = NSAlert(error: aErr)
						DispatchQueue.main.async {
							alertErr.runModal()
							
							return
						}
					}
				})
			}
			return true
		} else if theUTI  == MADGenericUTI {
			let invExt = NSLocalizedString("Invalid Extension", comment: "Invalid Extension")
			let invExtDes = NSLocalizedString("The file %@ is identified as as a generic MAD tracker, and not a specific one. Renaming it will fix this. Do you want to rename the file extension?", comment: "Invalid extension description")
			let renameFile =  NSLocalizedString("Rename", comment: "rename file")
			let openFile = NSLocalizedString("Open", comment: "Open a file")
			let cancelOp = NSLocalizedString("Cancel", comment: "Cancel")
			let unwrapped = String(format: invExtDes, theURL.lastPathComponent)
			
			let retVal = PPRunInformationalAlertPanel(title: invExt, message: unwrapped, defaultButton: renameFile, alternateButton: openFile, otherButton: cancelOp);
			switch (retVal) {
			case NSApplication.ModalResponse.alertFirstButtonReturn:
				do {
					let identRet = try madLib.identifyFile(at: theURL)
					let info = try! madLib.information(from: theURL, type: identRet)
					let tmpURL = theURL.deletingPathExtension().appendingPathExtension(info.signature.lowercased())
					do {
						try FileManager.default.moveItem(at: theURL, to: tmpURL)
						theURL = tmpURL
						//TODO: regenerate the UTI
						
					} catch {
						let couldNotRenameStr = String(format: NSLocalizedString("The file could not be renamed to \"%@\".\n\nThe music file \"%@\" will still be loaded.", comment: "Could not rename file"), tmpURL.lastPathComponent, theURL.lastPathComponent)
						PPRunInformationalAlertPanel(title: NSLocalizedString("Rename Error", comment: "Rename Error"), message: couldNotRenameStr)
					}
				} catch {
					PPRunCriticalAlertPanel(title: NSLocalizedString("Unknown File", comment: "unknown file"), message: NSLocalizedString("The file type could not be identified.", comment: "Unidentified file"))
					return false
				}
				
			case NSApplication.ModalResponse.alertSecondButtonReturn:
				break;
				
				//case NSAlertThirdButtonReturn:
			default:
				return false;
			}
		}
		if (sharedWorkspace.type(theUTI, conformsToType:PPPCMDUTI)) {
			do {
				try importPcmd(from: theURL)
				return true
			} catch let error {
				NSAlert(error: error).runModal()
				return false
			}
		} else if (sharedWorkspace.type(theUTI, conformsToType:PPInstrumentListUTI)) {
			do {
				try importInstrumentList(from: theURL)
				return true
			} catch let err {
				NSAlert(error: err).runModal()
				return false
			}
		} //else
		
		for obj in complexImport {
			for aUTI in obj.utiTypes {
				if sharedWorkspace.type(theUTI, conformsToType: aUTI) {
					let canImport: Bool
					do {
						try obj.canImport(theURL1)
						canImport = true
					} catch _ {
						canImport = false
					}
					if canImport {
						obj.beginImport(of: theURL1, withHandler: { (ourObject, anErr) -> Void in
							if let anErr = anErr {
								if PPErrorIsUserCancelled(anErr) == false {
									NSAlert(error: anErr).runModal()
								} else {
									NSSound.beep()
								}
							} else if let ourObject = ourObject {
								let aPPDoc = PPDocument(music: ourObject)
								
								self.addDocument(aPPDoc)
								aPPDoc.makeWindowControllers()
								aPPDoc.showWindows()
								aPPDoc.displayName = ((theURL1.lastPathComponent as NSString).deletingPathExtension)
							} else {
								fatalError("Either ourObject or anErr should be nil, not both!")
							}
						})
						return true
					}
				}
			}
		}
		
		
		//TODO: check for valid extension.
		for aUTI in trackerUTIs {
			if sharedWorkspace.type(theUTI, conformsToType: aUTI) {
				if let _ = document(for: theURL1) {
					return true
				} else {
					openDocument(withContentsOf: theURL1, display: true, completionHandler: { (_, alreadyOpen, error) -> Void in
						
						if alreadyOpen {
							print("\(theURL1) is already open? How did we not catch this?")
						}
						
						if let aErr = error {
							let alertErr = NSAlert(error: aErr)
							DispatchQueue.main.async {
								alertErr.runModal()
								
								return
							}
						}
					})
				}
				return true
			}
		}
		
		for obj in instrumentPlugHandler {
			for aUTI in obj.utiTypes {
				if sharedWorkspace.type(theUTI, conformsToType:aUTI) {
					do {
						try importInstrument(from: theURL, makeUserSelectInstrument: true)
						return true
					} catch let theErr {
						NSAlert(error: theErr).runModal()
						return false
					}
				}
			}
		}
		
		for obj in samplesHandler {
			for aUTI in obj.utiTypes {
				if sharedWorkspace.type(theUTI, conformsToType:aUTI) {
					do {
						try importSample(from: theURL, makeUserSelectSample: true)
						return true
					} catch let theErr {
						NSAlert(error: theErr).runModal()
						return false
					}
				}
			}
		}
		
		return false;
	}
	
	func importSample(from theURL: URL, makeUserSelectSample: Bool = false) throws {
		throw PPMADError(.orderNotImplemented)
		
	}
	
	func importInstrument(from theURL: URL, makeUserSelectInstrument: Bool = false) throws {
		throw PPMADError(.orderNotImplemented)
		
	}

	func importPcmd(from url: URL) throws {
		throw PPMADError(.orderNotImplemented)
		
	}
	
	func importInstrumentList(from url: URL) throws {
		throw PPMADError(.orderNotImplemented)
		
	}
	
	required init?(coder: NSCoder) {
		super.init(coder: coder)
		registerDefaults()
	}
	
	func applicationDidFinishLaunching(_ notification: Notification) {
		PPLibrary.registerDebugBlock(CocoaDebugStr)
		let defaults = UserDefaults.standard
		
		for (i, obj) in instrumentPlugHandler.enumerated() {
			if (obj.mode == MADPlugModes.importExport || obj.mode == MADPlugModes.export) {
				let mi = NSMenuItem(title: obj.menuName, action: #selector(InstrumentPanelController.exportInstrument(_:)), keyEquivalent: "")
				mi.tag = i;
				mi.target = nil
				instrumentExportMenu.addItem(mi);
			}
		}
		
		for (i, obj) in (madLib).enumerated() {
			if (obj.canExport) {
				let mi = NSMenuItem(title: "\(obj.menuName)…", action: #selector(DocumentWindowController.exportMusicAs(_:)) , keyEquivalent: "")
				mi.tag = i
				mi.target = nil
				musicExportMenu.addItem(mi)
			}
		}
		
		for i in 1 ... 96 {
			var theColor: NSColor
			if let colorData = defaults.data(forKey: "PPColor \(i)"), let tmpColor = NSColor.decodeColor(with: colorData) {
				theColor = tmpColor
			} else {
				defaults.removeObject(forKey: "PPColor \(i)")
				theColor = NSColor.decodeColor(with: defaults.data(forKey: "PPColor \(i)")!)!
			}
			thePPColors.append(theColor)
		}
		
		updatePlugInInfoMenu()
	}
	
	override func makeUntitledDocument(ofType typeName: String) throws -> NSDocument {
		assert(typeName == MADNativeUTI, "Unknown type passed to \(#function): \(typeName)")
		let theDoc = PPDocument(music: PPMusicObject())
		
		return theDoc
	}
	
	@IBAction func openFile(_ sender: AnyObject?) {
		let panel = NSOpenPanel();
		let otherDict: [String : [String]]  = ["PCMD": [PPPCMDUTI], "Instrument List": [PPInstrumentListUTI]];
		var samplesDict = [String: [String]]()
		for obj in instrumentPlugHandler {
			if (obj.mode == .import || obj.mode == .importExport) {
				samplesDict[obj.menuName] = (obj.utiTypes );
			}
		}
		
		for obj in samplesHandler {
			if obj.canImport {
				samplesDict[obj.menuName] = obj.utiTypes
			}
		}
		
		if let av = OpenPanelViewController(openPanel: panel, trackerDictionary: importDict, instrumentDictionary: samplesDict, additionalDictionary: otherDict) {
			av.setupDefaults()
			av.beginWithCompletionHandler { (retval) -> Void in
				if retval == NSFileHandlingPanelOKButton {
					let panelURL = panel.url!
					let filename = panelURL.path
					var err: NSError? = nil
					let utiFile: String?
					do {
						utiFile = try NSWorkspace.shared.type(ofFile: filename)
					} catch let error as NSError {
						err = error
						utiFile = nil
					} catch {
						fatalError()
					}
					if let err = err {
						PPRunAlertPanel(title: "Error opening file", message: String(format:"Unable to open %@: %@", (filename as NSString).lastPathComponent, err.localizedFailureReason!))
						return
					}
					self.handleFile(panelURL, ofType: utiFile!)
				}
			}
		}
	}
	
	override func documentClass(forType typeName: String) -> AnyClass? {
		let utiArray = trackerDict.values.flatMap({ $0 })
		if utiArray.contains(typeName) {
			return PPDocument.self
		}
		return nil
	}
	
	func application(_ theApplication: NSApplication, openFile filename: String) -> Bool {
		let utiFile: String?
		do {
			utiFile = try NSWorkspace.shared.type(ofFile: filename)
		} catch let err as NSError {
			PPRunAlertPanel(title: "Error opening file", message: String(format:"Unable to open %@: %@", (filename as NSString).lastPathComponent, err.localizedFailureReason!))
			return false
		}
		return handleFile(URL(fileURLWithPath: filename), ofType: utiFile!)
	}
}

extension AppDelegate: ExportObjectDelegate {
	func add(exportObject expObj: ExportObject) {
		exportObjects.append(expObj);
		expObj.run()
	}

	func exportObject(didFinish theObj: ExportObject) {
		
	}
	
	func exportObject(_ theObj: ExportObject, errorCode errCode: MADErr, errorString errStr: String?) {
		
	}
}
