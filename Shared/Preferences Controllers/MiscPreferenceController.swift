//
//  MiscPreferenceController.swift
//  PPMacho
//
//  Created by C.W. Betts on 1/5/15.
//
//

import Cocoa
import SwiftAdditions
import MASPreferences.MASPreferencesViewController

final class MiscPreferenceController: NSViewController, MASPreferencesViewController {
	@IBOutlet weak var addExt: NSButtonCell!
	@IBOutlet weak var madCompression: NSButtonCell!
	@IBOutlet weak var noLoadMixerFromFile: NSButtonCell!
	@IBOutlet weak var oscDrawLines: NSButton!
	
	class func newPreferenceView() -> MiscPreferenceController? {
		let aCont = MiscPreferenceController(nibName:NSNib.Name(rawValue: "MiscPrefs"), bundle:nil)
		aCont.title = NSLocalizedString("Misc.", tableName: "PreferenceNames", comment: "Misc.")
		return aCont
	}
	
	override var identifier: NSUserInterfaceItemIdentifier? {
		get {
			return NSUserInterfaceItemIdentifier(rawValue: PPMiscPrefID)
		}
		set {
			//do nothing
		}
	}
	
	var toolbarItemLabel: String? {
		return NSLocalizedString("Misc.", tableName: "PreferenceNames", comment: "Misc.")
	}
	
	var toolbarItemImage: NSImage? {
		return NSImage(named: NSImage.Name.advanced)
	}

	override func awakeFromNib() {
		super.awakeFromNib()
		let defaults = UserDefaults.standard
		addExt.state = defaults.bool(forKey: PPMAddExtension) ? NSControl.StateValue.onState : NSControl.StateValue.offState
		madCompression.state = defaults.bool(forKey: PPMMadCompression) ? NSControl.StateValue.onState : NSControl.StateValue.offState
		noLoadMixerFromFile.state = defaults.bool(forKey: PPMNoLoadMixerFromFiles) ? NSControl.StateValue.onState : NSControl.StateValue.offState
		oscDrawLines.state = defaults.bool(forKey: PPMOscilloscopeDrawLines) ? NSControl.StateValue.onState : NSControl.StateValue.offState
	}
	
	@IBAction func changePrefs(_ sender: AnyObject?) {
		let defaults = UserDefaults.standard
		
		defaults[PPMAddExtension] = addExt.state == .onState
		defaults[PPMMadCompression] = madCompression.state == .onState
		defaults[PPMNoLoadMixerFromFiles] = noLoadMixerFromFile.state == .onState
		defaults[PPMOscilloscopeDrawLines] = oscDrawLines.state == .onState
		
		NotificationCenter.default.post(name: .PPMiscPreferencesDidChange, object: self)
	}
	
	var hasResizableWidth: Bool {
		return false
	}
	
	var hasResizableHeight: Bool {
		return false
	}
}
