//
//  APPLObject.swift
//  PPMacho
//
//  Created by C.W. Betts on 11/29/14.
//
//

import Foundation

final class APPLObject: NSObject {
	let resourceID: ResID
	let resourceType: ResType
	let size: Int
	let name: String
	
	init(handle resHandle: Handle) {
		var aresourceID: ResID = 0
		var aresourceType: ResType = 0
		var asize = 0
		var aname: NSString? = nil
		PPGetResInfo(resHandle, &aresourceID, &aresourceType, &asize, &aname)

		resourceID = aresourceID
		resourceType = aresourceType
		size = asize
		name = aname ?? ""
		super.init()
	}
	
	var data: NSData? {
		return PPAPPLDataFromResource(resourceID, resourceType)
	}
}
