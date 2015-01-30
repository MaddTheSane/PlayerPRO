//
//  SoundSettingsViewController.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/20/14.
//
//

import PlayerPROCore
import Cocoa

private enum soundRate {
	case rate11Khz
	case rate22Khz
	case rate44Khz
	case rate48Khz
}

private enum bitRate {
	case bits8
	case bits16
	case bits20
	case bits24
}

internal struct TagCoupling {
	var amount:		Int32
	var tag:		Int
	
	init(_ theAmount: Int32, _ theTag: Int) {
		amount = theAmount
		tag = theTag
	}
}

private func stereoDelayFromTag(theTag: Int) -> Int32 {
	var toSet: Int32 = 0;

	for i in StereoDelayCoupling {
		if i.tag == theTag {
			toSet = i.amount
			break
		}
	}
	
	if (toSet == 0) {
		toSet = 30;
	}
	
	return toSet;
}

private func oversamplingFromTag(theTag: Int) -> Int32 {
	var toSet: Int32 = 0;
	
	for i in OversamplingCoupling {
		if i.tag == theTag {
			toSet = i.amount
			break
		}
	}

	if (toSet == 0) {
		toSet = 1;
	}
	
	return toSet;
}

private let StereoDelayCoupling = [TagCoupling(1, 1), TagCoupling(10, 2), TagCoupling(25, 3), TagCoupling(30, 4) ,TagCoupling(35, 5),
	TagCoupling(40, 6), TagCoupling(45, 7), TagCoupling(50, 8), TagCoupling(60, 9), TagCoupling(70, 10), TagCoupling(80, 11), TagCoupling(90, 12), TagCoupling(100, 13), TagCoupling(200, 14)];

private let ReverbAmountCoupling = [TagCoupling(25, 1), TagCoupling(30, 2), TagCoupling(35, 3), TagCoupling(40, 4), TagCoupling(45, 5),
	TagCoupling(50, 6), TagCoupling(60, 7), TagCoupling(70, 8), TagCoupling(80, 9), TagCoupling(90, 10), TagCoupling(100, 11), TagCoupling(200, 12), TagCoupling(300, 13), TagCoupling(400, 14)];

private let ReverbPercentCoupling = [TagCoupling(10, 1), TagCoupling(20, 2), TagCoupling(30, 3), TagCoupling(40, 4),
	TagCoupling(50, 5), TagCoupling(60, 6), TagCoupling(70, 7)]

private let OversamplingCoupling = [TagCoupling(1, 1), TagCoupling(2, 2), TagCoupling(3, 3), TagCoupling(4, 4),
	TagCoupling(6, 5), TagCoupling(8, 6), TagCoupling(10, 7), TagCoupling(16, 8), TagCoupling(20, 9), TagCoupling(25, 10), TagCoupling(30, 11)];

@objc public protocol SoundSettingsViewControllerDelegate : NSObjectProtocol {
	func soundOutBitsDidChange(bits: Int16)
	func soundOutRateDidChange(rat: UInt32)
	func soundOutReverbDidChangeActive(isAct: Bool)
	func soundOutOversamplingDidChangeActive(isAct: Bool)
	func soundOutStereoDelayDidChangeActive(isAct: Bool)
	func soundOutSurroundDidChangeActive(isAct: Bool)
	func soundOutReverbStrengthDidChange(rev: Int32)
	func soundOutReverbSizeDidChange(rev: Int32)
	func soundOutOversamplingAmountDidChange(ovs: Int32)
	func soundOutStereoDelayAmountDidChange(std: Int32)
	
	optional func soundOutDriverDidChange(driv: MADSoundOutput)
}

public class SoundSettingsViewController: NSViewController {
	private var reverbActive		= false
	private var surroundActive		= false
	private var stereoDelayActive	= false
	private var oversamplingActive	= false
	
	@IBOutlet weak public var oversampling:		NSButton!
	@IBOutlet weak public var reverb:			NSButton!
	@IBOutlet weak public var stereoDelay:		NSButton!
	@IBOutlet weak public var surround:			NSButton!
	@IBOutlet weak public var rate:				NSMatrix!
	@IBOutlet weak public var outputBits:		NSMatrix!
	@IBOutlet weak public var soundDriver:		NSMatrix!
	@IBOutlet weak public var oversamplingNum:	NSPopUpButton!
	@IBOutlet weak public var reverbNum:		NSPopUpButton!
	@IBOutlet weak public var stereoDelayNum:	NSPopUpButton!
	@IBOutlet weak public var reverbPercent:	NSPopUpButton!
	@IBOutlet weak public var delegate:			SoundSettingsViewControllerDelegate? = nil
	
    override public func viewDidLoad() {
        super.viewDidLoad()
        // Do view setup here.
    }
	
	private func setCurrentBits(bits: bitRate) {
		switch (bits) {
		case .bits8:
			outputBits.selectCellAtRow(0, column: 0)
			break;
			
		case .bits20:
			outputBits.selectCellAtRow(0, column: 2)
			break;
			
		case .bits24:
			outputBits.selectCellAtRow(0, column: 3)
			break;
			
		default:
			outputBits.selectCellAtRow(0, column: 1)
			break;
		}
	}
	
	private func setCurrentRate(currRate: soundRate) {
		switch (currRate) {
		case .rate11Khz:
			rate.selectCellAtRow(0, column: 0)
			break;
			
		case .rate22Khz:
			rate.selectCellAtRow(0, column: 1)
			break;
			
			
		case .rate48Khz:
			rate.selectCellAtRow(0, column: 3)
			break;
			
		default:
			rate.selectCellAtRow(0, column: 2)
			break;
		}
	}
	
	private func currentRate() -> soundRate {
		let curSelected: AnyObject! = rate.selectedCell()
		if (rate.cellAtRow(0, column: 0) === curSelected) {
			return .rate11Khz;
		} else if (rate.cellAtRow(0, column: 1) === curSelected) {
			return .rate22Khz;
		} else if (rate.cellAtRow(0, column: 2) === curSelected) {
			return .rate44Khz;
		} else if (rate.cellAtRow(0, column: 3) === curSelected) {
			return .rate48Khz;
		} else {
			return .rate44Khz;
		}
	}
	
	private func setCurrentSoundDriver(theDriver: MADSoundOutput) {
		switch (theDriver) {
		case .CoreAudioDriver:
			soundDriver.selectCellAtRow(0, column:0)
			break;
			
		case .MIDISoundDriver:
			soundDriver.selectCellAtRow(2, column:0)
			break;
			
		default:
			soundDriver.selectCellAtRow(3, column:0)
			break;
		}
	}
	
	private func currentSoundDriver() -> MADSoundOutput {
		let curSelected: AnyObject! = soundDriver.selectedCell()
		if (soundDriver.cellAtRow(0, column: 0) === curSelected) {
			return .CoreAudioDriver;
			//} else if ([soundDriver cellAtRow:1 column:0] == curSelected) {
			//	return NoHardwareDriver;
		} else if (soundDriver.cellAtRow(2, column: 0) === curSelected) {
			return .MIDISoundDriver;
		} else {
			return .NoHardwareDriver;
		}
	}
	
	private func currentBits() -> bitRate {
		let curSelected: AnyObject! = outputBits.selectedCell();
		if (outputBits.cellAtRow(0, column: 0) === curSelected) {
			return .bits8;
		} else if (outputBits.cellAtRow(0, column: 1) === curSelected) {
			return .bits16;
		} else if (outputBits.cellAtRow(0, column: 2) === curSelected) {
			return .bits20;
		} else if (outputBits.cellAtRow(0, column: 3) === curSelected) {
			return .bits24;
		} else {
			return .bits16;
		}
	}
	
	required override public init?(nibName: String!, bundle: NSBundle!) {
		super.init(nibName: nibName, bundle: bundle)
	}
	
	public class func newSoundSettingWindow() -> Self? {
		return self(nibName: "PPSoundSettingsViewController", bundle: NSBundle(forClass: SoundSettingsViewController.self))
	}

	public func settingsFromDriverSettings(sett: MADDriverSettings) {
		let oversamplingState: Bool = sett.oversampling > 1;
		let reverbState = sett.Reverb;
		let stereoDelayState: Bool = sett.MicroDelaySize > 0;
		
		self.reverbActive = reverbState;
		self.oversamplingActive = oversamplingState;
		self.stereoDelayActive = stereoDelayState;
		self.surroundActive = sett.surround;
		
			oversampling.state = Int(oversamplingState)
			reverb.state = Int(reverbState)
			stereoDelay.state = Int(stereoDelayState)
			surround.state = Int(sett.surround)
			
			oversamplingNum.enabled = oversamplingState
			reverbNum.enabled = reverbState
			reverbPercent.enabled = reverbState
			stereoDelayNum.enabled = stereoDelayState
			
			setCurrentSoundDriver(sett.driverMode)
			
			let unConvBits = sett.outPutBits
			var ConvBits = bitRate.bits8
			switch (unConvBits) {
			case 8:
				ConvBits = .bits8;
				break;
				
			case 20:
				ConvBits = .bits20;
				break;
				
			case 24:
				ConvBits = .bits24;
				break;
				
			default:
				ConvBits = .bits16;
				break;
			}
			setCurrentBits(ConvBits)
			
			let unConvRate = sett.outPutRate
			var convRate = soundRate.rate11Khz
			switch (unConvRate) {
			case 11025:
				convRate = .rate11Khz;
				break;
				
			case 22050:
				convRate = .rate22Khz;
				break;
				
			case 48000:
				convRate = .rate48Khz;
				break;
				
			default:
				convRate = .rate44Khz;
				break;
			}
			setCurrentRate(convRate)
			
			let theRate = sett.MicroDelaySize
			var toSet: Int? = nil
			for i in ReverbPercentCoupling {
				if i.amount == theRate {
					toSet = i.tag
					break
				}
			}
			
			if toSet == nil {
				toSet = 4;
			}
			
			stereoDelayNum.selectItemAtIndex(toSet! - 1)
			
			toSet = nil
			let reverbAmount = sett.ReverbSize
			for i in ReverbAmountCoupling {
				if i.amount == reverbAmount {
					toSet = i.tag
					break
				}
			}
			
			if toSet == nil {
				toSet = 4;
			}
			reverbNum.selectItemAtIndex(toSet! - 1)
			
			toSet = nil
			let reverbPercentage = sett.ReverbStrength;
			for i in ReverbPercentCoupling {
				if i.amount == reverbPercentage {
					toSet = i.tag
					break
				}
			}
			
			if toSet == nil {
				toSet = 3;
			}
			reverbPercent.selectItemAtIndex(toSet! - 1)
			
			toSet = nil
			let oversamplingAmount = sett.oversampling;
			for i in OversamplingCoupling {
				if i.amount == oversamplingAmount {
					toSet = i.tag
					break
				}
			}
			
			if toSet == nil {
				toSet = 1;
			}
			oversamplingNum.selectItemAtIndex(toSet! - 1)
			
			if (delegate?.soundOutDriverDidChange? == nil) {
				soundDriver.enabled = false;
			}
		}

	required public init?(coder: NSCoder) {
		super.init(coder: coder)
	}
	
	@IBAction public func changeDriver(sender: AnyObject!) {
		let driver = currentSoundDriver()
		delegate?.soundOutDriverDidChange?(driver)
	}
	
	@IBAction public func changeBits(sender: AnyObject!) {
		let theBits = currentBits();
		var returnBits: Int16 = 0
		switch (theBits) {
		case .bits8:
			returnBits = 8;
			break;
			
		case .bits20:
			returnBits = 20;
			break;
			
		case .bits24:
			returnBits = 24;
			break;
			
		default:
			returnBits = 16;
			break;
		}
		delegate?.soundOutBitsDidChange(returnBits)
	}
	
	@IBAction public func changeRate(sender: AnyObject!) {
		let rate1 = currentRate()
		var returnRate: UInt32 = 0
		switch (rate1) {
		case .rate11Khz:
			returnRate = 11025;
			break;
			
		case .rate22Khz:
			returnRate = 22050;
			break;
			
		case .rate48Khz:
			returnRate = 48000;
			break;
			
		default:
			returnRate = 44100;
			break;
		}
		delegate?.soundOutRateDidChange(returnRate)
	}
	
	@IBAction public func changeChecked(sender: AnyObject!) {
		let reverbState = reverb.state == 0 ? false : true;
		let stereoDelayState = stereoDelay.state == 0 ? false : true
		let oversamplingState = oversampling.state == 0 ? false : true
		let surroundState = surround.state == 0 ? false : true
		
		oversamplingNum.enabled = oversamplingState
		reverbNum.enabled = reverbState
		reverbPercent.enabled = reverbState
		stereoDelayNum.enabled = stereoDelayState
		
			if (reverbState != reverbActive) {
				delegate?.soundOutReverbDidChangeActive(reverbState)
			}
			if (stereoDelayState != stereoDelayActive) {
				delegate?.soundOutStereoDelayDidChangeActive(stereoDelayState)
				if stereoDelayState {
					delegate?.soundOutStereoDelayAmountDidChange(stereoDelayFromTag(stereoDelayNum.selectedItem!.tag))
				}
			}
			if (oversamplingState != oversamplingActive) {
				delegate?.soundOutOversamplingDidChangeActive(oversamplingState)
				if (oversamplingState) {
					delegate?.soundOutOversamplingAmountDidChange(oversamplingFromTag(oversamplingNum.selectedItem!.tag))
				}
			}
			if (surroundState != surroundActive) {
				delegate?.soundOutSurroundDidChangeActive(surroundState)
			}
		self.reverbActive = reverbState;
		self.stereoDelayActive = stereoDelayState;
		self.oversamplingActive = oversamplingState;
		self.surroundActive = surroundState;
	}
	
	@IBAction func changeOversampling(sender: AnyObject!) {
		var toSet = oversamplingFromTag((sender as NSMenuItem).tag)
		delegate?.soundOutOversamplingAmountDidChange(toSet)
	}
	
	@IBAction public func changeReverbAmount(sender: AnyObject!) {
		var toSet: Int32 = 0;
		let tag = (sender as NSMenuItem).tag

		for i in ReverbAmountCoupling {
			if i.tag == tag {
				toSet = i.amount
				break
			}
		}
		
		if (toSet == 0) {
			toSet = 25;
		}
		
		delegate?.soundOutReverbSizeDidChange(toSet)
	}
	
	@IBAction public func changeReverbPercent(sender: AnyObject!) {
		var toSet: Int32 = 0
		let tag = (sender as NSMenuItem).tag
		
		for i in ReverbPercentCoupling {
			if i.tag == tag {
				toSet = i.amount
				break
			}
		}

		if (toSet == 0) {
			toSet = 30;
		}
		delegate?.soundOutReverbStrengthDidChange(toSet)
	}
	
	@IBAction public func changeStereoDelay(sender: AnyObject!) {
		var toSet = stereoDelayFromTag((sender as NSMenuItem).tag)
		delegate?.soundOutStereoDelayAmountDidChange(toSet)
	}
}
