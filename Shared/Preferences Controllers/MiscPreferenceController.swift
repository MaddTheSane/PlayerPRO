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
		let aCont = MiscPreferenceController(nibName: NSNib.Name("MiscPrefs"), bundle: nil)
		aCont.title = NSLocalizedString("Misc.", tableName: "PreferenceNames", comment: "Misc.")
		return aCont
	}
	
	override var identifier: NSUserInterfaceItemIdentifier? {
		get {
			return .PPMiscPrefID
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
		addExt.state = defaults.bool(forKey: PPMAddExtension) ? .on : .off
		madCompression.state = defaults.bool(forKey: PPMMadCompression) ? .on : .off
		noLoadMixerFromFile.state = defaults.bool(forKey: PPMNoLoadMixerFromFiles) ? .on : .off
		oscDrawLines.state = defaults.bool(forKey: PPMOscilloscopeDrawLines) ? .on : .off
	}
	
	@IBAction func changePrefs(_ sender: AnyObject?) {
		let defaults = UserDefaults.standard
		
		defaults[PPMAddExtension] = addExt.state == .on
		defaults[PPMMadCompression] = madCompression.state == .on
		defaults[PPMNoLoadMixerFromFiles] = noLoadMixerFromFile.state == .on
		defaults[PPMOscilloscopeDrawLines] = oscDrawLines.state == .on
		
		NotificationCenter.default.post(name: .PPMiscPreferencesDidChange, object: self)
	}
	
	var hasResizableWidth: Bool {
		return false
	}
	
	var hasResizableHeight: Bool {
		return false
	}
}
