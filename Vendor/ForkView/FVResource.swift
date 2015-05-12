//
//  FVResource.swift
//  ForkView
//
//  Created by Kevin Wojniak on 8/3/14.
//  Copyright (c) 2014 Kevin Wojniak. All rights reserved.
//

import Foundation

final class FVResource: NSObject {
    var ident: UInt16 = 0
    var name: String = ""
    var dataSize: UInt32 = 0
    var dataOffset: UInt32 = 0
    var type: FVResourceType? = nil
    var file: FVResourceFile? = nil
    
    var data: NSData? {
        return file?.dataForResource(self)
    }
}
