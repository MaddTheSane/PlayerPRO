//
//  ExportObject.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/18/14.
//
//

import Cocoa

typealias PPExportBlock = (theURL: NSURL, errStr: AutoreleasingUnsafeMutablePointer<String?>) -> MADErr

protocol PPExportObjectDelegate: NSObjectProtocol {
	func PPExportObjectDidFinish(theObj: ExportObject)
	func PPExportObjectEncounteredError(theObj:ExportObject, errorCode errCode: MADErr, errorString errStr: String?)
}

class ExportObject: NSObject {
	weak var delegate: PPExportObjectDelegate?
	var destination: NSURL
	var exportBlock: PPExportBlock
	init(destination dest: NSURL, exportBlock exportCode:PPExportBlock) {
		self.exportBlock = exportCode
		self.destination = dest
		
		super.init()
	}
	
	func run() {
		dispatch_async(dispatch_get_global_queue(0, 0)) {
			var retErr = MADErr.NoErr;
			var errStr: String? = nil;
			
			retErr = self.exportBlock(theURL: self.destination, errStr: &errStr);
			
			if (self.delegate != nil) {
				dispatch_async(dispatch_get_main_queue()) {
					if (retErr == .NoErr) {
						self.delegate?.PPExportObjectDidFinish(self)
					} else {
						if (errStr == nil) {
							var tmpErr = CreateErrorFromMADErrorType(retErr);
							errStr = tmpErr.description;
						}
						self.delegate?.PPExportObjectEncounteredError(self, errorCode: retErr, errorString: errStr)
					}
				}
			}
		}
	}
}
