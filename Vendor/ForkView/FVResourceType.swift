//
//  FVResourceType.swift
//  ForkView
//
//  Created by Kevin Wojniak on 8/3/14.
//  Copyright (c) 2014 Kevin Wojniak. All rights reserved.
//

import Foundation
import SwiftAdditions

final class FVResourceType: NSObject {
    var type: OSType = 0
    var count: UInt32 = 0
    var offset: UInt32 = 0
    var resources: [FVResource] = []

    var typeString: String {
        if let aType = OSTypeToString(type) {
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
