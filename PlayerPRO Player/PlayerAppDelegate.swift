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

private func cocoaDebugStr(line: Int16, file: UnsafePointer<Int8>?, text: UnsafePointer<Int8>?) {
	let swiftFile = FileManager.default.string(withFileSystemRepresentation: file!, length: Int(strlen(file)))
	let swiftText = String(validatingUTF8: text!)!
	print("\(swiftFile):\(line), error text: \(swiftText)")
	let errStr = NSLocalizedString("MyDebugStr_Error", comment: "Error")
	let mainStr = String(format: NSLocalizedString("MyDebugStr_MainText", comment: "The Main text to display"), text!)
	let quitStr = NSLocalizedString("MyDebugStr_Quit", comment: "Quit")
	let contStr = NSLocalizedString("MyDebugStr_Continue", comment: "Continue")
	let debuStr = NSLocalizedString("MyDebugStr_Debug", comment: "Debug")
	
	let alert = PPRunCriticalAlertPanel(title: errStr, message: mainStr, defaultButton: quitStr, alternateButton: contStr, otherButton: debuStr)
	switch (alert) {
	case NSAlertSecondButtonReturn:
		break
		
	case NSAlertThirdButtonReturn:
		assert(false, "Chose to go to debugger.")
		
	case NSAlertFirstButtonReturn:
		print("Choosing to fail!")
		fallthrough
	default:
		abort()
	}
}

@NSApplicationMain
class PlayerAppDelegate: NSObject, NSApplicationDelegate, NSTableViewDelegate, NSToolbarDelegate, NSTableViewDataSource {
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
	dynamic var music: PPMusicObject?
	
	@IBOutlet var toolsPanel: NSPanel!
	var timeChecker: Timer!
	var madDriver: PPDriver!
	let madLib: PPLibrary = {
		return try! PPLibrary()
	}()
	
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
		if (UserDefaults.standard.bool(forKey: PPLoadMusicAtListLoad) && musicList.countOfMusicList > 0) {
			currentlyPlayingIndex.index = selMusFromList != -1 ? selMusFromList : 0;
			selectCurrentlyPlayingMusic()
			do {
				try loadMusicFromCurrentlyPlayingIndex()
			} catch let err as NSError {
				NSAlert(error: err).runModal()
			}
		} else if (selMusFromList != -1) {
			selectMusic(atIndex: selMusFromList)
		}
		let lostCount = musicList.lostMusicCount;
		if (lostCount != 0) {
			let uresolvedStr = String(format: kUnresolvableFileDescription, lostCount)
			_ = PPRunAlertPanel(title: kUnresolvableFile, message: uresolvedStr)
		}
	}
	
	private func changeValueForMusicListKey(_ theClosure: () -> Void) {
		willChangeValue(forKey: kMusicListKVO)
		theClosure()
		didChangeValue(forKey: kMusicListKVO)
	}
	
	private func musicListContentsDidMove() {
		if (currentlyPlayingIndex.index != -1) {
			for i in 0..<musicList.countOfMusicList {
				if (currentlyPlayingIndex.playbackURL == musicList[i].musicURL) {
					currentlyPlayingIndex.index = i;
					break;
				}
			}
		}
		if (previouslyPlayingIndex.index != -1) {
			for i in 0..<musicList.countOfMusicList {
				if (previouslyPlayingIndex.playbackURL == musicList[i].musicURL) {
					previouslyPlayingIndex.index = i;
					break;
				}
			}
		}
	}
	
	@IBAction func showMusicList(_ sender: AnyObject?) {
		window.makeKeyAndOrderFront(sender)
	}
	
	
	@IBAction func showPreferences(_ sender: AnyObject!) {
		preferences.window!.center()
		preferences.showWindow(sender)
	}
	
	override init() {
		let ourDefaults: [String: Any] = [PPRememberMusicList: true,
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
		
		UserDefaults.standard.register(defaults: ourDefaults);
		super.init()
	}
	
	@objc private func updateMusicStats(_ theTimer: Timer?) {
		if (self.music != nil) {
			var time = madDriver.musicStatusTime!
			if (madDriver.isDonePlayingMusic && paused == false && madDriver.isExporting == false) {
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
			selectMusic(atIndex: currentlyPlayingIndex.index)
		}
	}
	
	func addMusicToMusicList(_ theURL: URL, loadIfPreferencesAllow load: Bool = true) {
		willChangeValue(forKey: kMusicListKVO)
		let okayMusic = musicList.addMusicURL(theURL, force: false)
		didChangeValue(forKey: kMusicListKVO)
		switch okayMusic {
		case .Failure:
			return
			
		case .SimilarURL:
			let similarMusicIndex = musicList.indexOfObjectSimilar(to: theURL)!
			let similarAlert = NSAlert()
			similarAlert.messageText = "Existing object";
			similarAlert.informativeText = "There is already a tracker file that points to the added file. Do you still wish to add it?";
			similarAlert.addButton(withTitle: "Add")
			
			let cancelButton = similarAlert.addButton(withTitle: "Cancel")
			cancelButton.keyEquivalent = PPEscapeValue;
			
			similarAlert.addButton(withTitle: "Load Existing")
			
			let alertSelect = similarAlert.runModal()
			
			switch (alertSelect) {
			case NSAlertFirstButtonReturn:
				_ = musicList.addMusicURL(theURL, force: true)
				if load && UserDefaults.standard.bool(forKey: PPLoadMusicAtMusicLoad) {
					self.currentlyPlayingIndex.index = musicList.countOfMusicList - 1;
					//currentlyPlayingIndex.playbackURL = [musicList URLAtIndex:currentlyPlayingIndex.index];
					selectCurrentlyPlayingMusic()
					do {
						try loadMusicFromCurrentlyPlayingIndex()
					} catch let err as NSError {
						NSAlert(error: err).runModal()
					}
				} else {
					selectMusic(atIndex: musicList.countOfMusicList - 1)
				}
				break;
				
			case NSAlertSecondButtonReturn:
				return;
				
			case NSAlertThirdButtonReturn:
				if load && UserDefaults.standard.bool(forKey: PPLoadMusicAtMusicLoad) {
					self.currentlyPlayingIndex.index = similarMusicIndex;
					selectCurrentlyPlayingMusic()
					do {
						try loadMusicFromCurrentlyPlayingIndex()
					} catch let err as NSError {
						NSAlert(error: err).runModal()
					}
				} else {
					selectMusic(atIndex: similarMusicIndex)
				}
				
				break;
				
			default:
				break;
			}
			
		case .Success:
			if load && UserDefaults.standard.bool(forKey: PPLoadMusicAtMusicLoad) {
				self.currentlyPlayingIndex.index = musicList.countOfMusicList - 1;
				//currentlyPlayingIndex.playbackURL = [musicList URLAtIndex:currentlyPlayingIndex.index];
				selectCurrentlyPlayingMusic()
				do {
					try loadMusicFromCurrentlyPlayingIndex()
				} catch let err as NSError {
					NSAlert(error: err).runModal()
				}
			} else {
				selectMusic(atIndex: musicList.countOfMusicList - 1)
			}
		}
	}
	
	private func songIsDonePlaying() {
		let userDefaults = UserDefaults.standard
		switch PlaylistMode(rawValue: userDefaults.integer(forKey: PPAfterPlayingMusic))! {
			
		case .LoopMusic:
			madDriver.musicPosition = 0;
			madDriver.play()
			break;
			
		case .LoadNext:
			self.currentlyPlayingIndex.index += 1
			if musicList.countOfMusicList > self.currentlyPlayingIndex.index {
				selectCurrentlyPlayingMusic()
				do {
					try loadMusicFromCurrentlyPlayingIndex()
				} catch let err as NSError {
					NSAlert(error: err).runModal()
				}
			} else {
				if (userDefaults.bool(forKey: PPLoopMusicWhenDone)) {
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
					if (userDefaults.bool(forKey: PPGotoStartupAfterPlaying)) {
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
			if (userDefaults.bool(forKey: PPGotoStartupAfterPlaying)) {
				madDriver.musicPosition = 0;
			}
			self.paused = true
			break;
		}
	}
	
	@objc(loadMusicFromCurrentlyPlayingIndexAndReturnError:) func loadMusicFromCurrentlyPlayingIndex() throws {
		var theErr: NSError! = NSError(domain: "Migrator", code: 0, userInfo: nil)
		currentlyPlayingIndex.playbackURL = musicList.url(at: currentlyPlayingIndex.index)
		let isGood: Bool
		do {
			try loadMusic(url: currentlyPlayingIndex.playbackURL!)
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
	
	private func addMusics(toMusicList theMusList: [URL]) {
		for i in 0 ..< (theMusList.count - 1) {
			addMusicToMusicList(theMusList[i], loadIfPreferencesAllow: false)
		}
		addMusicToMusicList(theMusList.last!, loadIfPreferencesAllow: true)
	}
	
	func loadMusic(url musicToLoad: URL, autoPlay: Bool = true) throws {
		var error: NSError? = nil
		var fileType: String? = nil
		if music != nil {
			madDriver.stop()
			madDriver.stopDriver()
		}
		
		do {
			let fileUTI = try NSWorkspace.shared().type(ofFile: musicToLoad.path)
			if let afileType = madLib.typeFromUTI(fileUTI) {
				try madLib.testFile(at: musicToLoad, as: afileType)
				fileType = afileType
			}
		} catch let err3 as NSError {
			error = err3
		}
		
		if fileType == nil {
			do {
				fileType = try madLib.identifyFile(at: musicToLoad)
				error = nil
			} catch let err3 as NSError {
				error = err3
			}
		}
		
		if let error = error {
			paused = true
			clearMusic()
			throw error
		}
		
		do {
			self.music = try PPMusicObject(url: musicToLoad, type: fileType!, library: madLib)
		} catch let err3 {
			self.paused = true
			clearMusic()
			throw err3
		}
		
		music!.attach(to: madDriver)
		
		if autoPlay {
			madDriver.play()
			
			paused = false
		}
		
		let time = madDriver.musicStatusTime!
		songPos.maxValue = Double(time.total)
		songPos.minValue = 0
		setTitleForSongLabelBasedOnMusic()
		songTotalTime.integerValue = time.total
		
		NotificationCenter.default.post(name: NSNotification.Name.PPMusicDidChange, object:self)
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
		NotificationCenter.default.post(name: NSNotification.Name.PPMusicDidChange, object:self)
		music!.attach(to: madDriver)
	}
	
	private func setTitleForSongLabelBasedOnMusic() {
		self.musicName = music?.title ?? ""
		self.musicInfo = music?.information ?? ""
	}
	
	private func MADDriverWithPreferences() {
		var madWasReading = false;
		var fullTime = 0, curTime = 0;
		if madDriver != nil {
			madWasReading = !madDriver.isPaused
			madDriver.stop()
			//[madDriver stopDriver];
			
			madDriver.getMusicStatus(withCurrentTime: &curTime, totalTime:&fullTime)
		}
		var theSettinit = MADDriverSettings.new();
		let defaults = UserDefaults.standard
		
		//TODO: Sanity Checking
		theSettinit.surround = defaults.bool(forKey: PPSurroundToggle)
		theSettinit.outPutRate = UInt32(defaults.integer(forKey: PPSoundOutRate))
		theSettinit.outPutBits = Int16(defaults.integer(forKey: PPSoundOutBits))
		if defaults.bool(forKey: PPOversamplingToggle) {
			theSettinit.oversampling = Int32(defaults.integer(forKey: PPOversamplingAmount))
		} else {
			theSettinit.oversampling = 1;
		}
		theSettinit.Reverb = defaults.bool(forKey: PPReverbToggle)
		theSettinit.ReverbSize = Int32(defaults.integer(forKey: PPReverbAmount))
		theSettinit.ReverbStrength = Int32(defaults.integer(forKey: PPReverbStrength))
		if defaults.bool(forKey: PPStereoDelayToggle) {
			theSettinit.MicroDelaySize = Int32(defaults.integer(forKey: PPStereoDelayAmount))
		} else {
			theSettinit.MicroDelaySize = 0;
		}
		
		theSettinit.driverMode = MADSoundOutput(rawValue: Int16(defaults.integer(forKey: PPSoundDriver))) ?? .CoreAudioDriver
		theSettinit.repeatMusic = false;
		
		//OSErr returnerr = MADCreateDriver(&init, madLib, &madDriver);
		if madDriver == nil {
			do {
				madDriver = try PPDriver(library: madLib, settings: &theSettinit)
			} catch let error as NSError {
				for pref in [PPSoundOutBits, PPSoundOutRate, PPSoundDriver, PPStereoDelayToggle,
					PPReverbToggle, PPSurroundToggle, PPOversamplingToggle, PPStereoDelayAmount,
					PPReverbAmount, PPReverbStrength, PPOversamplingAmount] {
					defaults.removeObject(forKey: pref)
				}
				defaults.synchronize()

				let alert = NSAlert()
				alert.messageText = "Sound Driver failure"
				alert.informativeText = "Unable to create the PlayerPRO sound driver due to error type \(error.code). Sound preferences will be reset.\n\nPlayerPRO will now close."
				alert.alertStyle = .critical
				alert.runModal()
				
				print("Could not create initial driver, error \(error)")
				exit(EXIT_FAILURE)
			}
		} else {
			do {
				try madDriver.changeDriverSettings(to: &theSettinit)
			} catch let error as NSError {
				NotificationCenter.default.post(name: .PPDriverDidChange, object: self)
				NSAlert(error: error).runModal()
				return;
			}
		}
		NotificationCenter.default.post(name: .PPDriverDidChange, object: self)
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
		
		plugInInfos.sort(by: { (lhs, rhs) -> Bool in
			let menuNam1 = lhs.plugName
			let menuNam2 = rhs.plugName
			let res = menuNam1.localizedStandardCompare(menuNam2)
			return res == .orderedAscending
		})
		
		aboutPlugInMenu.removeAllItems()
		
		for (i, pi) in plugInInfos.enumerated() {
			let mi = NSMenuItem(title: pi.plugName, action: #selector(PlayerAppDelegate.showPlugInInfo(_:)), keyEquivalent: "")
			mi.tag = i
			mi.target = self
			aboutPlugInMenu.addItem(mi)
		}
	}
	
	@IBAction func showPlugInInfo(_ sender: NSMenuItem) {
		let inf = (self.plugInInfos)[sender.tag]
		
		let infoCont = PlugInInfoController.windowController(from: inf)
		if let infoWindow = infoCont.window {
			infoWindow.center()
			NSApplication.shared().runModal(for: infoWindow)
		}
	}
	
	private func selectMusic(atIndex anIdx: Int) {
		if (anIdx < 0 || anIdx >= musicList.countOfMusicList) {
			NSBeep();
			return;
		}
		let idx = IndexSet(integer: anIdx)
		tableView.selectRowIndexes(idx, byExtendingSelection: false)
		tableView.scrollRowToVisible(anIdx)
	}
	
	@IBAction func clearMusicList(_ sender: AnyObject?) {
		if musicList.countOfMusicList != 0 {
			let clearMusicListStr = String(format: NSLocalizedString("The music list contains %ld items. Do you really want to remove them?", comment: "Clear Music List?"), musicList.countOfMusicList)
			
			let returnVal = PPRunAlertPanel(title: NSLocalizedString("Clear list", comment: "Clear Music List"), message: clearMusicListStr, defaultButton: NSLocalizedString("No", comment: "No"), alternateButton: NSLocalizedString("Yes", comment: "Yes"));
			
			if (returnVal == NSAlertSecondButtonReturn) {
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
	
	@IBAction func saveMusicList(_ sender: AnyObject!) {
		let savePanel = NSSavePanel()
		savePanel.allowedFileTypes = [PPMusicListUTI]
		savePanel.canCreateDirectories = true
		savePanel.canSelectHiddenExtension = true
		savePanel.beginSheetModal(for: self.window, completionHandler: { (result) -> Void in
			if result == NSFileHandlingPanelOKButton {
				self.musicList.saveMusicList(to: savePanel.url!)
			}
		})
	}
	
	@IBAction func showInstrumentsList(_ sender: AnyObject?) {
		instrumentController!.showWindow(sender)
	}
	
	func handleFile(_ theURL1: URL, ofType theUTI: String) -> Bool {
		let sharedWorkspace = NSWorkspace.shared()
		var theURL = theURL1
		if (theUTI  == MADGenericUTI) {
			let invExt = NSLocalizedString("Invalid Extension", comment: "Invalid Extension")
			let invExtDes = String(format: NSLocalizedString("The file %@ is identified as as a generic MAD tracker, and not a specific one. Renaming it will fix this. Do you want to rename the file extension?", comment: "Invalid extension description"), theURL.lastPathComponent)
			let renameFile =  NSLocalizedString("Rename", comment: "rename file")
			let openFile = NSLocalizedString("Open", comment: "Open a file")
			let cancelOp = NSLocalizedString("Cancel", comment: "Cancel")
			let retVal = PPRunInformationalAlertPanel(title: invExt, message:invExtDes, defaultButton: renameFile, alternateButton:openFile, otherButton: cancelOp)
			switch (retVal) {
			case NSAlertFirstButtonReturn:
				
				do {
					let identRet = try madLib.identifyFile(at: theURL)
					let tmpURL = theURL.deletingPathExtension().appendingPathExtension(identRet.lowercased())
					
					do {
						try FileManager.default.moveItem(at: theURL, to: tmpURL)
						theURL = tmpURL
						//TODO: regenerate the UTI
					} catch let err as NSError {
						print("Could not move file, error \(err)")
						let couldNotRenameStr = String(format: NSLocalizedString("The file could not be renamed to \"%@\".\n\nThe music file \"%@\" will still be loaded.", comment: "Could not rename file"), tmpURL.lastPathComponent, theURL.lastPathComponent)
						_ = PPRunInformationalAlertPanel(title: NSLocalizedString("Rename Error", comment: "Rename Error"), message: couldNotRenameStr)
					}
					
				} catch {
					PPRunCriticalAlertPanel(title: NSLocalizedString("Unknown File", comment: "unknown file"), message: NSLocalizedString("The file type could not be identified.", comment: "Unidentified file"))
					return false
				}
				
			case NSAlertSecondButtonReturn:
				break;
				
				//case NSAlertThirdButtonReturn:
			default:
				return true;
			}
		}
		if (sharedWorkspace.type(theUTI, conformsToType:PPMusicListUTI)) {
			if (musicListWillChange()) {
				changeValueForMusicListKey({ () -> Void in
					_ = self.musicList.loadMusicList(from: theURL)
					self.selMusFromList = self.musicList.selectedMusic
				})
				musicListDidChange()
				return true
			}
		} else if (sharedWorkspace.type(theUTI, conformsToType:PPOldMusicListUTI)) {
			if musicListWillChange() {
				self.willChangeValue(forKey: kMusicListKVO)
				musicList.beginLoadingOfOldMusicListAtURL(toOpen: theURL, completionHandle: { (theErr) -> Void in
					self.selMusFromList = self.musicList.selectedMusic
					self.didChangeValue(forKey: kMusicListKVO)
					if let theErr = theErr {
						NSAlert(error: theErr).runModal()
					}
				})
				return true;
			}
		} else {
			//TODO: check for valid extension.
			for aUTI in trackerUTIs {
				if sharedWorkspace.type(theUTI, conformsToType: aUTI) {
					addMusicToMusicList(theURL)
					return true;
				}
			}
			
			// Final check, just in case the UTI is messed up...
			// Yes, it does happen.
			do {
				let aType = try madLib.identifyFile(at: theURL)
				try madLib.testFile(at: theURL, as: aType)
				addMusicToMusicList(theURL)
			} catch _ {
				
			}
		}
		return false;
	}
	
	@IBAction func openFile(_ sender: AnyObject?) {
		let panel = NSOpenPanel()
		let playlistDict = ["PlayerPRO Music List" : [PPMusicListUTI],
			"PlayerPRO Old Music List" : [PPOldMusicListUTI]];
		
		if let av = OpenPanelViewController(openPanel: panel, trackerDictionary: trackerDict, playlistDictionary: playlistDict) {
			av.setupDefaults()
			av.allowsMultipleSelectionOfTrackers = true;
			av.beginOpenPanel(window, completionHandler: { (result) -> Void in
				if result != NSFileHandlingPanelOKButton {
					return
				}
				let ws = NSWorkspace.shared()
				let panelURLs = panel.urls
				for theURL in panelURLs {
					let fileName = theURL.path
					var err: NSError?
					do {
						let utiFile = try ws.type(ofFile: fileName)
						_=self.handleFile(theURL, ofType: utiFile) //TODO: more efficient way of doing this!
					} catch let error as NSError {
						err = error
						_=PPRunAlertPanel(title: "Error opening file", message: "Unable to open \((fileName as NSString).lastPathComponent): \(err!.localizedFailureReason)")
						return
					} catch {
						fatalError()
					}
				}
			})
		}
	}
	
	func saveMusic(to tosave: URL) {
		do {
			try music!.saveMusic(to: tosave)
		} catch let error as NSError {
			NSAlert(error: error).runModal()
		}
	}
	
	@IBAction func saveMusicAs(_ sender: AnyObject?) {
		madDriver.beginExport()
		
		let savePanel = NSSavePanel()
		savePanel.allowedFileTypes = [MADNativeUTI]
		savePanel.canCreateDirectories = true
		savePanel.canSelectHiddenExtension = true
		if musicName != "" {
			savePanel.nameFieldStringValue = musicName
		}
		
		savePanel.beginSheetModal(for: window) { (result) -> Void in
			if result == NSFileHandlingPanelOKButton {
				let saveURL = savePanel.url!
				self.saveMusic(to: saveURL)
				self.addMusicToMusicList(saveURL, loadIfPreferencesAllow: false)
			}
			self.madDriver.endExport()
		}
	}
	
	@IBAction func saveMusic(_ sender: AnyObject) {
		madDriver.beginExport()
		
		if (previouslyPlayingIndex.index == -1) {
			// saveMusicAs: will end the exporting when it is done.
			saveMusicAs(sender)
		} else {
			let fileURL = previouslyPlayingIndex.playbackURL!
			let filename = fileURL.path
			let sharedWorkspace = NSWorkspace.shared()
			let utiFile = try! sharedWorkspace.type(ofFile: filename)
			if /*[sharedWorkspace type:utiFile conformsToType:MADNativeUTI]*/ utiFile == MADNativeUTI {
				saveMusic(to: fileURL)
				madDriver.endExport()
			} else {
				// saveMusicAs: will end the exporting when it is done.
				saveMusicAs(sender)
			}
		}
	}
	
	@objc private func soundPreferencesDidChange(_ notification: NSNotification) {
		MADDriverWithPreferences()
	}
	
	//MARK: - NSApplicationDelegate
	func applicationDidFinishLaunching(_ notification: Notification) {
		srandom(UInt32(time(nil) & 0x7fffffff))
		PPLibrary.registerDebugBlock(cocoaDebugStr)
		//the NIB won't store the value anymore, so do this hackery to make sure there's some value in it.
		songTotalTime.integerValue = 0
		songCurTime.integerValue = 0
		
		tableView.register(forDraggedTypes: [PPMLDCUTI, kUTTypeFileURL as String]);
		//self.paused = YES;
		willChangeValue(forKey: kMusicListKVO)
		if UserDefaults.standard.bool(forKey: PPRememberMusicList) {
			_=musicList.loadApplicationMusicList()
		}
		let selMus = musicList.selectedMusic;
		didChangeValue(forKey: kMusicListKVO)
		
		tableView.doubleAction = #selector(PlayerAppDelegate.doubleClickMusicList)
		
		let defaultCenter = NotificationCenter.default
		defaultCenter.addObserver(self, selector: #selector(PlayerAppDelegate.soundPreferencesDidChange(_:)), name: NSNotification.Name.PPSoundPreferencesDidChange, object: nil)
		
		MADDriverWithPreferences()
		for (i, obj) in madLib.enumerated() {
			if obj.canExport {
				let mi = NSMenuItem(title: "\(obj.menuName)…", action: #selector(PlayerAppDelegate.exportMusicAs(_:)), keyEquivalent: "")
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
		
		timeChecker = Timer(fireAt: Date(), interval: 1.0/8.0, target: self, selector: #selector(PlayerAppDelegate.updateMusicStats(_:)), userInfo: nil, repeats: true)
		RunLoop.main.add(timeChecker, forMode: RunLoopMode.commonModes)
		let lostCount = musicList.lostMusicCount
		
		if lostCount != 0 {
			let unresolvable = String(format: kUnresolvableFileDescription, lostCount)
			PPRunAlertPanel(title: kUnresolvableFile, message: unresolvable);
		}
		
		if selMus != -1 {
			selectMusic(atIndex: selMus)
			do {
				try loadMusic(url: musicList.url(at: selMus), autoPlay: false)
				currentlyPlayingIndex.index = selMus
				currentlyPlayingIndex.playbackURL = musicList.url(at: selMus)
				previouslyPlayingIndex = currentlyPlayingIndex
			} catch let error1 as NSError {
				NSAlert(error: error1).runModal()
			}
		} else {
			self.music = PPMusicObject();
			music!.attach(to: madDriver)
			setTitleForSongLabelBasedOnMusic()
		}
	}
	
	func applicationShouldTerminate(_ sender: NSApplication) -> NSApplicationTerminateReply {
		if madDriver.isExporting {
			let anAlert = NSAlert()
			//TODO: localize
			anAlert.messageText = "Exporting"
			anAlert.informativeText = "PlayerPRO is currently exporting a tracker file.\nQuitting will stop this. Do you really wish to quit?"
			anAlert.addButton(withTitle: "Wait")
			anAlert.addButton(withTitle: "Quit")
			anAlert.addButton(withTitle: "Cancel")
			switch anAlert.runModal() {
			case NSAlertFirstButtonReturn:
				//Double-check to make sure we're still exporting
				if madDriver.isExporting {
					isQuitting = true
					return .terminateLater
				} else {
					return .terminateNow
				}
				
			case NSAlertSecondButtonReturn:
				return .terminateNow
				
			case NSAlertThirdButtonReturn:
				return .terminateCancel
				
			default:
				return .terminateNow
			}
		}
		return .terminateNow
	}
	
	func applicationWillTerminate(_ notification: Notification) {
		timeChecker.invalidate()
		
		madDriver.stopDriver()
		musicList.selectedMusic = currentlyPlayingIndex.index
		if UserDefaults.standard.bool(forKey: PPRememberMusicList) {
			_ = musicList.saveApplicationMusicList()
		}
		
		NotificationCenter.default.removeObserver(self)
	}
	
	func application(_ sender: NSApplication, openFile filename: String) -> Bool {
		do {
			let utiFile = try NSWorkspace.shared().type(ofFile: filename)
			return handleFile(URL(fileURLWithPath: filename), ofType: utiFile)
		} catch let err as NSError {
			let tmpAlert = NSAlert()
			tmpAlert.alertStyle = .warning
			tmpAlert.messageText = "Error opening file"
			tmpAlert.informativeText = "Unable to open \((filename as NSString).lastPathComponent): \(err.localizedFailureReason)"
			tmpAlert.runModal()
			return false;
		}
	}
	
	// MARK: - Tools functions
	
	@IBAction func fastForwardButtonPressed(_ sender: AnyObject!) {
		let currentPos = madDriver.musicPosition
		let totalPos = madDriver.totalMusicPlaybackTime
		guard currentPos + 2 < totalPos else {
			return
		}
		madDriver.musicPosition = currentPos + 2
	}
	
	@IBAction func loopButtonPressed(_ sender: AnyObject!) {
		//TODO: implement
	}
	
	@IBAction func nextButtonPressed(_ sender: AnyObject!) {
		if (self.currentlyPlayingIndex.index + 1 < musicList.countOfMusicList) {
			currentlyPlayingIndex.index += 1;
			selectCurrentlyPlayingMusic()
			do {
				try loadMusicFromCurrentlyPlayingIndex()
			} catch let err as NSError {
				NSAlert(error: err).runModal()
			}
		} else if (UserDefaults.standard.bool(forKey: PPLoopMusicWhenDone)) {
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
	
	@IBAction func playButtonPressed(_ sender: AnyObject!) {
		if (self.music != nil) {
			madDriver.play()
			paused = false;
		}
	}
	
	@IBAction func stopButtonPressed(_ sender: AnyObject!) {
		if (self.music != nil) {
			madDriver.stop()
			madDriver.cleanDriver()
			madDriver.setMusicStatusWithCurrentTime(0, maximumTime: 100, minimumTime: 0)
			paused = true;
		}
	}
	
	@IBAction func pauseButtonPressed(_ sender: AnyObject!) {
		if (self.music != nil) {
			if (self.paused) {
				madDriver.play()
			} else {
				madDriver.pause()
			}
			paused = !paused;
		}
	}
	
	@IBAction func prevButtonPressed(_ sender: AnyObject!) {
		if (currentlyPlayingIndex.index > 0) {
			currentlyPlayingIndex.index -= 1;
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
	
	@IBAction func rewindButtonPressed(_ sender: AnyObject!) {
		let currentPos = madDriver.musicPosition
		guard currentPos - 2 >= 0 else {
			return
		}
		madDriver.musicPosition = currentPos - 2
	}
	
	@IBAction func sliderChanged(_ sender: AnyObject!) {
		if (self.music != nil) {
			madDriver.setMusicStatusWithCurrentTime(Int(songPos.doubleValue), maximumTime: Int(songPos.maxValue), minimumTime: 0)
		}
	}
	
	@IBAction func showTools(_ sender: AnyObject?) {
		toolsPanel.makeKeyAndOrderFront(sender)
	}
	
	// MARK: - Exporting functions
	
	@IBAction func okayExportSettings(_ sender: AnyObject!) {
		window.endSheet(exportWindow, returnCode: NSAlertFirstButtonReturn)
		exportWindow.close()
	}
	
	@IBAction func cancelExportSettings(_ sender: AnyObject!) {
		window.endSheet(exportWindow, returnCode: NSAlertSecondButtonReturn)
		exportWindow.close()
	}
	
	private func beginExportSettings(with theHandle: @escaping (NSModalResponse) -> Void) {
		exportSettings.resetToBestDriver()
		exportSettings.driverMode = .NoHardwareDriver;
		exportSettings.repeatMusic = false;
		exportController.settingsFromDriverSettings(exportSettings)
		window.beginSheet(exportWindow) { (resp) in
			theHandle(resp)
		}
	}
	
	private func rawSoundData(_ settings: inout MADDriverSettings, handler: (Data) throws -> Void) throws {
		let theRec = try PPDriver(library: madLib, settings: &settings)
		theRec.cleanDriver()
		theRec.currentMusic = music
		theRec.play()
		
		while let newData = theRec.directSave() {
			try handler(newData)
		}
	}
	
	private func saveMusic(waveToURL theURL: URL, theSett: inout MADDriverSettings) throws {
		var audioFile: ExtAudioFile!
		let tmpChannels: UInt32
		
		switch (theSett.outPutMode) {
		case .MonoOutPut:
			tmpChannels = 1
			
		case .PolyPhonic:
			tmpChannels = 4
			
		default:
			tmpChannels = 2
		}
		
		var asbd = AudioStreamBasicDescription(sampleRate: Float64(theSett.outPutRate), formatFlags: [.signedInteger, .packed], bitsPerChannel: UInt32(theSett.outPutBits), channelsPerFrame: tmpChannels)
		var realFormat = AudioStreamBasicDescription(sampleRate: Float64(theSett.outPutRate), formatFlags: [.signedInteger, .packed, .nativeEndian], bitsPerChannel: UInt32(theSett.outPutBits), channelsPerFrame: tmpChannels)
		
		audioFile = try ExtAudioFile(createURL: theURL, fileType: .WAVE, streamDescription: &asbd, flags: .eraseFile)
		
		audioFile.clientDataFormat = realFormat
		
		func handler(data data1: Data) throws {
			let data = data1 as NSData
			let toWriteSize = data.length
			
			var audBufList = AudioBufferList()
			audBufList.mNumberBuffers = 1
			audBufList.mBuffers.mNumberChannels = tmpChannels
			audBufList.mBuffers.mDataByteSize = UInt32(toWriteSize)
			audBufList.mBuffers.mData = UnsafeMutableRawPointer(mutating: data.bytes)
			
			try audioFile.write(frames: UInt32(toWriteSize) / realFormat.mBytesPerFrame, data: &audBufList)
		}
		
		try rawSoundData(&theSett, handler: handler)
		
		applyMetadata(to: audioFile)
		audioFile = nil
	}
	
	private func saveMusic(AIFFToURL theURL: URL, theSett: inout MADDriverSettings) throws {
		var audioFile: ExtAudioFile!
		let tmpChannels: UInt32
		
		switch (theSett.outPutMode) {
		case .MonoOutPut:
			tmpChannels = 1
			
		case .PolyPhonic:
			tmpChannels = 4
			
		default:
			tmpChannels = 2
		}
		
		var asbd = AudioStreamBasicDescription(sampleRate: Float64(theSett.outPutRate), formatFlags: [.signedInteger, .packed, .bigEndian], bitsPerChannel: UInt32(theSett.outPutBits), channelsPerFrame: tmpChannels)
		var realFormat = AudioStreamBasicDescription(sampleRate: Float64(theSett.outPutRate), formatFlags: [.signedInteger, .packed, .nativeEndian], bitsPerChannel: UInt32(theSett.outPutBits), channelsPerFrame: tmpChannels)
		
		audioFile = try ExtAudioFile(createURL: theURL, fileType: .AIFF, streamDescription: &asbd, flags: .eraseFile)
		
		audioFile.clientDataFormat = realFormat
		
		func handler(data data1: Data) throws {
			let data = data1 as NSData
			let toWriteSize = data.length
			
			var audBufList = AudioBufferList()
			audBufList.mNumberBuffers = 1
			audBufList.mBuffers.mNumberChannels = tmpChannels
			audBufList.mBuffers.mDataByteSize = UInt32(toWriteSize)
			audBufList.mBuffers.mData = UnsafeMutableRawPointer(mutating: data.bytes)
			
			try audioFile.write(frames: UInt32(toWriteSize) / realFormat.mBytesPerFrame, data: &audBufList)
		}
		
		try rawSoundData(&theSett, handler: handler)
		
		applyMetadata(to: audioFile)
		audioFile = nil
	}
	
	private func applyMetadata(to theID: ExtAudioFile) {
		//TODO: implement, but how?
	}
	
	@IBAction func exportMusicAs(_ sender: AnyObject!) {
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
			savePanel.beginSheetModal(for: self.window, completionHandler: { (result) -> Void in
				guard result == NSFileHandlingPanelOKButton else {
					self.madDriver.endExport()
					return
				}
				
				self.beginExportSettings(with: { (result) -> Void in
					guard result == NSAlertFirstButtonReturn else {
						self.madDriver.endExport()
						return
					}
					
					DispatchQueue.global().async() {
						do {
							try self.saveMusic(AIFFToURL: savePanel.url!, theSett: &self.exportSettings);
							self.madDriver.endExport()
							DispatchQueue.main.async() {
								if self.isQuitting {
									NSApplication.shared().reply(toApplicationShouldTerminate: true)
								} else {
									let retVal = PPRunInformationalAlertPanel(title: "Export complete", message: "The export of the file \"\(savePanel.url!.lastPathComponent)\" is complete.", defaultButton: "OK", alternateButton: "Show File");
									if (retVal == NSAlertSecondButtonReturn) {
										NSWorkspace.shared().activateFileViewerSelecting([savePanel.url!])
									}
								}
							}
						} catch let thErr as NSError {
							if (self.isQuitting) {
								NSApplication.shared().reply(toApplicationShouldTerminate: true)
							} else {
								DispatchQueue.main.async() {
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
			savePanel.beginSheetModal(for: self.window, completionHandler: {(result) -> Void in
				if (result != NSFileHandlingPanelOKButton) {
					self.madDriver.endExport()
					return;
				}
				
				self.beginExportSettings(with: { (result) -> Void in
					let saveURL = savePanel.url!
					if result != NSAlertFirstButtonReturn {
						self.madDriver.endExport()
						return;
					}
					
					DispatchQueue.global().async() {
						var theErr = MADErr.noErr;
						
						var oldURL = self.musicList.objectInMusicList(at: self.previouslyPlayingIndex.index).musicURL
						var expErr: NSError? = nil;
						var errBlock: () -> Void = {
							if (self.isQuitting) {
								NSApplication.shared().reply(toApplicationShouldTerminate: true)
							} else {
								PPRunAlertPanel(title: "Export failed", message: "Export/coversion of the music file failed:\n\(expErr!.localizedDescription)");
							}
						};
						let oldMusicName = self.musicName
						let oldMusicInfo = self.musicInfo
						func generateAVMetadataInfo() -> [AVMetadataItem] {
							let titleName = AVMutableMetadataItem()
							titleName.keySpace = AVMetadataKeySpaceCommon
							titleName.key = AVMetadataCommonKeyTitle as NSString
							titleName.value = oldMusicName as NSString
							
							let dataInfo = AVMutableMetadataItem()
							dataInfo.keySpace = AVMetadataKeySpaceQuickTimeUserData
							dataInfo.key = AVMetadataQuickTimeUserDataKeySoftware as NSString
							dataInfo.value = "PlayerPRO Player" as NSString
							dataInfo.locale = Locale(identifier: "en")
							
							let musicInfoQTUser = AVMutableMetadataItem();
							musicInfoQTUser.keySpace = AVMetadataKeySpaceQuickTimeUserData
							musicInfoQTUser.key = AVMetadataQuickTimeUserDataKeyInformation as NSString
							musicInfoQTUser.value = oldMusicInfo as NSString
							musicInfoQTUser.locale = Locale.current
							
							let musicNameQTUser = AVMutableMetadataItem()
							musicNameQTUser.keySpace = AVMetadataKeySpaceQuickTimeUserData
							musicNameQTUser.key = AVMetadataQuickTimeUserDataKeyFullName as NSString
							musicNameQTUser.value = oldMusicName as NSString
							musicNameQTUser.locale = Locale.current
							
							let musicInfoiTunes = AVMutableMetadataItem()
							musicInfoiTunes.keySpace = AVMetadataKeySpaceiTunes
							musicInfoiTunes.key = AVMetadataiTunesMetadataKeyUserComment as NSString
							musicInfoiTunes.value = oldMusicInfo as NSString
							
							let musicInfoQTMeta = AVMutableMetadataItem();
							musicInfoQTMeta.keySpace = AVMetadataKeySpaceQuickTimeMetadata
							musicInfoQTMeta.key = AVMetadataQuickTimeMetadataKeyInformation as NSString
							musicInfoQTMeta.value = oldMusicInfo as NSString
							musicInfoQTMeta.locale = Locale.current
							
							return [titleName, dataInfo, musicInfoQTUser, musicInfoiTunes, musicInfoQTMeta, musicNameQTUser];
						}
						
						let tmpName = oldMusicName != "" ? oldMusicName : "untitled"
						let tmpURL = (try! FileManager.default.url(for: .itemReplacementDirectory, in: .userDomainMask, appropriateFor: oldURL, create: true)).appendingPathComponent("\(tmpName).aiff", isDirectory: false)
						
						do {
							try self.saveMusic(AIFFToURL: tmpURL, theSett: &self.exportSettings)
						} catch let anErr as NSError {
							expErr = anErr
							DispatchQueue.main.async(execute: errBlock)
							return
						}
						defer {
							do {
								try FileManager.default.removeItem(at: tmpURL)
							} catch _ { }
						}
						
						let exportMov = AVAsset(url: tmpURL)
						let metadataInfo = generateAVMetadataInfo()
						
						guard let session = AVAssetExportSession(asset:exportMov, presetName: AVAssetExportPresetAppleM4A) else {
							expErr = NSError(domain: NSCocoaErrorDomain, code: NSFileWriteUnknownError, userInfo: nil)
							NSLog("Init Failed for %@, error: %@", oldMusicName, expErr!.localizedDescription)
							DispatchQueue.main.async(execute: errBlock)
							return;
						}
						do {
							try FileManager.default.removeItem(at: saveURL)
						} catch _ { }
						session.outputURL = saveURL
						session.outputFileType = AVFileTypeAppleM4A
						session.metadata = metadataInfo
						let sessionWaitSemaphore = DispatchSemaphore(value: 0)
						session.exportAsynchronously(completionHandler: { () -> Void in
							_ = sessionWaitSemaphore.signal()
						})
						_=sessionWaitSemaphore.wait(timeout: DispatchTime.distantFuture)
						
						let didFinish = session.status == .completed
						self.madDriver.endExport()
						
						if didFinish {
							DispatchQueue.main.async() {
								if (self.isQuitting) {
									NSApplication.shared().reply(toApplicationShouldTerminate: true)
								} else {
									let retVal = PPRunInformationalAlertPanel(title: "Export complete", message: "The export of the file \"\(savePanel.url!.lastPathComponent)\" is complete.", defaultButton: "OK", alternateButton: "Show File");
									if (retVal == NSAlertSecondButtonReturn) {
										NSWorkspace.shared().activateFileViewerSelecting([savePanel.url!])
									}
								}
							}
						} else {
							NSLog("\(session.error)");
							if self.isQuitting {
								NSApplication.shared().reply(toApplicationShouldTerminate: true)
							}
						}
					}
				})
			})
			
		case -3: // wave
			savePanel.allowedFileTypes = [AVFileTypeWAVE]
			savePanel.title = "Export as Wave Audio"
			savePanel.beginSheetModal(for: window, completionHandler: { (result) -> Void in
				if (result != NSFileHandlingPanelOKButton) {
					self.madDriver.endExport()
					return;
				}
				self.beginExportSettings(with: { (result) -> Void in
					guard result == NSAlertFirstButtonReturn else {
						self.madDriver.endExport()
						if (self.isQuitting) {
							NSApplication.shared().reply(toApplicationShouldTerminate: true)
						}
						return;
					}
					
					DispatchQueue.global().async() {
						autoreleasepool {
							do {
								try self.saveMusic(waveToURL: savePanel.url!, theSett: &self.exportSettings);
								self.madDriver.endExport()
								DispatchQueue.main.async() {
									if self.isQuitting {
										NSApplication.shared().reply(toApplicationShouldTerminate: true)
									} else {
										let retVal = PPRunInformationalAlertPanel(title: "Export complete", message: "The export of the file \"\(savePanel.url!.lastPathComponent)\" is complete.", defaultButton: "OK", alternateButton: "Show File");
										if (retVal == NSAlertSecondButtonReturn) {
											NSWorkspace.shared().activateFileViewerSelecting([savePanel.url!])
										}
									}
								}
							} catch let error as NSError {
								if (self.isQuitting) {
									NSApplication.shared().reply(toApplicationShouldTerminate: true)
								} else {
									DispatchQueue.main.async() {
										PPRunAlertPanel(title: "Export failed", message: "Export/coversion of the music file failed:\n\(error.localizedDescription)")
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
					NSApplication.shared().reply(toApplicationShouldTerminate: true)
				}
				
				return;
			}
			savePanel.allowedFileTypes = madLib[tag].UTITypes
			savePanel.title = "Export as \(madLib[tag].menuName)"
			
			savePanel.beginSheetModal(for: self.window, completionHandler: {(result) -> Void in
				defer {
					self.madDriver.endExport()
				}
				if (result != NSFileHandlingPanelOKButton) {
					return;
				}
				
				let fileURL = savePanel.url!
				do {
					try self.music!.exportMusic(to: fileURL, format: self.madLib[tag].type, library: self.madLib)
					self.addMusicToMusicList(fileURL, loadIfPreferencesAllow: false)
					if (self.isQuitting) {
						NSApplication.shared().reply(toApplicationShouldTerminate: true)
					} else {
						let retVal = PPRunInformationalAlertPanel(title: "Export complete", message: "The export of the file \"\(savePanel.url!.lastPathComponent)\" is complete.", defaultButton: "OK", alternateButton: "Show File");
						if (retVal == NSAlertSecondButtonReturn) {
							NSWorkspace.shared().activateFileViewerSelecting([fileURL])
						}
					}
				} catch let error as NSError {
					if (self.isQuitting) {
						NSApp.reply(toApplicationShouldTerminate: true)
					} else {
						NSAlert(error: error).runModal()
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
	
	@IBAction func toggleInfo(_ sender: AnyObject?) {
		infoDrawer.toggle(sender)
	}
	
	@IBAction func sortMusicList(_ sender: AnyObject?) {
		willChangeValue(forKey: kMusicListKVO)
		musicList.sortMusicListByName()
		didChangeValue(forKey: kMusicListKVO)
		musicListContentsDidMove()
	}
	
	@IBAction func playSelectedMusic(_ sender: AnyObject?) {
		self.currentlyPlayingIndex.index = tableView.selectedRow;
		do {
			try loadMusicFromCurrentlyPlayingIndex()
		} catch let err as NSError {
			NSAlert(error: err).runModal()
		}
	}
	
	@IBAction func removeSelectedMusic(_ sender: AnyObject?) {
		let selMusic = tableView.selectedRowIndexes
		willChangeValue(forKey: kMusicListKVO)
		musicList.remove(at: selMusic)
		didChangeValue(forKey: kMusicListKVO)
		musicListContentsDidMove()
	}
	
	@IBAction func addMusic(_ sender: AnyObject?) {
		let panel = NSOpenPanel()
		
		if let av = OpenPanelViewController(openPanel: panel, trackerDictionary: trackerDict) {
			av.setupDefaults()
			av.allowsMultipleSelectionOfTrackers = true
			av.beginOpenPanel(self.window, completionHandler: { (result) -> Void in
				if (result == NSFileHandlingPanelOKButton) {
					self.addMusics(toMusicList: panel.urls )
				}
			})
		} else {
			NSBeep()
		}
	}
	
	override func validateToolbarItem(_ theItem: NSToolbarItem) -> Bool {
		let theTag = MusicToolbarTypes(rawValue: theItem.tag) ?? .Unknown
		switch theTag {
		case .Sort, .AddMusic:
			return true
			
		case .FileInfo:
			if infoDrawer.state == Int(NSDrawerState.openingState.rawValue) || infoDrawer.state == Int(NSDrawerState.openState.rawValue) {
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
	
	func tableViewSelectionDidChange(_ notification: Notification) {
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
		
		if selected.count > 0 {
			musicList.selectedMusic = selected.first!
		}
		
		guard selected.count == 1 else {
			badTracker()
			return
		}
		
		let obj = musicList.objectInMusicList(at: selected.last!)
		let musicURL = obj.musicURL
		var aPPInfo: PPLibrary.MusicFileInfo? = nil
		
		func badValues() {
			do {
				let strType = try madLib.identifyFile(at: musicURL)
				aPPInfo = try madLib.information(from: musicURL, type: strType)
			} catch {
				aPPInfo = nil
			}
		}
		
		do {
			let fileUTI = try NSWorkspace.shared().type(ofFile: musicURL.path)
			if let info = madLib.typeFromUTI(fileUTI) {
				aPPInfo = try madLib.information(from: musicURL, type: info)
			} else {
				badValues()
			}
		} catch _ {
			badValues()
		}
		
		if let actualInfo = aPPInfo {
			fileName.stringValue = obj.fileName
			internalName.stringValue = actualInfo.internalFileName
			fileSize.objectValue = NSNumber(value: obj.fileSize)
			musicInstrument.integerValue = actualInfo.totalInstruments
			musicPatterns.integerValue = actualInfo.totalPatterns
			musicPlugType.stringValue = actualInfo.formatDescription
			musicSignature.stringValue = actualInfo.signature
			fileLocation.stringValue = musicURL.absoluteURL.path
		} else {
			badTracker()
		}
	}
	
	func tableView(_ tableView: NSTableView, acceptDrop info: NSDraggingInfo, row: Int, dropOperation: NSTableViewDropOperation) -> Bool {
		let dragPB = info.draggingPasteboard()
		if let tmpArray = dragPB.readObjects(forClasses: [MusicListDragClass.self], options: nil) , tmpArray.count != 0 {
			var minRow = 0;
			let dragClass = (tmpArray[0]) as! MusicListDragClass
			let dragIndexSet = dragClass.theIndexSet;
			
			for currentIndex in dragIndexSet {
				if currentIndex <= row {
					minRow += 1;
				}
			}
			
			changeValueForMusicListKey({
				let selArray = musicList.objectsInMusicList(at: dragIndexSet)
				musicList.remove(at: dragIndexSet)
				musicList.insert(selArray, at: row - minRow)
			})
			musicListContentsDidMove()
			return true;
		} else if let tmpArray = dragPB.readObjects(forClasses: [NSURL.self], options:[NSPasteboardURLReadingFileURLsOnlyKey : true,
			NSPasteboardURLReadingContentsConformToTypesKey : self.trackerUTIs]) {
				if (tmpArray.count < 1) {
					return false;
				}
				
				changeValueForMusicListKey({
					var mutArray = [MusicListObject]()
					for curURL in tmpArray as! [URL] {
						mutArray.append(MusicListObject(url: curURL))
					}
					self.musicList.insert(mutArray, at: row)
				})
				musicListContentsDidMove()
				return true;
		}
		
		return false;
	}
	
	func tableView(_ tableView1: NSTableView, validateDrop info: NSDraggingInfo, proposedRow row: Int, proposedDropOperation dropOperation: NSTableViewDropOperation) -> NSDragOperation {
		var result: NSDragOperation = [];
		
		if (info.draggingSource() as AnyObject? === tableView1) {
			result = .move;
			//TODO: check for number of indexes that are greater than the drop row.
			tableView1.setDropRow(row, dropOperation: .above)
		} else {
			let pb = info.draggingPasteboard();
			
			//list the file type UTIs we want to accept
			let acceptedTypes = trackerUTIs;
			
			if let urls = pb.readObjects(forClasses: [NSURL.self], options: [NSPasteboardURLReadingFileURLsOnlyKey : true,
				NSPasteboardURLReadingContentsConformToTypesKey : acceptedTypes]) {
					if urls.count > 0 {
						result = .copy
						tableView1.setDropRow(row, dropOperation: .above)
					}
			}
		}
		
		return result;
	}
	
	func tableView(_ tableView: NSTableView, toolTipFor cell: NSCell, rect: NSRectPointer, tableColumn: NSTableColumn?, row: Int, mouseLocation: NSPoint) -> String {
		if (row >= 0 || row <= musicList.countOfMusicList) {
			return ""
		}
		return musicList.url(at: row).path
	}
	
	func tableView(_ tableView: NSTableView, writeRowsWith rowIndexes: IndexSet, to pboard: NSPasteboard) -> Bool {
		var status = false;
		let dragClass = MusicListDragClass(indexSet: rowIndexes)
		let ppmobjects = musicList.objectsInMusicList(at: rowIndexes)
		let urlArrays = ppmobjects.map({ $0.musicURL })
		let tmpObjs: [NSPasteboardWriting] = [dragClass] + (urlArrays as [NSPasteboardWriting])
		pboard.clearContents(); // clear pasteboard to take ownership
		status = pboard.writeObjects(tmpObjs) // write the URLs
		return status;
	}
	
	func tableView(_ atableView: NSTableView, sortDescriptorsDidChange oldDescriptors: [NSSortDescriptor]) {
		changeValueForMusicListKey({
			self.musicList.sortMusicList(descriptors: atableView.sortDescriptors)
		})
		atableView.reloadData()
		musicListContentsDidMove()
	}
}

//MARK: - PPSoundSettingsViewController delegate methods
extension PlayerAppDelegate: SoundSettingsViewControllerDelegate {
	func soundView(_ view: SoundSettingsViewController, bitsDidChange bits: Int16) {
		exportSettings.outPutBits = bits;
	}
	
	func soundView(_ view: SoundSettingsViewController, rateDidChange rat: UInt32) {
		exportSettings.outPutRate = rat;
	}
	
	func soundView(_ view: SoundSettingsViewController, reverbDidChangeActive isAct: Bool) {
		exportSettings.Reverb = isAct;
	}
	
	func soundView(_ view: SoundSettingsViewController, oversamplingDidChangeActive isAct: Bool) {
		if (!isAct) {
			exportSettings.oversampling = 1;
		}
	}
	
	func soundView(_ view: SoundSettingsViewController, stereoDelayDidChangeActive isAct: Bool) {
		if (!isAct) {
			exportSettings.MicroDelaySize = 0;
		}
	}
	
	func soundView(_ view: SoundSettingsViewController, surroundDidChangeActive isAct: Bool) {
		exportSettings.surround = isAct;
	}
	
	func soundView(_ view: SoundSettingsViewController, reverbStrengthDidChange rev: Int32) {
		exportSettings.ReverbStrength = rev;
	}
	
	func soundView(_ view: SoundSettingsViewController, reverbSizeDidChange rev: Int32) {
		exportSettings.ReverbSize = rev;
	}
	
	func soundView(_ view: SoundSettingsViewController, oversamplingAmountDidChange ovs: Int32) {
		exportSettings.oversampling = ovs;
	}
	
	func soundView(_ view: SoundSettingsViewController, stereoDelayAmountDidChange std: Int32) {
		exportSettings.MicroDelaySize = std;
	}
}
