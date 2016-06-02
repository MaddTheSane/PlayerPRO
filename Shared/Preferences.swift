//
//  PPPreferences.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/12/14.
//
//

import Cocoa

extension TagCoupling {
	init(_ theAmount: Int32, _ theTag: Int) {
		amount = theAmount
		tag = theTag
	}
}

final class Preferences: NSWindowController, NSWindowDelegate {
	@IBOutlet weak var box:		NSBox! = nil
	@IBOutlet weak var popUp:	NSPopUpButton! = nil
	private var viewControllers = [NSViewController]()
	
	class func newPreferenceController() -> Self {
		let ourself = self.init(windowNibName: "preferences")
		var tmpControllers = [NSViewController]()
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

		ourself.viewControllers = tmpControllers
		
		return ourself
	}
	
	@IBAction func changeViewController(_ sender: AnyObject!) {
		if let i = (sender as? NSMenuItem)?.tag {
			let vc = viewControllers[i];
			displayViewController(vc)
		}
	}
	
	private func displayViewController(_ vc: NSViewController) {
		//try to end editing
		let w = box.window!
		let ended = w.makeFirstResponder(w)
		
		if (!ended) {
			NSBeep();
			return;
		}
		
		//Put the view in the box
		let v = vc.view;
		
		//Compute the new window frame
		let currentSize = (box.contentView)!.frame.size
		let newSize = v.frame.size;
		let deltaWidth = newSize.width - currentSize.width;
		let deltaHeight = newSize.height - currentSize.height;
		var windowFrame = w.frame;
		windowFrame.size.height += deltaHeight;
		windowFrame.origin.y -= deltaHeight;
		windowFrame.size.width += deltaWidth;
		
		//Clear the box for resizing
		box.contentView = nil
		w.setFrame(windowFrame, display: true, animate: true)
		
		box.contentView = v;
	}

	override var windowNibName: String {
		return "preferences"
	}
	
    override func awakeFromNib() {
        super.awakeFromNib()
    
        // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
		let menu = popUp.menu!
		
		for (i, vc) in viewControllers.enumerated() {
			let mi = NSMenuItem(title: vc.title!, action: #selector(Preferences.changeViewController(_:)), keyEquivalent: "")
			mi.tag = i
			menu.add(mi)
		}
		
		//initially show the first controller
		displayViewController(viewControllers[0])
		popUp.selectItem(at: 0)
    }
    
    @objc(displayViewControllerWithName:) func displayViewController(name: String) {
		let ourController = viewControllers.filter { (aVal) -> Bool in
			if let ourVal = aVal as? /*NSViewController where NSViewController:*/ PPPreferenceObject {
				return ourVal.preferenceIdentifier == name
			} else {
				assert(false, "\(aVal.dynamicType) should implement the \"PPPreferenceObject\" protocol!")
				return false
			}
		}
		assert(ourController.count == 1 || ourController.count == 0, "There should only be one preference class is labeled \(name)")

		if ourController.count == 1 {
			displayViewController(ourController[0])
		}
    }
	
	// MARK: NSWindowDelegate
	func windowWillClose(_ notification: NSNotification) {
		if let aWin = notification.object as? NSWindow {
			if aWin === self.window {
				if NSColorPanel.sharedColorPanelExists() {
					NSColorPanel.shared().close()
				}
			}
		}
	}
}
