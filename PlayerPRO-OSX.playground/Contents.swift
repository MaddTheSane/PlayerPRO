//: Playground - noun: a place where people can play

import Cocoa
import PlayerPROCore
import PlayerPROKit

print(MadID)

let aSetting = MADDriverSettings.new()
var bSetting = MADDriverSettings()

aSetting == bSetting

bSetting.resetToBestDriver()

aSetting == bSetting

print(aSetting)

var str = "Hello, playground"
