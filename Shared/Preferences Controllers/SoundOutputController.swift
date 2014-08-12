//
//  SoundOutputController.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/7/14.
//
//

import Cocoa
import PlayerPROKit

class SoundOutputController: PPSoundSettingsViewController, PPSoundSettingsViewControllerDelegate, PlayerPROPreference {
	
	class func newPreferenceView() -> Self {
		return self.newSoundSettingWindow()
	}
	
	override class func newSoundSettingWindow() -> Self {
		var ourself = self()
		ourself.delegate = (ourself as SoundOutputController)
		var aTitle = NSLocalizedString("Sound Output", tableName:"PreferenceNames", comment: "Sound Output");
		ourself.title = aTitle
		
		return ourself
	}
	
	/*
	convenience required init(coder: NSCoder!) {
		self.init()
	}
	
	convenience override init() {
		self.init(nibName: "PPSoundSettingsViewController", bundle: NSBundle(forClass: PPSoundSettingsViewController.self))
	}
	override init(nibName nibNameOrNil: String?, bundle nibBundleOrNil: NSBundle?) {
		super.init(nibName: nibNameOrNil, bundle: nibBundleOrNil)
		
		self.delegate = self;
		var aTitle = NSLocalizedString("Sound Output", tableName:"PreferenceNames", comment: "Sound Output");
		self.title = aTitle
	}*/
	
	func soundOutDriverDidChange(driv: Int16) {
		NSUserDefaults.standardUserDefaults().setInteger(Int(driv), forKey:PPSoundDriver);
		
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
		var defaults = NSUserDefaults.standardUserDefaults()
		
		var drivSet = MADDriverSettings();
		
		drivSet.surround = defaults.boolForKey(PPSurroundToggle);
		drivSet.driverMode = MADSoundOutput.fromRaw(Int16(defaults.integerForKey(PPSoundDriver)))!;
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
		
		settingsFromDriverSettings(&drivSet);
		super.awakeFromNib()
	}
	
}
