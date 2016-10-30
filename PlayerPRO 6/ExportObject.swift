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
	private let exportBlock: PPSwiftExportBlock
	private(set) var status = ExportStatus.NotRan
	@objc convenience init(destination dest: URL, exportBlock exportCode: @escaping PPExportBlock) {
		//exportBlock = exportCode
		//destination = dest
		
		//super.init()
		let tmpExportBlock: PPSwiftExportBlock = { (theURL, errStr) -> MADErr in
			var tmpStr: NSString? = nil
			let retErr = exportCode(dest, &tmpStr)
			if let tmp2Str = tmpStr as String? {
				errStr = tmp2Str
			}
			return retErr
		}
		
		self.init(destination: dest, block: tmpExportBlock)

	}
	
	init(destination dest: URL, block: @escaping PPSwiftExportBlock) {
		exportBlock = block
		destination = dest
		
		super.init()
	}
	
	func run() {
		status = .Running
		DispatchQueue.global().async(execute: { () -> Void in
			var aStr: String? = nil
			let errVal = self.exportBlock(self.destination, &aStr)
			if errVal == .noErr {
				DispatchQueue.main.async(execute: { () -> Void in
					self.delegate?.exportObject(didFinish: self)
					return
				})
				self.status = .Done
			} else {
				if aStr == nil {
					let tmpErr = createNSError(from: errVal)!
					aStr = tmpErr.description
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
