//
//  AppDelegate.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/18/14.
//
//

import Cocoa
import PlayerPROKit

private func makeNSRGB(red: UInt16, green: UInt16, blue:UInt16) -> NSColor {
	return NSColor(calibratedRed: CGFloat(red) / CGFloat(UInt16.max), green: CGFloat(green) / CGFloat(UInt16.max), blue: CGFloat(blue) / CGFloat(UInt16.max), alpha: 1)
}

class AppDelegate: NSDocumentController, NSApplicationDelegate, PPExportObjectDelegate {
	private var exportObjects = [PPExportObject]()
	private var _trackerDict = [String: [String]]()
	private var _trackerUTIs = [String]()
	var plugInInfos = [PlugInInfo]()
	let madLib = PPLibrary()
	let instrumentPlugHandler = PPInstrumentPlugHandler()
	let digitalHandler = PPDigitalPlugHandler()
	let filterHandler = PPFilterPlugHandler()
	let preferences = PPPreferences()

	@IBOutlet var aboutPlugInMenu: NSMenu!
	@IBOutlet var instrumentExportMenu: NSMenu!

	var trackerDict: [String: [String]] { get {
		if _trackerDict.isEmpty || _trackerDict.count != Int(madLib.pluginCount) + 2 {
			let localMADKName = NSLocalizedString("PPMADKFile", tableName: "InfoPlist", comment: "MADK Tracker")
			let localGenericMADName = NSLocalizedString("Generic MAD tracker", comment: "Generic MAD tracker")
			var tmpTrackerDict = [localMADKName: [MADNativeUTI], localGenericMADName: [MADGenericUTI]] as [String: [String]]
			
			for objRaw in madLib {
				var obj = objRaw as PPLibraryObject
				tmpTrackerDict[obj.menuName] = (obj.UTItypes) as? [String]
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
		
		var infoCont = PlugInInfoController.windowControllerFromInfo(inf)
		infoCont.window.center()
		NSApplication.sharedApplication().runModalForWindow(infoCont.window)
		//[infoCont showWindow:sender];
	}

	func updatePlugInInfoMenu() {
		let trackerPlugName = NSLocalizedString("TrackerPlugName", comment: "Tracker plug-in name")
		let instrumentPlugName = NSLocalizedString("InstrumentPlugName", comment: "Instrument plug-in name")
		let digitalPlugName = NSLocalizedString("DigitalPlugName", comment: "Digital plug-in name")
		let filterPlugName = NSLocalizedString("FilterPlugName", comment: "Filter plug-in name")
		for rawObj in madLib {
			let obj = rawObj as PPLibraryObject
			var tmpInfo = PlugInInfo(plugName: obj.menuName, author: obj.menuName, plugType: trackerPlugName, plugURL: obj.plugFile.bundleURL)
			//if (![plugInInfos containsObject:tmpInfo]) {
			//	[plugInInfos addObject:tmpInfo];
			//}
		}
		
		for objRaw in instrumentPlugHandler {
			let obj = objRaw as PPInstrumentImporterObject
			var tmpInfo = PlugInInfo(plugName: obj.menuName, author: obj.authorString, plugType: instrumentPlugName, plugURL: obj.file.bundleURL)
			//if (![plugInInfos containsObject:tmpInfo]) {
			plugInInfos.append(tmpInfo)
			//}
		}
		
		for objRaw in digitalHandler {
			let obj = objRaw as PPDigitalPlugInObject
			var tmpInfo = PlugInInfo(plugName: obj.menuName, author: obj.authorString, plugType: digitalPlugName, plugURL: obj.file.bundleURL)
			//if (![plugInInfos containsObject:tmpInfo]) {
			plugInInfos.append(tmpInfo)
			//}
		}
		
		for objRaw in filterHandler {
			let obj = objRaw as PPFilterPlugObject
			var tmpInfo = PlugInInfo(plugName: obj.menuName, author: obj.authorString, plugType: filterPlugName, plugURL: obj.file.bundleURL)
			//if (![plugInInfos containsObject:tmpInfo]) {
			plugInInfos.append(tmpInfo)
			//}
		}
		
		plugInInfos.sort({ (obj1:PlugInInfo, obj2:PlugInInfo) -> Bool in
			let menuNam1 = obj1.plugName
			let menuNam2 = obj2.plugName
			let res = menuNam1.localizedStandardCompare(menuNam2)
			return res == NSComparisonResult.OrderedAscending;
			
		})
		
		aboutPlugInMenu.removeAllItems()
		
		for (i, pi) in enumerate(plugInInfos) {
			var mi = NSMenuItem(title: pi.plugName, action: "showPlugInInfo:", keyEquivalent: "")
			mi.tag = i
			mi.target = self
			aboutPlugInMenu.addItem(mi)
		}
	}
	
	private func registerDefaults() {
		var tooLargeDict: [String: AnyObject] = [PPSoundDriver: Int(MADSoundOutput.CoreAudioDriver.toRaw()),
			PPDEMarkerColorPref: makeNSRGB(65535, 65535, 39321).PPencodeColor()]
		
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
		var alltogether = NSMutableDictionary(dictionary: tooLargeDict)
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
	
	required init(coder: NSCoder!) {
		
		
		super.init(coder: coder)
		registerDefaults()
	}
	
	func PPExportObjectDidFinish(theObj: PPExportObject!) {
		
	}
	
	func PPExportObjectEncounteredError(theObj: PPExportObject!, errorCode errCode: OSErr, errorString errStr: String!) {
		
	}
	
	func addExportObject(expObj: PPExportObject) {
	
	}
}
