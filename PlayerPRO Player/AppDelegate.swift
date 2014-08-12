//
//  AppDelegate.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/11/14.
//
//

import Cocoa
import PlayerPROKit
import PlayerPROCore
import AVFoundation

class AppDelegate: NSObject, NSApplicationDelegate, /*PPSoundSettingsViewControllerDelegate,*/ NSTableViewDelegate, NSToolbarDelegate, NSTableViewDataSource {
	@IBOutlet var window: NSWindow! = nil
	@IBOutlet var loopButton: NSButton!
	@IBOutlet var playButton: NSButton!
	@IBOutlet var stopButton: NSButton!
	@IBOutlet var songCurTime: NSTextField!
	@IBOutlet var songTotalTime: NSTextField!
	@IBOutlet var songPos: NSSlider!
	@IBOutlet var pauseButton: NSButton!
	
	@IBOutlet var aboutPlugInMenu: NSMenu!
	
	@IBOutlet var musicExportMenu: NSMenu!
	
	@IBOutlet var infoDrawer: NSDrawer!
	@IBOutlet var fileName: NSTextField!
	@IBOutlet var internalName: NSTextField!
	@IBOutlet var fileSize: NSTextField!
	@IBOutlet var musicInstrument: NSTextField!
	@IBOutlet var musicPatterns: NSTextField!
	@IBOutlet var musicPlugType: NSTextField!
	@IBOutlet var musicSignature: NSTextField!
	@IBOutlet var fileLocation: NSTextField!
	
	@IBOutlet var tableView: NSTableView!
	@IBOutlet var musicListController: NSArrayController!
	@IBOutlet var exportWindow: NSWindow!
	@IBOutlet var pauseDockMenuItem: NSMenuItem!
	var music: PPMusicObject!
	
	@IBOutlet var toolsPanel: NSPanel!
	var timeChecker: NSTimer!
	var madDriver: PPDriver!
	var madLib: PPLibrary!
	var paused = true
	var isQuitting = false
	var instrumentController: PPInstrumentWindowController! = nil
	
	var exportSettings = MADDriverSettings()
	@IBOutlet var exportSettingsBox: NSBox!
	var exportController: PPSoundSettingsViewController!

	var musicList = PPMusicList()
	var preferences: PPPreferences! = nil
	var plugInInfos = [PPPlugInInfo]()
	
	var currentlyPlayingIndex	= PPCurrentlyPlayingIndex()
	var previouslyPlayingIndex	= PPCurrentlyPlayingIndex()

	@IBAction func showMusicList(sender: AnyObject!) {
		self.window.makeKeyAndOrderFront(sender)
	}
}
