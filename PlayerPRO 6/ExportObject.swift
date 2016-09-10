//
//  ExportObject.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/18/14.
//
//

import Cocoa
import PlayerPROKit

typealias PPExportBlock = @convention(block) (theURL: NSURL, errStr: AutoreleasingUnsafeMutablePointer<NSString?>) -> MADErr
typealias PPSwiftExportBlock = (theURL: NSURL, inout errStr: String?) -> MADErr

@objc protocol ExportObjectDelegate: NSObjectProtocol {
	@objc(PPExportObjectDidFinish:) func exportObjectDidFinish(theObj: ExportObject)
	@objc(PPExportObjectEncounteredError:errorCode:errorString:) func exportObjectEncounteredError(theObj: ExportObject, errorCode errCode: MADErr, errorString errStr: String?)
	@objc(PPExportAddObject:) func addExportObject(expObj: ExportObject)
}

final class ExportObject: NSObject {
	enum ExportStatus {
		case NotRan
		case Running
		case Done
		case EncounteredError
	}
	
	weak var delegate: ExportObjectDelegate? = nil
	let destination: NSURL
	private let exportBlock: PPExportBlock
	private(set) var status = ExportStatus.NotRan
	@objc init(destination dest: NSURL, exportBlock exportCode: PPExportBlock) {
		exportBlock = exportCode
		destination = dest
		
		super.init()
	}
	
	convenience init(destination dest: NSURL, block: PPSwiftExportBlock) {
		let tmpExportBlock: PPExportBlock = { (theURL, errStr) -> MADErr in
			var tmpStr: String? = nil
			let retErr = block(theURL: dest, errStr: &tmpStr)
			errStr.memory = tmpStr
			return retErr
		}
		
		self.init(destination: dest, exportBlock: tmpExportBlock)
	}
	
	func run() {
		status = .Running
		dispatch_async(dispatch_get_global_queue(0, 0), { () -> Void in
			var aStr: NSString? = nil
			let errVal = self.exportBlock(theURL: self.destination, errStr: &aStr)
			if errVal == .NoErr {
				dispatch_async(dispatch_get_main_queue(), { () -> Void in
					self.delegate?.exportObjectDidFinish(self)
					return
				})
				self.status = .Done
			} else {
				if aStr == nil {
					let tmpErr = createNSErrorFromMADErrorType(errVal)!
					aStr = tmpErr.description
				}
				let bStr: String = (aStr ?? "Unknown error") as String
				dispatch_async(dispatch_get_main_queue(), { () -> Void in
					self.delegate?.exportObjectEncounteredError(self, errorCode: errVal, errorString: bStr)
					return
				})
				self.status = .EncounteredError
			}
		})
	}
}
