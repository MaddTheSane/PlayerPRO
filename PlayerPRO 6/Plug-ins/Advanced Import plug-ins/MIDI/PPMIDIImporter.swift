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
	
	public func canImportURL(theURL: NSURL, error outErr: NSErrorPointer) -> Bool {
		func getHeaderData() -> NSData {
			var headerData = NSMutableData(data: "MThd".dataUsingEncoding(NSASCIIStringEncoding, allowLossyConversion: true)!)
			let headerAddlData: [Int8] = [0, 0, 0, 6, 0]
			headerData.appendBytes(headerAddlData, length: headerAddlData.count)
			return NSData(data: headerData)
		}
		var myErr = MADErr.NoErr;
		if let aFile = NSFileHandle(forReadingFromURL:theURL, error: nil) {
			let fileData = aFile.readDataOfLength(9)
			aFile.closeFile()
			let headerData = getHeaderData()
			
			if fileData == headerData {
				return true
			} else {
				if outErr != nil {
					outErr.memory = CreateErrorFromMADErrorType(.FileNotSupportedByThisPlug)
				}
			}
		} else {
			if outErr != nil {
				outErr.memory = CreateErrorFromMADErrorType(.ReadingErr)
			}
		}
		return false
	}
}
