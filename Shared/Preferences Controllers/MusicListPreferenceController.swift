//
//  MusicListPreferenceController.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/12/14.
//
//

import Cocoa

class MusicListPreferenceController: NSViewController, PPPreferenceObject {
	@IBOutlet var askToSave: NSButtonCell!
	@IBOutlet var autoPlay: NSButtonCell!
	@IBOutlet var goToStartupPos: NSButtonCell!
	@IBOutlet var loadFirstMusic: NSButtonCell!
	@IBOutlet var musicPlayingOutlet: NSMatrix!
	@IBOutlet var rememberMusicList: NSButtonCell!
	@IBOutlet var loopMusic: NSButtonCell!
	
	@IBAction func changePlayedMusic(sender: AnyObject!) {
		NSUserDefaults.standardUserDefaults().setInteger(playedMusic, forKey: PPAfterPlayingMusic)
		
		NSNotificationCenter.defaultCenter().postNotificationName(PPListPreferencesDidChange, object: self)
	}
	
	@IBAction func toggleButton(sender: AnyObject) {
		var defaults = NSUserDefaults.standardUserDefaults()
		defaults.setBool(rememberMusicList.state == NSOnState, forKey: PPRememberMusicList)
		defaults.setBool(loadFirstMusic.state == NSOnState, forKey: PPLoadMusicAtListLoad)
		defaults.setBool(autoPlay.state == NSOnState, forKey: PPLoadMusicAtMusicLoad)
		defaults.setBool(goToStartupPos.state == NSOnState, forKey: PPGotoStartupAfterPlaying)
		defaults.setBool(loopMusic.state == NSOnState, forKey: PPLoopMusicWhenDone)
		
		NSNotificationCenter.defaultCenter().postNotificationName(PPListPreferencesDidChange, object: self)
	}

	required init(coder: NSCoder!) {
		super.init(coder: coder)
	}
	
	required override init(nibName nibNameOrNil: String!, bundle nibBundleOrNil: NSBundle!) {
		super.init(nibName: nibNameOrNil, bundle: nibBundleOrNil)
	}
	
	var playedMusic: Int {
		get {
			var curSelected: AnyObject = musicPlayingOutlet.selectedCell()
			if (musicPlayingOutlet.cellAtRow(0, column: 0) === curSelected) {
				return PPPlaylistMode.StopPlaying.toRaw()
			} else if (musicPlayingOutlet.cellAtRow(0, column: 1) === curSelected) {
				return PPPlaylistMode.LoadNext.toRaw()
			} else if (musicPlayingOutlet.cellAtRow(1, column: 0) === curSelected) {
				return PPPlaylistMode.LoopMusic.toRaw()
			} else if (musicPlayingOutlet.cellAtRow(1, column: 1) === curSelected) {
				return PPPlaylistMode.LoadRandom.toRaw()
			} else {
				return -1;
			}
			
		}
		set {
			switch (PPPlaylistMode.fromRaw(newValue)!) {
			case .LoadNext:
				musicPlayingOutlet.selectCellAtRow(0, column: 1)
				
			case .LoopMusic:
				musicPlayingOutlet.selectCellAtRow(1, column: 0)
				
			case .LoadRandom:
				musicPlayingOutlet.selectCellAtRow(1, column: 1)
				
			case .StopPlaying:
				fallthrough
			default:
				musicPlayingOutlet.selectCellAtRow(0, column: 0)
			}
			
		}
	}
	
	class func newPreferenceView() -> Self {
		var nullBundle: NSBundle! = nil
		var ourself = self(nibName: "MusicListPrefs", bundle: nullBundle)
		
		var aTitle = NSLocalizedString("Music List", tableName:"PreferenceNames", comment: "Music List");
		ourself.title = aTitle
		return ourself
	}
	
    override func awakeFromNib() {
        super.awakeFromNib()
        // Do view setup here.
		var defaults = NSUserDefaults.standardUserDefaults()
		
		rememberMusicList.state = defaults.boolForKey(PPRememberMusicList) ? NSOnState : NSOffState
		loadFirstMusic.state = defaults.boolForKey(PPLoadMusicAtListLoad) ? NSOnState : NSOffState
		goToStartupPos.state = defaults.boolForKey(PPGotoStartupAfterPlaying) ? NSOnState : NSOffState
		askToSave.state = defaults.boolForKey(PPSaveModList) ? NSOnState : NSOffState
		autoPlay.state = defaults.boolForKey(PPLoadMusicAtMusicLoad) ? NSOnState : NSOffState
		loopMusic.state = defaults.boolForKey(PPLoopMusicWhenDone) ? NSOnState : NSOffState
		playedMusic = defaults.integerForKey(PPAfterPlayingMusic)
    }
}
