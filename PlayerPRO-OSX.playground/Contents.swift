//: Playground - noun: a place where people can play

import Cocoa
import SwiftAdditions
import PlayerPROCore
import PlayerPROKit

let aSet: Set<Int> = [0,4,3,6,5,5,3,0,0,1,1,1,1,1,1]
let anArray = [0,4,3,6,5,5,3, 0,0]
let idxSet: NSIndexSet = {
	let toRet = NSMutableIndexSet()
	for i in [0,4,3,6,5,5,3, 0,0] {
		toRet.addIndex(i)
	}
	return toRet
}()

let setIdxSet = NSIndexSet(indexes: aSet)
let arrIdxSet = NSIndexSet(indexes: anArray)

idxSet == setIdxSet
idxSet == arrIdxSet
print(idxSet)
(idxSet as! NSMutableIndexSet).addIndex(1)
print(idxSet)
idxSet == setIdxSet
idxSet == arrIdxSet

let arr: [String] = {
	var inArr = Array(2.stride(through: 17, by: 2))
	
	return Array(inArr.map({return String($0)}))
}()

var arr1 = arr
var arr2 = arr
var arr3 = arr

arr1.removeAtIndexes(anArray)
arr2.removeAtIndexes(arrIdxSet)

arr1 == arr2

print(MadID)

let aSetting = MADDriverSettings.new()
var bSetting = MADDriverSettings()

aSetting == bSetting

bSetting.resetToBestDriver()

aSetting == bSetting

print(aSetting)
