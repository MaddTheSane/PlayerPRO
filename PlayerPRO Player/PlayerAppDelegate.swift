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
	@objc dynamic var music: PPMusicObject?
	
	@IBOutlet var toolsPanel: NSPanel!
	var timeChecker: Timer!
	var madDriver: PPDriver!
	let madLib: PPLibrary = {
		return try! PPLibrary()
	}()
	
	@objc dynamic var paused: Bool = true {
		didSet {
			if paused {
				pauseButton.state = .on
				pauseDockMenuItem.state = .off
			} else {
				pauseButton.state = .off
				pauseDockMenuItem.state = .on
			}
		}
	}
	private var isQuitting = false
	var instrumentController: InstrumentWindowController!
	
	var exportSettings = MADDriverSettings.new()
	@IBOutlet var exportSettingsBox: NSBox!
	var exportController: SoundSettingsViewController!
	
	@objc dynamic let musicList = MusicList()
	let preferences = Preferences.newPreferenceController()
	var plugInInfos = [PlugInInfo]()
	
	private var selectedIndex	= CurrentlyPlayingIndex()
	private var playingIndex	= CurrentlyPlayingIndex()
	
	@objc private dynamic var musicName = ""
	@objc private dynamic var musicInfo = ""
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
		let anArray = self.trackerDict.values.flatMap({$0})
		return Array(anArray)
		}()
	
	
	//MARK: -
	
	private func musicListWillChange() -> Bool {
		return true
	}
	
	func musicListDidChange() {
		if selectedIndex.index != -1 {
			madDriver.stop()
			madDriver.cleanDriver()
		}
		if UserDefaults.standard.bool(forKey: PPLoadMusicAtListLoad) && musicList.countOfMusicList > 0 {
			selectedIndex.index = selMusFromList != -1 ? selMusFromList : 0
			selectCurrentlyPlayingMusic()
			do {
				try loadMusicFromCurrentlyPlayingIndex()
			} catch let err {
				NSAlert(error: err).runModal()
			}
		} else if selMusFromList != -1 {
			selectMusic(at: selMusFromList)
		}
		let lostCount = musicList.lostMusicCount
		if lostCount != 0 {
			let uresolvedStr = String(format: kUnresolvableFileDescription, lostCount)
			PPRunAlertPanel(title: kUnresolvableFile, message: uresolvedStr)
		}
	}
	
	private func changeValueForMusicListKey(_ theClosure: () -> Void) {
		willChangeValue(forKey: kMusicListKVO)
		theClosure()
		didChangeValue(forKey: kMusicListKVO)
	}
	
	private func musicListContentsDidMove() {
		if selectedIndex.index != -1 {
			for i in 0..<musicList.countOfMusicList {
				if (selectedIndex.playbackURL == musicList[i].musicURL) {
					selectedIndex.index = i
					break
				}
			}
		}
		if playingIndex.index != -1 {
			for i in 0..<musicList.countOfMusicList {
				if (playingIndex.playbackURL == musicList[i].musicURL) {
					playingIndex.index = i
					break
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
		
		UserDefaults.standard.register(defaults: ourDefaults)
		super.init()
	}
	
	@objc private func updateMusicStats(_ theTimer: Timer?) {
		if music != nil {
			var time = madDriver.musicStatusTime!
			if madDriver.isDonePlayingMusic && paused == false && madDriver.isExporting == false {
				songIsDonePlaying()
				time = madDriver.musicStatusTime!
			}
			songPos.doubleValue = Double(time.current)
			songCurTime.integerValue = time.current
		}
	}
	
	private func selectCurrentlyPlayingMusic() {
		if (self.selectedIndex.index >= 0) {
			//currentlyPlayingIndex.playbackURL = [musicList URLAtIndex:currentlyPlayingIndex.index];
			selectMusic(at: selectedIndex.index)
		}
	}
	
	func addMusicToMusicList(_ theURL: URL, loadIfPreferencesAllow load: Bool = true) {
		// clear sort descriptors, so added files don't get sorted incorrectly.
		tableView.sortDescriptors = []
		willChangeValue(forKey: kMusicListKVO)
		let okayMusic = musicList.addMusicURL(theURL, force: false)
		didChangeValue(forKey: kMusicListKVO)
		switch okayMusic {
		case .failure:
			return
			
		case .similarURL:
			let similarMusicIndex = musicList.indexOfObjectSimilar(to: theURL)!
			let similarAlert = NSAlert()
			similarAlert.messageText = "Existing object"
			similarAlert.informativeText = "There is already a tracker file that points to the added file. Do you still wish to add it?"
			similarAlert.addButton(withTitle: "Add")
			
			do {
				let cancelButton = similarAlert.addButton(withTitle: "Cancel")
				cancelButton.keyEquivalent = "\u{1b}" //escape
			}
			
			similarAlert.addButton(withTitle: "Load Existing")
			
			let alertSelect = similarAlert.runModal()
			
			switch alertSelect {
			case .alertFirstButtonReturn:
				_ = musicList.addMusicURL(theURL, force: true)
				if load && UserDefaults.standard.bool(forKey: PPLoadMusicAtMusicLoad) {
					self.selectedIndex.index = musicList.countOfMusicList - 1
					//currentlyPlayingIndex.playbackURL = [musicList URLAtIndex:currentlyPlayingIndex.index];
					selectCurrentlyPlayingMusic()
					do {
						try loadMusicFromCurrentlyPlayingIndex()
					} catch let err {
						NSApp.presentError(err)
					}
				} else {
					selectMusic(at: musicList.countOfMusicList - 1)
				}
				
			case .alertSecondButtonReturn:
				return
				
			case .alertThirdButtonReturn:
				if load && UserDefaults.standard.bool(forKey: PPLoadMusicAtMusicLoad) {
					self.selectedIndex.index = similarMusicIndex
					selectCurrentlyPlayingMusic()
					do {
						try loadMusicFromCurrentlyPlayingIndex()
					} catch let err {
						NSApp.presentError(err)
					}
				} else {
					selectMusic(at: similarMusicIndex)
				}
				
			default:
				break
			}
			
		case .success:
			if load && UserDefaults.standard.bool(forKey: PPLoadMusicAtMusicLoad) {
				self.selectedIndex.index = musicList.countOfMusicList - 1
				//currentlyPlayingIndex.playbackURL = [musicList URLAtIndex:currentlyPlayingIndex.index];
				selectCurrentlyPlayingMusic()
				do {
					try loadMusicFromCurrentlyPlayingIndex()
				} catch let err {
					NSApp.presentError(err)
				}
			} else {
				selectMusic(at: musicList.countOfMusicList - 1)
			}
		}
	}
	
	private func songIsDonePlaying() {
		let userDefaults = UserDefaults.standard
		switch PlaylistMode(rawValue: userDefaults.integer(forKey: PPAfterPlayingMusic))! {
			
		case .LoopMusic:
			madDriver.musicPosition = 0
			madDriver.play()
			
		case .LoadNext:
			self.selectedIndex.index += 1
			if musicList.countOfMusicList > self.selectedIndex.index {
				selectCurrentlyPlayingMusic()
				do {
					try loadMusicFromCurrentlyPlayingIndex()
				} catch let err {
					NSApp.presentError(err)
				}
			} else {
				if (userDefaults.bool(forKey: PPLoopMusicWhenDone)) {
					selectedIndex.index = 0
					selectCurrentlyPlayingMusic()
					do {
						try loadMusicFromCurrentlyPlayingIndex()
					} catch let err {
						NSApp.presentError(err)
					}
				} else {
					madDriver.stop()
					madDriver.cleanDriver()
					if (userDefaults.bool(forKey: PPGotoStartupAfterPlaying)) {
						madDriver.musicPosition = 0
					}
				}
			}
			
		case .LoadRandom:
			self.selectedIndex.index = Int(arc4random_uniform(UInt32(musicList.countOfMusicList)))
			selectCurrentlyPlayingMusic()
			do {
				try loadMusicFromCurrentlyPlayingIndex()
			} catch let err {
				NSApp.presentError(err)
			}
			
			//case .StopPlaying:
			//	fallthrough
		default:
			madDriver.stop()
			madDriver.cleanDriver()
			if userDefaults.bool(forKey: PPGotoStartupAfterPlaying) {
				madDriver.musicPosition = 0
			}
			self.paused = true
		}
	}
	
	@objc(loadMusicFromCurrentlyPlayingIndexAndReturnError:)
	func loadMusicFromCurrentlyPlayingIndex() throws {
		selectedIndex.playbackURL = musicList.url(at: selectedIndex.index)
		try loadMusic(at: selectedIndex.playbackURL!)
		playingIndex = selectedIndex
	}
	
	@objc private func doubleClickMusicList() {
		self.selectedIndex.index = tableView.selectedRow
		do {
			try loadMusicFromCurrentlyPlayingIndex()
		} catch {
			NSAlert(error: error).runModal()
		}
	}
	
	private func addMusics(toMusicList theMusList: [URL]) {
		for i in 0 ..< (theMusList.count - 1) {
			addMusicToMusicList(theMusList[i], loadIfPreferencesAllow: false)
		}
		addMusicToMusicList(theMusList.last!, loadIfPreferencesAllow: true)
	}
	
	func loadMusic(at musicToLoad: URL, autoPlay: Bool = true) throws {
		if music != nil {
			madDriver.stop()
			madDriver.stopDriver()
		}
		
		do {
			var fileType: String? = nil
			
			do {
				let fileUTI = try NSWorkspace.shared.type(ofFile: musicToLoad.path)
				if let afileType = madLib.typeFromUTI(fileUTI) {
					try madLib.testFile(at: musicToLoad, as: afileType)
					fileType = afileType
				}
			} catch _ {}
			
			if fileType == nil {
				fileType = try madLib.identifyFile(at: musicToLoad)
			}
			
			self.music = try PPMusicObject(url: musicToLoad, stringType: fileType!, library: madLib)
			
			music!.attach(to: madDriver)
			
			if autoPlay {
				madDriver.play()
				
				paused = false
			}
			
			let (_, timeTotal) = madDriver.musicStatusTime!
			songPos.maxValue = Double(timeTotal)
			songPos.minValue = 0
			setTitleForSongLabelBasedOnMusic()
			songTotalTime.integerValue = timeTotal
			
			NotificationCenter.default.post(name: NSNotification.Name.PPMusicDidChange, object: self)
		} catch {
			self.paused = true
			clearMusic()
			throw error
		}
	}
	
	private func clearMusic() {
		if music != nil {
			madDriver.stop()
			madDriver.cleanDriver()
		}
		
		self.paused = true
		selectedIndex.index = -1
		selectedIndex.playbackURL = nil
		playingIndex = selectedIndex
		self.music = PPMusicObject()
		setTitleForSongLabelBasedOnMusic()
		NotificationCenter.default.post(name: NSNotification.Name.PPMusicDidChange, object:self)
		music!.attach(to: madDriver)
	}
	
	private func setTitleForSongLabelBasedOnMusic() {
		self.musicName = music?.title ?? ""
		self.musicInfo = music?.information ?? ""
	}
	
	private func MADDriverWithPreferences() {
		var madWasReading = false
		var fullTime = 0, curTime = 0
		if madDriver != nil {
			madWasReading = !madDriver.isPaused
			madDriver.getMusicStatusTime(current: &curTime, total: &fullTime)
			
			madDriver.stopDriver()
		}
		var theSettinit = MADDriverSettings.new()
		let defaults = UserDefaults.standard
		
		//TODO: Sanity Checking
		theSettinit.surround = defaults[PPSurroundToggle] ?? false
		theSettinit.outPutRate = UInt32(defaults.integer(forKey: PPSoundOutRate))
		theSettinit.outPutBits = Int16(defaults.integer(forKey: PPSoundOutBits))
		if defaults[PPOversamplingToggle] ?? false {
			theSettinit.oversampling = Int32(defaults.integer(forKey: PPOversamplingAmount))
		} else {
			theSettinit.oversampling = 1
		}
		theSettinit.Reverb = defaults[PPReverbToggle] ?? false
		theSettinit.ReverbSize = Int32(defaults.integer(forKey: PPReverbAmount))
		theSettinit.ReverbStrength = Int32(defaults.integer(forKey: PPReverbStrength))
		if defaults[PPStereoDelayToggle] ?? false {
			theSettinit.MicroDelaySize = Int32(defaults.integer(forKey: PPStereoDelayAmount))
		} else {
			theSettinit.MicroDelaySize = 0
		}
		
		if let i16 = Int16(exactly: defaults.integer(forKey: PPSoundDriver)),
			let driver = MADSoundOutput(rawValue: i16) {
			theSettinit.driverMode = driver
		} else {
			theSettinit.driverMode = .CoreAudioDriver
		}
		theSettinit.repeatMusic = false
		
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
				alert.informativeText = String(format: NSLocalizedString("Unable to create the PlayerPRO sound driver due to error type %ld. Sound preferences will be reset.\n\nPlayerPRO will now close.", comment: "Fatal sound driver error"), error.code)
				alert.alertStyle = .critical
				alert.runModal()
				
				print("Could not create initial driver, error \(error)")
				exit(EXIT_FAILURE)
			}
		} else {
			do {
				try madDriver.changeDriverSettings(to: &theSettinit)
			} catch {
				NotificationCenter.default.post(name: .PPDriverDidChange, object: self)
				NSApp.presentError(error)
				return
			}
		}
		NotificationCenter.default.post(name: .PPDriverDidChange, object: self)
		//MADStartDriver(madDriver);
		if self.music != nil {
			//MADAttachDriverToMusic(madDriver, music, NULL);
			if madWasReading {
				madDriver.setMusicStatusTime(current: curTime, maximum: fullTime, minimum: 0)
				madDriver.play()
			}
		}
	}
	
	private func updatePlugInInfoMenu() {
		let trackerPlugName = NSLocalizedString("TrackerPlugName", comment: "Tracker plug-in name")
		plugInInfos.removeAll(keepingCapacity: true)
		
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
			NSApplication.shared.runModal(for: infoWindow)
		}
	}
	
	private func selectMusic(at anIdx: Int) {
		guard anIdx >= 0 && anIdx < musicList.countOfMusicList else {
			NSSound.beep()
			return
		}
		let idx = IndexSet(integer: anIdx)
		tableView.selectRowIndexes(idx, byExtendingSelection: false)
		tableView.scrollRowToVisible(anIdx)
	}
	
	@IBAction func clearMusicList(_ sender: AnyObject?) {
		if musicList.countOfMusicList != 0 {
			let clearMusicListStr = String(format: NSLocalizedString("The music list contains %ld items. Do you really want to remove them?", comment: "Clear Music List?"), musicList.countOfMusicList)
			
			let returnVal = PPRunAlertPanel(title: NSLocalizedString("Clear list", comment: "Clear Music List"), message: clearMusicListStr, defaultButton: NSLocalizedString("No", comment: "No"), alternateButton: NSLocalizedString("Yes", comment: "Yes"))
			
			if returnVal == NSApplication.ModalResponse.alertSecondButtonReturn {
				changeValueForMusicListKey( {
					self.musicList.clearMusicList()
				})
				if self.selectedIndex.index != -1 {
					clearMusic()
				}
			}
		} else {
			NSSound.beep()
		}
	}
	
	@IBAction func saveMusicList(_ sender: AnyObject!) {
		let savePanel = NSSavePanel()
		savePanel.allowedFileTypes = [PPMusicListUTI]
		savePanel.canCreateDirectories = true
		savePanel.canSelectHiddenExtension = true
		savePanel.beginSheetModal(for: self.window, completionHandler: { (result) -> Void in
			if result.rawValue == NSFileHandlingPanelOKButton {
				self.musicList.saveMusicList(to: savePanel.url!)
			}
		})
	}
	
	@IBAction func showInstrumentsList(_ sender: AnyObject?) {
		instrumentController!.showWindow(sender)
	}
	
	func handleFile(_ theURL1: URL, ofType theUTI: String) -> Bool {
		let sharedWorkspace = NSWorkspace.shared
		var theURL = theURL1
		if theUTI == MADGenericUTI {
			let invExt = NSLocalizedString("Invalid Extension", comment: "Invalid Extension")
			let invExtDes = String(format: NSLocalizedString("The file %@ is identified as as a generic MAD tracker, and not a specific one. Renaming it will fix this. Do you want to rename the file extension?", comment: "Invalid extension description"), theURL.lastPathComponent)
			let renameFile =  NSLocalizedString("Rename", comment: "rename file")
			let openFile = NSLocalizedString("Open", comment: "Open a file")
			let cancelOp = NSLocalizedString("Cancel", comment: "Cancel")
			let retVal = PPRunInformationalAlertPanel(title: invExt, message:invExtDes, defaultButton: renameFile, alternateButton:openFile, otherButton: cancelOp)
			switch retVal {
			case NSApplication.ModalResponse.alertFirstButtonReturn:
				
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
						PPRunInformationalAlertPanel(title: NSLocalizedString("Rename Error", comment: "Rename Error"), message: couldNotRenameStr)
					}
					
				} catch {
					PPRunCriticalAlertPanel(title: NSLocalizedString("Unknown File", comment: "unknown file"), message: NSLocalizedString("The file type could not be identified.", comment: "Unidentified file"))
					return false
				}
				
			case NSApplication.ModalResponse.alertSecondButtonReturn:
				break
				
				//case NSAlertThirdButtonReturn:
			default:
				return true
			}
		}
		if sharedWorkspace.type(theUTI, conformsToType: PPMusicListUTI) {
			if musicListWillChange() {
				changeValueForMusicListKey({ () -> Void in
					self.musicList.loadMusicList(from: theURL)
					self.selMusFromList = self.musicList.selectedMusic
				})
				musicListDidChange()
				return true
			}
		} else if sharedWorkspace.type(theUTI, conformsToType: PPOldMusicListUTI) {
			if musicListWillChange() {
				self.willChangeValue(forKey: kMusicListKVO)
				musicList.beginLoadingOfOldMusicList(at: theURL, completionHandle: { (theErr) -> Void in
					self.selMusFromList = self.musicList.selectedMusic
					self.didChangeValue(forKey: kMusicListKVO)
					if let theErr = theErr {
						NSAlert(error: theErr).runModal()
					}
				})
				return true
			}
		} else {
			//TODO: check for valid extension.
			for aUTI in trackerUTIs {
				if sharedWorkspace.type(theUTI, conformsToType: aUTI) {
					addMusicToMusicList(theURL)
					return true
				}
			}
			
			// Final check, just in case the UTI is messed up...
			// Yes, it does happen.
			do {
				let aType = try madLib.identifyFile(at: theURL)
				try madLib.testFile(at: theURL, as: aType)
				addMusicToMusicList(theURL)
				return true
			} catch _ {
				
			}
		}
		return false
	}
	
	@IBAction func openFile(_ sender: AnyObject?) {
		let panel = NSOpenPanel()
		let playlistDict = ["PlayerPRO Music List" : [PPMusicListUTI],
			"PlayerPRO Old Music List" : [PPOldMusicListUTI]]
		
		if let av = OpenPanelViewController(openPanel: panel, trackerDictionary: trackerDict, playlistDictionary: playlistDict) {
			av.setupDefaults()
			av.allowsMultipleSelectionOfTrackers = true
			av.beginOpenPanel(window, completionHandler: { (result) -> Void in
				if result.rawValue != NSFileHandlingPanelOKButton {
					return
				}
				let ws = NSWorkspace.shared
				let panelURLs = panel.urls
				for theURL in panelURLs {
					let fileName = theURL.path
					do {
						let utiFile = try ws.type(ofFile: fileName)
						_=self.handleFile(theURL, ofType: utiFile) //TODO: more efficient way of doing this!
					} catch let error as NSError {
						PPRunAlertPanel(title: "Error opening file", message: "Unable to open \((fileName as NSString).lastPathComponent): \(error.localizedFailureReason ?? "Unknown error")")
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
			try music!.saveMusic(to: tosave, compress: false)
		} catch {
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
			if result.rawValue == NSFileHandlingPanelOKButton {
				let saveURL = savePanel.url!
				self.saveMusic(to: saveURL)
				self.addMusicToMusicList(saveURL, loadIfPreferencesAllow: false)
			}
			self.madDriver.endExport()
		}
	}
	
	@IBAction func saveMusic(_ sender: AnyObject) {
		madDriver.beginExport()
		
		if playingIndex.index == -1 {
			// saveMusicAs: will end the exporting when it is done.
			saveMusicAs(sender)
		} else {
			let fileURL = playingIndex.playbackURL!
			let filename = fileURL.path
			let sharedWorkspace = NSWorkspace.shared
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
		
		tableView.registerForDraggedTypes([.PPMLDCUTI, NSPasteboard.PasteboardType(rawValue: kUTTypeFileURL as String)])
		//tableView.register(forDraggedTypes: [PPMLDCUTI, kUTTypeFileURL as String]);
		//self.paused = YES;
		willChangeValue(forKey: kMusicListKVO)
		if UserDefaults.standard.bool(forKey: PPRememberMusicList) {
			musicList.loadApplicationMusicList()
		}
		let selMus = musicList.selectedMusic
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
		exportController.delegate = self
		exportSettingsBox.contentView = exportController.view
		
		instrumentController = InstrumentWindowController.newInstrumentWindow()
		
		timeChecker = Timer(fireAt: Date(), interval: 1.0/8.0, target: self, selector: #selector(PlayerAppDelegate.updateMusicStats(_:)), userInfo: nil, repeats: true)
		RunLoop.main.add(timeChecker, forMode: RunLoopMode.commonModes)
		let lostCount = musicList.lostMusicCount
		
		if lostCount != 0 {
			let unresolvable = String(format: kUnresolvableFileDescription, lostCount)
			PPRunAlertPanel(title: kUnresolvableFile, message: unresolvable)
		}
		
		if selMus != -1 && selMus < musicList.countOfMusicList {
			selectMusic(at: selMus)
			do {
				try loadMusic(at: musicList.url(at: selMus), autoPlay: false)
				selectedIndex.index = selMus
				selectedIndex.playbackURL = musicList.url(at: selMus)
				playingIndex = selectedIndex
			} catch let error1 {
				NSApp.presentError(error1)
			}
		} else {
			self.music = PPMusicObject()
			music!.attach(to: madDriver)
			setTitleForSongLabelBasedOnMusic()
			musicList.selectedMusic = -1
		}
	}
	
	func applicationShouldTerminate(_ sender: NSApplication) -> NSApplication.TerminateReply {
		if madDriver.isExporting {
			let anAlert = NSAlert()
			//TODO: localize
			anAlert.messageText = "Exporting"
			anAlert.informativeText = "PlayerPRO is currently exporting a tracker file.\nQuitting will stop this. Do you really wish to quit?"
			anAlert.addButton(withTitle: "Wait")
			anAlert.addButton(withTitle: "Quit")
			anAlert.addButton(withTitle: "Cancel")
			switch anAlert.runModal() {
			case NSApplication.ModalResponse.alertFirstButtonReturn:
				//Double-check to make sure we're still exporting
				if madDriver.isExporting {
					isQuitting = true
					return .terminateLater
				} else {
					return .terminateNow
				}
				
			case NSApplication.ModalResponse.alertSecondButtonReturn:
				return .terminateNow
				
			case NSApplication.ModalResponse.alertThirdButtonReturn:
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
		musicList.selectedMusic = selectedIndex.index
		if UserDefaults.standard.bool(forKey: PPRememberMusicList) {
			musicList.saveApplicationMusicList()
		}
		
		NotificationCenter.default.removeObserver(self)
	}
	
	func application(_ sender: NSApplication, openFile filename: String) -> Bool {
		do {
			let utiFile = try NSWorkspace.shared.type(ofFile: filename)
			return handleFile(URL(fileURLWithPath: filename), ofType: utiFile)
		} catch let err as NSError {
			let tmpAlert = NSAlert()
			tmpAlert.alertStyle = .warning
			tmpAlert.messageText = "Error opening file"
			tmpAlert.informativeText = "Unable to identify \((filename as NSString).lastPathComponent): \(err.localizedFailureReason ?? "Unknown Error")"
			tmpAlert.runModal()
			return false
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
		if self.selectedIndex.index + 1 < musicList.countOfMusicList {
			selectedIndex.index += 1
			selectCurrentlyPlayingMusic()
			do {
				try loadMusicFromCurrentlyPlayingIndex()
			} catch let err {
				NSAlert(error: err).runModal()
			}
		} else if UserDefaults.standard.bool(forKey: PPLoopMusicWhenDone) {
			selectedIndex.index = 0
			selectCurrentlyPlayingMusic()
			do {
				try loadMusicFromCurrentlyPlayingIndex()
			} catch let err {
				NSAlert(error: err).runModal()
			}
		} else {
			NSSound.beep()
		}
	}
	
	@IBAction func playButtonPressed(_ sender: AnyObject!) {
		if self.music != nil {
			madDriver.play()
			paused = false
		}
	}
	
	@IBAction func stopButtonPressed(_ sender: AnyObject!) {
		if self.music != nil {
			madDriver.stop()
			madDriver.cleanDriver()
			madDriver.setMusicStatusTime(current: 0, maximum: 100, minimum: 0)
			paused = true
		}
	}
	
	@IBAction func pauseButtonPressed(_ sender: AnyObject!) {
		if (self.music != nil) {
			if self.paused {
				madDriver.play()
			} else {
				madDriver.pause()
			}
			paused = !paused
		}
	}
	
	@IBAction func prevButtonPressed(_ sender: AnyObject!) {
		if selectedIndex.index > 0 {
			selectedIndex.index -= 1
			selectCurrentlyPlayingMusic()
			do {
				try loadMusicFromCurrentlyPlayingIndex()
			} catch let err {
				NSAlert(error: err).runModal()
			}
		} else {
			NSSound.beep()
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
			madDriver.setMusicStatusTime(current: Int(songPos.doubleValue), maximum: Int(songPos.maxValue), minimum: 0)
		}
	}
	
	@IBAction func showTools(_ sender: AnyObject?) {
		toolsPanel.makeKeyAndOrderFront(sender)
	}
	
	// MARK: - Exporting functions
	
	@IBAction func okayExportSettings(_ sender: AnyObject!) {
		window.endSheet(exportWindow, returnCode: NSApplication.ModalResponse.alertFirstButtonReturn)
		exportWindow.close()
	}
	
	@IBAction func cancelExportSettings(_ sender: AnyObject!) {
		window.endSheet(exportWindow, returnCode: NSApplication.ModalResponse.alertSecondButtonReturn)
		exportWindow.close()
	}
	
	private func beginExportSettings(with theHandle: @escaping (NSApplication.ModalResponse) -> Void) {
		exportSettings.resetToBestDriver()
		exportSettings.driverMode = .NoHardwareDriver
		exportSettings.repeatMusic = false
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
		
		switch theSett.outPutMode {
		case .MonoOutPut:
			tmpChannels = 1
			
		case .PolyPhonic:
			tmpChannels = 4
			
		default:
			tmpChannels = 2
		}
		
		var asbd = AudioStreamBasicDescription(sampleRate: Float64(theSett.outPutRate), formatFlags: [.signedInteger, .packed], bitsPerChannel: UInt32(theSett.outPutBits), channelsPerFrame: tmpChannels)
		var realFormat = AudioStreamBasicDescription(sampleRate: Float64(theSett.outPutRate), formatFlags: [.signedInteger, .packed, .nativeEndian], bitsPerChannel: UInt32(theSett.outPutBits), channelsPerFrame: tmpChannels)
		
		audioFile = try ExtAudioFile(create: theURL, fileType: .WAVE, streamDescription: &asbd, flags: .eraseFile)
		
		audioFile.clientDataFormat = realFormat
		
		func handler(data data1: Data) throws {
			try data1.withUnsafeBytes { (toWriteBytes: UnsafePointer<UInt8>) -> Void in
				let toWriteSize = data1.count

				var audBufList = AudioBufferList()
				audBufList.mNumberBuffers = 1
				audBufList.mBuffers.mNumberChannels = tmpChannels
				audBufList.mBuffers.mDataByteSize = UInt32(toWriteSize)
				audBufList.mBuffers.mData = UnsafeMutableRawPointer(mutating: toWriteBytes)
				
				try audioFile.write(frames: UInt32(toWriteSize) / realFormat.mBytesPerFrame, data: &audBufList)
			}
		}
		
		try rawSoundData(&theSett, handler: handler)
		
		applyMetadata(to: audioFile)
	}
	
	private func saveMusic(AIFFToURL theURL: URL, theSett: inout MADDriverSettings) throws {
		let tmpChannels: UInt32
		
		switch theSett.outPutMode {
		case .MonoOutPut:
			tmpChannels = 1
			
		case .PolyPhonic:
			tmpChannels = 4
			
		default:
			tmpChannels = 2
		}
		
		var asbd = AudioStreamBasicDescription(sampleRate: Float64(theSett.outPutRate), formatFlags: [.signedInteger, .packed, .bigEndian], bitsPerChannel: UInt32(theSett.outPutBits), channelsPerFrame: tmpChannels)
		var realFormat = AudioStreamBasicDescription(sampleRate: Float64(theSett.outPutRate), formatFlags: [.signedInteger, .packed, .nativeEndian], bitsPerChannel: UInt32(theSett.outPutBits), channelsPerFrame: tmpChannels)
		
		let audioFile = try ExtAudioFile(create: theURL, fileType: .AIFF, streamDescription: &asbd, flags: .eraseFile)
		
		audioFile.clientDataFormat = realFormat
		
		func handler(data data1: Data) throws {
			try data1.withUnsafeBytes { (toWriteBytes: UnsafePointer<UInt8>) -> Void in
				let toWriteSize = data1.count
				
				var audBufList = AudioBufferList()
				audBufList.mNumberBuffers = 1
				audBufList.mBuffers.mNumberChannels = tmpChannels
				audBufList.mBuffers.mDataByteSize = UInt32(toWriteSize)
				audBufList.mBuffers.mData = UnsafeMutableRawPointer(mutating: toWriteBytes)
				
				try audioFile.write(frames: UInt32(toWriteSize) / realFormat.mBytesPerFrame, data: &audBufList)
			}
		}
		
		try rawSoundData(&theSett, handler: handler)
		
		applyMetadata(to: audioFile)
	}
	
	private func applyMetadata(to theID: ExtAudioFile) {
		//TODO: implement, but how?
	}
	
	@IBAction func exportMusicAs(_ sender: AnyObject!) {
		let tag = (sender as! NSMenuItem).tag
		madDriver.beginExport()
		let savePanel = NSSavePanel()
		savePanel.canCreateDirectories = true
		savePanel.canSelectHiddenExtension = true
		if musicName != "" {
			savePanel.nameFieldStringValue = musicName
		}
		savePanel.prompt = "Export"
		
		switch tag {
		case -1: //AIFF
			savePanel.allowedFileTypes = [AVFileType.aiff.rawValue]
			savePanel.title = "Export as AIFF audio"
			savePanel.beginSheetModal(for: self.window, completionHandler: { (result) -> Void in
				guard result.rawValue == NSFileHandlingPanelOKButton else {
					self.madDriver.endExport()
					return
				}
				
				self.beginExportSettings(with: { (result) -> Void in
					guard result == NSApplication.ModalResponse.alertFirstButtonReturn else {
						self.madDriver.endExport()
						return
					}
					
					DispatchQueue.global().async() {
						do {
							try self.saveMusic(AIFFToURL: savePanel.url!, theSett: &self.exportSettings)
							self.madDriver.endExport()
							DispatchQueue.main.async() {
								if self.isQuitting {
									NSApplication.shared.reply(toApplicationShouldTerminate: true)
								} else {
									let retVal = PPRunInformationalAlertPanel(title: "Export complete", message: "The export of the file \"\(savePanel.url!.lastPathComponent)\" is complete.", defaultButton: "OK", alternateButton: "Show File")
									if retVal == NSApplication.ModalResponse.alertSecondButtonReturn {
										NSWorkspace.shared.activateFileViewerSelecting([savePanel.url!])
									}
								}
							}
						} catch let thErr {
							if self.isQuitting {
								NSApplication.shared.reply(toApplicationShouldTerminate: true)
							} else {
								DispatchQueue.main.async() {
									NSApp.presentError(thErr)
								}
							}
						}
					}
				})
			})
			
		case -2: //MP4
			savePanel.allowedFileTypes = [AVFileType.m4a.rawValue]
			savePanel.title = "Export as MPEG-4 Audio"
			savePanel.beginSheetModal(for: self.window, completionHandler: {(result) -> Void in
				if result.rawValue != NSFileHandlingPanelOKButton {
					self.madDriver.endExport()
					return
				}
				
				self.beginExportSettings(with: { (result) -> Void in
					let saveURL = savePanel.url!
					if result != NSApplication.ModalResponse.alertFirstButtonReturn {
						self.madDriver.endExport()
						return
					}
					
					DispatchQueue.global().async() {
						var theErr = MADErr.noErr
						
						var oldURL = self.musicList.objectInMusicList(at: self.playingIndex.index).musicURL
						var expErr: Error? = nil
						var errBlock: () -> Void = {
							if self.isQuitting {
								NSApplication.shared.reply(toApplicationShouldTerminate: true)
							} else {
								PPRunAlertPanel(title: "Export failed", message: "Export/coversion of the music file failed:\n\(expErr!.localizedDescription)")
							}
						}
						let oldMusicName = self.musicName
						let oldMusicInfo = self.musicInfo
						let metadataInfo: [AVMetadataItem] = {
							var toRet = [AVMetadataItem]()
							
							let titleName = AVMutableMetadataItem()
							titleName.keySpace = AVMetadataKeySpace.common
							titleName.key = AVMetadataKey.commonKeyTitle as NSString
							titleName.value = oldMusicName as NSString
							
							let dataInfo = AVMutableMetadataItem()
							dataInfo.keySpace = AVMetadataKeySpace.quickTimeUserData
							dataInfo.key = AVMetadataKey.quickTimeUserDataKeySoftware as NSString
							dataInfo.value = "PlayerPRO Player" as NSString
							dataInfo.locale = Locale(identifier: "en")
							
							let musicInfoQTUser = AVMutableMetadataItem()
							musicInfoQTUser.keySpace = AVMetadataKeySpace.quickTimeUserData
							musicInfoQTUser.key = AVMetadataKey.quickTimeUserDataKeyInformation as NSString
							musicInfoQTUser.value = oldMusicInfo as NSString
							musicInfoQTUser.locale = Locale.current
							
							let musicNameQTUser = AVMutableMetadataItem()
							musicNameQTUser.keySpace = AVMetadataKeySpace.quickTimeUserData
							musicNameQTUser.key = AVMetadataKey.quickTimeUserDataKeyFullName as NSString
							musicNameQTUser.value = oldMusicName as NSString
							musicNameQTUser.locale = Locale.current
							
							let musicInfoiTunes = AVMutableMetadataItem()
							musicInfoiTunes.keySpace = AVMetadataKeySpace.iTunes
							musicInfoiTunes.key = AVMetadataKey.iTunesMetadataKeyUserComment as NSString
							musicInfoiTunes.value = oldMusicInfo as NSString
							
							let musicInfoQTMeta = AVMutableMetadataItem()
							musicInfoQTMeta.keySpace = .quickTimeMetadata
							musicInfoQTMeta.key = AVMetadataKey.quickTimeMetadataKeyInformation as NSString
							musicInfoQTMeta.value = oldMusicInfo as NSString
							musicInfoQTMeta.locale = Locale.current
							
							return [titleName, dataInfo, musicInfoQTUser, musicInfoiTunes, musicInfoQTMeta, musicNameQTUser]
						}()
						
						let tmpName = oldMusicName != "" ? oldMusicName : "untitled"
						let tmpURL = (try! FileManager.default.url(for: .itemReplacementDirectory, in: .userDomainMask, appropriateFor: oldURL, create: true)).appendingPathComponent("\(tmpName).aiff", isDirectory: false)
						
						do {
							try self.saveMusic(AIFFToURL: tmpURL, theSett: &self.exportSettings)
						} catch let anErr {
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
						
						guard let session = AVAssetExportSession(asset: exportMov, presetName: AVAssetExportPresetAppleM4A) else {
							expErr = NSError(domain: NSCocoaErrorDomain, code: NSFileWriteUnknownError, userInfo: nil)
							NSLog("Init Failed for %@, error: %@", oldMusicName, expErr!.localizedDescription)
							DispatchQueue.main.async(execute: errBlock)
							return
						}
						do {
							try FileManager.default.removeItem(at: saveURL)
						} catch _ { }
						session.outputURL = saveURL
						session.outputFileType = AVFileType.m4a
						session.metadata = metadataInfo
						let sessionWaitSemaphore = DispatchSemaphore(value: 0)
						session.exportAsynchronously(completionHandler: { () -> Void in
							_=sessionWaitSemaphore.signal()
						})
						_=sessionWaitSemaphore.wait(timeout: DispatchTime.distantFuture)
						
						let didFinish = session.status == .completed
						self.madDriver.endExport()
						
						if didFinish {
							DispatchQueue.main.async() {
								if self.isQuitting {
									NSApplication.shared.reply(toApplicationShouldTerminate: true)
								} else {
									let retVal = PPRunInformationalAlertPanel(title: "Export complete", message: "The export of the file \"\(savePanel.url!.lastPathComponent)\" is complete.", defaultButton: "OK", alternateButton: "Show File")
									if retVal == NSApplication.ModalResponse.alertSecondButtonReturn {
										NSWorkspace.shared.activateFileViewerSelecting([savePanel.url!])
									}
								}
							}
						} else {
							var errStr = "Unknown Error"
							if let err = session.error {
								errStr = err.localizedDescription
							}
							NSLog(errStr)
							if self.isQuitting {
								NSApplication.shared.reply(toApplicationShouldTerminate: true)
							} else if let err = session.error {
								DispatchQueue.main.async {
									NSApp.presentError(err)
								}
							}
						}
					}
				})
			})
			
		case -3: // wave
			savePanel.allowedFileTypes = [AVFileType.wav.rawValue]
			savePanel.title = "Export as Wave Audio"
			savePanel.beginSheetModal(for: window, completionHandler: { (result) -> Void in
				if result.rawValue != NSFileHandlingPanelOKButton {
					self.madDriver.endExport()
					return
				}
				self.beginExportSettings(with: { (result) -> Void in
					guard result == NSApplication.ModalResponse.alertFirstButtonReturn else {
						self.madDriver.endExport()
						if self.isQuitting {
							NSApplication.shared.reply(toApplicationShouldTerminate: true)
						}
						return
					}
					
					DispatchQueue.global().async() {
						autoreleasepool {
							do {
								try self.saveMusic(waveToURL: savePanel.url!, theSett: &self.exportSettings)
								self.madDriver.endExport()
								DispatchQueue.main.async() {
									if self.isQuitting {
										NSApplication.shared.reply(toApplicationShouldTerminate: true)
									} else {
										let retVal = PPRunInformationalAlertPanel(title: "Export complete", message: "The export of the file \"\(savePanel.url!.lastPathComponent)\" is complete.", defaultButton: "OK", alternateButton: "Show File")
										if retVal == NSApplication.ModalResponse.alertSecondButtonReturn {
											NSWorkspace.shared.activateFileViewerSelecting([savePanel.url!])
										}
									}
								}
							} catch let error as NSError {
								if self.isQuitting {
									NSApplication.shared.reply(toApplicationShouldTerminate: true)
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
			break
			
		default:
			guard !(tag > madLib.pluginCount || tag < 0) else {
				NSSound.beep()
				
				madDriver.endExport()
				if isQuitting {
					NSApplication.shared.reply(toApplicationShouldTerminate: true)
				}
				
				return
			}
			savePanel.allowedFileTypes = madLib[tag].UTITypes
			savePanel.title = "Export as \(madLib[tag].menuName)"
			
			savePanel.beginSheetModal(for: self.window, completionHandler: {(result) -> Void in
				defer {
					self.madDriver.endExport()
				}
				guard result.rawValue == NSFileHandlingPanelOKButton else {
					return
				}
				
				let fileURL = savePanel.url!
				do {
					try self.music!.exportMusic(to: fileURL, format: self.madLib[tag].type, library: self.madLib)
					self.addMusicToMusicList(fileURL, loadIfPreferencesAllow: false)
					if self.isQuitting {
						NSApplication.shared.reply(toApplicationShouldTerminate: true)
					} else {
						let retVal = PPRunInformationalAlertPanel(title: "Export complete", message: "The export of the file \"\(savePanel.url!.lastPathComponent)\" is complete.", defaultButton: "OK", alternateButton: "Show File")
						if retVal == NSApplication.ModalResponse.alertSecondButtonReturn {
							NSWorkspace.shared.activateFileViewerSelecting([fileURL])
						}
					}
				} catch {
					if self.isQuitting {
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
		self.selectedIndex.index = tableView.selectedRow
		do {
			try loadMusicFromCurrentlyPlayingIndex()
		} catch let err {
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
				if (result.rawValue == NSFileHandlingPanelOKButton) {
					self.addMusics(toMusicList: panel.urls)
				}
			})
		} else {
			NSSound.beep()
		}
	}
	
	override func validateToolbarItem(_ theItem: NSToolbarItem) -> Bool {
		let theTag = MusicToolbarTypes(rawValue: theItem.tag) ?? .Unknown
		switch theTag {
		case .Sort, .AddMusic:
			return true
			
		case .FileInfo:
			if infoDrawer.state == Int(NSDrawer.State.openingState.rawValue) || infoDrawer.state == Int(NSDrawer.State.openState.rawValue) {
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
			let fileUTI = try NSWorkspace.shared.type(ofFile: musicURL.path)
			if let info = madLib.typeFromUTI(fileUTI) {
				aPPInfo = try madLib.information(from: musicURL, type: info)
			} else {
				throw PPMADError(.none)
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
	
	func tableView(_ tableView: NSTableView, acceptDrop info: NSDraggingInfo, row: Int, dropOperation: NSTableView.DropOperation) -> Bool {
		let dragPB = info.draggingPasteboard()
		if let tmpArray = dragPB.readObjects(forClasses: [MusicListDragClass.self], options: nil) , tmpArray.count != 0 {
			var minRow = 0
			let dragClass = (tmpArray[0]) as! MusicListDragClass
			let dragIndexSet = dragClass.theIndexSet
			
			for currentIndex in dragIndexSet {
				if currentIndex <= row {
					minRow += 1
				}
			}
			
			changeValueForMusicListKey({
				let selArray = musicList.objectsInMusicList(at: dragIndexSet)
				musicList.remove(at: dragIndexSet)
				musicList.insert(selArray, at: row - minRow)
			})
			musicListContentsDidMove()
			return true
		} else if let tmpArray = dragPB.readObjects(forClasses: [NSURL.self], options:[.urlReadingFileURLsOnly : true,
		                                                                               .urlReadingContentsConformToTypes : self.trackerUTIs]) {
				if tmpArray.count < 1 {
					return false
				}
				
				changeValueForMusicListKey({
					let mutArray = (tmpArray as! [URL]).map({ (curURL) -> MusicListObject in
						return MusicListObject(url: curURL)
					})
					self.musicList.insert(mutArray, at: row)
				})
				musicListContentsDidMove()
				return true
		}
		
		return false
	}
	
	func tableView(_ tableView1: NSTableView, validateDrop info: NSDraggingInfo, proposedRow row: Int, proposedDropOperation dropOperation: NSTableView.DropOperation) -> NSDragOperation {
		var result: NSDragOperation = []
		
		if info.draggingSource() as AnyObject? === tableView1 {
			result = .move
			//TODO: check for number of indexes that are greater than the drop row.
			tableView1.setDropRow(row, dropOperation: .above)
		} else {
			let pb = info.draggingPasteboard()
			
			//list the file type UTIs we want to accept
			let acceptedTypes = trackerUTIs
			
			if let urls = pb.readObjects(forClasses: [NSURL.self], options: [NSPasteboard.ReadingOptionKey.urlReadingFileURLsOnly : true,
			                                                                 NSPasteboard.ReadingOptionKey.urlReadingContentsConformToTypes : acceptedTypes]) {
					if urls.count > 0 {
						result = .copy
						tableView1.setDropRow(row, dropOperation: .above)
					}
			}
		}
		
		return result
	}
	
	func tableView(_ tableView: NSTableView, toolTipFor cell: NSCell, rect: NSRectPointer, tableColumn: NSTableColumn?, row: Int, mouseLocation: NSPoint) -> String {
		if row >= 0 || row <= musicList.countOfMusicList {
			return ""
		}
		return musicList.url(at: row).path
	}
	
	func tableView(_ tableView: NSTableView, writeRowsWith rowIndexes: IndexSet, to pboard: NSPasteboard) -> Bool {
		var status = false
		let dragClass = MusicListDragClass(indexSet: rowIndexes)
		let ppmobjects = musicList.objectsInMusicList(at: rowIndexes)
		let urlArrays = ppmobjects.map({ $0.musicURL })
		let tmpObjs: [NSPasteboardWriting] = [dragClass] + (urlArrays as [NSPasteboardWriting])
		pboard.clearContents() // clear pasteboard to take ownership
		status = pboard.writeObjects(tmpObjs) // write the URLs
		return status
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
		exportSettings.outPutBits = bits
	}
	
	func soundView(_ view: SoundSettingsViewController, rateDidChange rat: UInt32) {
		exportSettings.outPutRate = rat
	}
	
	func soundView(_ view: SoundSettingsViewController, reverbDidChangeActive isAct: Bool) {
		exportSettings.Reverb = isAct
	}
	
	func soundView(_ view: SoundSettingsViewController, oversamplingDidChangeActive isAct: Bool) {
		if (!isAct) {
			exportSettings.oversampling = 1
		}
	}
	
	func soundView(_ view: SoundSettingsViewController, stereoDelayDidChangeActive isAct: Bool) {
		if (!isAct) {
			exportSettings.MicroDelaySize = 0
		}
	}
	
	func soundView(_ view: SoundSettingsViewController, surroundDidChangeActive isAct: Bool) {
		exportSettings.surround = isAct
	}
	
	func soundView(_ view: SoundSettingsViewController, reverbStrengthDidChange rev: Int32) {
		exportSettings.ReverbStrength = rev
	}
	
	func soundView(_ view: SoundSettingsViewController, reverbSizeDidChange rev: Int32) {
		exportSettings.ReverbSize = rev
	}
	
	func soundView(_ view: SoundSettingsViewController, oversamplingAmountDidChange ovs: Int32) {
		exportSettings.oversampling = ovs
	}
	
	func soundView(_ view: SoundSettingsViewController, stereoDelayAmountDidChange std: Int32) {
		exportSettings.MicroDelaySize = std
	}
}
