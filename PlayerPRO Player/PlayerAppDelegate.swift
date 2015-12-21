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
	print("\(swiftFile):\(line), error text: \(swiftText)")
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
		print("Choosing to fail!")
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
			currentlyPlayingIndex.index = selMusFromList != -1 ? selMusFromList : 0;
			selectCurrentlyPlayingMusic()
			do {
				try loadMusicFromCurrentlyPlayingIndex()
			} catch let err as NSError {
				NSAlert(error: err).runModal()
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
		let ourDefaults: [String: AnyObject] = [PPRememberMusicList: true,
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
			var time = madDriver.musicStatusTime!
			if (madDriver.donePlayingMusic && paused == false && madDriver.exporting == false) {
				songIsDonePlaying()
				time = madDriver.musicStatusTime!
			}
			songPos.doubleValue = Double(time.current)
			songCurTime.integerValue = time.current
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
					do {
						try loadMusicFromCurrentlyPlayingIndex()
					} catch let err as NSError {
						NSAlert(error: err).runModal()
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
					do {
						try loadMusicFromCurrentlyPlayingIndex()
					} catch let err as NSError {
						NSAlert(error: err).runModal()
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
				do {
					try loadMusicFromCurrentlyPlayingIndex()
				} catch let err as NSError {
					NSAlert(error: err).runModal()
				}
			} else {
				selectMusicAtIndex(musicList.countOfMusicList - 1)
			}
		}
	}
	
	private func songIsDonePlaying() {
		let userDefaults = NSUserDefaults.standardUserDefaults()
		switch PlaylistMode(rawValue: userDefaults.integerForKey(PPAfterPlayingMusic))! {
			
		case .LoopMusic:
			madDriver.musicPosition = 0;
			madDriver.play()
			break;
			
		case .LoadNext:
			if musicList.countOfMusicList > ++self.currentlyPlayingIndex.index {
				selectCurrentlyPlayingMusic()
				do {
					try loadMusicFromCurrentlyPlayingIndex()
				} catch let err as NSError {
					NSAlert(error: err).runModal()
				}
			} else {
				if (userDefaults.boolForKey(PPLoopMusicWhenDone)) {
					currentlyPlayingIndex.index = 0;
					selectCurrentlyPlayingMusic()
					do {
						try loadMusicFromCurrentlyPlayingIndex()
					} catch let err as NSError {
						NSAlert(error: err).runModal()
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
			do {
				try loadMusicFromCurrentlyPlayingIndex()
			} catch let err as NSError {
				NSAlert(error: err).runModal()
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
	
	@objc(loadMusicFromCurrentlyPlayingIndexAndReturnError:) func loadMusicFromCurrentlyPlayingIndex() throws {
		var theErr: NSError! = NSError(domain: "Migrator", code: 0, userInfo: nil)
		currentlyPlayingIndex.playbackURL = musicList.URLAtIndex(currentlyPlayingIndex.index)
		let isGood: Bool
		do {
			try loadMusicURL(currentlyPlayingIndex.playbackURL!)
			isGood = true
			previouslyPlayingIndex = currentlyPlayingIndex
		} catch let error as NSError {
			theErr = error
			isGood = false
		}
		if isGood {
			return
		}
		throw theErr;
	}
	
	@objc private func doubleClickMusicList() {
		self.currentlyPlayingIndex.index = tableView.selectedRow;
		do {
			try loadMusicFromCurrentlyPlayingIndex()
		} catch let err as NSError {
			NSAlert(error: err).runModal()
		}
	}
	
	private func addMusicsToMusicList(theMusList: [NSURL]) {
		for i in 0 ..< (theMusList.count - 1) {
			addMusicToMusicList(theMusList[i], loadIfPreferencesAllow: false)
		}
		addMusicToMusicList(theMusList.last!, loadIfPreferencesAllow: true)
	}
	
	func loadMusicURL(musicToLoad: NSURL, autoPlay: Bool = true) throws {
		var error: NSError? = nil
		var fileType: String? = nil
		var theOSErr = MADErr.NoErr
		if music != nil {
			madDriver.stop()
			madDriver.stopDriver()
		}
		
		do {
			let fileUTI = try NSWorkspace.sharedWorkspace().typeOfFile(musicToLoad.path!)
			if let afileType = madLib.typeFromUTI(fileUTI) {
				theOSErr = madLib.testFile(URL: musicToLoad, type: afileType)
				if theOSErr == .NoErr {
					fileType = afileType
				} else {
					
				}
			}
		} catch _ {
		}
		
		if fileType == nil {
			do {
				fileType = try madLib.identifyFile(URL: musicToLoad)
				theOSErr = .NoErr
			} catch let err3 as NSError {
				error = err3
			}
		}
		
		if theOSErr != .NoErr || error != nil {
			paused = true
			clearMusic()
			throw error ?? theOSErr
		}
		
		do {
			self.music = try PPMusicObject(URL: musicToLoad, type: fileType!, library: madLib)
		} catch let err3 {
			self.paused = true
			clearMusic()
			throw err3
		}
		
		music.attachToDriver(madDriver)
		
		if autoPlay {
			madDriver.play()
			
			paused = false
		}
		
		let time = madDriver.musicStatusTime!
		songPos.maxValue = Double(time.total)
		songPos.minValue = 0
		setTitleForSongLabelBasedOnMusic()
		songTotalTime.integerValue = time.total
		
		NSNotificationCenter.defaultCenter().postNotificationName(PPMusicDidChange, object:self)
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
		self.musicName = music.title
		self.musicInfo = music.information
	}
	
	private func MADDriverWithPreferences() {
		var madWasReading = false;
		var fullTime = 0, curTime = 0;
		if madDriver != nil {
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
		if madDriver == nil {
			do {
				madDriver = try PPDriver(library: madLib, settings: &theSettinit)
			} catch let error as NSError {
				let defaults = NSUserDefaults.standardUserDefaults()
				for pref in [PPSoundOutBits, PPSoundOutRate, PPSoundDriver, PPStereoDelayToggle,
					PPReverbToggle, PPSurroundToggle, PPOversamplingToggle, PPStereoDelayAmount,
					PPReverbAmount, PPReverbStrength, PPOversamplingAmount] {
					defaults.removeObjectForKey(pref)
				}
				defaults.synchronize()

				let alert = NSAlert()
				alert.messageText = "Sound Driver failure"
				alert.informativeText = "Unable to create the PlayerPRO sound driver due to error type \(error.code). Sound preferences will be reset.\n\nPlayerPRO will now close."
				alert.runModal()
				
				print("Could not create initial driver, error \(error)")
				exit(EXIT_FAILURE)
			}
		} else {
			do {
				try madDriver.changeDriverSettingsToSettings(&theSettinit)
			} catch let error as NSError {
				NSNotificationCenter.defaultCenter().postNotificationName(PPDriverDidChange, object: self)
				NSAlert(error: error).runModal()
				return;
			}
		}
		NSNotificationCenter.defaultCenter().postNotificationName(PPDriverDidChange, object: self)
		//MADStartDriver(madDriver);
		if self.music != nil {
			//MADAttachDriverToMusic(madDriver, music, NULL);
			if madWasReading {
				madDriver.setMusicStatusWithCurrentTime(curTime, maximumTime: fullTime, minimumTime: 0)
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
		
		plugInInfos.sortInPlace({ (lhs, rhs) -> Bool in
			let menuNam1 = lhs.plugName
			let menuNam2 = rhs.plugName
			let res = menuNam1.localizedStandardCompare(menuNam2)
			return res == .OrderedAscending
		})
		
		aboutPlugInMenu.removeAllItems()
		
		for (i, pi) in plugInInfos.enumerate() {
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
				self.musicList.saveMusicList(URL: savePanel.URL!)
			}
		})
	}
	
	@IBAction func showInstrumentsList(sender: AnyObject?) {
		instrumentController!.showWindow(sender)
	}
	
	func handleFile(theURL1: NSURL, ofType theUTI: String) -> Bool {
		let sharedWorkspace = NSWorkspace.sharedWorkspace()
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
				
				do {
					let identRet = try madLib.identifyFile(URL: theURL)
					let tmpURL = theURL.URLByDeletingPathExtension!.URLByAppendingPathExtension(identRet.lowercaseString)
					
					do {
						try NSFileManager.defaultManager().moveItemAtURL(theURL, toURL: tmpURL)
						theURL = tmpURL
						//TODO: regenerate the UTI
					} catch let err as NSError {
						print("Could not move file, error \(err)")
						let couldNotRenameStr = String(format: NSLocalizedString("The file could not be renamed to \"%@\".\n\nThe music file \"%@\" will still be loaded.", comment: "Could not rename file"), tmpURL.lastPathComponent!, theURL.lastPathComponent!)
						PPRunInformationalAlertPanel(NSLocalizedString("Rename Error", comment: "Rename Error"), message: couldNotRenameStr)
					}
					
				} catch {
					PPRunCriticalAlertPanel(NSLocalizedString("Unknown File", comment: "unknown file"), message: NSLocalizedString("The file type could not be identified.", comment: "Unidentified file"))
					return false
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
			do {
				let aType = try madLib.identifyFile(URL: theURL)
				let theRet = madLib.testFile(URL: theURL, type: aType)
				if theRet == .NoErr {
					addMusicToMusicList(theURL)
				} else {
					throw theRet
				}
			} catch _ {
				
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
				let panelURLs = panel.URLs
				for theURL in panelURLs {
					let fileName = theURL.path!
					var err: NSError?
					do {
						let utiFile = try ws.typeOfFile(fileName)
						self.handleFile(theURL, ofType: utiFile) //TODO: more efficient way of doing this!
					} catch let error as NSError {
						err = error
						PPRunAlertPanel("Error opening file", message: "Unable to open \((fileName as NSString).lastPathComponent): \(err!.localizedFailureReason)")
						return
					} catch {
						fatalError()
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
			let utiFile = try! sharedWorkspace.typeOfFile(filename)
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
		
		tableView.registerForDraggedTypes([PPMLDCUTI, kUTTypeFileURL as String]);
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
		for (i, obj) in (madLib).enumerate() {
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
			selectMusicAtIndex(selMus)
			do {
				try loadMusicURL(musicList.URLAtIndex(selMus), autoPlay: false)
				currentlyPlayingIndex.index = selMus
				currentlyPlayingIndex.playbackURL = musicList.URLAtIndex(selMus)
				previouslyPlayingIndex = currentlyPlayingIndex
			} catch let error1 as NSError {
				NSAlert(error: error1).runModal()
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
		do {
			let utiFile = try NSWorkspace.sharedWorkspace().typeOfFile(filename)
			return handleFile(NSURL(fileURLWithPath: filename), ofType: utiFile)
		} catch let err as NSError {
			let tmpAlert = NSAlert()
			tmpAlert.alertStyle = .WarningAlertStyle
			tmpAlert.messageText = "Error opening file"
			tmpAlert.informativeText = "Unable to open \((filename as NSString).lastPathComponent): \(err.localizedFailureReason)"
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
		if (self.currentlyPlayingIndex.index + 1 < musicList.countOfMusicList) {
			currentlyPlayingIndex.index++;
			selectCurrentlyPlayingMusic()
			do {
				try loadMusicFromCurrentlyPlayingIndex()
			} catch let err as NSError {
				NSAlert(error: err).runModal()
			}
		} else if (NSUserDefaults.standardUserDefaults().boolForKey(PPLoopMusicWhenDone)) {
			currentlyPlayingIndex.index = 0;
			selectCurrentlyPlayingMusic()
			do {
				try loadMusicFromCurrentlyPlayingIndex()
			} catch let err as NSError {
				NSAlert(error: err).runModal()
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
			do {
				try loadMusicFromCurrentlyPlayingIndex()
			} catch let err as NSError {
				NSAlert(error: err).runModal()
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
	
	private func rawSoundData(inout settings: MADDriverSettings, handler: (NSData) throws -> Void) throws {
		let theRec = try PPDriver(library: madLib, settings: &settings)
		theRec.cleanDriver()
		theRec.currentMusic = music
		theRec.play()
		
		while let newData = theRec.directSave() {
			try handler(newData)
		}
	}
	
	private func saveMusic(waveToURL theURL: NSURL, inout theSett: MADDriverSettings) throws {
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
		
		var asbd = AudioStreamBasicDescription(sampleRate: Float64(theSett.outPutRate), formatFlags: [.SignedInteger, .Packed], bitsPerChannel: UInt32(theSett.outPutBits), channelsPerFrame: tmpChannels)
		var realFormat = AudioStreamBasicDescription(sampleRate: Float64(theSett.outPutRate), formatFlags: [.SignedInteger, .Packed, .NativeEndian], bitsPerChannel: UInt32(theSett.outPutBits), channelsPerFrame: tmpChannels)
		
		res = ExtAudioFileCreate(URL: theURL, fileType: .WAVE, streamDescription: &asbd, flags: .EraseFile, audioFile: &audioFile)
		
		defer {
			if (audioFile != nil) {
				ExtAudioFileDispose(audioFile)
			}
		}
		
		if res != noErr {
			throw NSError(domain: NSOSStatusErrorDomain, code: Int(res), userInfo: nil)
		}
		
		res = ExtAudioFileSetProperty(audioFile, propertyID: .ClientDataFormat, dataSize: UInt32(sizeof(AudioStreamBasicDescription)), data: &realFormat)
		if (res != noErr) {
			throw NSError(domain: NSOSStatusErrorDomain, code: Int(res), userInfo: nil)
		}
		
		func handler(data: NSData) throws {
			let toWriteSize = data.length
			
			var audBufList = AudioBufferList()
			audBufList.mNumberBuffers = 1
			audBufList.mBuffers.mNumberChannels = tmpChannels
			audBufList.mBuffers.mDataByteSize = UInt32(data.length)
			audBufList.mBuffers.mData = UnsafeMutablePointer<Void>(data.bytes)
			
			
			let res = ExtAudioFileWrite(audioFile, UInt32(toWriteSize) / realFormat.mBytesPerFrame, &audBufList)
			
			if res != noErr {
				throw NSError(domain: NSOSStatusErrorDomain, code: Int(res), userInfo: nil)
			}
		}
		
		try rawSoundData(&theSett, handler: handler)
		
		applyMetadataToFileID(audioFile)
		res = ExtAudioFileDispose(audioFile)
		audioFile = nil
		if (res != noErr) {
			throw NSError(domain: NSOSStatusErrorDomain, code: Int(res), userInfo: nil)
		}
	}
	
	private func saveMusic(AIFFToURL theURL: NSURL, inout theSett: MADDriverSettings) throws {
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
		
		var asbd = AudioStreamBasicDescription(sampleRate: Float64(theSett.outPutRate), formatFlags: [.SignedInteger, .Packed, .BigEndian], bitsPerChannel: UInt32(theSett.outPutBits), channelsPerFrame: tmpChannels)
		var realFormat = AudioStreamBasicDescription(sampleRate: Float64(theSett.outPutRate), formatFlags: [.SignedInteger, .Packed, .NativeEndian], bitsPerChannel: UInt32(theSett.outPutBits), channelsPerFrame: tmpChannels)
		
		res = ExtAudioFileCreate(URL: theURL, fileType: .AIFF, streamDescription: &asbd, flags: .EraseFile, audioFile: &audioFile)
		
		defer {
			if (audioFile != nil) {
				ExtAudioFileDispose(audioFile)
			}
		}
		if res != noErr {
			throw NSError(domain: NSOSStatusErrorDomain, code: Int(res), userInfo: nil)
		}
		
		res = ExtAudioFileSetProperty(audioFile, propertyID: .ClientDataFormat, dataSize: UInt32(sizeof(AudioStreamBasicDescription)), data: &realFormat)
		if (res != noErr) {
			throw NSError(domain: NSOSStatusErrorDomain, code: Int(res), userInfo: nil)
		}
		
		func handler(data: NSData) throws {
			let toWriteSize = data.length
			
			var audBufList = AudioBufferList()
			audBufList.mNumberBuffers = 1
			audBufList.mBuffers.mNumberChannels = tmpChannels
			audBufList.mBuffers.mDataByteSize = UInt32(data.length)
			audBufList.mBuffers.mData = UnsafeMutablePointer<Void>(data.bytes)
			
			
			let res = ExtAudioFileWrite(audioFile, UInt32(toWriteSize) / realFormat.mBytesPerFrame, &audBufList)
			
			if res != noErr {
				throw NSError(domain: NSOSStatusErrorDomain, code: Int(res), userInfo: nil)
			}
		}
		
		try rawSoundData(&theSett, handler: handler)
		
		applyMetadataToFileID(audioFile)
		res = ExtAudioFileDispose(audioFile)
		audioFile = nil
		if res != noErr {
			throw NSError(domain: NSOSStatusErrorDomain, code: Int(res), userInfo: nil)
		}
	}
	
	private func applyMetadataToFileID(theID: AudioFileID) {
		//TODO: implement, but how?
	}
	
	@IBAction func exportMusicAs(sender: AnyObject!) {
		var tag = (sender as! NSMenuItem).tag;
		madDriver.beginExport()
		let savePanel = NSSavePanel()
		savePanel.canCreateDirectories = true
		savePanel.canSelectHiddenExtension = true
		if musicName != "" {
			savePanel.nameFieldStringValue = musicName
		}
		savePanel.prompt = "Export"
		
		switch (tag) {
		case -1: //AIFF
			savePanel.allowedFileTypes = [AVFileTypeAIFF]
			savePanel.title = "Export as AIFF audio"
			savePanel.beginSheetModalForWindow(self.window, completionHandler: { (result) -> Void in
				guard result == NSFileHandlingPanelOKButton else {
					self.madDriver.endExport()
					return
				}
				
				self.beginExportSettingsWithHandler( { (result) -> Void in
					guard result == NSAlertDefaultReturn else {
						self.madDriver.endExport()
						return
					}
					
					dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0)) {
							do {
								try self.saveMusic(AIFFToURL: savePanel.URL!, theSett: &self.exportSettings);
								self.madDriver.endExport()
								dispatch_async(dispatch_get_main_queue()) {
									if self.isQuitting {
										NSApplication.sharedApplication().replyToApplicationShouldTerminate(true)
									} else {
										let retVal = PPRunInformationalAlertPanel("Export complete", message: "The export of the file \"\(savePanel.URL!.lastPathComponent!)\" is complete.", defaultButton: "OK", alternateButton: "Show File");
										if (retVal == NSAlertAlternateReturn) {
											NSWorkspace.sharedWorkspace().activateFileViewerSelectingURLs([savePanel.URL!])
										}
									}
								}
							} catch let thErr as NSError {
								if (self.isQuitting) {
									NSApplication.sharedApplication().replyToApplicationShouldTerminate(true)
								} else {
									dispatch_async(dispatch_get_main_queue()) {
										NSAlert(error: thErr).runModal()
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
							titleName.key = AVMetadataCommonKeyTitle
							titleName.value = oldMusicName
							
							let dataInfo = AVMutableMetadataItem()
							dataInfo.keySpace = AVMetadataKeySpaceQuickTimeUserData
							dataInfo.key = AVMetadataQuickTimeUserDataKeySoftware
							dataInfo.value = "PlayerPRO Player"
							dataInfo.locale = NSLocale(localeIdentifier: "en_US")
							
							let musicInfoQTUser = AVMutableMetadataItem();
							musicInfoQTUser.keySpace = AVMetadataKeySpaceQuickTimeUserData
							musicInfoQTUser.key = AVMetadataQuickTimeUserDataKeyInformation
							musicInfoQTUser.value = oldMusicInfo
							musicInfoQTUser.locale = NSLocale.currentLocale()
							
							let musicNameQTUser = AVMutableMetadataItem()
							musicNameQTUser.keySpace = AVMetadataKeySpaceQuickTimeUserData
							musicNameQTUser.key = AVMetadataQuickTimeUserDataKeyFullName
							musicNameQTUser.value = oldMusicName
							musicNameQTUser.locale = NSLocale.currentLocale()
							
							let musicInfoiTunes = AVMutableMetadataItem()
							musicInfoiTunes.keySpace = AVMetadataKeySpaceiTunes
							musicInfoiTunes.key = AVMetadataiTunesMetadataKeyUserComment
							musicInfoiTunes.value = oldMusicInfo
							
							let musicInfoQTMeta = AVMutableMetadataItem();
							musicInfoQTMeta.keySpace = AVMetadataKeySpaceQuickTimeMetadata
							musicInfoQTMeta.key = AVMetadataQuickTimeMetadataKeyInformation
							musicInfoQTMeta.value = oldMusicInfo
							musicInfoQTMeta.locale = NSLocale.currentLocale()
							
							return [titleName, dataInfo, musicInfoQTUser, musicInfoiTunes, musicInfoQTMeta, musicNameQTUser];
						}
						
						let tmpName = oldMusicName != "" ? oldMusicName : "untitled"
						let tmpURL = (try! NSFileManager.defaultManager().URLForDirectory(.ItemReplacementDirectory, inDomain: .UserDomainMask, appropriateForURL: oldURL, create: true)).URLByAppendingPathComponent("\(tmpName).aiff", isDirectory: false)
						
						do {
							try self.saveMusic(AIFFToURL: tmpURL, theSett: &self.exportSettings)
						} catch let anErr as NSError {
							expErr = anErr
							dispatch_async(dispatch_get_main_queue(), errBlock);
							return
						}
						defer {
							do {
								try NSFileManager.defaultManager().removeItemAtURL(tmpURL)
							} catch _ {
								
							}
						}
						
						let exportMov = AVAsset(URL: tmpURL)
						let metadataInfo = generateAVMetadataInfo()
						
						guard let session = AVAssetExportSession(asset:exportMov, presetName: AVAssetExportPresetAppleM4A) else {
							expErr = NSError(domain: NSCocoaErrorDomain, code: NSFileWriteUnknownError, userInfo: nil)
							NSLog("Init Failed for %@, error: %@", oldMusicName, expErr!.localizedDescription)
							dispatch_async(dispatch_get_main_queue(), errBlock)
							return;

						}
							#if false
								if (session == nil) {
									expErr = NSError(domain: NSCocoaErrorDomain, code: NSFileWriteUnknownError, userInfo: nil)
									NSLog("Export session creation for %@ failed, error: %@", oldMusicName, expErr!.localizedDescription);
									NSFileManager.defaultManager().removeItemAtURL(tmpURL, error: nil)
									dispatch_async(dispatch_get_main_queue(), errBlock);
									return;
								}
							#endif
							do {
								try NSFileManager.defaultManager().removeItemAtURL(saveURL)
							} catch _ {
							}
							session.outputURL = saveURL
							session.outputFileType = AVFileTypeAppleM4A
							session.metadata = metadataInfo;
							let sessionWaitSemaphore = dispatch_semaphore_create(0);
							session.exportAsynchronouslyWithCompletionHandler({ () -> Void in
								_ = dispatch_semaphore_signal(sessionWaitSemaphore)
							})
							dispatch_semaphore_wait(sessionWaitSemaphore, DISPATCH_TIME_FOREVER)
							
							let didFinish = session.status == .Completed;
							
							if didFinish {
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
								if self.isQuitting {
									NSApplication.sharedApplication().replyToApplicationShouldTerminate(true)
								}
							}
					}
				})
			})
			
		case -3: // wave
			savePanel.allowedFileTypes = [AVFileTypeWAVE]
			savePanel.title = "Export as Wave Audio"
			savePanel.beginSheetModalForWindow(window, completionHandler: { (result) -> Void in
				if (result != NSFileHandlingPanelOKButton) {
					self.madDriver.endExport()
					return;
				}
				self.beginExportSettingsWithHandler({ (result) -> Void in
					guard result == NSAlertDefaultReturn else {
						self.madDriver.endExport()
						if (self.isQuitting) {
							NSApplication.sharedApplication().replyToApplicationShouldTerminate(true)
						}
						return;
					}
					
					dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0)) {
						autoreleasepool {
							do {
								try self.saveMusic(waveToURL: savePanel.URL!, theSett: &self.exportSettings);
								self.madDriver.endExport()
								dispatch_async(dispatch_get_main_queue()) {
									if self.isQuitting {
										NSApplication.sharedApplication().replyToApplicationShouldTerminate(true)
									} else {
										let retVal = PPRunInformationalAlertPanel("Export complete", message: "The export of the file \"\(savePanel.URL!.lastPathComponent!)\" is complete.", defaultButton: "OK", alternateButton: "Show File");
										if (retVal == NSAlertAlternateReturn) {
											NSWorkspace.sharedWorkspace().activateFileViewerSelectingURLs([savePanel.URL!])
										}
									}
								}
							} catch let error as NSError {
								if (self.isQuitting) {
									NSApplication.sharedApplication().replyToApplicationShouldTerminate(true)
								} else {
									dispatch_async(dispatch_get_main_queue()) {
									PPRunAlertPanel("Export failed", message: "Export/coversion of the music file failed:\n\(error.localizedDescription)")
									}
								}
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
				let err = self.music.exportMusicToURL(fileURL!, format: self.madLib[tag].type, library: self.madLib)
				self.madDriver.endExport()
				if (err != .NoErr) {
					if (self.isQuitting) {
						NSApplication.sharedApplication().replyToApplicationShouldTerminate(true)
					} else {
						let aerr = createErrorFromMADErrorType(err)!
						NSAlert(error: aerr).runModal()
					}
				} else {
					self.addMusicToMusicList(fileURL!, loadIfPreferencesAllow: false)
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
		self.currentlyPlayingIndex.index = tableView.selectedRow;
		do {
			try loadMusicFromCurrentlyPlayingIndex()
		} catch let err as NSError {
			NSAlert(error: err).runModal()
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
					self.addMusicsToMusicList(panel.URLs )
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
		var aPPInfo: PPLibrary.MusicFileInfo? = nil
		
		func badValues() {
			do {
				let strType = try madLib.identifyFile(URL: musicURL)
				aPPInfo = try madLib.informationFromFile(URL: musicURL, type: strType)
				
			} catch {
				aPPInfo = nil
			}
		}
		
		do {
			let fileUTI = try NSWorkspace.sharedWorkspace().typeOfFile(musicURL.path!)
			if let info = madLib.typeFromUTI(fileUTI) {
				aPPInfo = try madLib.informationFromFile(URL:musicURL, type: info)
			} else {
				badValues()
			}
		} catch _ {
			badValues()
		}
		
		if let actualInfo = aPPInfo {
			fileName.stringValue = obj.fileName
			internalName.stringValue = actualInfo.internalFileName
			fileSize.objectValue = NSNumber(unsignedLongLong: obj.fileSize)
			musicInstrument.integerValue = actualInfo.totalInstruments
			musicPatterns.integerValue = actualInfo.totalPatterns
			musicPlugType.stringValue = actualInfo.formatDescription
			musicSignature.stringValue = actualInfo.signature
			fileLocation.stringValue = musicURL.absoluteURL.path!
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
				let selArray = musicList.arrayOfObjectsInMusicListAtIndexes(dragIndexSet)
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
		var tmpObjs: [NSPasteboardWriting] = [dragClass] //+ urlArrays
		for obj in urlArrays {
			tmpObjs.append(obj)
		}
		//tmpObjs.extend(urlArrays)
		//tmpObjs += urlArrays
		pboard.clearContents(); // clear pasteboard to take ownership
		status = pboard.writeObjects(tmpObjs) // write the URLs
		return status;
	}
	
	func tableView(atableView: NSTableView, sortDescriptorsDidChange oldDescriptors: [NSSortDescriptor]) {
		changeValueForMusicListKey({
			self.musicList.sortMusicList(descriptors: atableView.sortDescriptors )
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
