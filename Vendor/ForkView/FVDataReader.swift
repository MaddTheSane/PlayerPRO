//
//  FVDataReader.swift
//  ForkView
//
//  Created by Kevin Wojniak on 5/2/15.
//  Copyright (c) 2015 Kevin Wojniak. All rights reserved.
//

import Foundation
import SwiftAdditions
import Darwin.POSIX.sys.xattr

final class FVDataReader {
    private var data = NSData()
    private(set) var position = 0
    
    var length: Int {
        get {
            return data.length
        }
    }
    
    init(_ data: NSData) {
        self.data = data
    }
    
    init?(URL: NSURL, resourceFork: Bool) {
        // Apple's docs say "The maximum size of the resource fork in a file is 16 megabytes"
        let maxResourceSize = 16777216
        if !resourceFork {
            var fileSize: AnyObject?
            do {
                try URL.getResourceValue(&fileSize, forKey: NSURLFileSizeKey)
            } catch _ {
            }
            let fileSizeNum = fileSize as? NSNumber
            if fileSizeNum == nil {
                return nil
            }
            if fileSizeNum!.integerValue == 0 || fileSizeNum!.integerValue >= maxResourceSize {
                return nil
            }
            if let data = NSData(contentsOfURL: URL) {
                self.data = data
            } else {
                return nil
            }
        } else {
            let rsrcSize = getxattr(URL.fileSystemRepresentation, XATTR_RESOURCEFORK_NAME, nil, 0, 0, 0)
            if rsrcSize <= 0 || rsrcSize >= maxResourceSize {
                return nil
            }
            if let data = NSMutableData(length: rsrcSize) {
                if getxattr(URL.path!, XATTR_RESOURCEFORK_NAME, data.mutableBytes, rsrcSize, 0, 0) != rsrcSize {
                    return nil
                }
            } else {
                return nil
            }
        }
    }
    
    class func dataReader(URL: NSURL, resourceFork: Bool) -> FVDataReader? {
        return FVDataReader(URL: URL, resourceFork: resourceFork)
    }
    
    func read(size: Int) -> NSData? {
        if (position + size > self.length) {
            return nil
        }
        let subdata = data.subdataWithRange(NSMakeRange(position, size))
        position += size
        return subdata
    }
    
    func read(size: CUnsignedInt, into buf: UnsafeMutablePointer<Void>) -> Bool {
        let data = self.read(Int(size))
        if data == nil {
            return false
        }
		data!.getBytes(buf, length: Int(size))
        return true
    }
    
    func seekTo(offset: Int) -> Bool {
        if (offset >= self.length) {
            return false
        }
        position = offset
        return true
    }
    
    func readUInt16(endian: ByteOrder, inout _ val: UInt16) -> Bool {
        if let dat = read(sizeof(UInt16)) {
			dat.getBytes(&val, length: sizeof(UInt16))
            switch endian {
            case .Big:
                val = val.bigEndian
                
            case .Little:
                val = val.littleEndian
                
            case .Unknown:
                break;
            }
            return true
        }
        return false
    }

    func readInt16(endian: ByteOrder, inout _ val: Int16) -> Bool {
        if let dat = read(sizeof(Int16)) {
            dat.getBytes(&val, length: sizeof(Int16))
            switch endian {
            case .Big:
                val = val.bigEndian
                
            case .Little:
                val = val.littleEndian
                
            case .Unknown:
                break;
            }
            return true
        }
        return false
    }

    func readUInt32(endian: ByteOrder, inout _ val: UInt32) -> Bool {
        if let dat = read(sizeof(UInt32)) {
            dat.getBytes(&val, length: sizeof(UInt32))
            switch endian {
            case .Big:
                val = val.bigEndian
                
            case .Little:
                val = val.littleEndian
                
            case .Unknown:
                break;
            }
            return true
        }
        return false
    }
    
    func readInt32(endian: ByteOrder, inout _ val: Int32) -> Bool {
        if let dat = read(sizeof(Int32)) {
            dat.getBytes(&val, length: sizeof(Int32))
            switch endian {
            case .Big:
                val = val.bigEndian
                
            case .Little:
                val = val.littleEndian
                
            case .Unknown:
                break;
            }
            return true
        }
        return false
    }
    
    func readUInt8(inout val: UInt8) -> Bool {
        if let dat = read(sizeof(UInt8)) {
            dat.getBytes(&val, length: sizeof(UInt8))
            return true
        }
        return false
    }

    func readInt8(inout val: Int8) -> Bool {
        if let dat = read(sizeof(Int8)) {
            dat.getBytes(&val, length: sizeof(Int8))
            return true
        }
        return false
    }
}
