//
//  PPPreferences.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/12/14.
//
//

import Cocoa
import MASPreferences

extension TagCoupling {
	init(_ theAmount: Int32, _ theTag: Int) {
		amount = theAmount
		tag = theTag
	}
}

final class Preferences: MASPreferencesWindowController {
	
	class func newPreferenceController() -> Self {
		var tmpControllers = [NSViewController & MASPreferencesViewController]()
		
		tmpControllers.append(SoundOutputController.newPreferenceView()!)
		#if PLAYERPRO6
			tmpControllers.append(PianoPreferencesController.newPreferenceView()!)
			tmpControllers.append(ColorPreferenceController.newPreferenceView()!)
			tmpControllers.append(BoxEditorPreferenceController.newPreferenceView()!)
			tmpControllers.append(DigitalEditorPreferenceControler.newPreferenceView()!)
			tmpControllers.append(ClassicEditorPreferenceController.newPreferenceView()!)
		#endif
		
		tmpControllers.append(MidiHardwarePreferenceController.newPreferenceView()!)
		#if !PLAYERPRO6
			tmpControllers.append(MusicListPreferenceController.newPreferenceView()!)
		#endif
		tmpControllers.append(MiscPreferenceController.newPreferenceView()!)
		return self.init(viewControllers: tmpControllers, title: "Preferences")
	}
	
	// MARK: NSWindowDelegate
	override func windowWillClose(_ notification: Notification) {
		if let aWin = notification.object as? NSWindow,
			aWin === self.window,
			NSColorPanel.sharedColorPanelExists {
			NSColorPanel.shared.close()
		}
	}
}
