//
//  ExportObject.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/18/14.
//
//

import Cocoa
import PlayerPROKit

typealias PPExportBlock = @convention(block) (_ theURL: URL, _ errStr: AutoreleasingUnsafeMutablePointer<NSString?>?) -> MADErr
typealias PPSwiftExportBlock = (_ theURL: URL, _ errStr: inout String?) -> MADErr

@objc protocol ExportObjectDelegate: NSObjectProtocol {
	func exportObject(didFinish theObj: ExportObject)
	func exportObject(_ theObj: ExportObject, errorCode errCode: MADErr, errorString errStr: String?)
	func add(exportObject expObj: ExportObject)
}

final class ExportObject: NSObject {
	enum ExportStatus {
		case NotRan
		case Running
		case Done
		case EncounteredError
	}
	
	weak var delegate: ExportObjectDelegate? = nil
	let destination: URL
	private let exportBlock: PPExportBlock
	private(set) var status = ExportStatus.NotRan
	@objc init(destination dest: URL, exportBlock exportCode: @escaping PPExportBlock) {
		exportBlock = exportCode
		destination = dest
		
		super.init()
	}
	
	convenience init(destination dest: URL, block: @escaping PPSwiftExportBlock) {
		let tmpExportBlock: PPExportBlock = { (theURL, errStr) -> MADErr in
			var tmpStr: String? = nil
			let retErr = block(dest, &tmpStr)
			errStr?.pointee = tmpStr as NSString?
			return retErr
		}
		
		self.init(destination: dest, exportBlock: tmpExportBlock)
	}
	
	func run() {
		status = .Running
		DispatchQueue.global().async(execute: { () -> Void in
			var aStr: NSString? = nil
			let errVal = self.exportBlock(self.destination, &aStr)
			if errVal == .noErr {
				DispatchQueue.main.async(execute: { () -> Void in
					self.delegate?.exportObject(didFinish: self)
					return
				})
				self.status = .Done
			} else {
				if aStr == nil {
					let tmpErr = createError(from: errVal)!
					aStr = tmpErr.description as NSString
				}
				let bStr: String = (aStr ?? "Unknown error") as String
				DispatchQueue.main.async(execute: { () -> Void in
					self.delegate?.exportObject(self, errorCode: errVal, errorString: bStr)
					return
				})
				self.status = .EncounteredError
			}
		})
	}
}
