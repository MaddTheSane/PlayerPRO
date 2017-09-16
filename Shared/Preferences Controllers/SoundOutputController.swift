//
//  SoundOutputController.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/7/14.
//
//

import Cocoa
import PlayerPROCore
import PlayerPROKit
import MASPreferences.MASPreferencesViewController
import SwiftAdditions

final class SoundOutputController: SoundSettingsViewController, SoundSettingsViewWithDriverControllerDelegate, MASPreferencesViewController {
	override var identifier: NSUserInterfaceItemIdentifier? {
		get {
			return NSUserInterfaceItemIdentifier(rawValue: PPSoundSettPrefID)
		}
		set {
			//do nothing
		}
	}
	
	var toolbarItemLabel: String? {
		return NSLocalizedString("Sound Output", tableName: "PreferenceNames", comment: "Sound Output")
	}
	
	var toolbarItemImage: NSImage? {
		// Try to use system-available image. Because the icon might change in the future
		if let localAppDir = (try? FileManager.default.url(for: .applicationDirectory, in: .localDomainMask, appropriateFor: nil, create: false))?.appendingPathComponent("Utilities").appendingPathComponent("Audio MIDI Setup.app"),
			let audMidSetupBundle = Bundle(url: localAppDir),
			let soundImg = audMidSetupBundle.image(forResource: NSImage.Name("SpeakerIcon")) {
			return soundImg
		}
		// Fall-back to known icon
		return #imageLiteral(resourceName: "Preferences/Sound")
	}
	
	class func newPreferenceView() -> Self? {
		 let ourself = self.init(nibName: NSNib.Name(rawValue: "PPSoundSettingsViewController"), bundle: Bundle(for: SoundSettingsViewController.self))
		ourself.delegate = (ourself as SoundOutputController)
		let aTitle = NSLocalizedString("Sound Output", tableName:"PreferenceNames", comment: "Sound Output");
		ourself.title = aTitle
		
		return ourself
	}
	
	override class func newSoundSettingWindow() -> Self? {
		 let ourself = self.init(nibName: NSNib.Name(rawValue: "PPSoundSettingsViewController"), bundle: Bundle(for: SoundSettingsViewController.self)) 
		ourself.delegate = (ourself as SoundOutputController)
		let aTitle = NSLocalizedString("Sound Output", tableName:"PreferenceNames", comment: "Sound Output");
		ourself.title = aTitle
		
		return ourself
	}
	
	func soundView(_ view: SoundSettingsViewController, driverDidChange driv: MADSoundOutput) {
		UserDefaults.standard[PPSoundDriver] = Int(driv.rawValue)
		
		NotificationCenter.default.post(name: .PPSoundPreferencesDidChange, object: self)
	}
	
	func soundView(_ view: SoundSettingsViewController, bitsDidChange bits: Int16) {
		UserDefaults.standard[PPSoundOutBits] = Int(bits)
		
		NotificationCenter.default.post(name: .PPSoundPreferencesDidChange, object: self)
	}
	
	func soundView(_ view: SoundSettingsViewController, rateDidChange rat: UInt32) {
		UserDefaults.standard[PPSoundOutRate] = Int(rat)
		
		NotificationCenter.default.post(name: .PPSoundPreferencesDidChange, object: self)
	}
	
	func soundView(_ view: SoundSettingsViewController, reverbDidChangeActive isAct: Bool) {
		UserDefaults.standard[PPReverbToggle] = isAct
		
		NotificationCenter.default.post(name: .PPSoundPreferencesDidChange, object: self)
	}
	
	func soundView(_ view: SoundSettingsViewController, oversamplingDidChangeActive isAct: Bool) {
		UserDefaults.standard[PPOversamplingToggle] = isAct
		
		NotificationCenter.default.post(name: .PPSoundPreferencesDidChange, object: self)
	}
	
	func soundView(_ view: SoundSettingsViewController, stereoDelayDidChangeActive isAct: Bool) {
		UserDefaults.standard[PPStereoDelayToggle] = isAct
		
		NotificationCenter.default.post(name: .PPSoundPreferencesDidChange, object: self)
	}
	
	func soundView(_ view: SoundSettingsViewController, surroundDidChangeActive isAct: Bool) {
		UserDefaults.standard[PPSurroundToggle] = isAct
		
		NotificationCenter.default.post(name: .PPSoundPreferencesDidChange, object: self)
	}
	
	func soundView(_ view: SoundSettingsViewController, reverbStrengthDidChange rev: Int32) {
		UserDefaults.standard[PPReverbStrength] = Int(rev)
		
		NotificationCenter.default.post(name: .PPSoundPreferencesDidChange, object: self)
	}
	
	func soundView(_ view: SoundSettingsViewController, reverbSizeDidChange rev: Int32) {
		UserDefaults.standard[PPReverbAmount] = Int(rev)
		
		NotificationCenter.default.post(name: .PPSoundPreferencesDidChange, object: self)
	}
	
	func soundView(_ view: SoundSettingsViewController, oversamplingAmountDidChange ovs: Int32) {
		UserDefaults.standard[PPOversamplingAmount] = Int(ovs)
		
		NotificationCenter.default.post(name: .PPSoundPreferencesDidChange, object: self)
	}
	
	func soundView(_ view: SoundSettingsViewController, stereoDelayAmountDidChange std: Int32) {
		UserDefaults.standard[PPStereoDelayAmount] = Int(std)
		
		NotificationCenter.default.post(name: .PPSoundPreferencesDidChange, object: self)
	}
	
	override func awakeFromNib() {
		super.awakeFromNib()
		let defaults = UserDefaults.standard
		
		var drivSet = MADDriverSettings.new()
		
		drivSet.surround = defaults[PPSurroundToggle]!
		drivSet.driverMode = MADSoundOutput(rawValue: Int16(defaults.integer(forKey: PPSoundDriver))) ?? .CoreAudioDriver
		drivSet.outPutBits = Int16(defaults.integer(forKey: PPSoundOutBits))
		drivSet.outPutRate = UInt32(defaults.integer(forKey: PPSoundOutRate))
		drivSet.outPutMode = .DeluxeStereoOutPut;
		if (defaults.bool(forKey: PPOversamplingToggle)) {
			drivSet.oversampling = Int32(defaults.integer(forKey: PPOversamplingAmount))
		} else {
			drivSet.oversampling = 1;
		}
		drivSet.Reverb = defaults[PPReverbToggle]!
		drivSet.ReverbSize = Int32(defaults[PPReverbAmount]! as Int)
		drivSet.ReverbStrength = Int32(defaults[PPReverbStrength]! as Int)
		if defaults[PPStereoDelayToggle]! {
			drivSet.MicroDelaySize = Int32(defaults[PPStereoDelayAmount]! as Int)
		} else {
			drivSet.MicroDelaySize = 0;
		}
		
		settingsFromDriverSettings(drivSet);
	}
	
	var hasResizableWidth: Bool {
		return false
	}
	
	var hasResizableHeight: Bool {
		return false
	}
}
