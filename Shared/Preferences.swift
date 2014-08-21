//
//  PPPreferences.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/12/14.
//
//

import Cocoa

enum PPPlaylistMode: Int {
	case StopPlaying = 0
	case LoopMusic
	case LoadNext
	case LoadRandom
};

class Preferences: NSWindowController {
	@IBOutlet var box: NSBox! = nil
	@IBOutlet var popUp: NSPopUpButton! = nil
	private var viewControllers = [NSViewController]()
	
	class func newPreferenceController() -> Self {
		var ourself = self(windowNibName: "preferences")
		var tmpControllers = [NSViewController]()
		tmpControllers.append(SoundOutputController.newPreferenceView())
		#if PLAYERPRO6
			tmpControllers.append(PianoPreferencesController.newPreferenceView())
			tmpControllers.append(ColorPreferenceController.newPreferenceView())
			tmpControllers.append(BoxEditorPreferenceController.newPreferenceView())
			tmpControllers.append(DigitalEditorPreferenceControler.newPreferenceView())
			tmpControllers.append(ClassicEditorPreferenceController.newPreferenceView())
		#endif
		
		tmpControllers.append(MidiHardwarePreferenceController.newPreferenceView())
		#if !PLAYERPRO6
			tmpControllers.append(MusicListPreferenceController.newPreferenceView())
		#endif
		tmpControllers.append(MiscPreferenceController.newPreferenceView())

		ourself.viewControllers = tmpControllers
		
		return ourself
	}
	
	@IBAction func changeViewController(sender: AnyObject!) {
		var i = (sender as NSMenuItem).tag
		var vc = viewControllers[i];
		displayViewController(vc)
	}
	
	func displayViewController(vc: NSViewController) {
		//try to end editing
		var w = box.window;
		var ended = w!.makeFirstResponder(w)
		
		if (!ended) {
			NSBeep();
			return;
		}
		
		//Put the view in the box
		var v = vc.view;
		
		//Compute the new window frame
		var currentSize = box.contentView.frame.size
		var newSize = v.frame.size;
		var deltaWidth = newSize.width - currentSize.width;
		var deltaHeight = newSize.height - currentSize.height;
		var windowFrame = w!.frame;
		windowFrame.size.height += deltaHeight;
		windowFrame.origin.y -= deltaHeight;
		windowFrame.size.width += deltaWidth;
		
		//Clear the box for resizing
		box.contentView = nil
		w!.setFrame(windowFrame, display: true, animate: true)
		
		box.contentView = v;
	}

	override var windowNibName: String {get {
		return "preferences"
		}}
	
    override func awakeFromNib() {
        super.awakeFromNib()
    
        // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
		var menu = popUp.menu
		var itemCount = viewControllers.count
		
		for (i, vc) in enumerate(viewControllers) {
			//var vc = viewControllers[i];
			var mi = NSMenuItem(title: vc.title, action: "changeViewController:", keyEquivalent: "")
			mi.tag = i
			menu.addItem(mi)
		}
		
		//initially show the first controller
		displayViewController(viewControllers[0])
		popUp.selectItemAtIndex(0)
    }

}
