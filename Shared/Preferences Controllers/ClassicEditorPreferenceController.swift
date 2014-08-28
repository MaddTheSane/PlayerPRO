//
//  ClassicEditorPreferenceController.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/12/14.
//
//

import Cocoa

let TrackHeightCoupling = [TagCoupling(100, 1), TagCoupling(110, 2), TagCoupling(120, 3), TagCoupling(130, 4), TagCoupling(140, 5),
	TagCoupling(160, 6), TagCoupling(180, 7), TagCoupling(200, 8)];

class ClassicEditorPreferenceController: NSViewController, PPPreferenceObject {
	@IBOutlet var tempoNumberValue:		NSTextField!
	@IBOutlet var markersLoopValue:		NSTextField!
	@IBOutlet var markersOffsetValue:	NSTextField!
	@IBOutlet var markersCheck:			NSButton!
	@IBOutlet var notesLengthCheck:		NSButton!
	@IBOutlet var trackHeightButton:	NSPopUpButton!
	@IBOutlet var tempoUnitValue:		NSTextField!
	
	class func newPreferenceView() -> Self {
		var nullBundle: NSBundle! = nil
		var ourself = self(nibName: "ClassicPrefs", bundle: nullBundle)
		
		var aTitle = NSLocalizedString("Classic Editor", tableName:"PreferenceNames", comment: "Classic Editor");
		ourself.title = aTitle
		return ourself
	}
	
	required init(coder: NSCoder!) {
		super.init(coder: coder)
	}
	
	required override init(nibName nibNameOrNil: String!, bundle nibBundleOrNil: NSBundle!) {
		super.init(nibName: nibNameOrNil, bundle: nibBundleOrNil)
	}
	
	override func awakeFromNib() {
		super.awakeFromNib()
		// Do view setup here.
		let defaults = NSUserDefaults.standardUserDefaults()
		
		let aRate = Int32(defaults.integerForKey(PPCETrackHeight))
		
		var toSet = 0;
		for iaa in TrackHeightCoupling {
			if aRate == iaa.amount {
				toSet = iaa.tag;
				break;
			}
		}
		if (toSet == 0) {
			toSet = 4;
		}
		trackHeightButton.selectItemAtIndex(toSet - 1)
		
		tempoUnitValue.integerValue = defaults.integerForKey(PPCETempoUnit)
		markersLoopValue.integerValue = defaults.integerForKey(PPCEMarkerLoop)
		markersOffsetValue.integerValue = defaults.integerForKey(PPCEMarkerOffset)
		notesLengthCheck.state = defaults.boolForKey(PPCEShowNotesLen) ? NSOnState : NSOffState
		
		let markersVal = defaults.boolForKey(PPCEShowMarkers)
		markersLoopValue.enabled = markersVal
		markersOffsetValue.enabled = markersVal
		markersCheck.state = markersVal ? NSOnState : NSOffState
	}
	
	@IBAction func changeTrackHeight(sender: AnyObject!) {
		
	}
	
	@IBAction func toggleNoteLength(sender: AnyObject!) {
		
	}
	
	@IBAction func toggleMarkers(sender: AnyObject!) {
		
	}

}
