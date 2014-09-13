//
//  AudioFileExt.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/28/14.
//
//

import Foundation
import AudioToolbox
import CoreAudio
#if os(OSX)
	import PlayerPROKit
#endif

internal enum AudioFormat: OSType {
	case DVIIntelIMA			= 0x6D730011
	case MicrosoftGSM			= 0x6D730031
	case LinearPCM				= 1819304813
	case AC3					= 1633889587
	case Six0958AC3				= 1667326771
	case AppleIMA4				= 1768775988
	case MPEG4AAC				= 1633772320
	case MPEG4CELP				= 1667591280
	case MPEG4HVXC				= 1752594531
	case MPEG4TwinVQ			= 1953986161
	case MACE3					= 1296122675
	case MACE6					= 1296122678
	case ULaw					= 1970037111
	case ALaw					= 1634492791
	case QDesign				= 1363430723
	case QDesign2				= 1363430706
	case QUALCOMM				= 1365470320
	case MPEGLayer1				= 778924081
	case MPEGLayer2				= 778924082
	case MPEGLayer3				= 778924083
	case TimeCode				= 1953066341
	case MIDIStream				= 1835623529
	case ParameterValueStream	= 1634760307
	case AppleLossless			= 1634492771
	case MPEG4AAC_HE			= 1633772392
	case MPEG4AAC_LD			= 1633772396
	case MPEG4AAC_ELD			= 1633772389
	case MPEG4AAC_ELD_SBR		= 1633772390
	case MPEG4AAC_ELD_V2		= 1633772391
	case MPEG4AAC_HE_V2			= 1633772400
	case MPEG4AAC_Spatial		= 1633772403
	case AMR					= 1935764850
	case AMR_WB					= 1935767394
	case Audible				= 1096107074
	case iLBC					= 1768710755
	case AES3					= 1634038579
	#if os(OSX)
	func stringValue() -> String {
		return OSTypeToString(self.toRaw())
	}
	
	init?(stringValue strVal: String) {
		let fourChar = StringToOSType(strVal)
		if let ourself = AudioFormat.fromRaw(fourChar) {
			self = ourself
		} else {
			return nil
		}
	}
	
	#endif
}
	
internal struct AudioFormatFlag : RawOptionSetType {
	private var value: UInt32 = 0
	init(_ value: UInt32) { self.value = value }
	func toRaw() -> UInt32 { return self.value }
	static var allZeros: AudioFormatFlag { return self(0) }
	static func fromRaw(raw: UInt32) -> AudioFormatFlag? { return self(raw) }
	static func fromMask(raw: UInt32) -> AudioFormatFlag { return self(raw) }
	static func convertFromNilLiteral() -> AudioFormatFlag { return FlagsAreAllClear }
	
	static var NativeFloatPacked: AudioFormatFlag {
		return IsFloat | NativeEndian | IsPacked
	}
	
	static var IsFloat:				AudioFormatFlag { return self(1 << 0) }
	static var IsBigEndian:			AudioFormatFlag { return self(1 << 1) }
	static var IsSignedInteger:		AudioFormatFlag { return self(1 << 2) }
	static var IsPacked:			AudioFormatFlag { return self(1 << 3) }
	static var IsAlignedHigh:		AudioFormatFlag { return self(1 << 4) }
	static var IsNonInterleaved:	AudioFormatFlag { return self(1 << 5) }
	static var IsNonMixable:		AudioFormatFlag { return self(1 << 6) }
	static var FlagsAreAllClear:	AudioFormatFlag { return self(1 << 31) }
	static var NativeEndian:		AudioFormatFlag {
		#if __BIG_ENDIAN__
			return IsBigEndian
		#else
			return self(0)
		#endif
	}
	init(_ value: LinearPCMFormatFlag) {
		self.value = value.toRaw()
	}
}

internal struct LinearPCMFormatFlag : RawOptionSetType {
	private var value: UInt32 = 0
	init(_ value: UInt32) { self.value = value }
	func toRaw() -> UInt32 { return self.value }
	static var allZeros: LinearPCMFormatFlag { return self(0) }
	static func fromRaw(raw: UInt32) -> LinearPCMFormatFlag? { return self(raw) }
	static func fromMask(raw: UInt32) -> LinearPCMFormatFlag { return self(raw) }
	static func convertFromNilLiteral() -> LinearPCMFormatFlag { return FlagsAreAllClear }
	
	static var NativeFloatPacked: LinearPCMFormatFlag {
		return IsFloat | NativeEndian | IsPacked
	}
	
	static var IsFloat:				LinearPCMFormatFlag { return self(1 << 0) }
	static var IsBigEndian:			LinearPCMFormatFlag { return self(1 << 1) }
	static var IsSignedInteger:		LinearPCMFormatFlag { return self(1 << 2) }
	static var IsPacked:			LinearPCMFormatFlag { return self(1 << 3) }
	static var IsAlignedHigh:		LinearPCMFormatFlag { return self(1 << 4) }
	static var IsNonInterleaved :	LinearPCMFormatFlag { return self(1 << 5) }
	static var IsNonMixable:		LinearPCMFormatFlag { return self(1 << 6) }
	static var FlagsAreAllClear:	LinearPCMFormatFlag { return self(1 << 31) }
	static var NativeEndian:		LinearPCMFormatFlag {
		#if __BIG_ENDIAN__
			return IsBigEndian
		#else
			return self(0)
		#endif
	}
	static var FlagsSampleFractionShift: LinearPCMFormatFlag { return self(7) }
	static var FlagsSampleFractionMask : LinearPCMFormatFlag { return self(0x3F << FlagsSampleFractionShift.toRaw()) }
}

extension AudioStreamBasicDescription {
	internal var audioFormatNativeEndian: Bool {
		get {
			if (formatID == .LinearPCM) {
				let ourFlags = formatFlags & .IsBigEndian
				if ourFlags == .NativeEndian {
					return true
				}
			}
			return false
		}
	}
	
	internal var formatID: AudioFormat {
		get {
			return AudioFormat.fromRaw(mFormatID)!
		}
		set {
			mFormatID = newValue.toRaw()
		}
	}
	
	internal var formatFlags: AudioFormatFlag {
		get {
			return AudioFormatFlag.fromMask(mFormatFlags)
		}
		set {
			mFormatFlags = newValue.toRaw()
		}
	}
	
	internal init() {
		mSampleRate = 0
		mFormatID = 0
		mFormatFlags = 0
		mBytesPerPacket = 0
		mFramesPerPacket = 1
		mBytesPerFrame = 1
		mChannelsPerFrame = 1
		mBitsPerChannel = 1
		mReserved = 0
	}
	
	internal init(sampleRate: Float64, formatID: AudioFormat = .LinearPCM, formatFlags: AudioFormatFlag, bitsPerChannel: UInt32, channelsPerFrame: UInt32, framesPerPacket: UInt32 = 1) {
		mSampleRate = sampleRate
		mFormatID = formatID.toRaw()
		mFormatFlags = formatFlags.toRaw()
		mBitsPerChannel = bitsPerChannel
		mChannelsPerFrame = channelsPerFrame
		mFramesPerPacket = framesPerPacket
		mBytesPerFrame = mBitsPerChannel * mChannelsPerFrame / 8
		mBytesPerPacket = mBytesPerFrame * mFramesPerPacket
		mReserved = 0
	}

	internal init(sampleRate: Float64, formatID: Int, formatFlags: Int, bitsPerChannel: UInt32, channelsPerFrame: UInt32, framesPerPacket: UInt32 = 1) {
		mSampleRate = sampleRate
		mFormatID = UInt32(formatID)
		mFormatFlags = UInt32(formatFlags)
		mBitsPerChannel = bitsPerChannel
		mChannelsPerFrame = channelsPerFrame
		mFramesPerPacket = framesPerPacket
		mBytesPerFrame = mBitsPerChannel * mChannelsPerFrame / 8
		mBytesPerPacket = mBytesPerFrame * mFramesPerPacket
		mReserved = 0
	}
}
