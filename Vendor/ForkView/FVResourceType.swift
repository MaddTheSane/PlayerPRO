//
//  FVResourceType.swift
//  ForkView
//
//  Created by Kevin Wojniak on 8/3/14.
//  Copyright (c) 2014 Kevin Wojniak. All rights reserved.
//

import Foundation

final public class FVResourceType: NSObject {
    public var type: OSType = 0
    public var count: UInt32 = 0
    public var offset: UInt32 = 0
    public var resources: NSArray = []

    public var typeString: String {
		if let aType = UTCreateStringForOSType(type)?.takeRetainedValue() as? String {
			return aType
		}
		
        return String(format:"%c%c%c%c",
            (type & 0xFF000000) >> 24,
            (type & 0x00FF0000) >> 16,
            (type & 0x0000FF00) >> 8,
            (type & 0x000000FF)
        );
    }
};
