//
//  LengthViewController.swift
//  PPMacho
//
//  Created by C.W. Betts on 11/4/14.
//
//

import Cocoa

class LengthViewController: NSViewController {
	@IBOutlet weak var samplesCount: NSTextField? = nil
	@objc dynamic var dataSize = 0
	@objc dynamic var samplesNumCount = 0
	@objc dynamic var isNewSize = false
	
	
    @available(OSX 10.10, *)
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do view setup here.
    }
	
	override var nibName: NSNib.Name? {
		return NSNib.Name(rawValue: "LengthViewController")
	}
	
}
