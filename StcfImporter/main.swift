//
//  main.swift
//  PPMacho
//
//  Created by C.W. Betts on 7/1/15.
//
//

import Foundation

// Get the singleton service listener object.
let serviceListener = NSXPCListener.serviceListener()

// Configure the service listener with a delegate.
let sharedImporter = StcfImporter.sharedImporter
serviceListener.delegate = sharedImporter

// Resume the listener. At this point, NSXPCListener will take over the execution of this service, managing its lifetime as needed.
serviceListener.resume()
