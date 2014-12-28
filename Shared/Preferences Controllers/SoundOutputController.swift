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
		return self.newSoundSettingWindow()
	}
	
	override class func newSoundSettingWindow() -> Self? {
		if let ourself = self(nibName: "PPSoundSettingsViewController", bundle: NSBundle(forClass: SoundSettingsViewController.self)) {
			ourself.delegate = (ourself as SoundOutputController)
			var aTitle = NSLocalizedString("Sound Output", tableName:"PreferenceNames", comment: "Sound Output");
			ourself.title = aTitle
			
			return ourself
		} else {
			return nil
		}
	}
	
	func soundOutDriverDidChange(driv: MADSoundOutput) {
		NSUserDefaults.standardUserDefaults().setInteger(Int(driv.rawValue), forKey:PPSoundDriver);
		
		NSNotificationCenter.defaultCenter().postNotificationName(PPSoundPreferencesDidChange, object: self)
	}
	
	func soundOutBitsDidChange(bits: Int16) {
		NSUserDefaults.standardUserDefaults().setInteger(Int(bits), forKey:PPSoundOutBits);
		
		NSNotificationCenter.defaultCenter().postNotificationName(PPSoundPreferencesDidChange, object: self)
	}
	
	func soundOutRateDidChange(rat: UInt32) {
		NSUserDefaults.standardUserDefaults().setInteger(Int(rat), forKey:PPSoundOutBits);
		
		NSNotificationCenter.defaultCenter().postNotificationName(PPSoundPreferencesDidChange, object: self)
	}
	
	func soundOutReverbDidChangeActive(isAct: Bool) {
		NSUserDefaults.standardUserDefaults().setBool(isAct, forKey:PPReverbToggle);
		
		NSNotificationCenter.defaultCenter().postNotificationName(PPSoundPreferencesDidChange, object: self)
	}
	
	func soundOutOversamplingDidChangeActive(isAct: Bool) {
		NSUserDefaults.standardUserDefaults().setBool(isAct, forKey:PPOversamplingToggle);
		
		NSNotificationCenter.defaultCenter().postNotificationName(PPSoundPreferencesDidChange, object: self)
	}
	
	func soundOutStereoDelayDidChangeActive(isAct: Bool) {
		NSUserDefaults.standardUserDefaults().setBool(isAct, forKey:PPStereoDelayToggle);
		
		NSNotificationCenter.defaultCenter().postNotificationName(PPSoundPreferencesDidChange, object: self)
	}
	
	func soundOutSurroundDidChangeActive(isAct: Bool) {
		NSUserDefaults.standardUserDefaults().setBool(isAct, forKey:PPSurroundToggle);
		
		NSNotificationCenter.defaultCenter().postNotificationName(PPSoundPreferencesDidChange, object: self)
	}
	
	func soundOutReverbStrengthDidChange(rev: Int32) {
		NSUserDefaults.standardUserDefaults().setInteger(Int(rev), forKey:PPReverbStrength);
		
		NSNotificationCenter.defaultCenter().postNotificationName(PPSoundPreferencesDidChange, object: self)
	}
	
	func soundOutReverbSizeDidChange(rev: Int32) {
		NSUserDefaults.standardUserDefaults().setInteger(Int(rev), forKey:PPReverbAmount);
		
		NSNotificationCenter.defaultCenter().postNotificationName(PPSoundPreferencesDidChange, object: self)
	}
	
	func soundOutOversamplingAmountDidChange(ovs: Int32) {
		NSUserDefaults.standardUserDefaults().setInteger(Int(ovs), forKey:PPOversamplingAmount);
		
		NSNotificationCenter.defaultCenter().postNotificationName(PPSoundPreferencesDidChange, object: self)
	}
	
	func soundOutStereoDelayAmountDidChange(std: Int32) {
		NSUserDefaults.standardUserDefaults().setInteger(Int(std), forKey:PPStereoDelayAmount);
		
		NSNotificationCenter.defaultCenter().postNotificationName(PPSoundPreferencesDidChange, object: self)
	}
	
	override func awakeFromNib() {
		super.awakeFromNib()
		let defaults = NSUserDefaults.standardUserDefaults()
		
		var drivSet = MADDriverSettings();
		
		drivSet.surround = defaults.boolForKey(PPSurroundToggle);
		drivSet.driverMode = MADSoundOutput(rawValue: Int16(defaults.integerForKey(PPSoundDriver)))!
		drivSet.outPutBits = Int16(defaults.integerForKey(PPSoundOutBits))
		drivSet.outPutRate = UInt32(defaults.integerForKey(PPSoundOutRate))
		drivSet.outPutMode = .DeluxeStereoOutPut;
		if (defaults.boolForKey(PPOversamplingToggle)) {
			drivSet.oversampling = Int32(defaults.integerForKey(PPOversamplingAmount))
		} else {
			drivSet.oversampling = 1;
		}
		drivSet.Reverb = defaults.boolForKey(PPReverbToggle)
		drivSet.ReverbSize = Int32(defaults.integerForKey(PPReverbAmount))
		drivSet.ReverbStrength = Int32(defaults.integerForKey(PPReverbStrength))
		if (defaults.boolForKey(PPStereoDelayToggle)) {
			drivSet.MicroDelaySize = Int32(defaults.integerForKey(PPStereoDelayAmount))
		} else {
			drivSet.MicroDelaySize = 0;
		}
		
		settingsFromDriverSettings(drivSet);
	}
}
