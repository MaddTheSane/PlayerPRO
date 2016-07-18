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
		let defaults = UserDefaults.standard
		addExt.state = defaults.bool(forKey: PPMAddExtension) ? NSOnState : NSOffState
		madCompression.state = defaults.bool(forKey: PPMMadCompression) ? NSOnState : NSOffState
		noLoadMixerFromFile.state = defaults.bool(forKey: PPMNoLoadMixerFromFiles) ? NSOnState : NSOffState
		oscDrawLines.state = defaults.bool(forKey: PPMOscilloscopeDrawLines) ? NSOnState : NSOffState
	}
	
	@IBAction func changePrefs(_ sender: AnyObject?) {
		let defaults = UserDefaults.standard
		
		defaults[PPMAddExtension] = addExt.state == NSOnState
		defaults[PPMMadCompression] = madCompression.state == NSOnState
		defaults[PPMNoLoadMixerFromFiles] = noLoadMixerFromFile.state == NSOnState
		defaults[PPMOscilloscopeDrawLines] = oscDrawLines.state == NSOnState
		
		NotificationCenter.default.post(name: .miscPreferencesDidChange, object: self)
	}
}
