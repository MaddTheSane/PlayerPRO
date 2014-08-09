//
//  OpenPanelViewController.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/6/14.
//
//

import Cocoa

class OpenPanelViewController: NSViewController {
	private var openPanel: NSOpenPanel!
	private var utiObjects = [OpenPanelViewItem]()
	@IBOutlet var popUp: NSPopUpButton! = nil
	
	var allowsMultipleSelectionOfTrackers:Bool { get {
		return self.allowsMultipleSelectionOfTrackers
	}
	set {
		self.allowsMultipleSelectionOfTrackers = newValue
		openPanel.allowsMultipleSelection = self.allowsMultipleSelectionOfTrackers
	}}
	
	required init(coder: NSCoder!) {
		
		super.init(coder: coder)
	}
	
	@IBAction func selectUTI(sender: AnyObject!) {
		var tag = (sender as NSMenuItem).tag;
		var allowedUTIs = [String]();
		switch (tag) {
		case utiType.allType.toRaw():
			for obj in utiObjects {
				for uti in obj.utis {
					allowedUTIs.append(uti)
				}
			}
			openPanel.allowedFileTypes = allowedUTIs
			if (allowsMultipleSelectionOfTrackers) {
				openPanel.allowsMultipleSelection = true
			}
			
		case utiType.trackerType.toRaw():
			for obj in utiObjects {
				if (obj.theUtiType == trackerType.tracker) {
					for uti in obj.utis {
						allowedUTIs.append(uti)
					}
				}
			}
			openPanel.allowedFileTypes = allowedUTIs
			if (allowsMultipleSelectionOfTrackers) {
				openPanel.allowsMultipleSelection = true
			}
			
		case utiType.playlistType.toRaw():
			for obj in utiObjects {
				if (obj.theUtiType == .playlist) {
					for uti in obj.utis {
						allowedUTIs.append(uti)
					}
				}
			}
			openPanel.allowedFileTypes = allowedUTIs
			if (allowsMultipleSelectionOfTrackers) {
				openPanel.allowsMultipleSelection = true
			}
			
		case utiType.instrumentType.toRaw():
			for obj in utiObjects {
				if (obj.theUtiType == .instrument) {
					for uti in obj.utis {
						allowedUTIs.append(uti)
					}
				}
			}
			openPanel.allowedFileTypes = allowedUTIs
			if (allowsMultipleSelectionOfTrackers) {
				openPanel.allowsMultipleSelection = true
			}
			
		case utiType.otherType.toRaw():
			for obj in utiObjects {
				if (obj.theUtiType == .other) {
					for uti in obj.utis {
						allowedUTIs.append(uti)
					}
				}
			}
			openPanel.allowedFileTypes = allowedUTIs
			if (allowsMultipleSelectionOfTrackers) {
				openPanel.allowsMultipleSelection = true
			}
			break;
			
		default:
			if (tag < utiObjects.count && tag >= 0) {
				var selObj = utiObjects[tag];
				openPanel.allowedFileTypes = selObj.utis
				if (allowsMultipleSelectionOfTrackers) {
					if (selObj.theUtiType == .tracker) {
						openPanel.allowsMultipleSelection = true
					} else {
						openPanel.allowsMultipleSelection = false
					}
				}
				
			}
			break;
		}
	}
	
	init(openPanel panel:NSOpenPanel, trackerDictionary td: Dictionary<String, [String]>? = nil, playlistDictionary pd :Dictionary<String, [String]>? = nil, instrumentDictionary insDict: Dictionary<String, [String]>? = nil, additionalDictionary adddict: Dictionary<String, [String]>? = nil) {
		openPanel = panel
		super.init(nibName: "OpenPanelViewController", bundle: nil)
		var mutArray = [OpenPanelViewItem]();
		if (td != nil) {
			var tdHere = td!
			for (key, utis) in tdHere {
				var obj = OpenPanelViewItem(type: .trackerType, utis: utis, name: key)
				utiObjects.append(obj)
			}
		}
		if ((pd) != nil) {
			var pdHere = td!
			for (key, utis) in pdHere {
				var obj = OpenPanelViewItem(type: .playlistType, utis: utis, name: key)
				utiObjects.append(obj)
			}
		}
		if ((insDict) != nil) {
			var insHere = insDict!
			for (key, utis) in insHere {
				var obj = OpenPanelViewItem(type: .instrumentType, utis: utis, name: key)
				utiObjects.append(obj)
			}
		}
		if (adddict != nil) {
			var addHere = adddict!
			for (key, utis) in addHere {
				var obj = OpenPanelViewItem(type: .otherType, utis: utis, name: key)
				utiObjects.append(obj)
			}
		}
		
		utiObjects.sort({(isOrderedBefore: (lhs: OpenPanelViewItem, rhs: OpenPanelViewItem)) -> Bool in
			let lhs = isOrderedBefore.lhs.theUtiType
			let rhs = isOrderedBefore.rhs.theUtiType
			if (lhs.toRaw() < rhs.toRaw()) {
				return true
			} else if (lhs.toRaw() > rhs.toRaw()) {
				return false
			} else {
				
				let result = isOrderedBefore.lhs.name.localizedStandardCompare(isOrderedBefore.rhs.name);
				return result == NSComparisonResult.OrderedAscending;
			}
			})
		allowsMultipleSelectionOfTrackers = false
	}
	
	convenience init(openPanel panel:NSOpenPanel, trackerDictionary td: Dictionary<String, [String]>?, playlistDictionary pd :Dictionary<String, [String]>?) {
		self.init(openPanel:panel, trackerDictionary:td, playlistDictionary:pd, instrumentDictionary:nil, additionalDictionary:nil)
	}
	/*
	- (id)initWithOpenPanel:(NSOpenPanel*)panel trackerDictionary:(NSDictionary *)td playlistDictionary:(NSDictionary*)pd;
	- (id)initWithOpenPanel:(NSOpenPanel*)panel trackerDictionary:(NSDictionary *)td playlistDictionary:(NSDictionary*)pd instrumentDictionary:(NSDictionary*)insDict;
	- (id)initWithOpenPanel:(NSOpenPanel*)panel trackerDictionary:(NSDictionary *)td playlistDictionary:(NSDictionary*)pd instrumentDictionary:(NSDictionary*)insDict additionalDictionary:(NSDictionary *)adddict;
	- (id)initWithOpenPanel:(NSOpenPanel*)panel trackerDictionary:(NSDictionary *)td playlistDictionary:(NSDictionary*)pd additionalDictionary:(NSDictionary *)adddict;
	*/
	
	override func viewDidLoad() {
		super.viewDidLoad()
		// Do view setup here.
		
		var fileTypeSelectionMenu = popUp.menu;
		var moreThanTwoTypes = hasMoreThanTwoTypes();
		if (moreThanTwoTypes) {
			var mi0 = NSMenuItem(title: "All Openable Files", action: "selectorUTI:", keyEquivalent: "")
			mi0.tag = utiType.allType.toRaw()
			mi0.target = self
			fileTypeSelectionMenu.addItem(mi0)
			
			fileTypeSelectionMenu.addItem(NSMenuItem.separatorItem())
		}
		
		for item in utiObjects {
			if (item.theUtiType == .tracker) {
				var mi0 = NSMenuItem(title: "All Trackers", action: "selectorUTI:", keyEquivalent: "")
				mi0.tag = utiType.trackerType.toRaw()
				mi0.target = self
				fileTypeSelectionMenu.addItem(mi0)
				
				break;
			}
		}
		
		for  item in utiObjects {
			if (item.theUtiType == .playlist) {
				var mi0 = NSMenuItem(title: "All Playlists", action: "selectorUTI:", keyEquivalent: "")
				mi0.tag = utiType.playlistType.toRaw()
				mi0.target = self
				fileTypeSelectionMenu.addItem(mi0)
				break;
			}
		}
		
		for item in utiObjects {
			if (item.theUtiType == .instrument) {
				var mi0 = NSMenuItem(title: "All Instruments", action: "selectorUTI:", keyEquivalent: "")
				mi0.tag = utiType.instrumentType.toRaw()
				mi0.target = self
				fileTypeSelectionMenu.addItem(mi0)
				break;
			}
		}
		
		for item in utiObjects {
			if (item.theUtiType == .other) {
				var mi0 = NSMenuItem(title: "All Other", action: "selectorUTI:", keyEquivalent: "")
				mi0.tag = utiType.otherType.toRaw()
				mi0.target = self
				fileTypeSelectionMenu.addItem(mi0)
				break;
			}
		}
		fileTypeSelectionMenu.addItem(NSMenuItem.separatorItem())
		
		for (i, curItem) in enumerate(utiObjects)  {
			if (moreThanTwoTypes) {
				if (i - 1 >= 0) {
					var prevItem = utiObjects[i - 1];
					if (curItem.theUtiType != prevItem.theUtiType) {
						fileTypeSelectionMenu.addItem(NSMenuItem.separatorItem())
					}
				}
			}
			var mi = NSMenuItem(title: curItem.name, action: "selectUTI:", keyEquivalent: "")
			mi.tag = i
			mi.target = self
			fileTypeSelectionMenu.addItem(mi)
		}
		popUp.selectItemAtIndex(0)
	}
	
	func setupDefaults() {
		var fileUTIs = [String]()
		
		for obj: OpenPanelViewItem in utiObjects {
			for someUTI in obj.utis {
				fileUTIs.append(someUTI)
			}
		}
		
		if (!allowsMultipleSelectionOfTrackers) {
			openPanel.allowsMultipleSelection = false
		}
		
		openPanel.allowedFileTypes = fileUTIs
		openPanel.accessoryView = self.view
	}
	
	func hasMoreThanTwoTypes() -> Bool {
		var i = 0;
		var utiCount = utiObjects.count;
		if (utiCount < 2) {
			return false;
		}
		
		for (i = 1; i < utiCount; i++) {
			
			var obj1 = utiObjects[i - 1];
			var obj2 = utiObjects[i];
			if (obj1.theUtiType != obj2.theUtiType) {
				return true;
			}
		}
		return false;
	}
}
