//
//  FVResource.swift
//  ForkView
//
//  Created by Kevin Wojniak on 8/3/14.
//  Copyright (c) 2014 Kevin Wojniak. All rights reserved.
//

import Foundation

final public class FVResource: NSObject {
    public var ident: UInt16 = 0
    public var name: String = ""
    public var dataSize: UInt32 = 0
    public var dataOffset: UInt32 = 0
    public var type: FVResourceType? = nil
    public var file: FVResourceFile? = nil
    
    public var data: NSData? {
        return file?.dataForResource(self)
    }
}
