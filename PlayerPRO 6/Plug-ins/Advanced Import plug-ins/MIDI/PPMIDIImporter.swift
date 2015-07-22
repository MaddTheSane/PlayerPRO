//
//  PPMIDIImporter.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/18/14.
//
//

import Cocoa
import PlayerPROCore
import PlayerPROKit

private func ==(rhs: NSData, lhs: NSData) -> Bool {
	return rhs.isEqualToData(lhs)
}

@objc(PPMIDIImporter) final public class MIDIImporter: NSObject, PPComplexImportPlugInterface {
	
	public convenience init(forPlugIn: ()) {
		self.init()
	}
	
	override public init() {
		super.init()
	}

	public func beginImportOfURL(theURL: NSURL, withHandler handler: PPComplexImportHandler) {
		let aImp = MIDIImporterController.newWithLocation(theURL, handler: handler)
		aImp.beginImportModalSession()
	}
	
	public func canImportURL(theURL: NSURL) throws {
		var outErr: NSError! = NSError(domain: "Migrator", code: 0, userInfo: nil)
		func getHeaderData() -> NSData {
			let headerData: [Int8] = [0x4D, 0x54, 0x68, 0x64, 0, 0, 0, 6, 0]
			return NSData(bytes: headerData, length: headerData.count)
		}
		//var myErr = MADErr.NoErr;
		do {
			let aFile = try NSFileHandle(forReadingFromURL:theURL)
			let fileData = aFile.readDataOfLength(9)
			aFile.closeFile()
			let headerData = getHeaderData()
			
			if fileData.isEqualToData(headerData) {
				return
			} else {
				outErr = createErrorFromMADErrorType(.FileNotSupportedByThisPlug)
			}
		}
	}
}
