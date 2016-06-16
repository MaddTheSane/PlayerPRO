//
//  FVResourceFile.swift
//  PPMacho
//
//  Created by C.W. Betts on 5/12/15.
//
//

import Foundation

extension NSError {
    private class func errorWithDescription(_ description: String) -> NSError {
        return NSError(domain: "FVResourceErrorDomain", code: -1, userInfo: [NSLocalizedFailureReasonErrorKey: description])
    }
}

private func ==(lhs: FVResourceFile.ResourceHeader, rhs: FVResourceFile.ResourceHeader) -> Bool {
    if lhs.dataOffset != rhs.dataOffset {
        return false
    } else if lhs.mapOffset != rhs.mapOffset {
        return false
    } else if lhs.dataLength != rhs.dataLength {
        return false
    } else if lhs.mapLength != rhs.mapLength {
        return false
    }
    
    return true
}

final class FVResourceFile: NSObject {
    private(set) var types: [FVResourceType] = []
    private(set) var isResourceFork = false
    private var header = ResourceHeader()
    private var map = ResourceMap()
    private let dataReader: FVDataReader!
    
    private struct ResourceHeader: Equatable {
        var dataOffset: UInt32 = 0
        var mapOffset: UInt32 = 0
        var dataLength: UInt32 = 0
        var mapLength: UInt32 = 0
    }
    
    private struct ResourceMap {
        var headerCopy = ResourceHeader()
        var nextMap: UInt32 = 0
        var fileRef: UInt16 = 0
        var attributes: UInt16 = 0
        var typesOffset: UInt16 = 0
        var namesOffset: UInt16 = 0
    }

    private func readHeader(_ aHeader: inout ResourceHeader) -> Bool {
        // read the header values

        if (!dataReader.read(CUnsignedInt(sizeofValue(aHeader.dataOffset)), into: &aHeader.dataOffset) ||
            !dataReader.read(CUnsignedInt(sizeofValue(aHeader.mapOffset)), into: &aHeader.mapOffset) ||
            !dataReader.read(CUnsignedInt(sizeofValue(aHeader.dataLength)), into: &aHeader.dataLength) ||
            !dataReader.read(CUnsignedInt(sizeofValue(aHeader.mapLength)), into: &aHeader.mapLength)) {
                return false
        }
        
        // swap from big endian to host
        aHeader.dataOffset = aHeader.dataOffset.bigEndian;
        aHeader.mapOffset = aHeader.mapOffset.bigEndian;
        aHeader.dataLength = aHeader.dataLength.bigEndian;
        aHeader.mapLength = aHeader.mapLength.bigEndian;

        let fileLength = UInt32(dataReader.length)
        if ((aHeader.dataOffset + aHeader.dataLength > fileLength) || (aHeader.mapOffset + aHeader.mapLength > fileLength)) {
            return false
        }

        return true
    }

    //- (instancetype)initWithContentsOfURL:(NSURL *)fileURL error:(NSError **)error resourceFork:(BOOL)resourceFork
    private init(contentsOfURL fileURL: URL, resourceFork: Bool) throws {
        if let dataReader = FVDataReader(URL: fileURL, resourceFork: resourceFork) {
            self.dataReader = dataReader
            super.init()
            
            if !readHeader(&header) {
                
                throw NSError.errorWithDescription("Invalid header.")
            }
            
            //NSLog(@"HEADER (%u, %u), (%u, %u)", header->dataOffset, header->dataLength, header->mapOffset, header->mapLength);
            
            if !readMap() {
                throw NSError.errorWithDescription("Invalid map.");
            }
            
            if !readTypes() {
                throw NSError.errorWithDescription("Invalid types list.");
            }
            
            // Don't open empty (but valid) resource forks
            if types.count == 0 {
                throw NSError.errorWithDescription("No resources.");
            }
            return
        } else {
            self.dataReader = nil
            super.init()
            
            throw NSError.errorWithDescription("Bad File.")
        }
    }
    
    private func readMap() -> Bool {
        // seek to the map offset
        if !dataReader.seekTo(Int(header.mapOffset)) {
            return false;
        }
        
        // read the map values
        if !readHeader(&map.headerCopy) {
            return false;
        }

        let zeros = [Int8](repeating: 0, count: 16)
        if (map.headerCopy != header) && (memcmp(&map.headerCopy, zeros, zeros.count) != 0) {
            print("Bad match!")
        }
        
        if (!dataReader.read(CUnsignedInt(sizeofValue(map.nextMap)), into: &map.nextMap) ||
            !dataReader.read(CUnsignedInt(sizeofValue(map.fileRef)), into: &map.fileRef) ||
            !dataReader.read(CUnsignedInt(sizeofValue(map.attributes)), into: &map.attributes) ||
            !dataReader.read(CUnsignedInt(sizeofValue(map.typesOffset)), into: &map.typesOffset) ||
            !dataReader.read(CUnsignedInt(sizeofValue(map.namesOffset)), into: &map.namesOffset)) {
                return false
        }
        
        map.nextMap = map.nextMap.bigEndian
        map.fileRef = map.fileRef.bigEndian
        map.attributes = map.attributes.bigEndian
        map.typesOffset = map.typesOffset.bigEndian
        map.namesOffset = map.namesOffset.bigEndian

        //NSLog(@"MAP (%u, %u, %x, %u, %u)", map->nextMap, map->fileRef, map->attributes, map->typesOffset, map->namesOffset);

        return true
    }
    
    private func readTypes() -> Bool {
        let typesOffset = dataReader.position
        
        var numberOfTypes: UInt16 = 0
        if !dataReader.read(CUnsignedInt(sizeofValue(numberOfTypes)), into: &numberOfTypes) {
            return false
        }
        numberOfTypes = numberOfTypes.bigEndian + 1
        
        
        var typesTemp = [FVResourceType]()
        for _ in 0..<numberOfTypes {
            var type: OSType = 0
            var numberOfResources: UInt16 = 0
            var referenceListOffset: UInt16 = 0
            if !dataReader.read(CUnsignedInt(sizeofValue(type)), into: &type) ||
                !dataReader.read(CUnsignedInt(sizeofValue(numberOfResources)), into: &numberOfResources) ||
                !dataReader.read(CUnsignedInt(sizeofValue(referenceListOffset)), into: &referenceListOffset) {
                    return false;
            }
            type = type.bigEndian
            numberOfResources = numberOfResources.bigEndian
            referenceListOffset = referenceListOffset.bigEndian
            
            let obj = FVResourceType()
            obj.type = type;
            obj.count = UInt32(numberOfResources)
            obj.offset = UInt32(referenceListOffset)
            typesTemp.append(obj)
        }
        
        for obj in typesTemp {
            var tmpResources = [FVResource]()
            
            for resIndex in 0..<obj.count {
                if !dataReader.seekTo(typesOffset + Int(obj.offset) + Int(resIndex * 12)) {
                    return false
                }
                
                var resourceID: UInt16 = 0
                var nameOffset: Int16 = 0
                var attributes: UInt8 = 0
                var dataOffsetBytes: (UInt8, UInt8, UInt8) = (0, 0, 0)
                var resHandle: UInt32 = 0
                
                if !dataReader.read(UInt32(sizeofValue(resourceID)), into: &resourceID) ||
                    !dataReader.read(UInt32(sizeofValue(nameOffset)), into: &nameOffset) ||
                    !dataReader.read(UInt32(sizeofValue(attributes)), into: &attributes) ||
                    !dataReader.read(UInt32(sizeofValue(dataOffsetBytes)), into: &dataOffsetBytes) ||
                    !dataReader.read(UInt32(sizeofValue(resHandle)), into: &resHandle) {
                        return false;
                }
                
                resourceID = resourceID.bigEndian
                nameOffset = nameOffset.bigEndian
                resHandle = resHandle.bigEndian
                
                let dataOffset: UInt32 = {
                    var toRet = UInt32(dataOffsetBytes.2)
                    toRet |= UInt32(dataOffsetBytes.0) << 16
                    toRet |= UInt32(dataOffsetBytes.1) << 8

                    return toRet
                }()
                
                var name = Array<Int8>(repeating: 0, count: 256)
                var nameLength: UInt8 = 0
                
                if (nameOffset != -1) && (dataReader.seekTo(Int(header.mapOffset) + Int(map.namesOffset) + Int(nameOffset))) {
                    if !dataReader.read(UInt32(sizeofValue(nameLength)), into: &nameLength) || !dataReader.read(UInt32(nameLength), into: &name) {
                        nameLength = 0;
                    }
                }
                name[Int(nameLength)] = 0;
                
                var dataLength: UInt32 = 0
                if dataReader.seekTo(Int(header.dataOffset + dataOffset)) && dataReader.read(UInt32(sizeofValue(dataLength)), into: &dataLength) {
                    dataLength = dataLength.bigEndian
                }

                //NSLog(@"%@[%u] %u %s", obj.typeString, resourceID, dataLength, name);
                let resource = FVResource()
                resource.ident = resourceID
                resource.dataSize = dataLength
                resource.dataOffset = dataOffset + UInt32(sizeofValue(dataOffset))
                if strlen(name) != 0 {
                    resource.name = String(cString: name, encoding: String.Encoding.macOSRoman)!
                }
                resource.file = self
                resource.type = obj
                tmpResources.append(resource)
            }
            tmpResources.sort(isOrderedBefore: { (lhs, rhs) -> Bool in
                return lhs.ident > rhs.ident
            })
            obj.resources = tmpResources
        }
        
        typesTemp.sort { (lhs, rhs) -> Bool in
            let compVal = lhs.typeString.caseInsensitiveCompare(rhs.typeString)
            return compVal == .orderedAscending
        }
        
        types = typesTemp
        return true
    }

    internal func dataForResource(_ resource: FVResource) -> Data? {
        if !dataReader.seekTo(Int(header.dataOffset + resource.dataOffset)) {
            return nil;
        }
        if let data = NSMutableData(length: Int(resource.dataSize)) {
            if !dataReader.read(resource.dataSize, into: data.mutableBytes) {
                return nil;
            }
            return data as Data
        }
        return nil
    }
    
    class func resourceFileWithContentsOfURL(_ fileURL: URL) throws -> FVResourceFile {
        var tmpError: NSError?
        
        do {
            let file = try FVResourceFile(contentsOfURL: fileURL, resourceFork: true)
            file.isResourceFork = true
            return file
        } catch let error1 as NSError {
            tmpError = error1
            do {
                let file = try FVResourceFile(contentsOfURL: fileURL, resourceFork: false)
                return file
            } catch let error as NSError {
                tmpError = error
            }
        }
        throw tmpError!
    }
}
