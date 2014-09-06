//
//  ExportObject.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/18/14.
//
//

import Cocoa
import PlayerPROKit

enum ExportStatus {
	case NotRan
	case Running
	case Done
	case EncounteredError
}

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
	private let exportBlock: PPExportBlock
	private(set) var status = ExportStatus.NotRan
	@objc init(destination dest: NSURL, exportBlock exportCode: PPExportBlock) {
		exportBlock = exportCode
		destination = dest
		
		super.init()
	}
	
	convenience init(destination dest: NSURL, block: PPSwiftExportBlock) {
		let tmpExportBlock: PPExportBlock = { (theURL: NSURL, errStr: AutoreleasingUnsafeMutablePointer<NSString?>) -> MADErr in
			var tmpStr: String? = nil
			let retErr = block(theURL: dest, errStr: &tmpStr)
			errStr.memory = tmpStr
			return retErr
		}
		
		self.init(destination: dest, exportBlock: tmpExportBlock)
	}
	
	func run() {
		status = .Running
		// TODO: multi-thread this!
		var aStr: NSString? = nil
		let errVal = exportBlock(theURL: destination, errStr: &aStr)
		if errVal == .NoErr {
			delegate?.exportObjectDidFinish(self)
			status = .Done
		} else {
			if aStr == nil {
				let tmpErr = CreateErrorFromMADErrorType(errVal)!
				aStr = tmpErr.description
			}
			let bStr: NSString = aStr ?? "Unknown error"
			delegate?.exportObjectEncounteredError(self, errorCode: errVal, errorString: bStr)
			status = .EncounteredError
		}
	}
}
