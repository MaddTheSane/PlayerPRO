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
	dynamic var dataSize = 0
	dynamic var samplesNumCount = 0
	dynamic var isNewSize = false
	
	
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do view setup here.
    }
	
	override var nibName: String {
		return "LengthViewController"
	}
	
}
