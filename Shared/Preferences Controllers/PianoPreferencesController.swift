//
//  PianoPreferencesController.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/12/14.
//
//

import Cocoa

class PianoPreferencesController: NSViewController, PlayerPROPreference {
	required init(coder: NSCoder!) {
		super.init(coder: coder)
	}
	
	required override init(nibName nibNameOrNil: String!, bundle nibBundleOrNil: NSBundle!) {
		super.init(nibName: nibNameOrNil, bundle: nibBundleOrNil)
	}
	
	class func newPreferenceView() -> Self {
		var nullBundle: NSBundle! = nil
		var ourself = self(nibName: "PianoPrefs", bundle: nullBundle)
		
		var aTitle = NSLocalizedString("Piano", tableName:"PreferenceNames", comment: "Piano");
		ourself.title = aTitle
		return ourself
	}

    override func awakeFromNib() {
        super.awakeFromNib()
        // Do view setup here.
    }
    
}
