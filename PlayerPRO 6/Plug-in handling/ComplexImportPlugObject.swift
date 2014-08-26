//
//  ComplexImportPlugObject.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/26/14.
//
//

import Cocoa

class ComplexImportPlugObject: PPPlugInObject, PPComplexImportPlugInterfaceBase {
	let UTITypes: [String]
	var plugInterface: PPComplexImportPlugInterface?
	override init(bundle ourBundle: NSBundle) {
		var bundClass: AnyClass = ourBundle.principalClass!
		var nsBundClass = bundClass as NSObject.Type
		//if (bundClass.conformsToProtocol("PPComplexImportPlugInterface")) {
		//
		//}
		//plugInterface = bundClass()
		var tempDict = ourBundle.infoDictionary
		//[tempDict addEntriesFromDictionary:[ourBundle localizedInfoDictionary]];
		let DictionaryTemp: AnyObject? = tempDict[kMadPlugUTITypesKey]
		if let unWrapped = DictionaryTemp as? String {
			UTITypes = [unWrapped]
		} else if let unWrapped = DictionaryTemp as? NSArray {
			UTITypes = unWrapped as [String]
		} else {
			UTITypes = []
		}
		
		super.init(bundle: ourBundle)
	}
	
	override var type: MADFourChar { get {
		return "CImp"
		}}
	
	func canImportURL(theURL: NSURL, error outErr: NSErrorPointer) -> Bool {
		return plugInterface!.canImportURL(theURL, error: outErr)
	}
	
	func beginImportOfURL(theURL: NSURL, withHandler handler: PPComplexImportHandler!) {
		plugInterface!.beginImportOfURL(theURL, withHandler: handler)
	}
}
