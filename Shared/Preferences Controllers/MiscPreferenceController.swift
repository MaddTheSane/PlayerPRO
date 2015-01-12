//
//  MiscPreferenceController.swift
//  PPMacho
//
//  Created by C.W. Betts on 1/5/15.
//
//

import Cocoa
import SwiftAdditions

final class MiscPreferenceController: NSViewController, PPPreferenceObject {
	@IBOutlet weak var addExt: NSButtonCell!
	@IBOutlet weak var madCompression: NSButtonCell!
	@IBOutlet weak var noLoadMixerFromFile: NSButtonCell!
	@IBOutlet weak var oscDrawLines: NSButton!
	
	class func newPreferenceView() -> MiscPreferenceController? {
		let aCont = MiscPreferenceController(nibName:"MiscPrefs", bundle:nil)
		aCont?.title = NSLocalizedString("Misc.", tableName: "PreferenceNames", comment: "Misc.")
		return aCont
	}
	
	let preferenceIdentifier = PPMiscPrefID
	
	override func awakeFromNib() {
		super.awakeFromNib()
		let defaults = NSUserDefaults.standardUserDefaults()
		addExt.state = defaults.boolForKey(PPMAddExtension) ? NSOnState : NSOffState
		madCompression.state = defaults.boolForKey(PPMMadCompression) ? NSOnState : NSOffState
		noLoadMixerFromFile.state = defaults.boolForKey(PPMNoLoadMixerFromFiles) ? NSOnState : NSOffState
		oscDrawLines.state = defaults.boolForKey(PPMOscilloscopeDrawLines) ? NSOnState : NSOffState
	}
	
	@IBAction func changePrefs(sender: AnyObject?) {
		let defaults = NSUserDefaults.standardUserDefaults()
		
		defaults[PPMAddExtension] = addExt.state == NSOnState
		defaults[PPMMadCompression] = madCompression.state == NSOnState
		defaults[PPMNoLoadMixerFromFiles] = noLoadMixerFromFile.state == NSOnState
		defaults[PPMOscilloscopeDrawLines] = oscDrawLines.state == NSOnState
		
		NSNotificationCenter.defaultCenter().postNotificationName(PPMiscPreferencesDidChange, object: self)
	}
}
