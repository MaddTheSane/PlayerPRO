//
//  ExportObject.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/18/14.
//
//

import Cocoa

typealias PPExportBlock = @objc_block (theURL: NSURL, errStr: AutoreleasingUnsafeMutablePointer<NSString?>) -> MADErr

@objc /*(PPExportObjectDelegate)*/ protocol ExportObjectDelegate: NSObjectProtocol {
	@objc(PPExportObjectDidFinish:) func ExportObjectDidFinish(theObj: ExportObject)
	@objc(PPExportObjectEncounteredError:errorCode:errorString:) func ExportObjectEncounteredError(theObj:ExportObject, errorCode errCode: MADErr, errorString errStr: NSString?)
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
	
	func run() {
		#if false
		dispatch_async(dispatch_get_global_queue(0, 0), { [weak self] () -> Void in
			var retErr = MADErr.NoErr;
			var errStr: String? = nil;
			
			retErr = self!.exportBlock(theURL: self!.destination, errStr: &errStr);
			
			if (self!.delegate != nil) {
				dispatch_async(dispatch_get_main_queue(), { [weak self] () -> Void in
					var errStr2: String? = errStr
					if (retErr == MADErr.NoErr) {
						self!.delegate?.PPExportObjectDidFinish(self!)
					} else {
						if (errStr == nil) {
							let tmpErr = CreateErrorFromMADErrorType(retErr)
							errStr2 = tmpErr.description
						}
						self!.delegate?.PPExportObjectEncounteredError(self!, errorCode: retErr, errorString: errStr2 != nil ? errStr2! : "Unknown error")
					}
				})
			}
		})
			#elseif false
			var retErr = MADErr.NoErr;
			var errStr: String? = nil;
			
			retErr = exportBlock(self.destination, &errStr);
			
			if (delegate != nil) {
					var errStr2: String? = errStr
					if (retErr == MADErr.NoErr) {
						self.delegate?.PPExportObjectDidFinish(self)
					} else {
						if (errStr == nil) {
							let tmpErr = CreateErrorFromMADErrorType(retErr)
							errStr = tmpErr.description
						}
						self.delegate?.PPExportObjectEncounteredError(self, errorCode: retErr, errorString: errStr != nil ? errStr! : "Unknown error")
					}
			}

			#endif
	}
}
