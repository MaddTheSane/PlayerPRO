//
//  ExportObject.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/18/14.
//
//

import Cocoa
import PlayerPROKit

typealias PPExportBlock = @objc_block (theURL: NSURL, errStr: AutoreleasingUnsafeMutablePointer<NSString?>) -> MADErr
typealias PPSwiftExportBlock = (theURL: NSURL, inout errStr: String?) -> MADErr

@objc protocol ExportObjectDelegate: NSObjectProtocol {
	@objc(PPExportObjectDidFinish:) func exportObjectDidFinish(theObj: ExportObject)
	@objc(PPExportObjectEncounteredError:errorCode:errorString:) func exportObjectEncounteredError(theObj:ExportObject, errorCode errCode: MADErr, errorString errStr: NSString?)
	@objc(PPExportAddObject:) func addExportObject(expObj: ExportObject)
}

class ExportObject: NSObject {
	weak var delegate: ExportObjectDelegate?
	let destination: NSURL
	let exportBlock: PPExportBlock
	@objc init(destination dest: NSURL, exportBlock exportCode: PPExportBlock) {
		exportBlock = exportCode
		destination = dest
		
		super.init()
	}
	
	convenience init(destination dest: NSURL, block: PPSwiftExportBlock) {
		let tmpExportBlock: PPExportBlock = { (theURL: NSURL, errStr: AutoreleasingUnsafeMutablePointer<NSString?>) -> MADErr in
			var tmpStr: String? = nil
			var retErr = block(theURL: dest, errStr: &tmpStr)
			errStr.memory = tmpStr
			return retErr
		}
		
		self.init(destination: dest, exportBlock: tmpExportBlock)
	}
	
	func run() {
		// TODO: multi-thread this!
		var aStr: NSString? = nil
		let errVal = exportBlock(theURL: destination, errStr: &aStr)
		if errVal == .NoErr {
			delegate?.exportObjectDidFinish(self)
		} else {
			if aStr == nil {
				let tmpErr = CreateErrorFromMADErrorType(errVal)!
				aStr = tmpErr.description
			}
			let bStr: NSString = aStr ?? "Unknown error"
			delegate?.exportObjectEncounteredError(self, errorCode: errVal, errorString: bStr)
		}
	}
}
