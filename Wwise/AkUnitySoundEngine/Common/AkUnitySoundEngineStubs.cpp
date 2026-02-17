/*******************************************************************************
The content of this file includes portions of the proprietary AUDIOKINETIC Wwise
Technology released in source code form as part of the game integration package.
The content of this file may not be used without valid licenses to the
AUDIOKINETIC Wwise Technology.
Note that the use of the game engine is subject to the Unity(R) Terms of
Service at https://unity3d.com/legal/terms-of-service
 
License Usage
 
Licensees holding valid licenses to the AUDIOKINETIC Wwise Technology may use
this file in accordance with the end user license agreement provided with the
software or, alternatively, in accordance with the terms contained
in a written agreement between you and Audiokinetic Inc.
Copyright (c) 2026 Audiokinetic Inc.
*******************************************************************************/

#include "stdafx.h"
#include <AK/SoundEngine/Common/AkTypes.h>
#include <AK/Tools/Common/AkLock.h>
#include <AK/Tools/Common/AkAutoLock.h>

#define GCC_HASCLASSVISIBILITY
#include <AK/Tools/Common/AkAssert.h>
#include <AK/Tools/Common/AkPlatformFuncs.h>
#if defined (__APPLE__)
	#include <malloc/malloc.h>
	#include <sys/mman.h>
#else
	#include <stdlib.h>
#endif // #if defined (__APPLE__)

#include <AK/SoundEngine/Common/AkMemoryMgr.h>
#include <AK/SoundEngine/Common/AkMemoryMgrFuncs.h>
#include <AK/SoundEngine/Common/IAkStreamMgr.h>

#include "AkFilePackageLowLevelIODeferred.h"
CAkFilePackageLowLevelIODeferred  g_lowLevelIO;

#ifndef AK_OPTIMIZED
#include <AK/Comm/AkCommunication.h>
#endif // #ifndef AK_OPTIMIZED

#if defined AK_ANDROID 
#include "../Android/jni/AkUnityAndroidSystem.h"
#elif defined AK_HARMONY
#include "../OpenHarmony/AkUnityOpenHarmonySystem.h"
#endif // defined AK_ANDROID

#include "AkCallbackSerializer.h"

// Register plugins that are static linked in this DLL.  Others will be loaded dynamically.
#include <AK/Plugin/AkSilenceSourceFactory.h>					// Silence generator
#include <AK/Plugin/AkSineSourceFactory.h>						// Sine wave generator
#include <AK/Plugin/AkToneSourceFactory.h>						// Tone generator
#include <AK/Plugin/AkAudioInputSourceFactory.h>

#if !defined(AK_SKIP_BUILTIN_PLUGINS)
#include <AK/Plugin/Ak3DAudioBedMixerFXFactory.h>				// 3D Audio Bed Mixer
#include <AK/Plugin/AkCompressorFXFactory.h>					// Compressor
#include <AK/Plugin/AkDelayFXFactory.h>							// Delay
#include <AK/Plugin/AkFlangerFXFactory.h>						// Flanger
#include <AK/Plugin/AkGainFXFactory.h>							// Gain
#include <AK/Plugin/AkGuitarDistortionFXFactory.h>				// Guitar distortion
#include <AK/Plugin/AkHarmonizerFXFactory.h>					// Harmonizer
#include <AK/Plugin/AkMatrixReverbFXFactory.h>					// Matrix reverb
#include <AK/Plugin/AkParametricEQFXFactory.h>					// Parametric equalizer
#include <AK/Plugin/AkPeakLimiterFXFactory.h>					// Peak limiter
#include <AK/Plugin/AkPitchShifterFXFactory.h>					// Pitch Shifter
#include <AK/Plugin/AkRecorderFXFactory.h>						// Recorder
#include <AK/Plugin/AkRoomVerbFXFactory.h>						// RoomVerb
#include <AK/Plugin/AkReflectFXFactory.h>						// Reflect
#include <AK/Plugin/AkStereoDelayFXFactory.h>					// Stereo Delay
#include <AK/Plugin/AkTimeStretchFXFactory.h>					// Time Stretch
#include <AK/Plugin/AkTremoloFXFactory.h>						// Tremolo
#endif

// Required by codecs plug-ins
#include <AK/Plugin/AkVorbisDecoderFactory.h>
#include <AK/Plugin/AkOpusDecoderFactory.h>

#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include <AK/SoundEngine/Common/AkMemoryMgrModule.h>
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#include <AK/Tools/Common/AkMonitorError.h>
#include <AK/Plugin/AkReflectGameData.h>
#include <AK/SpatialAudio/Common/AkSpatialAudioTypes.h>
#include <AK/SpatialAudio/Common/AkSpatialAudio.h>

#if !defined(AK_PLATFORM_SPECIFIC_STUBS) || !defined(__has_include)
#include "AkUnitySoundEngineStubs.h"
#else
#if __has_include("AkUnitySoundEngineStubs_Platform.h")
#include "AkUnitySoundEngineStubs_Platform.h"
#endif
#endif

#include <AK/AkWwiseSDKVersion.h>

// Defines.

#if defined (WIN32) || defined (WIN64) || defined (_XBOX_VER)
	#define AK_PLATFORM_PATH_SEPARATOR '\\'
	#define AK_STRING_PATH_SEPERATOR AKTEXT("\\")
#else
	#define AK_PLATFORM_PATH_SEPARATOR '/'
	#define AK_STRING_PATH_SEPERATOR AKTEXT("/")
#endif // #if defined (WIN32) || defined (WIN64) || defined (_XBOX_VER)

#ifndef AK_OPTIMIZED
char g_GameName[AK_COMM_SETTINGS_MAX_STRING_SIZE];

// AK_WPTR is a macro that defines an 8-byte struct member representing a pointer, regardless of word size.
// It relies on AK_POINTER_64 being defined on 64-bit platforms and undefined on 32-bit platforms.
// The test below ensures that AK_POINTER_64 is properly specified where appropriate during SWIG processing.
struct __ak_word_size_test { AK_WPTR(void*, wide_ptr); };
static_assert(sizeof(__ak_word_size_test) == 8, "Word size is incorrectly defined for this platform!");
#endif // AK_OPTIMIZED

AkOSChar g_basePath[AK_MAX_PATH] = AKTEXT("");
AkOSChar g_decodedBankPath[AK_MAX_PATH] = AKTEXT("");

namespace AkCallbackSerializerHelper
{
	void LocalOutput(AK::Monitor::ErrorCode in_eErrorCode, const AkOSChar* in_pszError, AK::Monitor::ErrorLevel in_eErrorLevel, AkPlayingID in_playingID, AkGameObjectID in_gameObjID);
}

void AkUnityAssertHook(
	const char * in_pszExpression,	///< Expression
	const char * in_pszFileName,	///< File Name
	int in_lineNumber		///< Line Number
	)
{
	AkOSChar* os_pszExpression = NULL;
	AkOSChar* os_pszFileName = NULL;
	CONVERT_CHAR_TO_OSCHAR(in_pszExpression, os_pszExpression);
	CONVERT_CHAR_TO_OSCHAR(in_pszFileName, os_pszFileName);
	size_t msgLen = AKPLATFORM::OsStrLen(os_pszExpression) + AKPLATFORM::OsStrLen(os_pszFileName) + 128;
	AkOSChar* msg = (AkOSChar*)AkAlloca(msgLen);
	AK_OSPRINTF(msg, msgLen, AKTEXT("AKASSERT: %s. File: %s, line: %d"), os_pszExpression, os_pszFileName, in_lineNumber);

	AkCallbackSerializerHelper::LocalOutput(AK::Monitor::ErrorCode_NoError, msg, AK::Monitor::ErrorLevel_Error, 0, AK_INVALID_GAME_OBJECT);
}

// Prototype declaration
AKRESULT SaveDecodedBank(AkOSChar * in_OrigBankFile, void * in_pDecodedData, AkUInt32 in_decodedSize, bool in_bIsLanguageSpecific);

AKRESULT CreateDirectoryStructure(const AkOSChar* in_pszDirectory)
{
	// We expect in_pszDirectory to be a path to a directory, not a file
	bool bSkipFilename = false;
	return CAkFileHelpers::ForEachDirectoryLevel(in_pszDirectory,
		[](const AkOSChar* directory) {
			return CAkFileHelpers::CreateEmptyDirectory(directory);
		}
	, bSkipFilename);
}

void AK_STDCALL AkDefaultLogger(const char* message)
{
	const AkUInt32 length = (AkUInt32)(1 + strlen(message));
	char* formattedMessage = (char*)AkAlloca(length * sizeof(char));

	AKPLATFORM::SafeStrCpy(formattedMessage, message, length);
	AKPLATFORM::SafeStrCat(formattedMessage, "\n", length);
	AKPLATFORM::OutputDebugMsg(formattedMessage);
}

AkErrorLogger errorlogger = AkDefaultLogger;

void SetErrorLogger(AkErrorLogger logger)
{
	errorlogger = (logger == NULL) ? AkDefaultLogger : logger;
}

AkGetAudioSamples audioSamplesFunction = NULL;
AkGetAudioFormat audioFormatFunction = NULL;

void GetAudioFormat(AkPlayingID playingID, AkAudioFormat& format)
{
	if (audioFormatFunction)
		audioFormatFunction(playingID, &format);

	format.uTypeID = AK_FLOAT;
	format.uInterleaveID = AK_NONINTERLEAVED;
}

void GetAudioSamples(AkPlayingID playingID, AkAudioBuffer* buffer)
{
	if (!buffer)
		return;

	buffer->eState = AK_NoMoreData;

	const AkUInt16 frameCount = buffer->MaxFrames();
	buffer->uValidFrames = frameCount;

	if (!audioSamplesFunction)
	{
		buffer->ZeroPadToMaxFrames();
		return;
	}

	const AkUInt32 channelCount = buffer->NumChannels();
	for (AkUInt32 i = 0; i < channelCount; ++i)
		if (audioSamplesFunction(playingID, buffer->GetChannel(i), i, frameCount))
			buffer->eState = AK_DataReady;
}

void SetAudioInputCallbacks(AkGetAudioSamples getAudioSamples, AkGetAudioFormat getAudioFormat)
{
	audioSamplesFunction = getAudioSamples;
	audioFormatFunction = getAudioFormat;

	SetAudioInputCallbacks(GetAudioSamples, GetAudioFormat, NULL);
}

AkCommunicationSettings::AkCommunicationSettings()
{
	szAppNetworkName[0] = 0;

#ifndef AK_OPTIMIZED
	AkCommSettings settingsComm;
	AK::Comm::GetDefaultInitSettings(settingsComm);

	uDiscoveryBroadcastPort = settingsComm.ports.uDiscoveryBroadcast;
	uCommandPort = settingsComm.ports.uCommand;

	bInitSystemLib = settingsComm.bInitSystemLib;
	commSystem = AkCommSystem_Socket;

#ifdef AK_NEEDS_PLATFORM_SPECIFIC_COMMS
	PlatformSpecificCommSettingsCopy(settingsComm, *this);
#endif

#endif // AK_OPTIMIZED
}

AkInitializationSettings::AkInitializationSettings()
{
	AK::StreamMgr::GetDefaultSettings(streamMgrSettings);
	AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);
	AK::SoundEngine::GetDefaultInitSettings(initSettings);
	AK::SoundEngine::GetDefaultPlatformInitSettings(platformSettings);

	uMemoryPrimarySbaInitSize = 2097152;
	uMemoryPrimaryTlsfInitSize = 8388608;
	uMemoryPrimaryTlsfSpanSize = 2097152;
	uMemoryPrimaryAllocSizeHuge = 2097152;
	uMemoryPrimaryReservedLimit = 0;

	uMemoryMediaTlsfInitSize = 2097152;
	uMemoryMediaTlsfSpanSize = 2097152;
	uMemoryMediaAllocSizeHuge = 2097152;
	uMemoryMediaReservedLimit = 0;

	uMemDebugLevel = 0;

	bUseSubFoldersForGeneratedFiles = false;

#ifdef AK_NEEDS_PLATFORM_SPECIFIC_THREADING_SETTINGS
	PlatformSpecificThreadingSettings(platformSettings, deviceSettings);
#endif
}

AkInitializationSettings::~AkInitializationSettings()
{
	delete[] initSettings.szPluginDLLPath;
}

AkSerializedExternalSourceInfo::~AkSerializedExternalSourceInfo()
{
	// ensure proper deletion of the szFile member
	AkSerializedExternalSourceInfo_szFile_set(this, 0);
}

AkSerializedExternalSourceInfo::AkSerializedExternalSourceInfo(const char* in_pszFileName, AkUInt32 in_iExternalSrcCookie, AkCodecID in_idCodec)
	: AkExternalSourceInfo()
{
	iExternalSrcCookie = in_iExternalSrcCookie;
	idCodec = in_idCodec;
	AkSerializedExternalSourceInfo_szFile_set(this, in_pszFileName);
}

void AkSerializedExternalSourceInfo::Clear()
{
	// this function is called at construction of the array class in C# to wipe the memory as if this specific object were default constructed
	iExternalSrcCookie = 0;
	idCodec = 0;
	szFile = 0;
	pInMemory = 0;
	uiMemorySize = 0;
	idFile = 0;
}

void AkSerializedExternalSourceInfo::Clone(const AkSerializedExternalSourceInfo& other)
{
	iExternalSrcCookie = other.iExternalSrcCookie;
	idCodec = other.idCodec;
	AkSerializedExternalSourceInfo_szFile_set(this, other.szFile);
	pInMemory = other.pInMemory;
	uiMemorySize = other.uiMemorySize;
	idFile = other.idFile;
}

AkUInt32 AkSerializedExternalSourceInfo_iExternalSrcCookie_get(AkSerializedExternalSourceInfo *info) { return info->iExternalSrcCookie; }
void AkSerializedExternalSourceInfo_iExternalSrcCookie_set(AkSerializedExternalSourceInfo *info, AkUInt32 value) { info->iExternalSrcCookie = value; }

AkCodecID AkSerializedExternalSourceInfo_idCodec_get(AkSerializedExternalSourceInfo *info) { return info->idCodec; }
void AkSerializedExternalSourceInfo_idCodec_set(AkSerializedExternalSourceInfo *info, AkCodecID value) { info->idCodec = value; }

const char* AkSerializedExternalSourceInfo_szFile_get(AkSerializedExternalSourceInfo *info) { return info->szFile; }
void AkSerializedExternalSourceInfo_szFile_set(AkSerializedExternalSourceInfo *info, const char* fileName)
{
	delete[] info->szFile;
	info->szFile = 0;

	if (fileName)
	{
		size_t length = strlen(fileName);
		if (length > 0)
		{
			char * strCopy = new char[length + 1];
			AKPLATFORM::SafeStrCpy(strCopy, fileName, length + 1);
			info->szFile = strCopy;
		}
	}
}

void* AkSerializedExternalSourceInfo_pInMemory_get(AkSerializedExternalSourceInfo *info) { return info->pInMemory; }
void AkSerializedExternalSourceInfo_pInMemory_set(AkSerializedExternalSourceInfo *info, void* value) { info->pInMemory = value; }

AkUInt32 AkSerializedExternalSourceInfo_uiMemorySize_get(AkSerializedExternalSourceInfo *info) { return info->uiMemorySize; }
void AkSerializedExternalSourceInfo_uiMemorySize_set(AkSerializedExternalSourceInfo *info, AkUInt32 value) { info->uiMemorySize = value; }

AkFileID AkSerializedExternalSourceInfo_idFile_get(AkSerializedExternalSourceInfo *info) { return info->idFile; }
void AkSerializedExternalSourceInfo_idFile_set(AkSerializedExternalSourceInfo *info, AkFileID value) { info->idFile = value; }

namespace ResourceMonitoring
{
	CAkLock Lock;
	AkResourceMonitorDataSummary savedDataSummary;

	void ClearDataSummary()
	{
		AkAutoLock<CAkLock> autoLock(Lock);
		savedDataSummary = AkResourceMonitorDataSummary{};
	}

	void GetResourceMonitorDataSummary(AkResourceMonitorDataSummary& resourceMonitorDataSummary)
	{
		AkAutoLock<CAkLock> autoLock(Lock);
		resourceMonitorDataSummary = savedDataSummary;
	}

	void Callback(const AkResourceMonitorDataSummary* in_pdataSummary)
	{
		if (in_pdataSummary)
		{
			AkAutoLock<CAkLock> autoLock(Lock);
			savedDataSummary = *in_pdataSummary;
		}
	}
}

namespace Capture
{
	template<typename Type>
	struct ListBareWithClear : AkListBare<Type>
	{
		auto begin() const { return AkListBare<Type>::Begin(); }
		auto end() const { return AkListBare<Type>::End(); }

		void Clear()
		{
			auto nn = end();
			for (auto it = begin(); it != nn; )
			{
				auto item = *it;
				++it;
				AkDelete(AkMemID_Integration, item);
			}

			AkListBare<Type>::RemoveAll();
		}
	};

	struct AudioData final
	{
		static AkUInt32 GetBufferSampleCount(const AkAudioBuffer& akAudioBuffer)
		{
			return static_cast<AkUInt32>(akAudioBuffer.uValidFrames) * akAudioBuffer.NumChannels();
		}

		const AkUInt32 uNumSamples{};

		AudioData(AkUInt32 uNumSamples)
			: uNumSamples(uNumSamples)
			, pSamples(reinterpret_cast<AkReal32*>(AkAlloc(AkMemID_Integration, uNumSamples * sizeof(*pSamples))))
		{
		}

		~AudioData()
		{
			AkFree(AkMemID_Integration, pSamples);
		}

		AudioData* pNextItem{};

		void CopyFrom(const AkAudioBuffer& akAudioBuffer)
		{
			AKASSERT(GetBufferSampleCount(akAudioBuffer) == uNumSamples);
			AKASSERT(pSamples);
			AKPLATFORM::AkMemCpy(pSamples, const_cast<AkAudioBuffer&>(akAudioBuffer).GetInterleavedData(), uNumSamples * sizeof(*pSamples));
		}

		AkUInt32 CopyTo(AkReal32* out_pSamples, AkUInt32 uCopySize, AkUInt32 uReadOffset)
		{
			AKASSERT(out_pSamples);
			AKASSERT(uNumSamples > uReadOffset);

			const AkUInt32 uRemainder{ uNumSamples - uReadOffset };
			if (uRemainder < uCopySize)
			{
				uCopySize = uRemainder;
			}

			AKPLATFORM::AkMemCpy(out_pSamples, pSamples + uReadOffset, uCopySize * sizeof(*pSamples));
			return uCopySize;
		}

		AudioData(AudioData&&) = default;
		AudioData(const AudioData&) = delete;
		AudioData& operator=(const AudioData) = delete;

	private:
		AkReal32* const pSamples{};
	};

	struct AudioDataList : ListBareWithClear<AudioData>
	{
		void Splice(AudioDataList& other)
		{
			if (m_pFirst == NULL)
			{
				m_pFirst = other.m_pFirst;
			}
			else
			{
				AkListBareNextItem<AudioData>::Get(Last()) = other.m_pFirst;
			}

			SetLast(other.Last());

			for (AudioData* audioData : other)
			{
				IncrementCount(audioData);
			}

			other.RemoveAll();
		}
	};

	struct AudioDataListWithLock final
	{
		void AddAudioData(AudioData* audioData)
		{
			AkAutoLock<CAkLock> autoLock(lock);
			list.AddLast(audioData);
		}

		void TransferTo(AudioDataList& other)
		{
			AkAutoLock<CAkLock> autoLock(lock);
			other.Splice(list);
		}

		AudioData* ExtractAudioData(AkUInt32 numSamples)
		{
			{
				AkAutoLock<CAkLock> autoLock(lock);
				auto end = list.End();
				for (auto it = list.BeginEx(); it != end; ++it)
				{
					auto audioData = *it;
					if (audioData->uNumSamples == numSamples)
					{
						list.Erase(it);
						return audioData;
					}
				}
			}

			return nullptr;
		}

		void Clear()
		{
			AkAutoLock<CAkLock> autoLock(lock);
			list.Clear();
		}

	private:
		CAkLock lock;
		AudioDataList list;
	};

	static AudioDataListWithLock g_reuseList;

	struct DeviceCaptureData final
	{
		DeviceCaptureData(AkOutputDeviceID in_idOutput) : outputDeviceId(in_idOutput) {}

		~DeviceCaptureData()
		{
			if (isRegistered)
				Unregister();
		}

		DeviceCaptureData* pNextItem{};

		void Register()
		{
			if (isRegistered || AK::SoundEngine::RegisterCaptureCallback(&Callback, outputDeviceId, this) != AK_Success)
				return;

			isRegistered = true;
		}

		void Unregister()
		{
			if (!isRegistered)
				return;

			AK::SoundEngine::UnregisterCaptureCallback(&Callback, outputDeviceId, this);
			isRegistered = false;
		}

		AkOutputDeviceID GetOutputDeviceID() const { return outputDeviceId; }

		AkUInt32 UpdateSampleCount()
		{
			accumulatingList.TransferTo(outgoingList);

			AkUInt32 uCapturedSampleCount{};
			for (AudioData* audioData : outgoingList)
			{
				uCapturedSampleCount += audioData->uNumSamples;
			}

			AKASSERT(uCapturedSampleCount >= readOffset);
			return uCapturedSampleCount - readOffset;
		}

		AkUInt32 GetSamples(AkReal32* out_pSamples, AkUInt32 in_uBufferSize)
		{
			auto it = outgoingList.BeginEx();

			AkUInt32 uRemainingOutputBufferSize{ in_uBufferSize };
			for (auto end = outgoingList.End(); it != end; )
			{
				if (uRemainingOutputBufferSize == 0)
					break;

				auto audioData = *it;
				it = outgoingList.Erase(it);

				if (readOffset > audioData->uNumSamples)
				{
					AKASSERT(audioData->uNumSamples > readOffset);
					readOffset = 0;
				}

				const AkUInt32 uCopySize = audioData->CopyTo(out_pSamples, uRemainingOutputBufferSize, readOffset);
				out_pSamples += uCopySize;
				readOffset += uCopySize;
				readOffset %= audioData->uNumSamples;
				uRemainingOutputBufferSize -= uCopySize;

				g_reuseList.AddAudioData(audioData);
			}

			return in_uBufferSize - uRemainingOutputBufferSize;
		}

		void Clear()
		{
			accumulatingList.Clear();
			outgoingList.Clear();
			readOffset = 0;
		}

	private:
		AudioDataListWithLock accumulatingList;
		AudioDataList outgoingList;
		const AkOutputDeviceID outputDeviceId{ AK_INVALID_OUTPUT_DEVICE_ID };
		AkUInt32 readOffset{};
		bool isRegistered{ false };

		void FillBuffers(AkAudioBuffer& in_CaptureBuffer)
		{
			auto numSamples = AudioData::GetBufferSampleCount(in_CaptureBuffer);
			if (numSamples == 0)
				return;

			auto audioData = g_reuseList.ExtractAudioData(numSamples);
			if (!audioData)
				audioData = AkNew(AkMemID_Integration, AudioData(numSamples));
			AKASSERT(audioData);

			audioData->CopyFrom(in_CaptureBuffer);
			accumulatingList.AddAudioData(audioData);
		}

		static void Callback(AkAudioBuffer& in_CaptureBuffer, AkOutputDeviceID in_idOutput, void* in_pCookie)
		{
			if (auto deviceCaptureData = reinterpret_cast<DeviceCaptureData*>(in_pCookie))
			{
				deviceCaptureData->FillBuffers(in_CaptureBuffer);
			}
		}
	};

	struct DeviceCaptureDataMap final
	{
		DeviceCaptureData& GetOrCreateDeviceCaptureData(AkOutputDeviceID in_idOutput)
		{
			auto deviceCaptureData = TryGetDeviceCaptureData(in_idOutput);
			if (deviceCaptureData)
				return *deviceCaptureData;

			deviceCaptureData = AkNew(AkMemID_Integration, DeviceCaptureData(in_idOutput));
			AKASSERT(deviceCaptureData);

			{
				AkAutoLock<CAkLock> autoLock(lock);
				deviceIdToCaptureDataList.AddLast(deviceCaptureData);
			}

			return *deviceCaptureData;
		}

		DeviceCaptureData* TryGetDeviceCaptureData(AkOutputDeviceID in_idOutput)
		{
			AkAutoLock<CAkLock> autoLock(lock);
			for (DeviceCaptureData* deviceCaptureData : deviceIdToCaptureDataList)
				if (deviceCaptureData->GetOutputDeviceID() == in_idOutput)
					return deviceCaptureData;

			return nullptr;
		}

		void ClearData()
		{
			AkAutoLock<CAkLock> autoLock(lock);
			for (DeviceCaptureData* deviceCaptureData : deviceIdToCaptureDataList)
				deviceCaptureData->Clear();
		}

		void Terminate()
		{
			AkAutoLock<CAkLock> autoLock(lock);
			deviceIdToCaptureDataList.Clear();
		}

	private:
		CAkLock lock;
		ListBareWithClear<DeviceCaptureData> deviceIdToCaptureDataList;
	};

	static DeviceCaptureDataMap g_perDeviceCaptureData;

	AkForceInline static DeviceCaptureData& GetDeviceCaptureData(AkOutputDeviceID in_idOutput)
	{
		return g_perDeviceCaptureData.GetOrCreateDeviceCaptureData(in_idOutput);
	}

	AkForceInline static DeviceCaptureData* TryGetDeviceCaptureData(AkOutputDeviceID in_idOutput)
	{
		return g_perDeviceCaptureData.TryGetDeviceCaptureData(in_idOutput);
	}

	AkForceInline static void ClearCaptureData()
	{
		g_perDeviceCaptureData.ClearData();
		g_reuseList.Clear();
	}

	AkForceInline static void Terminate()
	{
		g_perDeviceCaptureData.Terminate();
		g_reuseList.Clear();
	}
}

//-----------------------------------------------------------------------------------------
// Set the g_Activity variable necessary for the io hook initialization on android
//-----------------------------------------------------------------------------------------
#if defined AK_ANDROID
void SetAndroidActivity(void* activity)
{
	AkUnityAndroidSystem::SetAndroidActivity(activity);
}
#endif

//-----------------------------------------------------------------------------------------
// Sound Engine initialization.
//-----------------------------------------------------------------------------------------

AKRESULT Init(AkInitializationSettings* settings)
{
	if (!settings)
	{
		errorlogger("Null pointer to AkInitializationSettings structure.");
		return AK_InvalidParameter;
	}

	AkMemSettings memSettings;
	AK::MemoryMgr::GetDefaultSettings(memSettings);

	settings->initSettings.pfnAssertHook = AkUnityAssertHook;	

	memSettings.memoryArenaSettings[AkMemoryMgrArena_Primary].uSbaInitSize      = settings->uMemoryPrimarySbaInitSize;
	memSettings.memoryArenaSettings[AkMemoryMgrArena_Primary].uTlsfInitSize     = settings->uMemoryPrimaryTlsfInitSize;
	memSettings.memoryArenaSettings[AkMemoryMgrArena_Primary].uTlsfSpanSize     = settings->uMemoryPrimaryTlsfSpanSize;
	memSettings.memoryArenaSettings[AkMemoryMgrArena_Primary].uAllocSizeHuge    = settings->uMemoryPrimaryAllocSizeHuge;
	memSettings.memoryArenaSettings[AkMemoryMgrArena_Primary].uMemReservedLimit = settings->uMemoryPrimaryReservedLimit;

	memSettings.memoryArenaSettings[AkMemoryMgrArena_Media].uTlsfInitSize       = settings->uMemoryMediaTlsfInitSize;
	memSettings.memoryArenaSettings[AkMemoryMgrArena_Media].uTlsfSpanSize       = settings->uMemoryMediaTlsfSpanSize;
	memSettings.memoryArenaSettings[AkMemoryMgrArena_Media].uAllocSizeHuge      = settings->uMemoryMediaAllocSizeHuge;
	memSettings.memoryArenaSettings[AkMemoryMgrArena_Media].uMemReservedLimit   = settings->uMemoryMediaReservedLimit;
	
	memSettings.uMemoryDebugLevel = settings->uMemDebugLevel;

	// Create and initialize an instance of our memory manager.
	if (AK::MemoryMgr::Init(&memSettings) != AK_Success)
	{
		errorlogger("Could not create the memory manager.");
		return AK_MemManagerNotInitialized;
	}

	// Create and initialize an instance of the default stream manager.
	if (!AK::StreamMgr::Create(settings->streamMgrSettings))
	{
		errorlogger("Could not create the Stream Manager.");
		return AK_StreamMgrNotInitialized;
	}

	// Create an IO device.
#if defined AK_ANDROID
	settings->platformSettings.pJavaVM = AkUnityAndroidSystem::g_JavaVm;
	if (AkUnityAndroidSystem::InitAndroidSystem(settings->platformSettings.jActivity) != AK_Success)
	{
		errorlogger("Android initialization failure.");
		return AK_Fail;
	}
#else
	g_lowLevelIO.SetUseSubfoldering(settings->bUseSubFoldersForGeneratedFiles);
#endif
	if (g_lowLevelIO.Init(settings->deviceSettings) != AK_Success)
	{
		errorlogger("Cannot create streaming I/O device.");
		return AK_Fail;
	}

#if defined AK_HARMONY
	g_lowLevelIO.UseResourceManager(AkUnityOpenHarmonySystem::Instance().GetResourceManager());
#endif

#if defined(AK_IOS) || defined(AK_TVOS) || defined(AK_VISIONOS)
	settings->platformSettings.audioCallbacks.interruptionCallback = AkCallbackSerializer::AudioInterruptionCallbackFunc;
#endif
	settings->initSettings.BGMCallback = AkCallbackSerializer::AudioSourceChangeCallbackFunc;

#ifdef AK_VORBIS_HW_SUPPORTED
	static bool bVorbisHwAcceleratorLoaded = false;
	if (!bVorbisHwAcceleratorLoaded && settings->platformSettings.bVorbisHwAcceleration)
	{
		AKRESULT eResult = AK::LoadVorbisHwAcceleratorLibrary("AkVorbisHwAccelerator", settings->initSettings.szPluginDLLPath);
		switch (eResult)
		{
			case AK_Success:
				bVorbisHwAcceleratorLoaded = true;
				break;
			// A failure here is not critical. We emit the proper error in the log, but we can allow the sound engine to continue to initialize.
			case AK_NotCompatible:
				errorlogger("Vorbis acceleration library failed to load due to platform SDK mismatch.");
				break;
			case AK_InvalidFile:
				errorlogger("Vorbis acceleration library failed to load because the library file could not be found in the game package.");
				break;
			case AK_InsufficientMemory:
				errorlogger("Vorbis acceleration library failed to load due to insufficient resources or memory.");
				break;
			default:
				errorlogger("Vorbis acceleration library failed to load due an unexpected error.");
				break;
		}
	}
#endif

	// Initialize sound engine.
	AKRESULT res = AK::SoundEngine::Init(&settings->initSettings, &settings->platformSettings);
	if ( res != AK_Success )
	{
		errorlogger("Cannot initialize sound engine.");
		return res;
	}

	return AK_Success;
}

AKRESULT InitSpatialAudio(AkSpatialAudioInitSettings* settings)
{
	if (!settings)
	{
		errorlogger("Null pointer to AkSpatialAudioInitSettings structure.");
		return AK_InvalidParameter;
	}

	// Initialize SpatialAudio.
	if (AK::SpatialAudio::Init(*settings) != AK_Success)
	{
		errorlogger("Cannot initialize spatial audio.");
		return AK_Fail;
	}

	// disable spatial audio until a spatial audio listener is registered
	AK::SpatialAudio::RegisterListener(AK_INVALID_GAME_OBJECT);
	return AK_Success;
}

AKRESULT InitCommunication(AkCommunicationSettings* settings)
{
#ifdef AK_OPTIMIZED
	return AK_NotImplemented;
#else
	if (!settings)
	{
		errorlogger("Null pointer to AkCommunicationSettings structure.");
		return AK_InvalidParameter;
	}

	// Initialize communication.
	AkCommSettings settingsComm;
	AK::Comm::GetDefaultInitSettings(settingsComm);
	settingsComm.ports.uDiscoveryBroadcast = settings->uDiscoveryBroadcastPort;
	settingsComm.ports.uCommand = settings->uCommandPort;
	settingsComm.bInitSystemLib = settings->bInitSystemLib;
	settingsComm.commSystem = static_cast<AkCommSettings::AkCommSystem>(settings->commSystem);

	AKPLATFORM::SafeStrCpy(settingsComm.szAppNetworkName, settings->szAppNetworkName, AK_COMM_SETTINGS_MAX_STRING_SIZE);
	if (AK::Comm::Init(settingsComm) != AK_Success)
	{
		errorlogger("Cannot initialize Wwise communication.");
		return AK_Fail;
	}

	return AK_Success;
#endif // AK_OPTIMIZED
}

//-----------------------------------------------------------------------------------------
// Sound Engine termination.
//-----------------------------------------------------------------------------------------
void Term()
{
	if (!AK::SoundEngine::IsInitialized())
	{
		errorlogger("Term() called before successful initialization.");
		return;
	}

	Capture::Terminate();

	AK::SoundEngine::StopAll();

#ifndef AK_OPTIMIZED
	AK::Comm::Term();
#endif // AK_OPTIMIZED

	AK::SoundEngine::Term();

	AkCallbackSerializer::Term();

	g_lowLevelIO.Term();
	
#if defined AK_ANDROID
	AkUnityAndroidSystem::TermAndroidSystem();
#endif
	

	if ( AK::IAkStreamMgr::Get() )
		AK::IAkStreamMgr::Get()->Destroy();

	AK::MemoryMgr::Term();
}

AKRESULT SetGameName(const AkOSChar* in_GameName)
{
#ifdef AK_OPTIMIZED
	return AK_NotImplemented;
#else
	AkOSChar* GameNameOsString = NULL;
	char* CharName;
	CONVERT_OSCHAR_TO_CHAR(in_GameName, CharName);
	AKPLATFORM::SafeStrCpy(g_GameName, CharName, AK_COMM_SETTINGS_MAX_STRING_SIZE);
	return AK_Success;
#endif
}

//-----------------------------------------------------------------------------------------
// Access to LowLevelIO's file localization.
//-----------------------------------------------------------------------------------------
// File system interface.
AKRESULT SetBasePath(const AkOSChar* in_pszBasePath)
{
	AKPLATFORM::SafeStrCpy(g_basePath, in_pszBasePath, AK_MAX_PATH);
	return g_lowLevelIO.SetBasePath(in_pszBasePath);
}

AKRESULT AddBasePath(const AkOSChar* in_pszBasePath)
{
	return g_lowLevelIO.AddBasePath(in_pszBasePath);
}

AKRESULT SetDecodedBankPath(const AkOSChar* in_DecodedPath)
{

	AKRESULT result = CreateDirectoryStructure(in_DecodedPath);
	AKPLATFORM::SafeStrCpy(g_decodedBankPath, (result == AK_Success) ? in_DecodedPath : AKTEXT(""), AK_MAX_PATH);

	return result;
}

AKRESULT SetCurrentLanguage(const AkOSChar*	in_pszLanguageName)
{
	if (AKPLATFORM::OsStrCmp(g_decodedBankPath, AKTEXT("")) != 0)
	{
		AkOSChar szDecodedPathCopy[AK_MAX_PATH];
		AKPLATFORM::SafeStrCpy(szDecodedPathCopy, g_decodedBankPath, AK_MAX_PATH);
		AKPLATFORM::SafeStrCat(szDecodedPathCopy, &AK_PATH_SEPARATOR[0], AK_MAX_PATH);
		AKPLATFORM::SafeStrCat(szDecodedPathCopy, in_pszLanguageName, AK_MAX_PATH);
		CreateDirectoryStructure(szDecodedPathCopy);
	}
	return AK::StreamMgr::SetCurrentLanguage(in_pszLanguageName);
}

const AkOSChar* GetCurrentLanguage()
{
	return AK::StreamMgr::GetCurrentLanguage();
}

AKRESULT LoadFilePackage(
	const AkOSChar* in_pszFilePackageName,	// File package name. Location is resolved using base class' Open().
	AkUInt32 &		out_uPackageID)			// Returned package ID.
{
	return g_lowLevelIO.LoadFilePackage(in_pszFilePackageName, out_uPackageID);
}

// Unload a file package.
// Returns AK_Success if in_uPackageID exists, AK_Fail otherwise.
// WARNING: This method is not thread safe. Ensure there are no I/O occurring on this device
// when unloading a file package.
AKRESULT UnloadFilePackage(AkUInt32 in_uPackageID)
{
	return g_lowLevelIO.UnloadFilePackage(in_uPackageID);
}

// Unload all file packages.
// Returns AK_Success;
// WARNING: This method is not thread safe. Ensure there are no I/O occurring on this device
// when unloading a file package.
AKRESULT UnloadAllFilePackages()
{
	return g_lowLevelIO.UnloadAllFilePackages();
}

AKRESULT RegisterGameObjInternal(AkGameObjectID in_GameObj)
{
	if (AK::SoundEngine::IsInitialized())
		return AK::SoundEngine::RegisterGameObj(in_GameObj);
	return AK_Fail;
}

AKRESULT RegisterGameObjInternal_WithName(AkGameObjectID in_GameObj, const char* in_pszObjName)
{
	if (AK::SoundEngine::IsInitialized())
	{
		return AK::SoundEngine::RegisterGameObj(in_GameObj, in_pszObjName);
	}
	return AK_Fail;
}

AKRESULT UnregisterGameObjInternal(AkGameObjectID in_GameObj)
{
	if (AK::SoundEngine::IsInitialized())
		return AK::SoundEngine::UnregisterGameObj(in_GameObj);
	return AK_Fail;
}

AKRESULT LoadAndDecodeInternal(void* in_BankData, AkUInt32 in_BankDataSize, bool in_bSaveDecodedBank, AkOSChar* in_szDecodedBankName, bool in_bIsLanguageSpecific, AkBankID& out_bankID)
{
	AKRESULT eResult = AK_Fail;

	// Get the decoded size
	AkUInt32 decodedSize = 0;
	void * pDecodedData = NULL;
	eResult = AK::SoundEngine::DecodeBank(in_BankData, in_BankDataSize, AK_DEFAULT_POOL_ID, pDecodedData, decodedSize); // get the decoded size
	if( eResult == AK_Success )
	{
		pDecodedData = malloc(decodedSize);

		// Actually decode the bank
		if( pDecodedData != NULL )
		{
			eResult = AK::SoundEngine::DecodeBank(in_BankData, in_BankDataSize, AK_DEFAULT_POOL_ID, pDecodedData, decodedSize); 
			if( eResult == AK_Success )
			{
				// TODO: We could use the async load bank to Load and Save at the same time.

				// 3- Load the bank from the decoded memory pointer.
				AkBankType BankType;
				eResult = AK::SoundEngine::LoadBankMemoryCopy(pDecodedData, decodedSize, out_bankID, BankType);
					
				// 4- Save the decoded bank to disk. 
				if (in_bSaveDecodedBank)
				{
					AKRESULT eSaveResult = SaveDecodedBank(in_szDecodedBankName, pDecodedData, decodedSize, in_bIsLanguageSpecific);
					if (eSaveResult != AK_Success)
					{
						eResult = eSaveResult;
						AK::Monitor::PostString("Could not save the decoded bank !", AK::Monitor::ErrorLevel_Error);
					}
				}
			}

			free(pDecodedData);
		}
		else
		{
			eResult = AK_InsufficientMemory;
		}
	}

	return eResult;
}
	
void SanitizeBankNameWithoutExtension(const AkOSChar* in_BankName, AkOSChar* out_SanitizedString)
{
	AKPLATFORM::SafeStrCpy(out_SanitizedString, in_BankName, AK_MAX_PATH - 1);
	out_SanitizedString[AK_MAX_PATH - 1] = '\0';
}

void SanitizeBankName(const AkOSChar* in_BankName, AkOSChar* out_SanitizedString)
{
	const AkOSChar* ext = AKTEXT(".bnk");
	SanitizeBankNameWithoutExtension(in_BankName, out_SanitizedString);
	AKPLATFORM::SafeStrCat(out_SanitizedString, ext, AK_MAX_PATH - 1);
}

AKRESULT LoadAndDecodeBankFromMemory(void* in_BankData, AkUInt32 in_BankDataSize, bool in_bSaveDecodedBank, const AkOSChar* in_DecodedBankName, bool in_bIsLanguageSpecific, AkBankID& out_bankID)
{
	AkOSChar osString[AK_MAX_PATH];
	SanitizeBankName(in_DecodedBankName, osString);
	return LoadAndDecodeInternal(in_BankData, in_BankDataSize, in_bSaveDecodedBank, osString, in_bIsLanguageSpecific, out_bankID);
}

AKRESULT LoadAndDecodeBank(const AkOSChar* in_pszString, bool in_bSaveDecodedBank, AkBankID& out_bankID)
{
	AKRESULT eResult = AK_Fail;
	AkOSChar osString[AK_MAX_PATH];
	SanitizeBankName(in_pszString, osString);

	if( in_bSaveDecodedBank )
	{
		// 1- Open and read the file ourselves using the StreamMgr
		AkFileSystemFlags flags;
		AK::IAkStdStream *	pStream;
		flags.uCompanyID = AKCOMPANYID_AUDIOKINETIC;
		flags.uCodecID = AKCODECID_BANK;
		flags.uCustomParamSize = 0;
		flags.pCustomParam = NULL;
		flags.bIsLanguageSpecific = true;

		eResult = AK::IAkStreamMgr::Get()->CreateStd( AkFileOpenData(osString, AK_OpenModeRead, &flags), pStream, true );
		if( eResult == AK_Success )
		{
			AkStreamInfo info;
			pStream->GetInfo( info );
			AkUInt8 * pFileData = (AkUInt8*)malloc((size_t)info.uSize);
			if( pFileData != NULL )
			{
				AkUInt32 outSize;
				eResult = pStream->Read(pFileData, (AkUInt32)info.uSize, true, AK_DEFAULT_PRIORITY, info.uSize/AK_DEFAULT_BANK_THROUGHPUT, outSize);
				if( eResult == AK_Success )
				{
					pStream->Destroy();
					pStream = NULL;

					eResult = LoadAndDecodeInternal(pFileData, outSize, in_bSaveDecodedBank, osString, info.bIsLanguageSpecific, out_bankID);
				}

				free(pFileData);
				pFileData = NULL;
			}
			else
			{
				eResult = AK_InsufficientMemory;
			}
		}
	}
	else
	{
		// Use the Prepare Bank mechanism. This will decode on load, but not save anything
		eResult = AK::SoundEngine::PrepareBank(AK::SoundEngine::Preparation_LoadAndDecode, osString);

		AkOSChar osBankName[AK_MAX_PATH];
		SanitizeBankNameWithoutExtension(in_pszString, osBankName);
		out_bankID = AK::SoundEngine::GetIDFromString(osBankName);
	}

	return eResult;
}

AKRESULT SetObjectPosition(AkGameObjectID in_GameObjectID, AkVector Pos, AkVector Front, AkVector Top)
{
	if (!AK::SoundEngine::IsInitialized())
		return AK_Fail;

	AkTransform transform;
	transform.Set(Pos, Front, Top);
	return AK::SoundEngine::SetPosition(in_GameObjectID, transform);
}

AKRESULT GetSourceMultiplePlayPositions(
	AkPlayingID		in_PlayingID,				///< Playing ID returned by AK::SoundEngine::PostEvent()
	AkUniqueID *	out_audioNodeID,			///< Audio Node IDs of sources associated with the specified playing ID. Indexes in this array match indexes in out_msTime.
	AkUniqueID *	out_mediaID,				///< Media ID of playing item. (corresponds to 'ID' attribute of 'File' element in SoundBank metadata file)
	AkTimeMs *		out_msTime,					///< Audio positions of sources associated with the specified playing ID. Indexes in this array match indexes in out_audioNodeID.
	AkUInt32 *		io_pcPositions,				///< Number of entries in out_puPositions. Needs to be set to the size of the array: it is adjusted to the actual number of returned entries
	bool			in_bExtrapolate				///< Position is extrapolated based on time elapsed since last sound engine update
	)
{
	if (*io_pcPositions == 0)
	{
		return AK_Fail;
	}

	AkSourcePosition * sourcePositionInfo = (AkSourcePosition*)AkAlloca((*io_pcPositions) * sizeof(AkSourcePosition));
	if (!sourcePositionInfo)
	{
		return AK_Fail;
	}

	AKRESULT res = AK::SoundEngine::GetSourcePlayPositions(in_PlayingID, sourcePositionInfo, io_pcPositions, in_bExtrapolate);

	for (AkUInt32 i = 0; i < *io_pcPositions; ++i)
	{
		out_audioNodeID[i] = sourcePositionInfo[i].audioNodeID;
		out_mediaID[i] = sourcePositionInfo[i].mediaID;
		out_msTime[i] = sourcePositionInfo[i].msTime;
	}

	return res;
}

AKRESULT SetListeners(AkGameObjectID in_emitterGameObj, AkGameObjectID* in_pListenerGameObjs, AkUInt32 in_uNumListeners)
{
	return AK::SoundEngine::SetListeners(in_emitterGameObj, in_pListenerGameObjs, in_uNumListeners);
}

AKRESULT SetDefaultListeners(AkGameObjectID* in_pListenerObjs, AkUInt32 in_uNumListeners)
{
	return AK::SoundEngine::SetDefaultListeners(in_pListenerObjs, in_uNumListeners);
}
	
AkUInt32 GetNumOutputDevices(AkUniqueID in_AudioDeviceShareSetID)
{
	AkUInt32 NumDevices = 0;
	if (AK::SoundEngine::IsInitialized()) 
	{
		AKRESULT res = AK::SoundEngine::GetDeviceList(in_AudioDeviceShareSetID, NumDevices, nullptr);
	}
	else 
	{
		AKPLATFORM::OutputDebugMsg("Wwise warning in AK::SoundEngine::GetDeviceList(AkUniqueID,AkUInt32 &,AkDeviceDescription *): AkInitializer.cs Awake() was not executed yet. Set the Script Execution Order properly so the current call is executed after.");
	}

	return NumDevices;
}
AKRESULT AddOutput(const AkOutputSettings & in_Settings, AkOutputDeviceID *out_pDeviceID, AkGameObjectID* in_pListenerIDs, AkUInt32 in_uNumListeners)
{
	return AK::SoundEngine::AddOutput(in_Settings, out_pDeviceID, in_pListenerIDs, in_uNumListeners);
}

void GetDefaultStreamSettings(AkStreamMgrSettings & out_settings)
{
	AK::StreamMgr::GetDefaultSettings(out_settings);
}

void GetDefaultDeviceSettings(AkDeviceSettings & out_settings)
{
	AK::StreamMgr::GetDefaultDeviceSettings(out_settings);
}

void GetDefaultInitSettings(AkInitSettings & out_settings)
{
	AK::SoundEngine::GetDefaultInitSettings(out_settings);
}

void GetDefaultPlatformInitSettings(AkPlatformInitSettings &out_settings)
{
	AK::SoundEngine::GetDefaultPlatformInitSettings(out_settings);
}

AkUInt32 GetMajorMinorVersion()
{
	return (AK_WWISESDK_VERSION_MAJOR << 16) | AK_WWISESDK_VERSION_MINOR;
}

AkUInt32 GetSubminorBuildVersion()
{
	return (AK_WWISESDK_VERSION_SUBMINOR << 16) | AK_WWISESDK_VERSION_BUILD;
}

void StartResourceMonitoring()
{
	ResourceMonitoring::ClearDataSummary();
	AK::SoundEngine::RegisterResourceMonitorCallback(&ResourceMonitoring::Callback);
}

void GetResourceMonitorDataSummary(AkResourceMonitorDataSummary& resourceMonitorDataSummary)
{
	ResourceMonitoring::GetResourceMonitorDataSummary(resourceMonitorDataSummary);
}

void StopResourceMonitoring()
{
	AK::SoundEngine::UnregisterResourceMonitorCallback(&ResourceMonitoring::Callback);
	ResourceMonitoring::ClearDataSummary();
}

void ClearCaptureData()
{
	Capture::ClearCaptureData();
}

void StartDeviceCapture(AkOutputDeviceID in_idOutput)
{
	Capture::GetDeviceCaptureData(in_idOutput).Register();
}

void StopDeviceCapture(AkOutputDeviceID in_idOutput)
{
	if (auto deviceCaptureData = Capture::TryGetDeviceCaptureData(in_idOutput))
		deviceCaptureData->Unregister();
}

AkUInt32 UpdateCaptureSampleCount(AkOutputDeviceID in_idOutput)
{
	auto deviceCaptureData = Capture::TryGetDeviceCaptureData(in_idOutput);
	return deviceCaptureData ? deviceCaptureData->UpdateSampleCount() : 0;
}

AkUInt32 GetCaptureSamples(AkOutputDeviceID in_idOutput, AkReal32* out_pSamples, AkUInt32 in_uBufferSize)
{
	auto deviceCaptureData = Capture::TryGetDeviceCaptureData(in_idOutput);
	return deviceCaptureData ? deviceCaptureData->GetSamples(out_pSamples, in_uBufferSize) : 0;
}

AKRESULT SetRoomPortal(AkPortalID in_PortalID, AkRoomID FrontRoom, AkRoomID BackRoom, const AkTransform& Transform, const AkExtent& Extent, bool bEnabled, const char* in_pName)
{
	AkPortalParams portalParams;
	portalParams.FrontRoom = FrontRoom;
	portalParams.BackRoom = BackRoom;
	portalParams.Transform = Transform;
	portalParams.Extent = Extent;
	portalParams.bEnabled = bEnabled;

	return AK::SpatialAudio::SetPortal(in_PortalID, portalParams, in_pName);
}

AKRESULT SetRoom(AkRoomID in_RoomID, AkRoomParams& in_roomParams,AkGeometryInstanceID GeometryInstanceID, const char* in_pName)
{
	AkRoomParams roomParams;
	roomParams = in_roomParams;
	roomParams.GeometryInstanceID = GeometryInstanceID;

	return AK::SpatialAudio::SetRoom(in_RoomID, roomParams, in_pName);
}

AKRESULT RegisterSpatialAudioListener(AkGameObjectID in_gameObjectID)
{
	return AK::SpatialAudio::RegisterListener(in_gameObjectID);
}

AKRESULT UnregisterSpatialAudioListener(AkGameObjectID in_gameObjectID)
{
	return AK::SpatialAudio::UnregisterListener(in_gameObjectID);
}

AKRESULT SetGeometry(AkGeometrySetID in_GeomSetID,
	AkTriangle* Triangles,
	AkUInt32 NumTriangles,
	AkVertex* Vertices,
	AkUInt32 NumVertices,
	AkAcousticSurface* Surfaces,
	AkUInt32 NumSurfaces,
	bool EnableDiffraction,
	bool EnableDiffractionOnBoundaryEdges)
{
	AkGeometryParams params;
	params.Triangles = Triangles;
	params.NumTriangles = (AkTriIdx)NumTriangles;
	params.Vertices = Vertices;
	params.NumVertices = (AkVertIdx)NumVertices;
	params.Surfaces = Surfaces;
	params.NumSurfaces = (AkSurfIdx)NumSurfaces;
	params.EnableDiffraction = EnableDiffraction;
	params.EnableDiffractionOnBoundaryEdges = EnableDiffractionOnBoundaryEdges;

	return AK::SpatialAudio::SetGeometry(in_GeomSetID, params);
}

AKRESULT SetGeometryInstance(
	AkGeometryInstanceID in_GeomInstanceID,
	const AkTransform& Transform,
	const AkVector& Scale,
	AkGeometrySetID GeometrySetID,
	bool UseForReflectionAndDiffraction,
	bool BypassPortalSubtraction,
	bool Solid)
{
	AkGeometryInstanceParams params;
	params.PositionAndOrientation = Transform;
	params.Scale = Scale;
	params.GeometrySetID = GeometrySetID;
	params.UseForReflectionAndDiffraction = UseForReflectionAndDiffraction;
	params.BypassPortalSubtraction = BypassPortalSubtraction;
	params.IsSolid = Solid;

	return AK::SpatialAudio::SetGeometryInstance(in_GeomInstanceID, params);
}

AKRESULT SaveDecodedBank(AkOSChar * in_OrigBankFile, void * in_pDecodedData, AkUInt32 in_decodedSize, bool in_bIsLanguageSpecific)
{
	AkFileSystemFlags flags;
	AK::IAkStdStream *	pStream;
	flags.uCompanyID = AKCOMPANYID_AUDIOKINETIC;
	flags.uCodecID = AKCODECID_BANK;
	flags.uCustomParamSize = 0;
	flags.pCustomParam = NULL;
	flags.bIsLanguageSpecific = in_bIsLanguageSpecific;

	AKRESULT eResult = AK::IAkStreamMgr::Get()->CreateStd(AkFileOpenData(in_OrigBankFile, AK_OpenModeWrite , &flags), pStream, true);
	if (eResult == AK_Success)
	{
		AkUInt32 outSize = 0;
		eResult = pStream->Write(in_pDecodedData, in_decodedSize, true, AK_DEFAULT_PRIORITY, in_decodedSize / AK_DEFAULT_BANK_THROUGHPUT, outSize);

		pStream->Destroy();
		pStream = NULL;
	}

	return eResult;
}

AKRESULT QueryReflectionPaths(AkGameObjectID in_gameObjectID, AkUInt32 in_positionIndex, AkVector& out_listenerPos, AkVector& out_emitterPos, AkReflectionPathInfo* out_aPaths, AkUInt32& io_uArraySize)
{
	AkVector64 listenerPosDbl, emitterPosDbl;
	listenerPosDbl.Zero();
	emitterPosDbl.Zero();

	AKRESULT result = AK::SpatialAudio::QueryReflectionPaths(in_gameObjectID, in_positionIndex, listenerPosDbl, emitterPosDbl, out_aPaths, io_uArraySize);

	out_listenerPos.X = (float)listenerPosDbl.X;
	out_listenerPos.Y = (float)listenerPosDbl.Y;
	out_listenerPos.Z = (float)listenerPosDbl.Z;

	out_emitterPos.X = (float)emitterPosDbl.X;
	out_emitterPos.Y = (float)emitterPosDbl.Y;
	out_emitterPos.Z = (float)emitterPosDbl.Z;

	return result;
}

AKRESULT QueryDiffractionPaths(AkGameObjectID in_gameObjectID, AkUInt32 in_positionIndex, AkVector& out_listenerPos, AkVector& out_emitterPos, AkDiffractionPathInfo* out_aPaths, AkUInt32& io_uArraySize)
{
	AkVector64 listenerPosDbl, emitterPosDbl;
	listenerPosDbl.Zero();
	emitterPosDbl.Zero();

	AKRESULT result = AK::SpatialAudio::QueryDiffractionPaths(in_gameObjectID, in_positionIndex, listenerPosDbl, emitterPosDbl, out_aPaths, io_uArraySize);

	out_listenerPos.X = (float)listenerPosDbl.X;
	out_listenerPos.Y = (float)listenerPosDbl.Y;
	out_listenerPos.Z = (float)listenerPosDbl.Z;

	out_emitterPos.X = (float)emitterPosDbl.X;
	out_emitterPos.Y = (float)emitterPosDbl.Y;
	out_emitterPos.Z = (float)emitterPosDbl.Z;

	return result;
}

void PerformStreamMgrIO()
{
	AKVERIFY(AK_Success == AK::StreamMgr::PerformIO());
}

