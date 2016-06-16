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
    private var data = Data()
    private(set) var position = 0
    
    var length: Int {
        get {
            return data.count
        }
    }
    
    init(_ data: Data) {
        self.data = data
    }
    
    init?(URL: Foundation.URL, resourceFork: Bool) {
        // Apple's docs say "The maximum size of the resource fork in a file is 16 megabytes"
        let maxResourceSize = 16777216
        if !resourceFork {
            var fileSize: AnyObject?
            do {
                try (URL as NSURL).getResourceValue(&fileSize, forKey: URLResourceKey.fileSizeKey)
            } catch _ {
            }
            let fileSizeNum = fileSize as? NSNumber
            if fileSizeNum == nil {
                return nil
            }
            if fileSizeNum!.intValue == 0 || fileSizeNum!.intValue >= maxResourceSize {
                return nil
            }
            if let data = try? Data(contentsOf: URL) {
                self.data = data
            } else {
                return nil
            }
        } else {
            let rsrcSize = getxattr((URL as NSURL).fileSystemRepresentation, XATTR_RESOURCEFORK_NAME, nil, 0, 0, 0)
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
    
    class func dataReader(_ URL: Foundation.URL, resourceFork: Bool) -> FVDataReader? {
        return FVDataReader(URL: URL, resourceFork: resourceFork)
    }
    
    func read(_ size: Int) -> Data? {
        if (position + size > self.length) {
            return nil
        }
        let subdata = data.subdata(in: Range<Int>(position...size))
        position += size
        return subdata
    }
    
    func read(_ size: CUnsignedInt, into buf: UnsafeMutablePointer<Void>) -> Bool {
        let data = self.read(Int(size))
        if data == nil {
            return false
        }
		(data! as NSData).getBytes(buf, length: Int(size))
        return true
    }
    
    func seekTo(_ offset: Int) -> Bool {
        if (offset >= self.length) {
            return false
        }
        position = offset
        return true
    }
    
    func readUInt16(_ endian: ByteOrder, _ val: inout UInt16) -> Bool {
        if let dat = read(sizeof(UInt16)) {
			(dat as NSData).getBytes(&val, length: sizeof(UInt16))
            switch endian {
            case .big:
                val = val.bigEndian
                
            case .little:
                val = val.littleEndian
                
            case .unknown:
                break;
            }
            return true
        }
        return false
    }

    func readInt16(_ endian: ByteOrder, _ val: inout Int16) -> Bool {
        if let dat = read(sizeof(Int16)) {
            (dat as NSData).getBytes(&val, length: sizeof(Int16))
            switch endian {
            case .big:
                val = val.bigEndian
                
            case .little:
                val = val.littleEndian
                
            case .unknown:
                break;
            }
            return true
        }
        return false
    }

    func readUInt32(_ endian: ByteOrder, _ val: inout UInt32) -> Bool {
        if let dat = read(sizeof(UInt32)) {
            (dat as NSData).getBytes(&val, length: sizeof(UInt32))
            switch endian {
            case .big:
                val = val.bigEndian
                
            case .little:
                val = val.littleEndian
                
            case .unknown:
                break;
            }
            return true
        }
        return false
    }
    
    func readInt32(_ endian: ByteOrder, _ val: inout Int32) -> Bool {
        if let dat = read(sizeof(Int32)) {
            (dat as NSData).getBytes(&val, length: sizeof(Int32))
            switch endian {
            case .big:
                val = val.bigEndian
                
            case .little:
                val = val.littleEndian
                
            case .unknown:
                break;
            }
            return true
        }
        return false
    }
    
    func readUInt8(_ val: inout UInt8) -> Bool {
        if let dat = read(sizeof(UInt8)) {
            (dat as NSData).getBytes(&val, length: sizeof(UInt8))
            return true
        }
        return false
    }

    func readInt8(_ val: inout Int8) -> Bool {
        if let dat = read(sizeof(Int8)) {
            (dat as NSData).getBytes(&val, length: sizeof(Int8))
            return true
        }
        return false
    }
}
