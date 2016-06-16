//
//  NotificationAdditions.swift
//  PPMacho
//
//  Created by C.W. Betts on 6/16/16.
//
//

import Foundation

extension NSNotification.Name {
	static let soundPreferencesDidChange = NSNotification.Name(rawValue: PPSoundPreferencesDidChange)
	static let driverDidChange = NSNotification.Name(rawValue: PPDriverDidChange)
	static let musicDidChange = NSNotification.Name(rawValue: PPMusicDidChange)
	static let listPreferencesDidChange = NSNotification.Name(rawValue: PPListPreferencesDidChange)
	static let miscPreferencesDidChange = NSNotification.Name(rawValue: PPMiscPreferencesDidChange)
}
