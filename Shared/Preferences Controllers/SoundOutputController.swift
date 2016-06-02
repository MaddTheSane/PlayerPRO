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

final class SoundOutputController: SoundSettingsViewController, SoundSettingsViewControllerDelegate, PPPreferenceObject {
	let preferenceIdentifier = PPSoundSettPrefID
	
	class func newPreferenceView() -> Self? {
		guard let ourself = self.init(nibName: "PPSoundSettingsViewController", bundle: NSBundle(for: SoundSettingsViewController.self)) else {
			return nil
		}
		ourself.delegate = (ourself as SoundOutputController)
		let aTitle = NSLocalizedString("Sound Output", tableName:"PreferenceNames", comment: "Sound Output");
		ourself.title = aTitle
		
		return ourself
	}
	
	override class func newSoundSettingWindow() -> Self? {
		guard let ourself = self.init(nibName: "PPSoundSettingsViewController", bundle: NSBundle(for: SoundSettingsViewController.self)) else {
			return nil
		}
		ourself.delegate = (ourself as SoundOutputController)
		let aTitle = NSLocalizedString("Sound Output", tableName:"PreferenceNames", comment: "Sound Output");
		ourself.title = aTitle
		
		return ourself
	}
	
	func soundOutDriverDidChange(_ driv: MADSoundOutput) {
		NSUserDefaults.standard().set(Int(driv.rawValue), forKey:PPSoundDriver);
		
		NSNotificationCenter.default().post(name: PPSoundPreferencesDidChange, object: self)
	}
	
	func soundOutBitsDidChange(_ bits: Int16) {
		NSUserDefaults.standard().set(Int(bits), forKey:PPSoundOutBits);
		
		NSNotificationCenter.default().post(name: PPSoundPreferencesDidChange, object: self)
	}
	
	func soundOutRateDidChange(_ rat: UInt32) {
		NSUserDefaults.standard().set(Int(rat), forKey: PPSoundOutRate);
		
		NSNotificationCenter.default().post(name: PPSoundPreferencesDidChange, object: self)
	}
	
	func soundOutReverbDidChangeActive(_ isAct: Bool) {
		NSUserDefaults.standard().set(isAct, forKey:PPReverbToggle);
		
		NSNotificationCenter.default().post(name: PPSoundPreferencesDidChange, object: self)
	}
	
	func soundOutOversamplingDidChangeActive(_ isAct: Bool) {
		NSUserDefaults.standard().set(isAct, forKey:PPOversamplingToggle);
		
		NSNotificationCenter.default().post(name: PPSoundPreferencesDidChange, object: self)
	}
	
	func soundOutStereoDelayDidChangeActive(_ isAct: Bool) {
		NSUserDefaults.standard().set(isAct, forKey:PPStereoDelayToggle);
		
		NSNotificationCenter.default().post(name: PPSoundPreferencesDidChange, object: self)
	}
	
	func soundOutSurroundDidChangeActive(_ isAct: Bool) {
		NSUserDefaults.standard().set(isAct, forKey:PPSurroundToggle);
		
		NSNotificationCenter.default().post(name: PPSoundPreferencesDidChange, object: self)
	}
	
	func soundOutReverbStrengthDidChange(_ rev: Int32) {
		NSUserDefaults.standard().set(Int(rev), forKey:PPReverbStrength);
		
		NSNotificationCenter.default().post(name: PPSoundPreferencesDidChange, object: self)
	}
	
	func soundOutReverbSizeDidChange(_ rev: Int32) {
		NSUserDefaults.standard().set(Int(rev), forKey:PPReverbAmount);
		
		NSNotificationCenter.default().post(name: PPSoundPreferencesDidChange, object: self)
	}
	
	func soundOutOversamplingAmountDidChange(_ ovs: Int32) {
		NSUserDefaults.standard().set(Int(ovs), forKey:PPOversamplingAmount);
		
		NSNotificationCenter.default().post(name: PPSoundPreferencesDidChange, object: self)
	}
	
	func soundOutStereoDelayAmountDidChange(_ std: Int32) {
		NSUserDefaults.standard().set(Int(std), forKey:PPStereoDelayAmount);
		
		NSNotificationCenter.default().post(name: PPSoundPreferencesDidChange, object: self)
	}
	
	override func awakeFromNib() {
		super.awakeFromNib()
		let defaults = NSUserDefaults.standard()
		
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
}
