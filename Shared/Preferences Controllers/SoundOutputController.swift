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

final class SoundOutputController: SoundSettingsViewController, SoundSettingsViewWithDriverControllerDelegate, MASPreferencesViewController {
	override var identifier: String? {
		get {
		return PPSoundSettPrefID
		}
		set {
			//do nothing
		}
	}
	
	var toolbarItemLabel: String? {
		return NSLocalizedString("Sound Output", tableName: "PreferenceNames", comment: "Sound Output")
	}
	
	var toolbarItemImage: NSImage? {
		return nil //TODO: implement
	}
	
	class func newPreferenceView() -> Self? {
		guard let ourself = self.init(nibName: "PPSoundSettingsViewController", bundle: Bundle(for: SoundSettingsViewController.self)) else {
			return nil
		}
		ourself.delegate = (ourself as SoundOutputController)
		let aTitle = NSLocalizedString("Sound Output", tableName:"PreferenceNames", comment: "Sound Output");
		ourself.title = aTitle
		
		return ourself
	}
	
	override class func newSoundSettingWindow() -> Self? {
		guard let ourself = self.init(nibName: "PPSoundSettingsViewController", bundle: Bundle(for: SoundSettingsViewController.self)) else {
			return nil
		}
		ourself.delegate = (ourself as SoundOutputController)
		let aTitle = NSLocalizedString("Sound Output", tableName:"PreferenceNames", comment: "Sound Output");
		ourself.title = aTitle
		
		return ourself
	}
	
	func soundView(_ view: SoundSettingsViewController, driverDidChange driv: MADSoundOutput) {
		UserDefaults.standard.set(Int(driv.rawValue), forKey:PPSoundDriver);
		
		NotificationCenter.default.post(name: .PPSoundPreferencesDidChange, object: self)
	}
	
	func soundView(_ view: SoundSettingsViewController, bitsDidChange bits: Int16) {
		UserDefaults.standard.set(Int(bits), forKey:PPSoundOutBits);
		
		NotificationCenter.default.post(name: .PPSoundPreferencesDidChange, object: self)
	}
	
	func soundView(_ view: SoundSettingsViewController, rateDidChange rat: UInt32) {
		UserDefaults.standard.set(Int(rat), forKey: PPSoundOutRate);
		
		NotificationCenter.default.post(name: .PPSoundPreferencesDidChange, object: self)
	}
	
	func soundView(_ view: SoundSettingsViewController, reverbDidChangeActive isAct: Bool) {
		UserDefaults.standard.set(isAct, forKey:PPReverbToggle);
		
		NotificationCenter.default.post(name: .PPSoundPreferencesDidChange, object: self)
	}
	
	func soundView(_ view: SoundSettingsViewController, oversamplingDidChangeActive isAct: Bool) {
		UserDefaults.standard.set(isAct, forKey:PPOversamplingToggle);
		
		NotificationCenter.default.post(name: .PPSoundPreferencesDidChange, object: self)
	}
	
	func soundView(_ view: SoundSettingsViewController, stereoDelayDidChangeActive isAct: Bool) {
		UserDefaults.standard.set(isAct, forKey:PPStereoDelayToggle);
		
		NotificationCenter.default.post(name: .PPSoundPreferencesDidChange, object: self)
	}
	
	func soundView(_ view: SoundSettingsViewController, surroundDidChangeActive isAct: Bool) {
		UserDefaults.standard.set(isAct, forKey:PPSurroundToggle);
		
		NotificationCenter.default.post(name: .PPSoundPreferencesDidChange, object: self)
	}
	
	func soundView(_ view: SoundSettingsViewController, reverbStrengthDidChange rev: Int32) {
		UserDefaults.standard.set(Int(rev), forKey:PPReverbStrength);
		
		NotificationCenter.default.post(name: .PPSoundPreferencesDidChange, object: self)
	}
	
	func soundView(_ view: SoundSettingsViewController, reverbSizeDidChange rev: Int32) {
		UserDefaults.standard.set(Int(rev), forKey:PPReverbAmount);
		
		NotificationCenter.default.post(name: .PPSoundPreferencesDidChange, object: self)
	}
	
	func soundView(_ view: SoundSettingsViewController, oversamplingAmountDidChange ovs: Int32) {
		UserDefaults.standard.set(Int(ovs), forKey:PPOversamplingAmount);
		
		NotificationCenter.default.post(name: .PPSoundPreferencesDidChange, object: self)
	}
	
	func soundView(_ view: SoundSettingsViewController, stereoDelayAmountDidChange std: Int32) {
		UserDefaults.standard.set(Int(std), forKey:PPStereoDelayAmount);
		
		NotificationCenter.default.post(name: .PPSoundPreferencesDidChange, object: self)
	}
	
	override func awakeFromNib() {
		super.awakeFromNib()
		let defaults = UserDefaults.standard
		
		var drivSet = MADDriverSettings.new()
		
		drivSet.surround = defaults.bool(forKey: PPSurroundToggle);
		drivSet.driverMode = MADSoundOutput(rawValue: Int16(defaults.integer(forKey: PPSoundDriver))) ?? .CoreAudioDriver
		drivSet.outPutBits = Int16(defaults.integer(forKey: PPSoundOutBits))
		drivSet.outPutRate = UInt32(defaults.integer(forKey: PPSoundOutRate))
		drivSet.outPutMode = .DeluxeStereoOutPut;
		if (defaults.bool(forKey: PPOversamplingToggle)) {
			drivSet.oversampling = Int32(defaults.integer(forKey: PPOversamplingAmount))
		} else {
			drivSet.oversampling = 1;
		}
		drivSet.Reverb = defaults.bool(forKey: PPReverbToggle)
		drivSet.ReverbSize = Int32(defaults.integer(forKey: PPReverbAmount))
		drivSet.ReverbStrength = Int32(defaults.integer(forKey: PPReverbStrength))
		if (defaults.bool(forKey: PPStereoDelayToggle)) {
			drivSet.MicroDelaySize = Int32(defaults.integer(forKey: PPStereoDelayAmount))
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
