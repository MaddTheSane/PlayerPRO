//: Playground - noun: a place where people can play

import Cocoa
import PlayerPROCore
import PlayerPROKit

println(MadID)

let aSetting = MADDriverSettings.new()
var bSetting = MADDriverSettings()

aSetting == bSetting

bSetting.resetToBestDriver()

aSetting == bSetting

println(aSetting)

var str = "Hello, playground"
