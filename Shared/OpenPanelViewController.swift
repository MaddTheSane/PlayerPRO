//
//  OpenPanelViewController.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/6/14.
//
//

import Cocoa

private var OpenPanelsInUse = [OpenPanelViewController]()

private func allOpenableTypes(uti anUTI: String) -> [String] {
	var theOpenables = [anUTI]
	
	if #available(OSX 10.10, *) {
		let utiTagClasses: [NSString] = [kUTTagClassFilenameExtension, kUTTagClassMIMEType,
			/*kUTTagClassNSPboardType as String*/]
		
		for utiTag in utiTagClasses {
			guard let unPreArr = UTTypeCopyAllTagsWithClass(anUTI as NSString, utiTag) else {
				continue
			}
			
			let preArr = unPreArr.takeRetainedValue() as NSArray
			
			guard let anArr = preArr as? [String] else {
				continue
			}
			theOpenables += anArr
		}
		if let unPreArr = UTTypeCopyAllTagsWithClass(anUTI as NSString, kUTTagClassOSType),
			let anArr = unPreArr.takeRetainedValue() as NSArray as? [String] {
				let convertedArr = anArr.map({NSFileTypeForHFSTypeCode(UTGetOSTypeFromString($0 as NSString))!})
				theOpenables += convertedArr
		}
	} else {
		// Fallback on earlier versions
		if let aType = UTTypeCopyPreferredTagWithClass(anUTI as NSString, kUTTagClassFilenameExtension)?.takeRetainedValue() as String? {
			theOpenables.append(aType)
		}
	}
	
	return theOpenables
}

class OpenPanelViewController: NSViewController, NSOpenSavePanelDelegate {
	private enum utiType: Int {
		case allType = -1
		case trackerType = -2
		case playlistType = -3
		case instrumentType = -4
		case otherType = -5
	}
	
	private enum TrackerType: Int, Comparable {
		case tracker = 1
		case playlist
		case instrument
		case other
		
		static func <(lhs: OpenPanelViewController.TrackerType, rhs: OpenPanelViewController.TrackerType) -> Bool {
			return lhs.rawValue < rhs.rawValue
		}
	}
	
	private struct OpenPanelViewItem: CustomDebugStringConvertible, CustomStringConvertible, Hashable {
		let name: String
		let theUtiType: TrackerType
		let utis: [String]
		
		init(type typ: utiType, utis ut: [String], name nam: String) {
			switch (typ) {
			case .trackerType:
				theUtiType = .tracker;
				
			case .playlistType:
				theUtiType = .playlist
				
			case .instrumentType:
				theUtiType = .instrument
				
			case .otherType:
				theUtiType = .other
				
			default:
				theUtiType = .other
			}
			
			utis = ut
			name = nam
		}
		
		var hashValue: Int {
			return name.hashValue ^ theUtiType.hashValue ^ utis.count
		}
		
		var debugDescription: String {
			var des: String
			if (theUtiType == .playlist) {
				des = "Playlist";
			} else if (theUtiType == .instrument) {
				des = "Instrument";
			} else if (theUtiType == .tracker) {
				des = "Tracker";
			} else if (theUtiType == .other) {
				des = "Other";
			} else {
				des = "Unknown";
			}
			
			return "\(name): \(des) - \(utis.description)"
		}
		
		var description: String {
			var des: String
			if (theUtiType == .playlist) {
				des = "Playlist";
			} else if (theUtiType == .instrument) {
				des = "Instrument";
			} else if (theUtiType == .tracker) {
				des = "Tracker";
			} else if (theUtiType == .other) {
				des = "Other";
			} else {
				des = "Unknown";
			}
			return "\(name): \(des)"
		}
		
		static func ==(lhs: OpenPanelViewController.OpenPanelViewItem, rhs: OpenPanelViewController.OpenPanelViewItem) -> Bool {
			if lhs.theUtiType != rhs.theUtiType {
				return false
			} else if lhs.name != rhs.name {
				return false
			} else if lhs.utis != rhs.utis {
				return false
			} else {
				return true
			}
		}
	}
	
	private let openPanel: NSOpenPanel
	private let utiObjects: [OpenPanelViewItem]
	@IBOutlet weak var popUp: NSPopUpButton? = nil
	
	var allowsMultipleSelectionOfTrackers: Bool = false {
		didSet {
			self.openPanel.allowsMultipleSelection = allowsMultipleSelectionOfTrackers
		}
	}
	
	required init?(coder: NSCoder) {
		print("initWithCoder was called?")
		openPanel = NSOpenPanel()
		allowsMultipleSelectionOfTrackers = false
		utiObjects = [OpenPanelViewItem]()

		super.init(coder: coder)
	}
	
	@IBAction func selectUTI(_ sender: AnyObject?) {
		guard let menuIte = sender as? NSMenuItem else {
			NSSound.beep()
			return
		}
			let tag = menuIte.tag;
			var allowedUTIs = [String]();
			switch (tag) {
			case utiType.allType.rawValue:
				for obj in utiObjects {
					allowedUTIs += obj.utis
				}
				
				let fileTypes: [String] = {
					var aTyp = [String]()
					
					for uti in allowedUTIs {
						aTyp += allOpenableTypes(uti: uti)
					}
					
					return aTyp
				}()
				
				openPanel.allowedFileTypes = fileTypes
				if allowsMultipleSelectionOfTrackers {
					openPanel.allowsMultipleSelection = true
				}
				
			case utiType.trackerType.rawValue:
				for obj in utiObjects {
					if obj.theUtiType == .tracker {
						allowedUTIs += obj.utis
					}
				}
				
				let fileTypes: [String] = {
					var aTyp = [String]()
					
					for uti in allowedUTIs {
						aTyp += allOpenableTypes(uti: uti)
					}
					
					return aTyp
				}()
				
				openPanel.allowedFileTypes = fileTypes
				if allowsMultipleSelectionOfTrackers {
					openPanel.allowsMultipleSelection = true
				}
				
			case utiType.playlistType.rawValue:
				for obj in utiObjects {
					if obj.theUtiType == .playlist {
						allowedUTIs += obj.utis
					}
				}
				
				let fileTypes: [String] = {
					var aTyp = [String]()
					
					for uti in allowedUTIs {
						aTyp += allOpenableTypes(uti: uti)
					}
					
					return aTyp
				}()
				
				openPanel.allowedFileTypes = fileTypes
				if allowsMultipleSelectionOfTrackers {
					openPanel.allowsMultipleSelection = true
				}
				
			case utiType.instrumentType.rawValue:
				for obj in utiObjects {
					if obj.theUtiType == .instrument {
						allowedUTIs += obj.utis
					}
				}
				
				let fileTypes: [String] = {
					var aTyp = [String]()
					
					for uti in allowedUTIs {
						aTyp += allOpenableTypes(uti: uti)
					}
					
					return aTyp
				}()
				
				openPanel.allowedFileTypes = fileTypes
				if allowsMultipleSelectionOfTrackers {
					openPanel.allowsMultipleSelection = true
				}
				
			case utiType.otherType.rawValue:
				for obj in utiObjects {
					if (obj.theUtiType == .other) {
						allowedUTIs.append(contentsOf: obj.utis)
					}
				}
				
				let fileTypes: [String] = {
					var aTyp = [String]()
					
					for uti in allowedUTIs {
						aTyp += allOpenableTypes(uti: uti)
					}
					
					return aTyp
				}()

				openPanel.allowedFileTypes = fileTypes
				if allowsMultipleSelectionOfTrackers {
					openPanel.allowsMultipleSelection = true
				}
				break;
				
			default:
				if tag < utiObjects.count && tag >= 0 {
					let selObj = utiObjects[tag];
					let fileTypes: [String] = {
						var aTyp = [String]()
						
						for uti in selObj.utis {
							aTyp += allOpenableTypes(uti: uti)
						}
						
						return aTyp
					}()
					
					openPanel.allowedFileTypes = fileTypes
					if allowsMultipleSelectionOfTrackers {
						if selObj.theUtiType == .tracker {
							openPanel.allowsMultipleSelection = true
						} else {
							openPanel.allowsMultipleSelection = false
						}
					}
				}
			}
	}
	
	override init(nibName: NSNib.Name?, bundle: Bundle?) {
		openPanel = NSOpenPanel()
		allowsMultipleSelectionOfTrackers = false
		utiObjects = [OpenPanelViewItem]()

		super.init(nibName: nibName, bundle: bundle)
	}
	
	init?(openPanel panel: NSOpenPanel, trackerDictionary td: [String: [String]]? = nil, playlistDictionary pd: [String: [String]]? = nil, instrumentDictionary insDict: [String: [String]]? = nil, additionalDictionary adddict: [String: [String]]? = nil) {
		openPanel = panel
		allowsMultipleSelectionOfTrackers = false
		var tmpUTIs = [OpenPanelViewItem]()
		if let tdHere = td {
			for (key, utis) in tdHere {
				let obj = OpenPanelViewItem(type: .trackerType, utis: utis, name: key)
				tmpUTIs.append(obj)
			}
		}
		if let pdHere = pd {
			for (key, utis) in pdHere {
				let obj = OpenPanelViewItem(type: .playlistType, utis: utis, name: key)
				tmpUTIs.append(obj)
			}
		}
		if let insHere = insDict {
			for (key, utis) in insHere {
				let obj = OpenPanelViewItem(type: .instrumentType, utis: utis, name: key)
				tmpUTIs.append(obj)
			}
		}
		if let addHere = adddict {
			for (key, utis) in addHere {
				let obj = OpenPanelViewItem(type: .otherType, utis: utis, name: key)
				tmpUTIs.append(obj)
			}
		}
		
		tmpUTIs.sort(by: {(lhs, rhs) -> Bool in
			if lhs.theUtiType < rhs.theUtiType {
				return true
			} else if lhs.theUtiType > rhs.theUtiType {
				return false
			} else {
				
				let result = lhs.name.localizedStandardCompare(rhs.name)
				return result == ComparisonResult.orderedAscending
			}
			})
		
		utiObjects = tmpUTIs
		super.init(nibName: NSNib.Name(rawValue: "PPOpenPanel"), bundle: nil)
		
		openPanel.delegate = self
	}
	
	func beginOpenPanel(_ completionHandler: @escaping (_ result: Int) -> Void) {
		beginWithCompletionHandler(completionHandler)
	}
	
	func beginWithCompletionHandler(_ resultHandle: @escaping (_ result: Int) -> Void) {
		OpenPanelsInUse.append(self)
		openPanel.begin(completionHandler: { (result) -> Void in
			if let anInt = OpenPanelsInUse.index(of: self) {
				OpenPanelsInUse.remove(at: anInt)
			}
			resultHandle(result.rawValue)
		})
	}

	@objc func beginOpenPanel(_ parentWindow: NSWindow, completionHandler resultHandle: @escaping (_ result: NSApplication.ModalResponse) -> Void) {
		OpenPanelsInUse.append(self)
		openPanel.beginSheetModal(for: parentWindow, completionHandler: { (result) -> Void in
			if let anInt = OpenPanelsInUse.index(of: self) {
				OpenPanelsInUse.remove(at: anInt)
			}
			resultHandle(result)
		})
	}
	
	override func awakeFromNib() {
		super.awakeFromNib()
		// Do view setup here.
		
		if let fileTypeSelectionMenu = popUp?.menu {
			let moreThanTwoTypes = hasMoreThanTwoTypes
			if moreThanTwoTypes {
				let mi0 = NSMenuItem(title: "All Openable Files", action: #selector(OpenPanelViewController.selectUTI(_:)), keyEquivalent: "")
				mi0.tag = utiType.allType.rawValue
				mi0.target = self
				fileTypeSelectionMenu.addItem(mi0)
				
				fileTypeSelectionMenu.addItem(NSMenuItem.separator())
			}
			
			for item in utiObjects {
				if item.theUtiType == .tracker {
					let mi0 = NSMenuItem(title: "All Trackers", action: #selector(OpenPanelViewController.selectUTI(_:)), keyEquivalent: "")
					mi0.tag = utiType.trackerType.rawValue
					mi0.target = self
					fileTypeSelectionMenu.addItem(mi0)
					
					break;
				}
			}
			
			for item in utiObjects {
				if item.theUtiType == .playlist {
					let mi0 = NSMenuItem(title: "All Playlists", action: #selector(OpenPanelViewController.selectUTI(_:)), keyEquivalent: "")
					mi0.tag = utiType.playlistType.rawValue
					mi0.target = self
					fileTypeSelectionMenu.addItem(mi0)
					
					break;
				}
			}
			
			for item in utiObjects {
				if item.theUtiType == .instrument {
					let mi0 = NSMenuItem(title: "All Instruments", action: #selector(OpenPanelViewController.selectUTI(_:)), keyEquivalent: "")
					mi0.tag = utiType.instrumentType.rawValue
					mi0.target = self
					fileTypeSelectionMenu.addItem(mi0)
					
					break;
				}
			}
			
			for item in utiObjects {
				if item.theUtiType == .other {
					let mi0 = NSMenuItem(title: "All Other", action: #selector(OpenPanelViewController.selectUTI(_:)), keyEquivalent: "")
					mi0.tag = utiType.otherType.rawValue
					mi0.target = self
					fileTypeSelectionMenu.addItem(mi0)
					
					break;
				}
			}
			fileTypeSelectionMenu.addItem(NSMenuItem.separator())
			
			for (i, curItem) in utiObjects.enumerated()  {
				if moreThanTwoTypes {
					if i - 1 >= 0 {
						let prevItem = utiObjects[i - 1];
						if (curItem.theUtiType != prevItem.theUtiType) {
							fileTypeSelectionMenu.addItem(NSMenuItem.separator())
						}
					}
				}
				let mi = NSMenuItem(title: curItem.name, action: #selector(OpenPanelViewController.selectUTI(_:)), keyEquivalent: "")
				mi.tag = i
				mi.target = self
				fileTypeSelectionMenu.addItem(mi)
			}
		}
		popUp?.selectItem(at: 0)
	}
	
	func setupDefaults() {
		var fileUTIs = [String]()
		
		for obj in utiObjects {
			fileUTIs += obj.utis
		}
		
		if !allowsMultipleSelectionOfTrackers {
			openPanel.allowsMultipleSelection = false
		}
		
		var allowedTypes = [String]()
		
		for uti in fileUTIs {
			allowedTypes += allOpenableTypes(uti: uti)
		}
		
		self.openPanel.allowedFileTypes = allowedTypes
		openPanel.canChooseFiles = true
		openPanel.accessoryView = self.view
	}
	
	private var hasMoreThanTwoTypes: Bool {
		let utiCount = utiObjects.count
		if utiCount < 2 {
			return false
		}
		
		for i in 1 ..< utiCount {
			let obj1 = utiObjects[i - 1]
			let obj2 = utiObjects[i]
			if obj1.theUtiType != obj2.theUtiType {
				return true
			}
		}
		return false;
	}
	
	func panel(_ sender: Any, shouldEnable url: URL) -> Bool {
		// We don't support non-file URLs yet.
		if !url.isFileURL {
			return false
		}
		return true
	}
}
