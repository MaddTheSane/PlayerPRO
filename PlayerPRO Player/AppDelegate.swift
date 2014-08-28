//
//  AppDelegate.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/11/14.
//
//

import Cocoa
import PlayerPROKit
import PlayerPROCore
import AVFoundation
import AudioToolbox

private let kUnresolvableFile = "Unresolvable files"
private let kUnresolvableFileDescription = "There were %lu file(s) that were unable to be resolved."

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

func generateAVMetadataInfo(oldMusicName: String, oldMusicInfo: String) -> [AVMetadataItem] {
	let titleName = AVMutableMetadataItem()
	titleName.keySpace = AVMetadataKeySpaceCommon
	titleName.setKey(AVMetadataCommonKeyTitle)
	titleName.setValue(oldMusicName)
	
	let dataInfo = AVMutableMetadataItem()
	dataInfo.keySpace = AVMetadataKeySpaceCommon;
	dataInfo.setKey(AVMetadataCommonKeySoftware)
	dataInfo.setValue("PlayerPRO Player")
	
	let musicInfoQTUser = AVMutableMetadataItem();
	musicInfoQTUser.keySpace = AVMetadataKeySpaceQuickTimeUserData
	musicInfoQTUser.setKey(AVMetadataQuickTimeUserDataKeyInformation)
	musicInfoQTUser.setValue(oldMusicInfo)
	
	let musicInfoiTunes = AVMutableMetadataItem();
	musicInfoiTunes.keySpace = AVMetadataKeySpaceiTunes
	musicInfoiTunes.setKey(AVMetadataiTunesMetadataKeyUserComment)
	musicInfoiTunes.setValue(oldMusicInfo)
	
	let musicInfoQTMeta = AVMutableMetadataItem();
	musicInfoQTMeta.keySpace = AVMetadataKeySpaceQuickTimeMetadata
	musicInfoQTMeta.setKey(AVMetadataQuickTimeMetadataKeyInformation)
	musicInfoQTMeta.setValue(oldMusicInfo)
	
	return [titleName, dataInfo, musicInfoQTUser, musicInfoiTunes, musicInfoQTMeta];
}

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
	var madLib = PPLibrary()
	dynamic var paused = true
	private var isQuitting = false
	var instrumentController = InstrumentWindowController()
	
	var exportSettings = MADDriverSettings()
	@IBOutlet var exportSettingsBox: NSBox!
	var exportController: SoundSettingsViewController!

	dynamic let musicList = MusicList()
	let preferences = Preferences()
	var plugInInfos = [PlugInInfo]()
	
	private var currentlyPlayingIndex	= CurrentlyPlayingIndex()
	private var previouslyPlayingIndex	= CurrentlyPlayingIndex()

	private dynamic var musicName = ""
	private dynamic var musicInfo = ""
	private var selMusFromList = -1
	
	private var _trackerDict = [String: [String]]()
	private var _trackerUTIs: [String] = []
	
	var trackerDict: [String: [String]] { get {
		if _trackerDict.isEmpty || _trackerDict.count != Int(madLib.pluginCount) + 2 {
			let localMADKName = NSLocalizedString("PPMADKFile", tableName: "InfoPlist", comment: "MADK Tracker")
			let localGenericMADName = NSLocalizedString("Generic MAD tracker", comment: "Generic MAD tracker")
			var tmpTrackerDict = [localMADKName: [MADNativeUTI], localGenericMADName: [MADGenericUTI]] as [String: [String]]
			
			for objraw in madLib {
				let obj = objraw as PPLibraryObject
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
	
	@IBAction func showMusicList(sender: AnyObject!) {
		self.window.makeKeyAndOrderFront(sender)
	}
	
	func clearMusic() {
		if (music != nil) {
			madDriver.stop()
			madDriver.cleanDriver()
		}
		
		self.paused = true;
		currentlyPlayingIndex.index = -1;
		currentlyPlayingIndex.playbackURL = nil;
		currentlyPlayingIndex.movePlayingIndexToOtherIndex(previouslyPlayingIndex)
		self.music = PPMusicObject();
		setTitleForSongLabelBasedOnMusic()
		NSNotificationCenter.defaultCenter().postNotificationName(PPMusicDidChange, object:self)
		music.attachToDriver(madDriver)
	}
	
	private func setTitleForSongLabelBasedOnMusic() {
		self.musicName = music.internalFileName;
		self.musicInfo = music.madInformation
	}
	
	func loadMusicURL(musicToLoad: NSURL!, error theErr: NSErrorPointer? = nil) -> Bool {
		var fileType = [Int8](count: 5, repeatedValue: 0)
		var theOSErr = MADErr.NoErr;
		if (self.music != nil) {
			madDriver.stop()
			madDriver.stopDriver()
		}
		
		theOSErr = madLib.identifyFileAtURL(musicToLoad, type: &fileType)
		
		if (theOSErr != .NoErr) {
			theErr?.memory = CreateErrorFromMADErrorType(theOSErr);
			self.paused = true;
			self.clearMusic();
			return false;
		}
		
		self.music = PPMusicObject(URL:musicToLoad, library:madLib)
		if (self.music == nil) {
			theOSErr = .ReadingErr;
		}
		
		if (theOSErr != .NoErr) {
			theErr?.memory = CreateErrorFromMADErrorType(theOSErr);
			self.paused = true;
			self.clearMusic();
			return false;
		}
		
		music.attachToDriver(madDriver)
		madDriver.play()
		self.paused = false;
		
		var fT = 0
		var cT = 0
		madDriver.getMusicStatusWithCurrentTime(&cT, totalTime: &fT)
		songPos.maxValue = Double(fT)
		songPos.minValue = 0.0
		setTitleForSongLabelBasedOnMusic()
		songTotalTime.integerValue = fT
		
		NSNotificationCenter.defaultCenter().postNotificationName(PPMusicDidChange, object: self)
		
		theErr?.memory = nil;
		return true;
	}

	@IBAction func showPreferences(sender: AnyObject!) {
		preferences.window.center()
		preferences.showWindow(sender)
	}

	override func observeValueForKeyPath(keyPath: String!, ofObject object: AnyObject!, change: [NSObject : AnyObject]!, context: UnsafeMutablePointer<()>) {
		if (keyPath == "paused") {
			var boolVal = (change[NSKeyValueChangeNewKey] as NSNumber);
			
			pauseButton.highlight(boolVal.boolValue)
			switch (boolVal.boolValue) {
				
			case true:
				pauseButton.state = NSOnState
				pauseDockMenuItem.state = NSOffState
				break;
				
			//case false:
			//	fallthrough
			default:
				pauseButton.state = NSOffState
				pauseDockMenuItem.state = NSOnState
				break;
			}
		}
		//[super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
	}
	
	func loadMusicFromCurrentlyPlayingIndexWithError(theErr:NSErrorPointer?) -> Bool {
		currentlyPlayingIndex.playbackURL = musicList.URLAtIndex(currentlyPlayingIndex.index)
		let isGood = loadMusicURL(currentlyPlayingIndex.playbackURL, error: theErr)
		currentlyPlayingIndex.movePlayingIndexToOtherIndex(previouslyPlayingIndex)
		return isGood;
	}
	
	func doubleClickMusicList() {
		var err: NSError? = nil
		self.currentlyPlayingIndex.index = tableView.selectedRow;
		if (loadMusicFromCurrentlyPlayingIndexWithError(&err) == false) {
			NSAlert(error: err!).runModal()
		}
	}

	func selectCurrentlyPlayingMusic() {
		if (self.currentlyPlayingIndex.index >= 0) {
			//currentlyPlayingIndex.playbackURL = [musicList URLAtIndex:currentlyPlayingIndex.index];
			selectMusicAtIndex(currentlyPlayingIndex.index)
		}
	}
	
	func selectMusicAtIndex(anIdx: Int) {
		if (anIdx < 0 || anIdx >= musicList.countOfMusicList) {
			NSBeep();
			return;
		}
		let idx = NSIndexSet(index: anIdx);
		tableView.selectRowIndexes(idx, byExtendingSelection: false)
		tableView.scrollRowToVisible(anIdx)
	}
	
	func addMusicToMusicList(theURL: NSURL, loadIfPreferencesAllow load: Bool = true) {
		self.willChangeValueForKey(kMusicListKVO)
		let okayMusic = musicList.addMusicURL(theURL)
		self.didChangeValueForKey(kMusicListKVO)
		if (!okayMusic) {
			let similarMusicIndex = musicList.indexOfObjectSimilarToURL(theURL)
			if (similarMusicIndex == NSNotFound) {
				return;
			}
			if (load && NSUserDefaults.standardUserDefaults().boolForKey(PPLoadMusicAtMusicLoad)) {
				self.currentlyPlayingIndex.index = similarMusicIndex;
				selectCurrentlyPlayingMusic()
				var err: NSError? = nil
				if (!loadMusicFromCurrentlyPlayingIndexWithError(&err)) {
					NSAlert(error: err).runModal()
				}
			} else {
				selectMusicAtIndex(similarMusicIndex)
			}
		} else if (load && NSUserDefaults.standardUserDefaults().boolForKey(PPLoadMusicAtMusicLoad)) {
			self.currentlyPlayingIndex.index = musicList.countOfMusicList - 1;
			//currentlyPlayingIndex.playbackURL = [musicList URLAtIndex:currentlyPlayingIndex.index];
			selectCurrentlyPlayingMusic()
			var err: NSError? = nil
			if (!loadMusicFromCurrentlyPlayingIndexWithError(&err)) {
				NSAlert(error: err).runModal()
			}
		} else {
			selectMusicAtIndex(musicList.countOfMusicList - 1)
		}
	}
	
	func MADDriverWithPreferences() {
		var madWasReading = false;
		var fullTime = 0, curTime = 0;
		var returnerr = MADErr.NoErr;
		if (madDriver != nil) {
			madWasReading = !madDriver.paused
			madDriver.stop()
			//[madDriver stopDriver];
			
			madDriver.getMusicStatusWithCurrentTime(&curTime, totalTime:&fullTime)
			
			//madDriver = nil;
		}
		var theSettinit = MADDriverSettings(bestDriver: true);
		var defaults = NSUserDefaults.standardUserDefaults()
		
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
		
		theSettinit.driverMode = MADSoundOutput.fromRaw(Int16(defaults.integerForKey(PPSoundDriver))) ?? .CoreAudioDriver
		theSettinit.repeatMusic = false;
		
		//OSErr returnerr = MADCreateDriver(&init, madLib, &madDriver);
		if (madDriver == nil) {
			madDriver = PPDriver(library:madLib, settings:&theSettinit, error: &returnerr)
		} else {
			returnerr = madDriver.changeDriverSettingsToSettings(theSettinit)
		}
		NSNotificationCenter.defaultCenter().postNotificationName(PPDriverDidChange, object:self)
		if returnerr != .NoErr {
			var err = CreateErrorFromMADErrorType(returnerr);
			NSAlert(error: err).runModal()
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
	
	override init() {
		let ourDefaults: NSDictionary = [PPRememberMusicList: true,
			PPLoadMusicAtListLoad: false,
			PPAfterPlayingMusic: Int(PPPlaylistMode.StopPlaying.toRaw()),
			PPGotoStartupAfterPlaying: true,
			PPSaveModList: true,
			PPLoadMusicAtMusicLoad: false,
			PPLoopMusicWhenDone: false,
			
			PPSoundOutBits: 16,
			PPSoundOutRate: 44100,
			PPSoundDriver: Int(MADSoundOutput.CoreAudioDriver.toRaw()),
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

	func songIsDonePlaying() {
		var err: NSError? = nil
		let userDefaults = NSUserDefaults.standardUserDefaults()
		switch (PPPlaylistMode.fromRaw(userDefaults.integerForKey(PPAfterPlayingMusic))!) {
			
		case PPPlaylistMode.LoopMusic:
			madDriver.musicPosition = 0;
			madDriver.play()
			break;
			
		case PPPlaylistMode.LoadNext:
			if (musicList.countOfMusicList > ++self.currentlyPlayingIndex.index) {
				selectCurrentlyPlayingMusic()
				if (!loadMusicFromCurrentlyPlayingIndexWithError(&err)) {
					NSAlert(error: err).runModal()
				}
			} else {
				if (userDefaults.boolForKey(PPLoopMusicWhenDone)) {
					currentlyPlayingIndex.index = 0;
					selectCurrentlyPlayingMusic()
					if (!self.loadMusicFromCurrentlyPlayingIndexWithError(&err))
					{
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
			self.currentlyPlayingIndex.index = random() % musicList.countOfMusicList;
			selectCurrentlyPlayingMusic()
			if (!loadMusicFromCurrentlyPlayingIndexWithError(&err)) {
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
	
	private func changeValueForMusicListKey(theClosure: () -> Void) {
		self.willChangeValueForKey(kMusicListKVO)
		theClosure()
		self.didChangeValueForKey(kMusicListKVO)
	}
	
	@IBAction func sortMusicList(sender: AnyObject!) {
		changeValueForMusicListKey({
			self.musicList.sortMusicListByName()
		})
		musicListContentsDidMove()
	}
	
	func updateMusicStats(theTimer: NSTimer!) {
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
	
	func saveMusicToURL(tosave: NSURL) {
		music.saveMusicToURL(tosave)
	}

	func updatePlugInInfoMenu() {
		let trackerPlugName = NSLocalizedString("TrackerPlugName", comment: "Tracker plug-in name")
		
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
	func applicationDidFinishLaunching(notification: NSNotification!) {
		let sranTime = time(nil) & 0xffffffff
		srandom(UInt32(sranTime))
		//PPRegisterDebugFunc(CFunctionPointer(&CocoaDebugStr));
		MADRegisterDebugBlock(CocoaDebugStr)
		madLib = PPLibrary();
		//the NIB won't store the value anymore, so do this hackery to make sure there's some value in it.
		songTotalTime.integerValue = 0
		songCurTime.integerValue = 0
		
		tableView.registerForDraggedTypes([PPMLDCUTI, kUTTypeFileURL as NSString])
		self.addObserver(self, forKeyPath:"paused", options:.New, context:nil)
		self.paused = true;
		self.willChangeValueForKey(kMusicListKVO)
		if NSUserDefaults.standardUserDefaults().boolForKey(PPRememberMusicList) {
			musicList.loadApplicationMusicList()
		}
		var selMus = musicList.selectedMusic;
		self.didChangeValueForKey(kMusicListKVO)
		
		tableView.doubleAction = "doubleClickMusicList"
		
		let defaultCenter = NSNotificationCenter.defaultCenter()
		defaultCenter.addObserver(self, selector:"soundPreferencesDidChange:", name:PPSoundPreferencesDidChange, object:nil)
		
		MADDriverWithPreferences()
		//self.music = [PPMusicObject new];
		music.attachToDriver(madDriver)
		setTitleForSongLabelBasedOnMusic()
		
		//var i = 0
		
		for (i, objRaw) in enumerate(madLib) {
			let obj = objRaw as PPLibraryObject
			if (obj.canExport) {
				let mi = NSMenuItem(title: "\(obj.menuName)…", action: "exportMusicAs:", keyEquivalent: "")
				mi.tag = Int(i);
				mi.target = self
				musicExportMenu.addItem(mi)
			}
		}
		
		updatePlugInInfoMenu()
		
		exportController = SoundSettingsViewController();
		exportController.delegate = self;
		exportSettingsBox.contentView = exportController.view
		
		//instrumentController = InstrumentWindowController()
		
		timeChecker = NSTimer(fireDate:NSDate(), interval:1/8.0, target:self, selector: "updateMusicStats:", userInfo:nil, repeats:true);
		NSRunLoop.mainRunLoop().addTimer(timeChecker, forMode: NSRunLoopCommonModes)
		var lostCount = musicList.lostMusicCount;
		if (lostCount != 0) {
			PPRunAlertPanel(kUnresolvableFile, message: kUnresolvableFileDescription, args: lostCount);
		}
		if (selMus != -1) {
			selectMusicAtIndex(selMus)
		}
	}
	
	private func musicListContentsDidMove() {
		var i = 0;
		if (self.currentlyPlayingIndex.index != -1) {
			for (i = 0; i < musicList.countOfMusicList; i++) {
				if (self.currentlyPlayingIndex.playbackURL == musicList.URLAtIndex(i)) {
					self.currentlyPlayingIndex.index = i;
					break;
				}
			}
		}
		if (self.previouslyPlayingIndex.index != -1) {
			for (i = 0; i < musicList.countOfMusicList; i++) {
				if (self.previouslyPlayingIndex.playbackURL == musicList.URLAtIndex(i)) {
					self.previouslyPlayingIndex.index = i;
					break;
				}
			}
		}
	}
	
	@IBAction func removeSelectedMusic(sender: AnyObject!) {
		var selMusic = tableView.selectedRowIndexes
		changeValueForMusicListKey({
			self.musicList.removeObjectsInMusicListAtIndexes(selMusic)
		})
		musicListContentsDidMove()
	}
	
	@IBAction func newMusic(sender: AnyObject) {
		clearMusic()
	}
	
	func rawSoundData(theSet: UnsafeMutablePointer<MADDriverSettings>) -> NSMutableData? {
		var err = MADErr.NoErr;
		var theRec = PPDriver(library: madLib, settings: theSet, error: &err)
		
		#if false
		if (theRec == nil) {
			dispatch_async(dispatch_get_main_queue()) {
				var NSerr = CreateErrorFromMADErrorType(.UnknownErr);
				NSAlert(error:NSerr).runModal()
			}
			
			return nil;
		}
		#endif
		
		theRec.cleanDriver()
		theRec.currentMusic = music
		
		var soundPtr:UnsafeMutablePointer<()> = nil;
		var full = theRec.audioDataLength;
		if (theSet.memory.outPutBits == 16) {
			full *= 2;
		} else if (theSet.memory.outPutBits == 20 || theSet.memory.outPutBits == 24 ) {
			full *= 3;
		}
		
		switch (theSet.memory.outPutMode) {
		case .DeluxeStereoOutPut:
			fallthrough
		case .StereoOutPut:
			full *= 2;
			
		case .PolyPhonic:
			full *= 4;
			
		default:
			break;
		}
		
		theRec.play()
		
		var mutData = NSMutableData();
		soundPtr = UnsafeMutablePointer<()>.alloc(full)
		
		while (theRec.directSaveToPointer(soundPtr, settings:theSet)) {
			mutData.appendBytes(soundPtr, length: full)
		}
		
		soundPtr.dealloc(full);
		return mutData;
	}
	
	@IBAction func okayExportSettings(sender: AnyObject!) {
		window.endSheet(exportWindow, returnCode: NSAlertDefaultReturn)
		exportWindow.close()
	}
	
	@IBAction func cancelExportSettings(sender: AnyObject!) {
		window.endSheet(exportWindow, returnCode: NSAlertAlternateReturn)
		exportWindow.close()
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
			PPRunAlertPanel(kUnresolvableFile, message: kUnresolvableFileDescription, args: lostCount);
		}
	}
	
	func handleFile(theURL1: NSURL!, ofType theUTI: String) -> Bool {
		var sharedWorkspace = NSWorkspace.sharedWorkspace()
		var theURL = theURL1
		if (theUTI  == MADGenericUTI) {
			var invExt = NSLocalizedString("Invalid Extension", comment: "Invalid Extension")
			var invExtDes = NSLocalizedString("The file %@ is identified as as a generic MAD tracker, and not a specific one. Renaming it will fix this. Do you want to rename the file extension?", comment: "Invalid extension description")
			var renameFile =  NSLocalizedString("Rename", comment: "rename file")
			var openFile = NSLocalizedString("Open", comment: "Open a file")
			var cancelOp = NSLocalizedString("Cancel", comment: "Cancel")
			var retVal = PPRunInformationalAlertPanel(NSLocalizedString("Invalid Extension", comment: "Invalid extension"), message: NSLocalizedString("The file %@ is identified as as a generic MAD tracker, and not a specific one. Renaming it will fix this. Do you want to rename the file extension?", comment: "Invalid extension description"), defaultButton: NSLocalizedString("Rename", comment: "rename file"), alternateButton: NSLocalizedString("Open", comment:"Open a file"), otherButton: NSLocalizedString("Cancel", comment: "Cancel"), args: theURL.lastPathComponent);
			switch (retVal) {
			case NSAlertDefaultReturn:
				
				var rec: NSDictionary? = nil
				var ostype = [Int8](count: 5, repeatedValue: 0)
				
				var identified = madLib.identifyFileAtURL(theURL, type: &ostype)
				
				if (madLib.identifyFileAtURL(theURL, type: &ostype) != .NoErr) || madLib.getInformationFromFileAtURL(theURL, type: &ostype, infoDictionary: &rec) != .NoErr {
					PPRunCriticalAlertPanel(NSLocalizedString("Unknown File", comment: "unknown file"), message: NSLocalizedString("The file type could not be identified.", comment: "Unidentified file"));
					return false;
				}
				var sigVala: AnyObject = rec![kPPSignature] ?? NSNumber(unsignedInt: "madk")
				var sigValb: MADFourChar = (sigVala as NSNumber).unsignedIntValue
				var sigVal = sigValb.stringValue
				
				
				var tmpURL = theURL!.URLByDeletingPathExtension!.URLByAppendingPathExtension(sigVal.lowercaseString);
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
		if (sharedWorkspace.type(theUTI, conformsToType:PPMusicListUTI)) {
			if (musicListWillChange()) {
				changeValueForMusicListKey({ () -> Void in
					self.musicList.loadMusicListAtURL(theURL)
					self.selMusFromList = self.musicList.selectedMusic
				})
				musicListDidChange()
				return true
			}
		} else if (sharedWorkspace.type(theUTI, conformsToType:PPOldMusicListUTI)) {
			if (musicListWillChange()) {
				self.willChangeValueForKey(kMusicListKVO)
				musicList.beginLoadingOfMusicListAtURL(theURL, completionHandle: { (theErr) -> Void in
					self.selMusFromList = self.musicList.selectedMusic
					self.didChangeValueForKey(kMusicListKVO)
					if theErr != nil {
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
		}
		return false;
	}
	
	func musicListWillChange() -> Bool {
		return false
	}
	
	@IBAction func saveMusicList(sender: AnyObject!) {
		var savePanel = NSSavePanel()
		savePanel.allowedFileTypes = [PPMusicListUTI]
		savePanel.canCreateDirectories = true
		savePanel.canSelectHiddenExtension = true
		savePanel.beginSheetModalForWindow(self.window, completionHandler: { (result) -> Void in
			if result == NSFileHandlingPanelOKButton {
				self.musicList.saveMusicListToURL(savePanel.URL)
			}
		})
	}
	
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
				NSAlert(error:err).runModal()
			}
		} else if (NSUserDefaults.standardUserDefaults().boolForKey(PPLoopMusicWhenDone)) {
			currentlyPlayingIndex.index = 0;
			selectCurrentlyPlayingMusic()
			if (!loadMusicFromCurrentlyPlayingIndexWithError(&err)) {
				NSAlert(error:err).runModal()
			}
		} else {
			NSBeep();
		}
	}
	
	@IBAction func playButtonPressed(sender: AnyObject!) {
		if (self.music != nil) {
			madDriver.play()
			self.paused = false;
		}
	}
	
	@IBAction func prevButtonPressed(sender: AnyObject!) {
		if (currentlyPlayingIndex.index > 0) {
			currentlyPlayingIndex.index--;
			selectCurrentlyPlayingMusic()
			var err: NSError? = nil
			if (!loadMusicFromCurrentlyPlayingIndexWithError(&err)) {
				NSAlert(error:err).runModal()
			}
		} else {
			NSBeep();
		}
	}
	
	func beginExportSettingsWithHandler((Int) -> Void) {
		
	}
	
	func saveMusicAsAIFFToURL(aURL: NSURL, usingSettings: UnsafeMutablePointer<MADDriverSettings>) -> MADErr {
		return MADErr.UnknownErr
	}
	
	func rawSoundData(theSet1: MADDriverSettings) -> NSMutableData? {
		var err = MADErr.NoErr;
		var theSet = theSet1
		var theRec = PPDriver(library: madLib, settings: &theSet, error: &err)
		
		if (err != MADErr.NoErr) {
			dispatch_async(dispatch_get_main_queue()) {
				let NSerr = CreateErrorFromMADErrorType(err)!
				NSAlert(error: NSerr).runModal()
			}
			
			return nil;
		}
		
		theRec.cleanDriver()
		theRec.currentMusic = music
		
		var soundPtr: UnsafeMutablePointer<UInt8> = nil;
		var full = theRec.audioDataLength
		if (theSet.outPutBits == 16) {
			full *= 2;
		} else if (theSet.outPutBits == 20 || theSet.outPutBits == 24 ) {
			full *= 3;
		}
		
		switch (theSet.outPutMode) {
		case .DeluxeStereoOutPut:
			fallthrough
		case .StereoOutPut:
			full *= 2;
			break;
			
		case .PolyPhonic:
			full *= 4;
			break;
			
		default:
			break;
		}
		
		theRec.play()
		
		var mutData = NSMutableData(capacity: full * 60 * Int(theRec.totalMusicPlaybackTime) / 2)
		soundPtr = UnsafeMutablePointer<UInt8>.alloc(full)
		
		while theRec.directSaveToPointer(soundPtr, settings: &theSet) {
			mutData.appendBytes(soundPtr, length: full)
		}
		
		soundPtr.dealloc(full)
		
		return mutData;
	}
	
	func rawBESoundData(theSet: MADDriverSettings) -> NSData? {
		let rsd = rawSoundData(theSet)
		if rsd == nil {
			return nil
		}
		if (theSet.outPutBits == 16) {
			let sndSize = rsd!.length;
			let bePtr = UnsafeMutablePointer<UInt16>(rsd!.mutableBytes)
			dispatch_apply(UInt(sndSize) / 2, dispatch_get_global_queue(0, 0), { (i) -> Void in
				let iInt = Int(i)
				bePtr[iInt] = bePtr[iInt].bigEndian
				return
			})
		}
		return rsd?.copy() as NSData?
	}
	
	func rawLESoundData(theSet: MADDriverSettings) -> NSData? {
		return rawSoundData(theSet)?.copy() as NSData?
	}

	
	@IBAction func rewindButtonPressed(sender: AnyObject!) {
	var tag = (sender as NSMenuItem).tag;
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
			
		})
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
					autoreleasepool {
						var thErr = self.saveMusicAsAIFFToURL(savePanel.URL, usingSettings: &self.exportSettings);
						self.madDriver.endExport()
						if (thErr != .NoErr) {
							return;
						}
					}
					dispatch_async(dispatch_get_main_queue()) {
						if (self.isQuitting) {
							NSApplication.sharedApplication().replyToApplicationShouldTerminate(true)
						} else {
							var retVal = PPRunInformationalAlertPanel("Export complete", message: "The export of the file \"%@\" is complete.", defaultButton: "OK", alternateButton: "Show File", args: savePanel.URL.lastPathComponent);
							if (retVal == NSAlertAlternateReturn) {
								NSWorkspace.sharedWorkspace().activateFileViewerSelectingURLs([savePanel.URL])
							}
						}
					}
				}
			})
		})
		
	case -2: //MP4
		savePanel.allowedFileTypes = ["com.apple.m4a-audio"];
		savePanel.title = "Export as MPEG-4 Audio"
		savePanel.beginSheetModalForWindow(self.window, completionHandler: {(result) -> Void in
			if (result != NSFileHandlingPanelOKButton) {
				self.madDriver.endExport()
				return;
			}
			
			self.beginExportSettingsWithHandler( { (result) -> Void in
				let saveURL = savePanel.URL
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
							PPRunAlertPanel("Export failed", message: "Export/coversion of the music file failed:\n%@", args: expErr!.localizedDescription);
						}
					};
					let oldMusicName = self.musicName;
					let oldMusicInfo = self.musicInfo;
					let tmpName = oldMusicName != "" ? oldMusicName : "untitled"
					let tmpURL = NSFileManager.defaultManager().URLForDirectory(.ItemReplacementDirectory, inDomain: .UserDomainMask, appropriateForURL: oldURL, create: true, error: nil)!.URLByAppendingPathComponent("\(tmpName).aiff", isDirectory: false)
					
					theErr = self.saveMusicAsAIFFToURL(tmpURL, usingSettings:&self.exportSettings)
					if theErr != .NoErr {
						expErr = CreateErrorFromMADErrorType(theErr);
						NSFileManager.defaultManager().removeItemAtURL(tmpURL, error: nil)
						dispatch_async(dispatch_get_main_queue(), errBlock);
					}
					
					
					var exportMov = AVAsset.assetWithURL(tmpURL) as AVAsset!
					var metadataInfo = generateAVMetadataInfo(oldMusicName, oldMusicInfo)
					
					if (exportMov == nil) {
						expErr = NSError(domain: NSCocoaErrorDomain, code: NSFileWriteUnknownError, userInfo: nil)
						NSLog("Init Failed for %@, error: %@", oldMusicName, expErr!.localizedDescription);
						NSFileManager.defaultManager().removeItemAtURL(tmpURL, error: nil)
						dispatch_async(dispatch_get_main_queue(), errBlock);
						return;
					}
					
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
					session.outputFileType = AVFileTypeAppleM4A;
					session.metadata = metadataInfo;
					var sessionWaitSemaphore = dispatch_semaphore_create(0);
					session.exportAsynchronouslyWithCompletionHandler({ () -> Void in
						var tmp = dispatch_semaphore_signal(sessionWaitSemaphore)
					})
					dispatch_semaphore_wait(sessionWaitSemaphore, DISPATCH_TIME_FOREVER);
					
					var didFinish = session.status == .Completed;
					NSFileManager.defaultManager().removeItemAtURL(tmpURL, error: nil)
					
					if (didFinish) {
						dispatch_async(dispatch_get_main_queue()) {
							if (self.isQuitting) {
								NSApplication.sharedApplication().replyToApplicationShouldTerminate(true)
							} else {
								var retVal = PPRunInformationalAlertPanel("Export complete", message: "The export of the file \"%@\" is complete.", defaultButton: "OK", alternateButton: "Show File", args: saveURL.lastPathComponent);
								if (retVal == NSAlertAlternateReturn) {
									NSWorkspace.sharedWorkspace().activateFileViewerSelectingURLs([saveURL])
								}
							}
						}
					} else {
						NSLog("\(session.error)");
					}
				}
			});
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
						return;
					}
				})
				dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), { () -> Void in
					let saveData = self.rawLESoundData(self.exportSettings)
					self.madDriver.endExport()
					
					if saveData == nil {
						return
					}
					
					var expErr: NSError? = nil
					func errBlock() {
						if self.isQuitting {
							(NSApp as NSApplication).replyToApplicationShouldTerminate(true)
						} else {
							PPRunAlertPanel("Export failed", message: "Export of the music file failed:\n%@", args: expErr!.localizedDescription);
						}
					}
					
					var asbd = AudioStreamBasicDescription();
					asbd.mSampleRate = Float64(self.exportSettings.outPutRate)
					asbd.mFormatID = AudioFormatID(kAudioFormatLinearPCM)
					asbd.mFormatFlags = UInt32(kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked)
					asbd.mBitsPerChannel = UInt32(self.exportSettings.outPutBits)
					switch (self.exportSettings.outPutMode) {
					case .MonoOutPut:
						asbd.mChannelsPerFrame = 1;
						break;
						
						
					case .PolyPhonic:
						asbd.mChannelsPerFrame = 4;
						break;
						
					default:
						//case StereoOutPut:
						//case DeluxeStereoOutPut:
						asbd.mChannelsPerFrame = 2;
						break;
					}
					asbd.mFramesPerPacket = 1;
					asbd.mBytesPerFrame = asbd.mBitsPerChannel * asbd.mChannelsPerFrame / 8;
					asbd.mBytesPerPacket = asbd.mBytesPerFrame * asbd.mFramesPerPacket;
					
					let url: CFURL = savePanel.URL
					
					var audioFile: AudioFileID = nil
					var res: OSStatus = noErr
					res = AudioFileCreateWithURL(url, AudioFileTypeID(kAudioFileWAVEType), &asbd, UInt32(kAudioFileFlags_EraseFile), &audioFile);
					if (res != noErr) {
						expErr = NSError(domain: NSOSStatusErrorDomain, code: Int(res), userInfo: nil)
						dispatch_async(dispatch_get_main_queue(), errBlock)
						return;
					}
					
					var numBytes: UInt32 = UInt32(saveData!.length)
					
					res = AudioFileWriteBytes(audioFile, false, 0, &numBytes, saveData!.bytes);
					if (res != noErr) {
						expErr = NSError(domain: NSOSStatusErrorDomain, code: Int(res), userInfo: nil)
						dispatch_async(dispatch_get_main_queue(), errBlock)
						return;
					}
					
					res = AudioFileClose(audioFile);
					if (res != noErr) {
						expErr = NSError(domain: NSOSStatusErrorDomain, code: Int(res), userInfo: nil)
						dispatch_async(dispatch_get_main_queue(), errBlock)
						return;
					}
					
					dispatch_async(dispatch_get_main_queue(), { () -> Void in
						if self.isQuitting {
							(NSApp as NSApplication).replyToApplicationShouldTerminate(true)
						} else {
							let retVal = PPRunInformationalAlertPanel("Export complete", message: "The export of the file \"%@\" is complete.", defaultButton: "OK", alternateButton: "Show File", args: savePanel.URL.lastPathComponent);
							if (retVal == NSAlertAlternateReturn) {
								NSWorkspace.sharedWorkspace().activateFileViewerSelectingURLs([savePanel.URL])
							}
						}
						
					})
					
					
				})
			})
			break;
			
		default:
			let utag =  UInt(tag)
			if utag > madLib.pluginCount || tag < 0 {
				NSBeep();
				
				madDriver.endExport()
				if (isQuitting) {
					NSApplication.sharedApplication().replyToApplicationShouldTerminate(true)
				}
				
				return;
			}
			savePanel.allowedFileTypes = madLib.pluginAtIndex(utag).UTItypes
			savePanel.title = "Export as \(madLib.pluginAtIndex(utag).menuName)"
			
			savePanel.beginSheetModalForWindow(self.window, completionHandler: {(result) -> Void in
				if (result != NSFileHandlingPanelOKButton) {
					self.madDriver.endExport()
					return;
				}
				
				let fileURL = savePanel.URL
				var err = self.music.exportMusicToURL(fileURL, format:self.madLib.pluginAtIndex(utag).plugType, library:self.madLib)
				if (err != .NoErr) {
					if (self.isQuitting) {
						NSApplication.sharedApplication().replyToApplicationShouldTerminate(true)
					} else {
						var aerr = CreateErrorFromMADErrorType(err);
						NSAlert(error: aerr).runModal()
					}
				} else {
					self.addMusicToMusicList(fileURL, loadIfPreferencesAllow:false)
					if (self.isQuitting) {
						NSApplication.sharedApplication().replyToApplicationShouldTerminate(true)
					} else {
						let retVal = PPRunInformationalAlertPanel("Export complete", message: "The export of the file \"%@\" is complete.", defaultButton: "OK", alternateButton: "Show File", args: savePanel.URL.lastPathComponent);
						if (retVal == NSAlertAlternateReturn) {
							NSWorkspace.sharedWorkspace().activateFileViewerSelectingURLs([fileURL])
						}
					}
				}
			});
		}
	}
	
	@IBAction func sliderChanged(sender: AnyObject!) {
		if (self.music != nil) {
			madDriver.setMusicStatusWithCurrentTime(Int(songPos.doubleValue), maximumTime: Int(songPos.maxValue), minimumTime:0)
		}
	}
	
	@IBAction func stopButtonPressed(sender: AnyObject!) {
		if (self.music != nil) {
			madDriver.stop()
			madDriver.cleanDriver()
			madDriver.setMusicStatusWithCurrentTime(0, maximumTime: 100, minimumTime: 0)
			self.paused = true;
		}
	}
	
	@IBAction func pauseButtonPressed(sender: AnyObject!) {
		if (self.music != nil) {
			if (self.paused) {
				madDriver.play()
			} else {
				madDriver.pause()
			}
			self.paused = !paused;
		}
	}

	@IBAction func exportMusicAs(sender: AnyObject!) {
	
	}
	
	func application(sender: NSApplication!, openFile filename: String!) -> Bool {
		var err: NSError? = nil
		var utiFile = NSWorkspace.sharedWorkspace().typeOfFile(filename, error: &err)
		if (err != nil) {
			var tmpAlert = NSAlert()
			tmpAlert.alertStyle = .WarningAlertStyle
			tmpAlert.messageText = "Error opening file"
			tmpAlert.informativeText = "Unable to open \(filename.lastPathComponent): \(err?.localizedFailureReason)"
			PPRunAlertPanel("Error opening file", message: "Unable to open %@: %@", args: filename.lastPathComponent, err!.localizedFailureReason!);
			tmpAlert.runModal()
			return false;
		}
		return handleFile(NSURL(fileURLWithPath: filename), ofType: utiFile)
	}
	
	func tableViewSelectionDidChange(notification: NSNotification!) {
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

		var selected = tableView.selectedRowIndexes
		var theInfo: NSDictionary? = nil
		var info = [Int8](count: 5, repeatedValue: 0)
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
		if (madLib.identifyFileAtURL(musicURL, type:&info) != MADErr.NoErr) {
			badTracker()
			return
		}
		if (madLib.getInformationFromFileAtURL(musicURL, type:&info, infoDictionary:&theInfo) != MADErr.NoErr) {
			badTracker()
			return
		}
		fileName.stringValue = obj.fileName
		var intNameV: AnyObject! = theInfo?[kPPInternalFileName] ?? ""
		internalName.stringValue = intNameV as String
		
		let fsInt: AnyObject = theInfo![kPPFileSize] ?? NSNumber(long:0)
		let fsTmp = (fsInt as NSNumber).longValue
		fileSize.integerValue = fsTmp
		
		let ti: AnyObject = theInfo![kPPTotalInstruments] ?? NSNumber(short: 0)
		let tiTmp = (ti as NSNumber).shortValue
		musicInstrument.integerValue = Int(tiTmp);
		
		let mp: AnyObject = theInfo![kPPTotalPatterns] ?? NSNumber(short: 0)
		let mpTmp = (mp as NSNumber).shortValue
		musicPatterns.integerValue = Int(mpTmp)
		
		let musPlugTyp: AnyObject = theInfo?[kPPFormatDescription] ?? ""
		musicPlugType.stringValue = musPlugTyp as String
		
		let sig: AnyObject = theInfo![kPPSignature] ?? NSNumber(unsignedInt:0)
		let siga: MADFourChar = (sig as NSNumber).unsignedIntValue
		NSSig = siga.stringValue
		#if false
		if (NSSig == nil) {
			NSSig = NSString(format: "0x%08X", siga)
		}
		#endif
		musicSignature.stringValue = NSSig
		
		fileLocation.stringValue = musicURL.path
	}
	
	func tableView(tableView: NSTableView!, acceptDrop info: NSDraggingInfo!, row: Int, dropOperation: NSTableViewDropOperation) -> Bool {
		let dragPB = info.draggingPasteboard()
		var tmpArray = dragPB.readObjectsForClasses([MusicListDragClass.self], options: nil)
		if (tmpArray != nil && tmpArray.count != 0) {
			var minRow = 0;
			let tmpDragClass: AnyObject = tmpArray[0]
			let dragClass = tmpDragClass as MusicListDragClass
			let dragIndexSet = dragClass.theIndexSet;
			
			var currentIndex = dragIndexSet.firstIndex;
			while currentIndex != NSNotFound {
				if currentIndex <= row {
					minRow++;
				}
				currentIndex = dragIndexSet.indexGreaterThanIndex(currentIndex)
			}
			
			changeValueForMusicListKey({
				var selArray = self.musicList.arrayOfObjectsInMusicListAtIndexes(dragIndexSet)
				self.musicList.removeObjectsInMusicListAtIndexes(dragIndexSet)
				self.musicList.insertObjects(selArray, inMusicListAtIndex: row - minRow)
			})
			musicListContentsDidMove()
			return true;
		}
		tmpArray = dragPB.readObjectsForClasses([NSURL.self], options:[NSPasteboardURLReadingFileURLsOnlyKey : true,
			NSPasteboardURLReadingContentsConformToTypesKey : self.trackerUTIs])
		if (tmpArray != nil) {
			if (tmpArray.count < 1) {
				return false;
			}
			
			changeValueForMusicListKey({
				var mutArray = [MusicListObject]()
				for acurURL in tmpArray {
					let curURL = acurURL as NSURL
					mutArray.append(MusicListObject(URL: curURL))
				}
				self.musicList.insertObjects(mutArray, inMusicListAtIndex: row)
			})
			musicListContentsDidMove()
			return true;
		}
		
		return false;
	}
	
	func tableView(tableView1: NSTableView!, validateDrop info: NSDraggingInfo!, proposedRow row: Int, proposedDropOperation dropOperation: NSTableViewDropOperation) -> NSDragOperation {
		var result = NSDragOperation.None;
		
		if (info.draggingSource() === tableView1) {
			result = .Move;
			//TODO: check for number of indexes that are greater than the drop row.
			tableView1.setDropRow(row, dropOperation: .Above)
		} else {
			var pb = info.draggingPasteboard();
			
			//list the file type UTIs we want to accept
			var acceptedTypes = self.trackerUTIs;
			
			var urls = pb.readObjectsForClasses([NSURL.self], options: [NSPasteboardURLReadingFileURLsOnlyKey : true,
				NSPasteboardURLReadingContentsConformToTypesKey : acceptedTypes]);
			
			if (urls.count > 0) {
				result = .Copy
				tableView1.setDropRow(row, dropOperation: .Above)
			}
		}
		
		return result;
	}
	
	func tableView(tableView: NSTableView!, toolTipForCell cell: NSCell!, rect: NSRectPointer, tableColumn: NSTableColumn!, row: Int, mouseLocation: NSPoint) -> String! {
		if (row >= 0 || row <= musicList.countOfMusicList) {
			return nil;
		}
		return musicList.URLAtIndex(row).path
	}
	
	func tableView(tableView: NSTableView!, writeRowsWithIndexes rowIndexes: NSIndexSet!, toPasteboard pboard: NSPasteboard!) -> Bool {
		var status = false;
		let dragClass = MusicListDragClass(indexSet: rowIndexes)
		var urlArrays = [NSURL]()
		var ppmobjects = musicList.arrayOfObjectsInMusicListAtIndexes(rowIndexes)
		for obj in ppmobjects {
			urlArrays.append(obj.musicURL)
		}
		var tmpObjs:[AnyObject] = [dragClass] + urlArrays
		pboard.clearContents(); // clear pasteboard to take ownership
		status = pboard.writeObjects(tmpObjs) // write the URLs
		return status;
	}
	
	// PPSoundSettingsViewController delegate methods
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
