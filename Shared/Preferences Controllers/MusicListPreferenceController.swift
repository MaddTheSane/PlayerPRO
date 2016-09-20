//
//  MusicListPreferenceController.swift
//  PPMacho
//
//  Created by C.W. Betts on 3/4/15.
//
//

import Cocoa

enum PlaylistMode: Int {
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
		
		let defaults = UserDefaults.standard
		
		rememberMusicList.state = defaults.bool(forKey: PPRememberMusicList) ? NSOnState : NSOffState
		loadFirstMusic.state = defaults.bool(forKey: PPLoadMusicAtListLoad) ? NSOnState : NSOffState
		goToStartupPos.state = defaults.bool(forKey: PPGotoStartupAfterPlaying) ? NSOnState : NSOffState
		askToSave.state = defaults.bool(forKey: PPSaveModList) ? NSOnState : NSOffState
		autoPlay.state = defaults.bool(forKey: PPLoadMusicAtMusicLoad) ? NSOnState : NSOffState
		loopMusic.state = defaults.bool(forKey: PPLoopMusicWhenDone) ? NSOnState : NSOffState
		playedMusic = PlaylistMode(rawValue: defaults.integer(forKey: PPAfterPlayingMusic)) ?? .StopPlaying
	}
	
	@IBAction func changePlayedMusic(_ sender: AnyObject?) {
		let defaults = UserDefaults.standard
		
		defaults.set(playedMusic.rawValue, forKey: PPAfterPlayingMusic)
		
		NotificationCenter.default.post(name: .PPListPreferencesDidChange, object: self)
	}
	
	@IBAction func toggleButton(_ sender: AnyObject?) {
		let defaults = UserDefaults.standard
		
		defaults.set(rememberMusicList.state == NSOnState, forKey: PPRememberMusicList)
		defaults.set(loadFirstMusic.state == NSOnState, forKey: PPLoadMusicAtListLoad)
		defaults.set(autoPlay.state == NSOnState, forKey: PPLoadMusicAtMusicLoad)
		defaults.set(goToStartupPos.state == NSOnState, forKey: PPGotoStartupAfterPlaying)
		defaults.set(loopMusic.state == NSOnState, forKey: PPLoopMusicWhenDone)
		defaults.set(goToStartupPos.state == NSOnState, forKey: PPGotoStartupAfterPlaying)
		
		NotificationCenter.default.post(name: .PPListPreferencesDidChange, object: self)
	}
	
	var playedMusic: PlaylistMode {
		get {
			let curSelected: AnyObject! = musicPlayingOutlet.selectedCell()
			
			if musicPlayingOutlet.cell(atRow: 0, column: 0) === curSelected {
				return .StopPlaying
			} else if musicPlayingOutlet.cell(atRow: 0, column: 1) === curSelected {
				return .LoadNext
			} else if musicPlayingOutlet.cell(atRow: 1, column: 0) === curSelected {
				return .LoopMusic
			} else if musicPlayingOutlet.cell(atRow: 0, column: 1) === curSelected {
				return .LoadRandom
			} else {
				return .StopPlaying
			}
		}
		set {
			switch newValue {
			case .LoadNext:
				musicPlayingOutlet.selectCell(atRow: 0, column: 1)
				
			case .LoopMusic:
				musicPlayingOutlet.selectCell(atRow: 1, column: 0)
				
			case .LoadRandom:
				musicPlayingOutlet.selectCell(atRow: 1, column: 1)
				
			default:
				musicPlayingOutlet.selectCell(atRow: 0, column: 0)
			}
		}
	}
}
