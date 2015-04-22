//
//  PlayerAppDelegate.swift
//  PPMacho
//
//  Created by C.W. Betts on 4/9/15.
//
//

import Cocoa
import PlayerPROCore
import PlayerPROKit
import AVFoundation
import AudioToolbox
import SwiftAdditions
import SwiftAudioAdditions

private let kUnresolvableFile = "Unresolvable files"
private let kUnresolvableFileDescription = "There were %lu file(s) that were unable to be resolved."

private func cocoaDebugStr(line: Int16, file: UnsafePointer<Int8>, text: UnsafePointer<Int8>) {
	let swiftFile = NSFileManager.defaultManager().stringWithFileSystemRepresentation(file, length: Int(strlen(file)))
	let swiftText = String.fromCString(text)!
	println("\(swiftFile):\(line), error text: \(swiftText)")
	let errStr = NSLocalizedString("MyDebugStr_Error", comment: "Error")
	var mainStr = NSLocalizedString("MyDebugStr_MainText", comment: "The Main text to display")
	let quitStr = NSLocalizedString("MyDebugStr_Quit", comment: "Quit")
	let contStr = NSLocalizedString("MyDebugStr_Continue", comment: "Continue")
	let debuStr = NSLocalizedString("MyDebugStr_Debug", comment: "Debug")
	if let ohai = mainStr.rangeOfString("%s") {
		mainStr.replaceRange(ohai, with: swiftText)
	}
	
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

@NSApplicationMain
class PlayerAppDelegate: NSObject, NSApplicationDelegate, SoundSettingsViewControllerDelegate, NSTableViewDelegate, NSToolbarDelegate, NSTableViewDataSource {
	@IBOutlet var window: NSWindow! = nil
	@IBOutlet var loopButton: NSButton!
	@IBOutlet var playButton: NSButton!
	@IBOutlet var stopButton: NSButton!
	@IBOutlet var songCurTime: NSTextField!
	@IBOutlet var songTotalTime: NSTextField!
	@IBOutlet var songPos: NSSlider!
	@IBOutlet var pauseButton: NSButton!
	
	@IBOutlet var aboutPlugInMenu: NSMenu!
	
	@IBOutlet var musicExportMenu: NSMenu!
	
	@IBOutlet var infoDrawer: NSDrawer!
	@IBOutlet var fileName: NSTextField!
	@IBOutlet var internalName: NSTextField!
	@IBOutlet var fileSize: NSTextField!
	@IBOutlet var musicInstrument: NSTextField!
	@IBOutlet var musicPatterns: NSTextField!
	@IBOutlet var musicPlugType: NSTextField!
	@IBOutlet var musicSignature: NSTextField!
	@IBOutlet var fileLocation: NSTextField!
	
	@IBOutlet var tableView: NSTableView!
	@IBOutlet var musicListController: NSArrayController!
	@IBOutlet var exportWindow: NSWindow!
	@IBOutlet var pauseDockMenuItem: NSMenuItem!
	dynamic var music: PPMusicObject!
	
	@IBOutlet var toolsPanel: NSPanel!
	var timeChecker: NSTimer!
	var madDriver: PPDriver!
	var madLib = PPLibrary()!
	
	dynamic var paused: Bool = true {
		didSet {
			if paused {
				pauseButton.state = NSOnState
				pauseDockMenuItem.state = NSOffState
			} else {
				pauseButton.state = NSOffState
				pauseDockMenuItem.state = NSOnState
			}
		}
	}
	private var isQuitting = false
	var instrumentController: InstrumentWindowController!
	
	var exportSettings = MADDriverSettings.new()
	@IBOutlet var exportSettingsBox: NSBox!
	var exportController: SoundSettingsViewController!
	
	dynamic let musicList = MusicList()
	let preferences = Preferences.newPreferenceController()
	var plugInInfos = [PlugInInfo]()
	
	private var currentlyPlayingIndex	= CurrentlyPlayingIndex()
	private var previouslyPlayingIndex	= CurrentlyPlayingIndex()
	
	private dynamic var musicName = ""
	private dynamic var musicInfo = ""
	private var selMusFromList = -1

	private(set) lazy var trackerDict: [String: [String]] = {
		let localMADKName = NSLocalizedString("PPMADKFile", tableName: "InfoPlist", value: "MADK Tracker", comment: "MADK Tracker")
		let localGenericMADName = NSLocalizedString("Generic MAD tracker", comment: "Generic MAD tracker")
		var tmpTrackerDict = [localMADKName: [MADNativeUTI], localGenericMADName: [MADGenericUTI]]
		
		for obj in self.madLib {
			tmpTrackerDict[obj.menuName] = obj.UTITypes
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

	
	//MARK: -
	
	private func musicListWillChange() -> Bool {
		return true
	}
	
	func musicListDidChange() {
		if (currentlyPlayingIndex.index != -1) {
			madDriver.stop()
			madDriver.cleanDriver()
		}
		if (NSUserDefaults.standardUserDefaults().boolForKey(PPLoadMusicAtListLoad) && musicList.countOfMusicList > 0) {
			var err: NSError? = nil
			currentlyPlayingIndex.index = selMusFromList != -1 ? selMusFromList : 0;
			selectCurrentlyPlayingMusic()
			if (!loadMusicFromCurrentlyPlayingIndexWithError(&err)) {
				NSAlert(error: err!).runModal()
			}
		} else if (selMusFromList != -1) {
			selectMusicAtIndex(selMusFromList)
		}
		let lostCount = musicList.lostMusicCount;
		if (lostCount != 0) {
			let uresolvedStr = String(format: kUnresolvableFileDescription, lostCount)
			PPRunAlertPanel(kUnresolvableFile, message: uresolvedStr)
		}
	}
	
	private func changeValueForMusicListKey(@noescape theClosure: () -> Void) {
		willChangeValueForKey(kMusicListKVO)
		theClosure()
		didChangeValueForKey(kMusicListKVO)
	}
	
	private func musicListContentsDidMove() {
		var i = 0;
		if (currentlyPlayingIndex.index != -1) {
			for (i = 0; i < musicList.countOfMusicList; i++) {
				if (currentlyPlayingIndex.playbackURL == musicList.URLAtIndex(i)) {
					currentlyPlayingIndex.index = i;
					break;
				}
			}
		}
		if (previouslyPlayingIndex.index != -1) {
			for (i = 0; i < musicList.countOfMusicList; i++) {
				if (previouslyPlayingIndex.playbackURL == musicList.URLAtIndex(i)) {
					previouslyPlayingIndex.index = i;
					break;
				}
			}
		}
	}
	
	@IBAction func showMusicList(sender: AnyObject?) {
		window.makeKeyAndOrderFront(sender)
	}

	
	@IBAction func showPreferences(sender: AnyObject!) {
		preferences.window!.center()
		preferences.showWindow(sender)
	}
	
	override init() {
		let ourDefaults: [NSObject: AnyObject] = [PPRememberMusicList: true,
			PPLoadMusicAtListLoad: false,
			PPAfterPlayingMusic: Int(PlaylistMode.StopPlaying.rawValue),
			PPGotoStartupAfterPlaying: true,
			PPSaveModList: true,
			PPLoadMusicAtMusicLoad: false,
			PPLoopMusicWhenDone: false,
			
			PPSoundOutBits: 16,
			PPSoundOutRate: 44100,
			PPSoundDriver: Int(MADSoundOutput.CoreAudioDriver.rawValue),
			PPStereoDelayToggle: true,
			PPReverbToggle: false,
			PPSurroundToggle: false,
			PPOversamplingToggle: false,
			PPStereoDelayAmount: 30,
			PPReverbAmount: 25,
			PPReverbStrength: 30,
			PPOversamplingAmount: 1,
			
			PPMAddExtension: true,
			PPMMadCompression: true,
			PPMNoLoadMixerFromFiles: false,
			PPMOscilloscopeDrawLines: true]
		
		NSUserDefaults.standardUserDefaults().registerDefaults(ourDefaults);
		super.init()
	}
	
	@objc private func updateMusicStats(theTimer: NSTimer?) {
		if (self.music != nil) {
			var fT = 0
			var cT = 0;
			madDriver.getMusicStatusWithCurrentTime(&cT, totalTime: &fT)
			if (madDriver.donePlayingMusic && paused == false && madDriver.exporting == false) {
				songIsDonePlaying()
				madDriver.getMusicStatusWithCurrentTime(&cT, totalTime: &fT)
			}
			songPos.doubleValue = Double(cT)
			songCurTime.integerValue = cT
		}
	}
	
	private func selectCurrentlyPlayingMusic() {
		if (self.currentlyPlayingIndex.index >= 0) {
			//currentlyPlayingIndex.playbackURL = [musicList URLAtIndex:currentlyPlayingIndex.index];
			selectMusicAtIndex(currentlyPlayingIndex.index)
		}
	}
	
	func addMusicToMusicList(theURL: NSURL, loadIfPreferencesAllow load: Bool = true) {
		willChangeValueForKey(kMusicListKVO)
		let okayMusic = musicList.addMusicURL(theURL, force: false)
		didChangeValueForKey(kMusicListKVO)
		switch okayMusic {
		case .Failure:
			return
			
		case .SimilarURL:
			let similarMusicIndex = musicList.indexOfObjectSimilar(URL: theURL)!
			let similarAlert = NSAlert()
			similarAlert.messageText = "Existing object";
			similarAlert.informativeText = "There is already a tracker file that points to the added file. Do you still wish to add it?";
			similarAlert.addButtonWithTitle("Add")
			
			let cancelButton = similarAlert.addButtonWithTitle("Cancel")
			cancelButton.keyEquivalent = PPEscapeValue;
			
			similarAlert.addButtonWithTitle("Load Existing")
			
			let alertSelect = similarAlert.runModal()
			
			switch (alertSelect) {
			case NSAlertFirstButtonReturn:
				musicList.addMusicURL(theURL, force: true)
				if load && NSUserDefaults.standardUserDefaults().boolForKey(PPLoadMusicAtMusicLoad) {
					self.currentlyPlayingIndex.index = musicList.countOfMusicList - 1;
					//currentlyPlayingIndex.playbackURL = [musicList URLAtIndex:currentlyPlayingIndex.index];
					selectCurrentlyPlayingMusic()
					var err: NSError?
					if !loadMusicFromCurrentlyPlayingIndexWithError(&err) {
						NSAlert(error: err!).runModal()
					}
				} else {
					selectMusicAtIndex(musicList.countOfMusicList - 1)
				}
				break;
				
			case NSAlertSecondButtonReturn:
				return;
				
			case NSAlertThirdButtonReturn:
				if load && NSUserDefaults.standardUserDefaults().boolForKey(PPLoadMusicAtMusicLoad) {
					self.currentlyPlayingIndex.index = similarMusicIndex;
					selectCurrentlyPlayingMusic()
					var err: NSError?
					if !loadMusicFromCurrentlyPlayingIndexWithError(&err) {
						NSAlert(error: err!).runModal()
					}
				} else {
					selectMusicAtIndex(similarMusicIndex)
				}
				
				break;
				
			default:
				break;
			}
			
		case .Success:
			if load && NSUserDefaults.standardUserDefaults().boolForKey(PPLoadMusicAtMusicLoad) {
				self.currentlyPlayingIndex.index = musicList.countOfMusicList - 1;
				//currentlyPlayingIndex.playbackURL = [musicList URLAtIndex:currentlyPlayingIndex.index];
				selectCurrentlyPlayingMusic()
				var err: NSError?
				if !loadMusicFromCurrentlyPlayingIndexWithError(&err) {
					NSAlert(error: err!).runModal()
				}
			} else {
				selectMusicAtIndex(musicList.countOfMusicList - 1)
			}
		}
	}
	
	private func songIsDonePlaying() {
		var err: NSError? = nil
		let userDefaults = NSUserDefaults.standardUserDefaults()
		switch (PlaylistMode(rawValue: userDefaults.integerForKey(PPAfterPlayingMusic))!) {

		case .LoopMusic:
			madDriver.musicPosition = 0;
			madDriver.play()
			break;

		case .LoadNext:
			if (musicList.countOfMusicList > ++self.currentlyPlayingIndex.index) {
				selectCurrentlyPlayingMusic()
				if (!loadMusicFromCurrentlyPlayingIndexWithError(&err)) {
					NSAlert(error: err!).runModal()
				}
			} else {
				if (userDefaults.boolForKey(PPLoopMusicWhenDone)) {
					currentlyPlayingIndex.index = 0;
					selectCurrentlyPlayingMusic()
					if (!self.loadMusicFromCurrentlyPlayingIndexWithError(&err))
					{
						NSAlert(error: err!).runModal()
					}
				} else {
					madDriver.stop()
					madDriver.cleanDriver()
					if (userDefaults.boolForKey(PPGotoStartupAfterPlaying)) {
						madDriver.musicPosition = 0;
					}
				}
			}
			break;
			
		case .LoadRandom:
			self.currentlyPlayingIndex.index = Int(arc4random_uniform(UInt32(musicList.countOfMusicList)))
			selectCurrentlyPlayingMusic()
			if (!loadMusicFromCurrentlyPlayingIndexWithError(&err)) {
				NSAlert(error: err!).runModal()
			}
			
			break;
			
			//case .StopPlaying:
			//	fallthrough
		default:
			madDriver.stop()
			madDriver.cleanDriver()
			if (userDefaults.boolForKey(PPGotoStartupAfterPlaying)) {
				madDriver.musicPosition = 0;
			}
			self.paused = true
			break;
		}
	}
	
	func loadMusicFromCurrentlyPlayingIndexWithError(theErr: NSErrorPointer) -> Bool {
		currentlyPlayingIndex.playbackURL = musicList.URLAtIndex(currentlyPlayingIndex.index)
		let isGood = loadMusicURL(currentlyPlayingIndex.playbackURL!, error: theErr)
		previouslyPlayingIndex = currentlyPlayingIndex
		return isGood;
	}
	
	@objc private func doubleClickMusicList() {
		var err: NSError? = nil
		self.currentlyPlayingIndex.index = tableView.selectedRow;
		if (loadMusicFromCurrentlyPlayingIndexWithError(&err) == false) {
			NSAlert(error: err!).runModal()
		}
	}
	
	private func addMusicsToMusicList(theMusList: [NSURL]) {
		for i in 0 ..< (theMusList.count - 1) {
			addMusicToMusicList(theMusList[i], loadIfPreferencesAllow: false)
		}
		addMusicToMusicList(theMusList.last!, loadIfPreferencesAllow: true)
	}
	
	func loadMusicURL(musicToLoad: NSURL, error: NSErrorPointer = nil, autoPlay: Bool = true) -> Bool {
		var fileType: String? = nil
		var theOSErr = MADErr.NoErr
		if music != nil {
			madDriver.stop()
			madDriver.stopDriver()
		}
		
		if let fileUTI = NSWorkspace.sharedWorkspace().typeOfFile(musicToLoad.path!, error: nil) {
			if let afileType = madLib.typeFromUTI(fileUTI) {
				theOSErr = madLib.testFile(URL: musicToLoad, type: afileType)
				if theOSErr == .NoErr {
					fileType = afileType
				}
			}
		}
		
		if fileType == nil {
			let iIdent = madLib.identifyFile(URL: musicToLoad)
			switch iIdent {
			case .Success(let val):
				theOSErr = .NoErr
				fileType = val
				
			case .Failure(let val):
				theOSErr = val
			}
		}
		
		if theOSErr != .NoErr {
			if error != nil {
				error.memory = createErrorFromMADErrorType(theOSErr)
			}
			paused = true
			clearMusic()
			return false
		}

		self.music = PPMusicObject(URL: musicToLoad, type: fileType!, library: madLib, error: error)
		if music == nil {
			//The PPMusicObject init should populate the error value.
			self.paused = true
			clearMusic()
			return false
		}
		
		music.attachToDriver(madDriver)
		
		if autoPlay {
			madDriver.play()
		}
		
		paused = false
		
		var cT = 0
		var fT = 0
		madDriver.getMusicStatusWithCurrentTime(&cT, totalTime:&fT)
		songPos.maxValue = Double(fT)
		songPos.minValue = 0
		setTitleForSongLabelBasedOnMusic()
		songTotalTime.integerValue = fT
		
		NSNotificationCenter.defaultCenter().postNotificationName(PPMusicDidChange, object:self)
		
		if error != nil {
			error.memory = nil
		}
		
		return true
	}
	
	private func clearMusic() {
		if (music != nil) {
			madDriver.stop()
			madDriver.cleanDriver()
		}
		
		self.paused = true;
		currentlyPlayingIndex.index = -1;
		currentlyPlayingIndex.playbackURL = nil;
		previouslyPlayingIndex = currentlyPlayingIndex
		self.music = PPMusicObject();
		setTitleForSongLabelBasedOnMusic()
		NSNotificationCenter.defaultCenter().postNotificationName(PPMusicDidChange, object:self)
		music.attachToDriver(madDriver)
	}
	
	private func setTitleForSongLabelBasedOnMusic() {
		self.musicName = music!.title
		self.musicInfo = music!.information
	}
	
	private func MADDriverWithPreferences() {
		var madWasReading = false;
		var fullTime = 0, curTime = 0;
		var returnerr: NSError?
		if (madDriver != nil) {
			madWasReading = !madDriver.paused
			madDriver.stop()
			//[madDriver stopDriver];
			
			madDriver.getMusicStatusWithCurrentTime(&curTime, totalTime:&fullTime)
		}
		var theSettinit = MADDriverSettings.new();
		let defaults = NSUserDefaults.standardUserDefaults()
		
		//TODO: Sanity Checking
		theSettinit.surround = defaults.boolForKey(PPSurroundToggle)
		theSettinit.outPutRate = UInt32(defaults.integerForKey(PPSoundOutRate))
		theSettinit.outPutBits = Int16(defaults.integerForKey(PPSoundOutBits))
		if defaults.boolForKey(PPOversamplingToggle) {
			theSettinit.oversampling = Int32(defaults.integerForKey(PPOversamplingAmount))
		} else {
			theSettinit.oversampling = 1;
		}
		theSettinit.Reverb = defaults.boolForKey(PPReverbToggle)
		theSettinit.ReverbSize = Int32(defaults.integerForKey(PPReverbAmount))
		theSettinit.ReverbStrength = Int32(defaults.integerForKey(PPReverbStrength))
		if defaults.boolForKey(PPStereoDelayToggle) {
			theSettinit.MicroDelaySize = Int32(defaults.integerForKey(PPStereoDelayAmount))
		} else {
			theSettinit.MicroDelaySize = 0;
		}
		
		theSettinit.driverMode = MADSoundOutput(rawValue: Int16(defaults.integerForKey(PPSoundDriver))) ?? .CoreAudioDriver
		theSettinit.repeatMusic = false;
		
		//OSErr returnerr = MADCreateDriver(&init, madLib, &madDriver);
		if (madDriver == nil) {
			madDriver = PPDriver(library: madLib, settings: &theSettinit, error: &returnerr)
		} else {
			returnerr = createErrorFromMADErrorType(madDriver.changeDriverSettingsToSettings(&theSettinit))
		}
		NSNotificationCenter.defaultCenter().postNotificationName(PPDriverDidChange, object: self)
		if let returnerr = returnerr {
			NSAlert(error: returnerr).runModal()
			return;
		}
		//MADStartDriver(madDriver);
		if (self.music != nil) {
			//MADAttachDriverToMusic(madDriver, music, NULL);
			if (madWasReading) {
				//MADSetMusicStatus(madDriver, 0, fullTime, curTime);
				madDriver.play()
			}
		}
	}
	
	private func updatePlugInInfoMenu() {
		let trackerPlugName = NSLocalizedString("TrackerPlugName", comment: "Tracker plug-in name")
		
		for obj in madLib {
			let tmpInfo = PlugInInfo(plugName: obj.menuName, author: obj.authorString, plugType: trackerPlugName, plugURL: obj.bundle.bundleURL)
			let infoArray = plugInInfos.filter({ (hi) -> Bool in
				return hi == tmpInfo
			})
			if infoArray.count == 0 {
				plugInInfos.append(tmpInfo)
			}
		}
		
		plugInInfos.sort({ (lhs, rhs) -> Bool in
			let menuNam1 = lhs.plugName
			let menuNam2 = rhs.plugName
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
	
	@IBAction func showPlugInInfo(sender: NSMenuItem) {
		let inf = (self.plugInInfos)[sender.tag]
	
		let infoCont = PlugInInfoController.windowControllerFromInfo(inf)
		if let infoWindow = infoCont.window {
			infoWindow.center()
			NSApplication.sharedApplication().runModalForWindow(infoWindow)
		}
	}
	
	private func selectMusicAtIndex(anIdx: Int) {
		if (anIdx < 0 || anIdx >= musicList.countOfMusicList) {
			NSBeep();
			return;
		}
		let idx = NSIndexSet(index:anIdx)
		tableView.selectRowIndexes(idx, byExtendingSelection: false)
		tableView.scrollRowToVisible(anIdx)
	}
	
	@IBAction func clearMusicList(sender: AnyObject?) {
		if musicList.countOfMusicList != 0 {
			let clearMusicListStr = String(format: NSLocalizedString("The music list contains %ld items. Do you really want to remove them?", comment: "Clear Music List?"), musicList.countOfMusicList)
			
			let returnVal = PPRunAlertPanel(NSLocalizedString("Clear list", comment: "Clear Music List"), message: clearMusicListStr, defaultButton: NSLocalizedString("No", comment: "No"), alternateButton: NSLocalizedString("Yes", comment: "Yes"));
			
			if (returnVal == NSAlertAlternateReturn) {
				changeValueForMusicListKey( {
					self.musicList.clearMusicList()
				})
				if (self.currentlyPlayingIndex.index != -1) {
					clearMusic()
				}
			}
		} else {
			NSBeep();
		}
	}
	
	@IBAction func saveMusicList(sender: AnyObject!) {
		let savePanel = NSSavePanel()
		savePanel.allowedFileTypes = [PPMusicListUTI]
		savePanel.canCreateDirectories = true
		savePanel.canSelectHiddenExtension = true
		savePanel.beginSheetModalForWindow(self.window, completionHandler: { (result) -> Void in
			if result == NSFileHandlingPanelOKButton {
				self.musicList.saveMusicListToURL(savePanel.URL!)
			}
		})
	}
	
	@IBAction func showInstrumentsList(sender: AnyObject?) {
		instrumentController!.showWindow(sender)
	}
	
	func handleFile(theURL1: NSURL, ofType theUTI: String) -> Bool {
		var sharedWorkspace = NSWorkspace.sharedWorkspace()
		var theURL = theURL1
		if (theUTI  == MADGenericUTI) {
			let invExt = NSLocalizedString("Invalid Extension", comment: "Invalid Extension")
			let invExtDes = String(format: NSLocalizedString("The file %@ is identified as as a generic MAD tracker, and not a specific one. Renaming it will fix this. Do you want to rename the file extension?", comment: "Invalid extension description"), theURL.lastPathComponent!)
			let renameFile =  NSLocalizedString("Rename", comment: "rename file")
			let openFile = NSLocalizedString("Open", comment: "Open a file")
			let cancelOp = NSLocalizedString("Cancel", comment: "Cancel")
			let retVal = PPRunInformationalAlertPanel(invExt, message:invExtDes, defaultButton: renameFile, alternateButton:openFile, otherButton: cancelOp)
			switch (retVal) {
			case NSAlertDefaultReturn:
				
				let identRet = madLib.identifyFile(URL: theURL)
				switch identRet {
				case .Failure(_):
					PPRunCriticalAlertPanel(NSLocalizedString("Unknown File", comment: "unknown file"), message: NSLocalizedString("The file type could not be identified.", comment: "Unidentified file"))
					return false
					
				case .Success(let madSubtype):
					let infoRet = madLib.informationFromFile(URL: theURL, type: madSubtype)
					if infoRet.error != .NoErr {
						PPRunCriticalAlertPanel(NSLocalizedString("Unknown File", comment: "unknown file"), message: NSLocalizedString("The file type could not be identified.", comment: "Unidentified file"))
						return false
					} else {
						let info = infoRet.info!
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
				return true;
			}
		}
		if (sharedWorkspace.type(theUTI, conformsToType:PPMusicListUTI)) {
			if (musicListWillChange()) {
				changeValueForMusicListKey({ () -> Void in
					self.musicList.loadMusicListFromURL(theURL)
					self.selMusFromList = self.musicList.selectedMusic
				})
				musicListDidChange()
				return true
			}
		} else if (sharedWorkspace.type(theUTI, conformsToType:PPOldMusicListUTI)) {
			if (musicListWillChange()) {
				self.willChangeValueForKey(kMusicListKVO)
				musicList.beginLoadingOfOldMusicListAtURL(theURL, completionHandle: { (theErr) -> Void in
					self.selMusFromList = self.musicList.selectedMusic
					self.didChangeValueForKey(kMusicListKVO)
					if let theErr = theErr {
						NSAlert(error: theErr).runModal()
					}
				})
				return true;
			}
		} else {
			//TODO: check for valid extension.
			for aUTI in trackerUTIs {
				if (sharedWorkspace.type(theUTI, conformsToType:aUTI)) {
					addMusicToMusicList(theURL)
					return true;
				}
			}
			
			// Final check, just in case the UTI is messed up...
			// Yes, it does happen.
			switch madLib.identifyFile(URL: theURL) {
			case .Success(let aType):
				let theRet = madLib.testFile(URL: theURL, type: aType)
				if theRet == .NoErr {
					addMusicToMusicList(theURL)
				}
				
			default:
				break
			}
		}
		return false;
	}
	
	@IBAction func openFile(sender: AnyObject?) {
		let panel = NSOpenPanel()
		let playlistDict = ["PlayerPRO Music List" : [PPMusicListUTI],
			"PlayerPRO Old Music List" : [PPOldMusicListUTI]];
		
		if let av = OpenPanelViewController(openPanel: panel, trackerDictionary: trackerDict, playlistDictionary: playlistDict) {
			av.setupDefaults()
			av.allowsMultipleSelectionOfTrackers = true;
			av.beginOpenPanel(parentWindow: window, completionHandler: { (result) -> Void in
				if result != NSFileHandlingPanelOKButton {
					return
				}
				let ws = NSWorkspace.sharedWorkspace()
				let panelURLs = panel.URLs as! [NSURL]
				for theURL in panelURLs {
					let fileName = theURL.path!
					var err: NSError?
					if let utiFile = ws.typeOfFile(fileName, error: &err) {
						self.handleFile(theURL, ofType: utiFile) //TODO: more efficient way of doing this!
					} else {
						PPRunAlertPanel("Error opening file", message: "Unable to open \(fileName.lastPathComponent): \(err!.localizedFailureReason)")
						return
					}
				}
			})
		}
	}
	
	func saveMusicToURL(tosave: NSURL) {
		music.saveMusicToURL(tosave)
	}

	
	@IBAction func saveMusicAs(sender: AnyObject?) {
		madDriver.beginExport()
	
		let savePanel = NSSavePanel()
		savePanel.allowedFileTypes = [MADNativeUTI]
		savePanel.canCreateDirectories = true
		savePanel.canSelectHiddenExtension = true
		if musicName != "" {
			savePanel.nameFieldStringValue = musicName
		}
		
		savePanel.beginSheetModalForWindow(window) { (result) -> Void in
			if result == NSFileHandlingPanelOKButton {
				let saveURL = savePanel.URL!
				self.saveMusicToURL(saveURL)
				self.addMusicToMusicList(saveURL, loadIfPreferencesAllow: false)
			}
			self.madDriver.endExport()
		}
	}

	@IBAction func saveMusic(sender: AnyObject) {
		madDriver.beginExport()
		
		if (previouslyPlayingIndex.index == -1) {
			// saveMusicAs: will end the exporting when it is done.
			saveMusicAs(sender)
		} else {
			let fileURL = previouslyPlayingIndex.playbackURL!
			let filename = fileURL.path!
			let sharedWorkspace = NSWorkspace.sharedWorkspace()
			let utiFile = sharedWorkspace.typeOfFile(filename, error: nil)!
			if /*[sharedWorkspace type:utiFile conformsToType:MADNativeUTI]*/ utiFile == MADNativeUTI {
				saveMusicToURL(fileURL)
				madDriver.endExport()
			} else {
				// saveMusicAs: will end the exporting when it is done.
				saveMusicAs(sender)
			}
		}
	}

	@objc private func soundPreferencesDidChange(notification: NSNotification) {
		MADDriverWithPreferences()
	}
	
	//MARK: - NSApplicationDelegate
	func applicationDidFinishLaunching(notification: NSNotification) {
		srandom(UInt32(time(nil) & 0x7fffffff))
		PPLibrary.registerDebugBlock(cocoaDebugStr)
		//the NIB won't store the value anymore, so do this hackery to make sure there's some value in it.
		songTotalTime.integerValue = 0
		songCurTime.integerValue = 0
		
		tableView.registerForDraggedTypes([PPMLDCUTI, kUTTypeFileURL as! String]);
		//self.paused = YES;
		willChangeValueForKey(kMusicListKVO)
		if NSUserDefaults.standardUserDefaults().boolForKey(PPRememberMusicList) {
			musicList.loadApplicationMusicList()
		}
		let selMus = musicList.selectedMusic;
		didChangeValueForKey(kMusicListKVO)

		tableView.doubleAction = "doubleClickMusicList"

		let defaultCenter = NSNotificationCenter.defaultCenter()
		defaultCenter.addObserver(self, selector: "soundPreferencesDidChange:", name: PPSoundPreferencesDidChange, object: nil)
		
		MADDriverWithPreferences()
		for (i, obj) in enumerate(madLib) {
			if obj.canExport {
				let mi = NSMenuItem(title: "\(obj.menuName)…", action: "exportMusicAs:", keyEquivalent: "")
				mi.tag = i
				mi.target = self
				musicExportMenu.addItem(mi)
			}
		}
		
		updatePlugInInfoMenu()
		
		exportController = SoundSettingsViewController.newSoundSettingWindow()
		exportController.delegate = self;
		exportSettingsBox.contentView = exportController.view

		instrumentController = InstrumentWindowController.newInstrumentWindow()
		
		timeChecker = NSTimer(fireDate: NSDate(), interval:1.0/8.0, target:self, selector: "updateMusicStats:", userInfo: nil, repeats: true)
		NSRunLoop.mainRunLoop().addTimer(timeChecker, forMode: NSRunLoopCommonModes)
		let lostCount = musicList.lostMusicCount
		
		if lostCount != 0 {
			let unresolvable = String(format: kUnresolvableFileDescription, lostCount)
			PPRunAlertPanel(kUnresolvableFile, message: unresolvable);
		}
		
		if selMus != -1 {
			var err: NSError?
			
			selectMusicAtIndex(selMus)
			if !loadMusicURL(musicList.URLAtIndex(selMus), error: &err, autoPlay: false) {
				NSAlert(error: err!).runModal()
			} else {
				currentlyPlayingIndex.index = selMus
				currentlyPlayingIndex.playbackURL = musicList.URLAtIndex(selMus)
				previouslyPlayingIndex = currentlyPlayingIndex
			}
		} else {
			self.music = PPMusicObject();
			music.attachToDriver(madDriver)
			setTitleForSongLabelBasedOnMusic()
		}
	}
	
	func applicationShouldTerminate(sender: NSApplication) -> NSApplicationTerminateReply {
		if madDriver.exporting {
			let anAlert = NSAlert()
			//TODO: localize
			anAlert.messageText = "Exporting"
			anAlert.informativeText = "PlayerPRO is currently exporting a tracker file.\nQuitting will stop this. Do you really wish to quit?"
			anAlert.addButtonWithTitle("Wait")
			anAlert.addButtonWithTitle("Quit")
			anAlert.addButtonWithTitle("Cancel")
			switch anAlert.runModal() {
			case NSAlertFirstButtonReturn:
				//Double-check to make sure we're still exporting
				if madDriver.exporting {
					isQuitting = true
					return .TerminateLater
				} else {
					return .TerminateNow
				}
				
			case NSAlertSecondButtonReturn:
				return .TerminateNow
				
			case NSAlertThirdButtonReturn:
				return .TerminateCancel
				
			default:
				return .TerminateNow
			}
		}
		return .TerminateNow
	}
	
	func applicationWillTerminate(notification: NSNotification) {
		timeChecker.invalidate()
		
		madDriver.stopDriver()
		if NSUserDefaults.standardUserDefaults().boolForKey(PPRememberMusicList) {
			musicList.saveApplicationMusicList()
		}
		
		NSNotificationCenter.defaultCenter().removeObserver(self)
	}
	
	func application(sender: NSApplication, openFile filename: String) -> Bool {
		var err: NSError? = nil
		if let utiFile = NSWorkspace.sharedWorkspace().typeOfFile(filename, error: &err) {
			return handleFile(NSURL(fileURLWithPath: filename)!, ofType: utiFile)
		} else {
			var tmpAlert = NSAlert()
			tmpAlert.alertStyle = .WarningAlertStyle
			tmpAlert.messageText = "Error opening file"
			tmpAlert.informativeText = "Unable to open \(filename.lastPathComponent): \(err?.localizedFailureReason)"
			tmpAlert.runModal()
			return false;
		}
	}
	
	// MARK: - Tools functions
	
	@IBAction func fastForwardButtonPressed(sender: AnyObject!) {
		
	}
	
	@IBAction func loopButtonPressed(sender: AnyObject!) {
		
	}
	
	@IBAction func nextButtonPressed(sender: AnyObject!) {
		var err: NSError? = nil
		
		if (self.currentlyPlayingIndex.index + 1 < musicList.countOfMusicList) {
			currentlyPlayingIndex.index++;
			selectCurrentlyPlayingMusic()
			if (!loadMusicFromCurrentlyPlayingIndexWithError(&err)) {
				NSAlert(error:err!).runModal()
			}
		} else if (NSUserDefaults.standardUserDefaults().boolForKey(PPLoopMusicWhenDone)) {
			currentlyPlayingIndex.index = 0;
			selectCurrentlyPlayingMusic()
			if (!loadMusicFromCurrentlyPlayingIndexWithError(&err)) {
				NSAlert(error:err!).runModal()
			}
		} else {
			NSBeep();
		}
	}
	
	@IBAction func playButtonPressed(sender: AnyObject!) {
		if (self.music != nil) {
			madDriver.play()
			paused = false;
		}
	}
	
	@IBAction func stopButtonPressed(sender: AnyObject!) {
		if (self.music != nil) {
			madDriver.stop()
			madDriver.cleanDriver()
			madDriver.setMusicStatusWithCurrentTime(0, maximumTime: 100, minimumTime: 0)
			paused = true;
		}
	}

	@IBAction func pauseButtonPressed(sender: AnyObject!) {
		if (self.music != nil) {
			if (self.paused) {
				madDriver.play()
			} else {
				madDriver.pause()
			}
			paused = !paused;
		}
	}

	@IBAction func prevButtonPressed(sender: AnyObject!) {
		if (currentlyPlayingIndex.index > 0) {
			currentlyPlayingIndex.index--;
			selectCurrentlyPlayingMusic()
			var err: NSError? = nil
			if (!loadMusicFromCurrentlyPlayingIndexWithError(&err)) {
				NSAlert(error:err!).runModal()
			}
		} else {
			NSBeep();
		}
	}
	
	@IBAction func rewindButtonPressed(sender: AnyObject!) {
		
	}
	
	@IBAction func sliderChanged(sender: AnyObject!) {
		if (self.music != nil) {
			madDriver.setMusicStatusWithCurrentTime(Int(songPos.doubleValue), maximumTime: Int(songPos.maxValue), minimumTime: 0)
		}
	}
	
	@IBAction func showTools(sender: AnyObject?) {
		toolsPanel.makeKeyAndOrderFront(sender)
	}

	// MARK: - Exporting functions
	
	@IBAction func okayExportSettings(sender: AnyObject!) {
		window.endSheet(exportWindow, returnCode: NSAlertDefaultReturn)
		exportWindow.close()
	}
	
	@IBAction func cancelExportSettings(sender: AnyObject!) {
		window.endSheet(exportWindow, returnCode: NSAlertAlternateReturn)
		exportWindow.close()
	}
	
	private func beginExportSettingsWithHandler(theHandle: (Int) -> Void) {
		exportSettings.resetToBestDriver()
		exportSettings.driverMode = .NoHardwareDriver;
		exportSettings.repeatMusic = false;
		exportController.settingsFromDriverSettings(exportSettings)
		window.beginSheet(exportWindow, completionHandler: theHandle)
	}
	
	private func rawSoundData(inout settings: MADDriverSettings, handler: (NSData) -> MADErr, @noescape callback: (NSError?) -> Void) {
		var err: NSError? = nil
		if let theRec = PPDriver(library: madLib, settings: &settings, error: &err) {
			theRec.cleanDriver()
			theRec.currentMusic = music
			theRec.play()
			
			while let newData = theRec.directSave() {
				let anErr = handler(newData)
				if anErr != .NoErr {
					callback(createErrorFromMADErrorType(anErr))
					return
				}
			}
			
			callback(nil)
		} else {
			callback(err)
		}
	}
	
	private func saveMusic(waveToURL theURL: NSURL, inout theSett: MADDriverSettings) -> MADErr {
		var iErr: NSError? = nil
		
		var audioFile: ExtAudioFileRef = nil;
		let tmpChannels: UInt32
		var res: OSStatus = noErr
		
		switch (theSett.outPutMode) {
		case .MonoOutPut:
			tmpChannels = 1
			
		case .PolyPhonic:
			tmpChannels = 4
			
		default:
			tmpChannels = 2
		}
		
		var asbd = AudioStreamBasicDescription(sampleRate: Float64(theSett.outPutRate), formatFlags: .SignedInteger | .Packed, bitsPerChannel: UInt32(theSett.outPutBits), channelsPerFrame: tmpChannels)
		var realFormat = AudioStreamBasicDescription(sampleRate: Float64(theSett.outPutRate), formatFlags: .SignedInteger | .Packed | .NativeEndian, bitsPerChannel: UInt32(theSett.outPutBits), channelsPerFrame: tmpChannels)
		
		res = ExtAudioFileCreate(URL: theURL, fileType: .WAVE, streamDescription: &asbd, flags: .EraseFile, audioFile: &audioFile)
		
		if res != noErr {
			return .WritingErr
		}
		
		res = ExtAudioFileSetProperty(audioFile, propertyID: .ClientDataFormat, dataSize: UInt32(sizeof(AudioStreamBasicDescription)), data: &realFormat)
		if (res != noErr) {
			if (audioFile != nil) {
				ExtAudioFileDispose(audioFile)
			}
			return .WritingErr
		}
		
		func handler(data: NSData) -> MADErr {
			var toWriteSize = data.length
			
			var audBufList = AudioBufferList()
			audBufList.mNumberBuffers = 1
			audBufList.mBuffers.mNumberChannels = tmpChannels
			audBufList.mBuffers.mDataByteSize = UInt32(data.length)
			audBufList.mBuffers.mData = UnsafeMutablePointer<Void>(data.bytes)
			
			
			let res = ExtAudioFileWrite(audioFile, UInt32(toWriteSize) / realFormat.mBytesPerFrame, &audBufList)
			
			if res != noErr {
				return .WritingErr
			}
			
			return .NoErr
		}
		
		rawSoundData(&theSett, handler: handler, callback: { (anErr) -> Void in
			iErr = anErr
		})
		
		if let iErr = iErr {
			res = ExtAudioFileDispose(audioFile)
			audioFile = nil
			if iErr.domain == PPMADErrorDomain {
				return MADErr(rawValue: OSErr(iErr.code)) ?? .UnknownErr
			} else {
				return .UnknownErr
			}
		}
		
		applyMetadataToFileID(audioFile)
		res = ExtAudioFileDispose(audioFile)
		audioFile = nil
		if (res != noErr) {
			if (audioFile != nil) {
				ExtAudioFileDispose(audioFile)
			}
			return .WritingErr
		}
		
		return .NoErr
	}
	
	private func saveMusic(AIFFToURL theURL: NSURL, inout theSett: MADDriverSettings) -> MADErr {
		var iErr: NSError? = nil
		
		var audioFile: ExtAudioFileRef = nil;
		let tmpChannels: UInt32
		var res: OSStatus = noErr
		
		switch (theSett.outPutMode) {
		case .MonoOutPut:
			tmpChannels = 1
			
		case .PolyPhonic:
			tmpChannels = 4
			
		default:
			tmpChannels = 2
		}
		
		var asbd = AudioStreamBasicDescription(sampleRate: Float64(theSett.outPutRate), formatFlags: .SignedInteger | .Packed | .BigEndian, bitsPerChannel: UInt32(theSett.outPutBits), channelsPerFrame: tmpChannels)
		var realFormat = AudioStreamBasicDescription(sampleRate: Float64(theSett.outPutRate), formatFlags: .SignedInteger | .Packed | .NativeEndian, bitsPerChannel: UInt32(theSett.outPutBits), channelsPerFrame: tmpChannels)
		
		res = ExtAudioFileCreate(URL: theURL, fileType: .AIFF, streamDescription: &asbd, flags: .EraseFile, audioFile: &audioFile)
		
		if res != noErr {
			return .WritingErr
		}
		
		res = ExtAudioFileSetProperty(audioFile, propertyID: .ClientDataFormat, dataSize: UInt32(sizeof(AudioStreamBasicDescription)), data: &realFormat)
		if (res != noErr) {
			if (audioFile != nil) {
				ExtAudioFileDispose(audioFile)
			}
			return .WritingErr
		}
		
		func handler(data: NSData) -> MADErr {
			var toWriteSize = data.length
			
			var audBufList = AudioBufferList()
			audBufList.mNumberBuffers = 1
			audBufList.mBuffers.mNumberChannels = tmpChannels
			audBufList.mBuffers.mDataByteSize = UInt32(data.length)
			audBufList.mBuffers.mData = UnsafeMutablePointer<Void>(data.bytes)
			
			
			let res = ExtAudioFileWrite(audioFile, UInt32(toWriteSize) / realFormat.mBytesPerFrame, &audBufList)
			
			if res != noErr {
				return .WritingErr
			}
			
			return .NoErr
		}
		
		rawSoundData(&theSett, handler: handler, callback: { (anErr) -> Void in
			iErr = anErr
		})
		
		if let iErr = iErr {
			res = ExtAudioFileDispose(audioFile)
			audioFile = nil
			if iErr.domain == PPMADErrorDomain {
				return MADErr(rawValue: OSErr(iErr.code)) ?? .UnknownErr
			} else {
				return .UnknownErr
			}
		}
		
		applyMetadataToFileID(audioFile)
		res = ExtAudioFileDispose(audioFile)
		audioFile = nil
		if (res != noErr) {
			if (audioFile != nil) {
				ExtAudioFileDispose(audioFile)
			}
			return .WritingErr
		}
		
		return .NoErr
	}
	
	private func applyMetadataToFileID(theID: AudioFileID) {
		//TODO: implement, but how?
	}
	
	@IBAction func exportMusicAs(sender: AnyObject!) {
		var tag = (sender as! NSMenuItem).tag;
		madDriver.beginExport()
		var savePanel = NSSavePanel()
		savePanel.canCreateDirectories = true
		savePanel.canSelectHiddenExtension = true
		if (musicName != "") {
			savePanel.nameFieldStringValue = musicName
		}
		savePanel.prompt = "Export"
		
		switch (tag) {
		case -1: //AIFF
			savePanel.allowedFileTypes = [AVFileTypeAIFF]
			savePanel.title = "Export as AIFF audio"
			savePanel.beginSheetModalForWindow(self.window, completionHandler: { (result) -> Void in
				if (result != NSFileHandlingPanelOKButton) {
					self.madDriver.endExport()
					return
				}
				
				self.beginExportSettingsWithHandler( { (result) -> Void in
					if (result != NSAlertDefaultReturn) {
						self.madDriver.endExport()
						return;
					}
					
					dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0)) {
						var thErr = MADErr.NoErr
						autoreleasepool {
							thErr = self.saveMusic(AIFFToURL: savePanel.URL!, theSett: &self.exportSettings);
							self.madDriver.endExport()
							if (thErr != .NoErr) {
								if (self.isQuitting) {
									NSApplication.sharedApplication().replyToApplicationShouldTerminate(true)
								}
								return;
							}
						}
						if thErr == .NoErr {
							dispatch_async(dispatch_get_main_queue()) {
								if (self.isQuitting) {
									NSApplication.sharedApplication().replyToApplicationShouldTerminate(true)
								} else {
									let retVal = PPRunInformationalAlertPanel("Export complete", message: "The export of the file \"\(savePanel.URL!.lastPathComponent!)\" is complete.", defaultButton: "OK", alternateButton: "Show File");
									if (retVal == NSAlertAlternateReturn) {
										NSWorkspace.sharedWorkspace().activateFileViewerSelectingURLs([savePanel.URL!])
									}
								}
							}
						} else {
							dispatch_async(dispatch_get_main_queue()) {
								if self.isQuitting {
									NSApplication.sharedApplication().replyToApplicationShouldTerminate(true)
								} else {
									NSAlert(error: createErrorFromMADErrorType(thErr)!).runModal()
								}
							}
						}
					}
				})
			})
			
		case -2: //MP4
			savePanel.allowedFileTypes = [AVFileTypeAppleM4A];
			savePanel.title = "Export as MPEG-4 Audio"
			savePanel.beginSheetModalForWindow(self.window, completionHandler: {(result) -> Void in
				if (result != NSFileHandlingPanelOKButton) {
					self.madDriver.endExport()
					return;
				}
				
				self.beginExportSettingsWithHandler( { (result) -> Void in
					let saveURL = savePanel.URL!
					if result != NSAlertDefaultReturn {
						self.madDriver.endExport()
						return;
					}
					
					dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0)) {
						var theErr = MADErr.NoErr;
						
						var oldURL = self.musicList.objectInMusicListAtIndex(self.previouslyPlayingIndex.index).musicURL
						var expErr: NSError? = nil;
						var errBlock: () -> Void = {
							if (self.isQuitting) {
								NSApplication.sharedApplication().replyToApplicationShouldTerminate(true)
							} else {
								PPRunAlertPanel("Export failed", message: "Export/coversion of the music file failed:\n\(expErr!.localizedDescription)");
							}
						};
						let oldMusicName = self.musicName;
						let oldMusicInfo = self.musicInfo;
						func generateAVMetadataInfo() -> [AVMetadataItem] {
							let titleName = AVMutableMetadataItem()
							titleName.keySpace = AVMetadataKeySpaceCommon
							titleName.setKey(AVMetadataCommonKeyTitle)
							titleName.setValue(oldMusicName)
							
							let dataInfo = AVMutableMetadataItem()
							dataInfo.keySpace = AVMetadataKeySpaceQuickTimeUserData;
							dataInfo.setKey(AVMetadataQuickTimeUserDataKeySoftware)
							dataInfo.setValue("PlayerPRO Player")
							dataInfo.locale = NSLocale(localeIdentifier: "en_US")
							
							let musicInfoQTUser = AVMutableMetadataItem();
							musicInfoQTUser.keySpace = AVMetadataKeySpaceQuickTimeUserData
							musicInfoQTUser.setKey(AVMetadataQuickTimeUserDataKeyInformation)
							musicInfoQTUser.setValue(oldMusicInfo)
							musicInfoQTUser.locale = NSLocale.currentLocale()
							
							let musicNameQTUser = AVMutableMetadataItem()
							musicNameQTUser.keySpace = AVMetadataKeySpaceQuickTimeUserData
							musicNameQTUser.setKey(AVMetadataQuickTimeUserDataKeyFullName)
							musicNameQTUser.setValue(oldMusicName)
							musicNameQTUser.locale = NSLocale.currentLocale()
							
							let musicInfoiTunes = AVMutableMetadataItem()
							musicInfoiTunes.keySpace = AVMetadataKeySpaceiTunes
							musicInfoiTunes.setKey(AVMetadataiTunesMetadataKeyUserComment)
							musicInfoiTunes.setValue(oldMusicInfo)
							
							let musicInfoQTMeta = AVMutableMetadataItem();
							musicInfoQTMeta.keySpace = AVMetadataKeySpaceQuickTimeMetadata
							musicInfoQTMeta.setKey(AVMetadataQuickTimeMetadataKeyInformation)
							musicInfoQTMeta.setValue(oldMusicInfo)
							musicInfoQTMeta.locale = NSLocale.currentLocale()
							
							return [titleName, dataInfo, musicInfoQTUser, musicInfoiTunes, musicInfoQTMeta, musicNameQTUser];
						}
						
						let tmpName = oldMusicName != "" ? oldMusicName : "untitled"
						let tmpURL = NSFileManager.defaultManager().URLForDirectory(.ItemReplacementDirectory, inDomain: .UserDomainMask, appropriateForURL: oldURL, create: true, error: nil)!.URLByAppendingPathComponent("\(tmpName).aiff", isDirectory: false)
						
						theErr = self.saveMusic(AIFFToURL: tmpURL, theSett: &self.exportSettings)
						if theErr != .NoErr {
							expErr = createErrorFromMADErrorType(theErr);
							NSFileManager.defaultManager().removeItemAtURL(tmpURL, error: nil)
							dispatch_async(dispatch_get_main_queue(), errBlock);
						}
						
						if let exportMov = AVAsset.assetWithURL(tmpURL) as? AVAsset {
							var metadataInfo = generateAVMetadataInfo()
							
							var session = AVAssetExportSession(asset:exportMov, presetName:AVAssetExportPresetAppleM4A)
							#if false
								if (session == nil) {
								expErr = NSError(domain: NSCocoaErrorDomain, code: NSFileWriteUnknownError, userInfo: nil)
								NSLog("Export session creation for %@ failed, error: %@", oldMusicName, expErr!.localizedDescription);
								NSFileManager.defaultManager().removeItemAtURL(tmpURL, error: nil)
								dispatch_async(dispatch_get_main_queue(), errBlock);
								return;
								}
							#endif
							NSFileManager.defaultManager().removeItemAtURL(saveURL, error: nil)
							session.outputURL = saveURL
							session.outputFileType = AVFileTypeAppleM4A
							session.metadata = metadataInfo;
							var sessionWaitSemaphore = dispatch_semaphore_create(0);
							session.exportAsynchronouslyWithCompletionHandler({ () -> Void in
								var tmp = dispatch_semaphore_signal(sessionWaitSemaphore)
							})
							dispatch_semaphore_wait(sessionWaitSemaphore, DISPATCH_TIME_FOREVER)
							
							var didFinish = session.status == .Completed;
							NSFileManager.defaultManager().removeItemAtURL(tmpURL, error: nil)
							
							if (didFinish) {
								dispatch_async(dispatch_get_main_queue()) {
									if (self.isQuitting) {
										NSApplication.sharedApplication().replyToApplicationShouldTerminate(true)
									} else {
										let retVal = PPRunInformationalAlertPanel("Export complete", message: "The export of the file \"\(savePanel.URL!.lastPathComponent!)\" is complete.", defaultButton: "OK", alternateButton: "Show File");
										if (retVal == NSAlertAlternateReturn) {
											NSWorkspace.sharedWorkspace().activateFileViewerSelectingURLs([savePanel.URL!])
										}
									}
								}
							} else {
								NSLog("\(session.error)");
							}
							
						} else {
							expErr = NSError(domain: NSCocoaErrorDomain, code: NSFileWriteUnknownError, userInfo: nil)
							NSLog("Init Failed for %@, error: %@", oldMusicName, expErr!.localizedDescription)
							NSFileManager.defaultManager().removeItemAtURL(tmpURL, error: nil)
							dispatch_async(dispatch_get_main_queue(), errBlock)
							return;
						}
					}
				})
				
			});
			
		case -3: // wave
			savePanel.allowedFileTypes = [AVFileTypeWAVE]
			savePanel.title = "Export as Wave Audio"
			savePanel.beginSheetModalForWindow(window, completionHandler: { (result) -> Void in
				if (result != NSFileHandlingPanelOKButton) {
					self.madDriver.endExport()
					return;
				}
				self.beginExportSettingsWithHandler({ (result) -> Void in
					if result != NSAlertDefaultReturn {
						self.madDriver.endExport()
						if (self.isQuitting) {
							NSApplication.sharedApplication().replyToApplicationShouldTerminate(true)
						}
						return;
					}
					
					dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0)) {
						var thErr = MADErr.NoErr
						autoreleasepool {
							thErr = self.saveMusic(waveToURL: savePanel.URL!, theSett: &self.exportSettings);
							self.madDriver.endExport()
							if (thErr != .NoErr) {
								if (self.isQuitting) {
									NSApplication.sharedApplication().replyToApplicationShouldTerminate(true)
								}
								return;
							}
						}
						dispatch_async(dispatch_get_main_queue()) {
							if (self.isQuitting) {
								NSApplication.sharedApplication().replyToApplicationShouldTerminate(true)
							} else if thErr == .NoErr {
								let retVal = PPRunInformationalAlertPanel("Export complete", message: "The export of the file \"\(savePanel.URL!.lastPathComponent!)\" is complete.", defaultButton: "OK", alternateButton: "Show File");
								if (retVal == NSAlertAlternateReturn) {
									NSWorkspace.sharedWorkspace().activateFileViewerSelectingURLs([savePanel.URL!])
								}
							} else {
								let expErr = createErrorFromMADErrorType(thErr)!
								PPRunAlertPanel("Export failed", message: "Export/coversion of the music file failed:\n\(expErr.localizedDescription)");
							}
						}
					}
					
				})
			})
			break;
			
		default:
			if tag > madLib.pluginCount || tag < 0 {
				NSBeep();
				
				madDriver.endExport()
				if (isQuitting) {
					NSApplication.sharedApplication().replyToApplicationShouldTerminate(true)
				}
				
				return;
			}
			savePanel.allowedFileTypes = madLib[tag].UTITypes
			savePanel.title = "Export as \(madLib[tag].menuName)"
			
			savePanel.beginSheetModalForWindow(self.window, completionHandler: {(result) -> Void in
				if (result != NSFileHandlingPanelOKButton) {
					self.madDriver.endExport()
					return;
				}
				
				let fileURL = savePanel.URL
				var err = self.music.exportMusicToURL(fileURL!, format: self.madLib[tag].type, library: self.madLib)
				self.madDriver.endExport()
				if (err != .NoErr) {
					if (self.isQuitting) {
						NSApplication.sharedApplication().replyToApplicationShouldTerminate(true)
					} else {
						var aerr = createErrorFromMADErrorType(err)!
						NSAlert(error: aerr).runModal()
					}
				} else {
					self.addMusicToMusicList(fileURL!, loadIfPreferencesAllow:false)
					if (self.isQuitting) {
						NSApplication.sharedApplication().replyToApplicationShouldTerminate(true)
					} else {
						let retVal = PPRunInformationalAlertPanel("Export complete", message: "The export of the file \"\(savePanel.URL!.lastPathComponent!)\" is complete.", defaultButton: "OK", alternateButton: "Show File");
						if (retVal == NSAlertAlternateReturn) {
							NSWorkspace.sharedWorkspace().activateFileViewerSelectingURLs([fileURL!])
						}
					}
				}
			})
		}
	}
	
	//MARK: - NSToolbar helpers
	enum MusicToolbarTypes: Int {
		case Unknown = 0
		case Sort = 1001
		case AddMusic
		case RemoveMusic
		case PlayMusic
		case FileInfo
	}
	
	@IBAction func toggleInfo(sender: AnyObject?) {
		infoDrawer.toggle(sender)
	}
	
	@IBAction func sortMusicList(sender: AnyObject?) {
		willChangeValueForKey(kMusicListKVO)
		musicList.sortMusicListByName()
		didChangeValueForKey(kMusicListKVO)
		musicListContentsDidMove()
	}
	
	@IBAction func playSelectedMusic(sender: AnyObject?) {
		var error: NSError?
		self.currentlyPlayingIndex.index = tableView.selectedRow;
		if loadMusicFromCurrentlyPlayingIndexWithError(&error) == false {
			NSAlert(error: error!).runModal()
		}
	}
	
	@IBAction func removeSelectedMusic(sender: AnyObject?) {
		let selMusic = tableView.selectedRowIndexes
		willChangeValueForKey(kMusicListKVO)
		musicList.removeObjectsInMusicListAtIndexes(selMusic)
		didChangeValueForKey(kMusicListKVO)
		musicListContentsDidMove()
	}

	@IBAction func addMusic(sender: AnyObject?) {
		let panel = NSOpenPanel()
		
		if let av = OpenPanelViewController(openPanel: panel, trackerDictionary: trackerDict) {
			av.setupDefaults()
			av.allowsMultipleSelectionOfTrackers = true
			av.beginOpenPanel(parentWindow: self.window, completionHandler: { (result) -> Void in
				if (result == NSFileHandlingPanelOKButton) {
					self.addMusicsToMusicList(panel.URLs as! [NSURL])
				}
			})
		} else {
			NSBeep()
		}
	}
	
	override func validateToolbarItem(theItem: NSToolbarItem) -> Bool {
		let theTag = MusicToolbarTypes(rawValue: theItem.tag) ?? .Unknown
		switch theTag {
		case .Sort, .AddMusic:
			return true
			
		case .FileInfo:
			if infoDrawer.state == Int(NSDrawerState.OpeningState.rawValue) || infoDrawer.state == Int(NSDrawerState.OpenState.rawValue) {
				return true
			}
			fallthrough
		case .PlayMusic:
			if tableView.selectedRowIndexes.count == 1 {
				return true
			} else {
				return false
			}
			
		case .RemoveMusic:
			if tableView.selectedRowIndexes.count > 0 {
				return true
			} else {
				return false
			}
			
		default:
			return false
		}
	}
	
	//MARK: - NSTableViewDelegate functions

	func tableViewSelectionDidChange(notification: NSNotification) {
		func badTracker() {
			fileName.stringValue = PPDoubleDash
			internalName.stringValue = PPDoubleDash
			fileSize.stringValue = PPDoubleDash
			musicInstrument.stringValue = PPDoubleDash
			musicPatterns.stringValue = PPDoubleDash
			musicPlugType.stringValue = PPDoubleDash
			musicSignature.stringValue = PPDoubleDash
			fileLocation.stringValue = PPDoubleDash
		}
		
		let selected = tableView.selectedRowIndexes
		var theInfo: NSDictionary? = nil
		var info = ""
		var NSSig = ""
		
		
		if (selected.count > 0) {
			musicList.selectedMusic = selected.firstIndex
		}
		
		if (selected.count != 1) {
			badTracker()
			return
		}
		
		let obj = musicList.objectInMusicListAtIndex(selected.lastIndex)
		let musicURL = obj.musicURL;
		var aPPInfo: (info: PPLibrary.MusicFileInfo?, error: MADErr)? = nil
		
		func badValues() {
			let iInfo = madLib.identifyFile(URL: musicURL)
			switch iInfo {
			case .Failure(_):
				aPPInfo = nil
				return
				
			case .Success(let strType):
				aPPInfo = madLib.informationFromFile(URL: musicURL, type: strType)
				if aPPInfo!.error != .NoErr {
					aPPInfo = nil
					return
				}
			}
		}

		
		if let fileUTI = NSWorkspace.sharedWorkspace().typeOfFile(musicURL.path!, error: nil) {
			if let info = madLib.typeFromUTI(fileUTI) {
				aPPInfo = madLib.informationFromFile(URL:musicURL, type: info)
				if aPPInfo!.error != .NoErr {
					badValues()
				}
			} else {
				badValues()
			}
		} else {
			badValues()
		}
		
		if let aPPInfo = aPPInfo, actualInfo = aPPInfo.info {
			fileName.stringValue = obj.fileName
			internalName.stringValue = actualInfo.internalFileName
			fileSize.objectValue = NSNumber(unsignedLongLong: obj.fileSize)
			musicInstrument.integerValue = actualInfo.totalInstruments
			musicPatterns.integerValue = actualInfo.totalPatterns
			musicPlugType.stringValue = actualInfo.formatDescription
			musicSignature.stringValue = actualInfo.signature
			fileLocation.stringValue = musicURL.absoluteURL!.path!
		} else {
			badTracker()
		}
	}
	
	func tableView(tableView: NSTableView, acceptDrop info: NSDraggingInfo, row: Int, dropOperation: NSTableViewDropOperation) -> Bool {
		let dragPB = info.draggingPasteboard()
		if let tmpArray = dragPB.readObjectsForClasses([MusicListDragClass.self], options: nil) where tmpArray.count != 0 {
			var minRow = 0;
			let dragClass = (tmpArray[0]) as! MusicListDragClass
			let dragIndexSet = dragClass.theIndexSet;
			
			var currentIndex = dragIndexSet.firstIndex;
			while currentIndex != NSNotFound {
				if currentIndex <= row {
					minRow++;
				}
				currentIndex = dragIndexSet.indexGreaterThanIndex(currentIndex)
			}
			
			changeValueForMusicListKey({
				var selArray = musicList.arrayOfObjectsInMusicListAtIndexes(dragIndexSet)
				musicList.removeObjectsInMusicListAtIndexes(dragIndexSet)
				musicList.insertObjects(selArray, inMusicListAtIndex: row - minRow)
			})
			musicListContentsDidMove()
			return true;
		} else if let tmpArray = dragPB.readObjectsForClasses([NSURL.self], options:[NSPasteboardURLReadingFileURLsOnlyKey : true,
			NSPasteboardURLReadingContentsConformToTypesKey : self.trackerUTIs]) {
			if (tmpArray.count < 1) {
				return false;
			}
			
			changeValueForMusicListKey({
				var mutArray = [MusicListObject]()
				for curURL in tmpArray as! [NSURL] {
					mutArray.append(MusicListObject(URL: curURL))
				}
				self.musicList.insertObjects(mutArray, inMusicListAtIndex: row)
			})
			musicListContentsDidMove()
			return true;
		}
		
		return false;
	}
	
	func tableView(tableView1: NSTableView, validateDrop info: NSDraggingInfo, proposedRow row: Int, proposedDropOperation dropOperation: NSTableViewDropOperation) -> NSDragOperation {
		var result = NSDragOperation.None;
		
		if (info.draggingSource() === tableView1) {
			result = .Move;
			//TODO: check for number of indexes that are greater than the drop row.
			tableView1.setDropRow(row, dropOperation: .Above)
		} else {
			let pb = info.draggingPasteboard();
			
			//list the file type UTIs we want to accept
			let acceptedTypes = trackerUTIs;
			
			if let urls = pb.readObjectsForClasses([NSURL.self], options: [NSPasteboardURLReadingFileURLsOnlyKey : true,
				NSPasteboardURLReadingContentsConformToTypesKey : acceptedTypes]) {
					if urls.count > 0 {
						result = .Copy
						tableView1.setDropRow(row, dropOperation: .Above)
					}
			}
		}
		
		return result;
	}
	
	func tableView(tableView: NSTableView, toolTipForCell cell: NSCell, rect: NSRectPointer, tableColumn: NSTableColumn?, row: Int, mouseLocation: NSPoint) -> String {
		if (row >= 0 || row <= musicList.countOfMusicList) {
			return ""
		}
		return musicList.URLAtIndex(row).path ?? ""
	}
	
	func tableView(tableView: NSTableView, writeRowsWithIndexes rowIndexes: NSIndexSet, toPasteboard pboard: NSPasteboard) -> Bool {
		var status = false;
		let dragClass = MusicListDragClass(indexSet: rowIndexes)
		let ppmobjects = musicList.arrayOfObjectsInMusicListAtIndexes(rowIndexes)
		let urlArrays = ppmobjects.map({ (mlo) -> NSURL in
			return mlo.musicURL
		})
		var tmpObjs: [AnyObject] = [dragClass] + urlArrays
		pboard.clearContents(); // clear pasteboard to take ownership
		status = pboard.writeObjects(tmpObjs) // write the URLs
		return status;
	}
	
	func tableView(atableView: NSTableView, sortDescriptorsDidChange oldDescriptors: [AnyObject]) {
		changeValueForMusicListKey({
			self.musicList.sortMusicList(descriptors: atableView.sortDescriptors as! [NSSortDescriptor])
		})
		atableView.reloadData()
		musicListContentsDidMove()
	}
	
	//MARK: - PPSoundSettingsViewController delegate methods
	func soundOutBitsDidChange(bits: Int16) {
		exportSettings.outPutBits = bits;
	}
	
	func soundOutRateDidChange(rat: UInt32) {
		exportSettings.outPutRate = rat;
	}
	
	func soundOutReverbDidChangeActive(isAct: Bool) {
		exportSettings.Reverb = isAct;
	}
	
	func soundOutOversamplingDidChangeActive(isAct: Bool) {
		if (!isAct) {
			exportSettings.oversampling = 1;
		}
	}
	
	func soundOutStereoDelayDidChangeActive(isAct: Bool) {
		if (!isAct) {
			exportSettings.MicroDelaySize = 0;
		}
	}
	
	func soundOutSurroundDidChangeActive(isAct: Bool) {
		exportSettings.surround = isAct;
	}
	
	func soundOutReverbStrengthDidChange(rev: Int32) {
		exportSettings.ReverbStrength = rev;
	}
	
	func soundOutReverbSizeDidChange(rev: Int32) {
		exportSettings.ReverbSize = rev;
	}
	
	func soundOutOversamplingAmountDidChange(ovs: Int32) {
		exportSettings.oversampling = ovs;
	}
	
	func soundOutStereoDelayAmountDidChange(std: Int32) {
		exportSettings.MicroDelaySize = std;
	}
}
