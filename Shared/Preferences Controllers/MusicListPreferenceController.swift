//
//  MusicListPreferenceController.swift
//  PPMacho
//
//  Created by C.W. Betts on 3/4/15.
//
//

import Cocoa

@objc enum PlaylistMode: Int {
	case StopPlaying = 0
	case LoopMusic
	case LoadNext
	case LoadRandom
}

final class MusicListPreferenceController: NSViewController, PPPreferenceObject {
	@IBOutlet weak var askToSave: NSButtonCell!
	@IBOutlet weak var autoPlay: NSButtonCell!
	@IBOutlet weak var goToStartupPos: NSButtonCell!
	@IBOutlet weak var loadFirstMusic: NSButtonCell!
	@IBOutlet weak var musicPlayingOutlet: NSMatrix!
	@IBOutlet weak var rememberMusicList: NSButtonCell!
	@IBOutlet weak var loopMusic: NSButtonCell!
	
	let preferenceIdentifier = PPMusicListPrefID
	
	static func newPreferenceView() -> MusicListPreferenceController? {
		let aCont = MusicListPreferenceController(nibName:"MusicListPrefs", bundle:nil)
		aCont?.title = NSLocalizedString("Music List", tableName: "PreferenceNames", comment: "Music List")
		return aCont
	}
	
	override func awakeFromNib() {
		super.awakeFromNib()
		
		let defaults = NSUserDefaults.standardUserDefaults()
		
		rememberMusicList.state = defaults.boolForKey(PPRememberMusicList) ? NSOnState : NSOffState
		loadFirstMusic.state = defaults.boolForKey(PPLoadMusicAtListLoad) ? NSOnState : NSOffState
		goToStartupPos.state = defaults.boolForKey(PPGotoStartupAfterPlaying) ? NSOnState : NSOffState
		askToSave.state = defaults.boolForKey(PPSaveModList) ? NSOnState : NSOffState
		autoPlay.state = defaults.boolForKey(PPLoadMusicAtMusicLoad) ? NSOnState : NSOffState
		loopMusic.state = defaults.boolForKey(PPLoopMusicWhenDone) ? NSOnState : NSOffState
		playedMusic = PlaylistMode(rawValue: defaults.integerForKey(PPAfterPlayingMusic)) ?? .StopPlaying
	}
	
	@IBAction func changePlayedMusic(sender: AnyObject?) {
		let defaults = NSUserDefaults.standardUserDefaults()
		
		defaults.setInteger(playedMusic.rawValue, forKey: PPAfterPlayingMusic)
		
		NSNotificationCenter.defaultCenter().postNotificationName(PPListPreferencesDidChange, object: self)
	}
	
	@IBAction func toggleButton(sender: AnyObject?) {
		let defaults = NSUserDefaults.standardUserDefaults()
		
		defaults.setBool(rememberMusicList.state == NSOnState, forKey: PPRememberMusicList)
		defaults.setBool(loadFirstMusic.state == NSOnState, forKey: PPLoadMusicAtListLoad)
		defaults.setBool(autoPlay.state == NSOnState, forKey: PPLoadMusicAtMusicLoad)
		defaults.setBool(goToStartupPos.state == NSOnState, forKey: PPGotoStartupAfterPlaying)
		defaults.setBool(loopMusic.state == NSOnState, forKey: PPLoopMusicWhenDone)
		defaults.setBool(goToStartupPos.state == NSOnState, forKey: PPGotoStartupAfterPlaying)
		
		NSNotificationCenter.defaultCenter().postNotificationName(PPListPreferencesDidChange, object: self)
	}
	
	var playedMusic: PlaylistMode {
		get {
			let curSelected: AnyObject! = musicPlayingOutlet.selectedCell()
			
			if musicPlayingOutlet.cellAtRow(0, column: 0) === curSelected {
				return .StopPlaying
			} else if musicPlayingOutlet.cellAtRow(0, column: 1) === curSelected {
				return .LoadNext
			} else if musicPlayingOutlet.cellAtRow(1, column: 0) === curSelected {
				return .LoopMusic
			} else if musicPlayingOutlet.cellAtRow(0, column: 1) === curSelected {
				return .LoadRandom
			} else {
				return .StopPlaying
			}
		}
		set {
			switch newValue {
			case .LoadNext:
				musicPlayingOutlet.selectCellAtRow(0, column: 1)
				
			case .LoopMusic:
				musicPlayingOutlet.selectCellAtRow(1, column: 0)
				
			case .LoadRandom:
				musicPlayingOutlet.selectCellAtRow(1, column: 1)
				
			default:
				musicPlayingOutlet.selectCellAtRow(0, column: 0)
			}
		}
	}
}
