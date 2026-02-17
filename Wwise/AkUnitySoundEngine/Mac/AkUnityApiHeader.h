/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Apache License Usage

Alternatively, this file may be used under the Apache License, Version 2.0 (the 
"Apache License"); you may not use this file except in compliance with the 
Apache License. You may obtain a copy of the Apache License at 
http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing, software distributed
under the Apache License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
OR CONDITIONS OF ANY KIND, either express or implied. See the Apache License for
the specific language governing permissions and limitations under the License.

  Copyright (c) 2026 Audiokinetic Inc.
*******************************************************************************/

// AkSoundEngine.h

/// \file 
/// The main sound engine interface.


#ifndef _AK_SOUNDENGINE_H_
#define _AK_SOUNDENGINE_H_

#include <AK/SoundEngine/Common/AkSoundEngineExport.h>
#include <AK/SoundEngine/Common/AkTypes.h>
#include <AK/SoundEngine/Common/IAkPlugin.h>
#include <AK/SoundEngine/Common/AkCallback.h>

#if defined(AK_NULL_PLATFORM)
// null platform has no platform-specific soundengine 
struct AkPlatformInitSettings { };
#elif defined(AK_WIN)
#include <AK/SoundEngine/Platforms/Windows/AkWinSoundEngine.h>
#include <AK/SoundEngine/Platforms/Windows/AkPlatformContext.h>

#elif defined (AK_MAC_OS_X)
#include <AK/SoundEngine/Platforms/Mac/AkMacSoundEngine.h>
#include <AK/SoundEngine/Platforms/Mac/AkPlatformContext.h>

#elif defined (AK_IOS)
#include <AK/SoundEngine/Platforms/iOS/AkiOSSoundEngine.h>
#include <AK/SoundEngine/Platforms/iOS/AkPlatformContext.h>

#elif defined(AK_XBOXGC) // Gaming.Xbox platform
#include <AK/SoundEngine/Platforms/XboxGC/AkXboxGCSoundEngine.h>
#include <AK/SoundEngine/Platforms/XboxGC/AkPlatformContext.h>

#elif defined( AK_ANDROID )
#include <AK/SoundEngine/Platforms/Android/AkAndroidSoundEngine.h>
#include <AK/SoundEngine/Platforms/Android/AkPlatformContext.h>

#elif defined( AK_HARMONY )
#include <AK/SoundEngine/Platforms/OpenHarmony/AkOpenHarmonySoundEngine.h>
#include <AK/SoundEngine/Platforms/OpenHarmony/AkPlatformContext.h>

#elif defined (AK_PS4)
#include <AK/SoundEngine/Platforms/PS4/AkPS4SoundEngine.h>
#include <AK/SoundEngine/Platforms/PS4/AkPlatformContext.h>

#elif defined (AK_PS5)
#include <AK/SoundEngine/Platforms/PS5/AkPS5SoundEngine.h>
#include <AK/SoundEngine/Platforms/PS5/AkPlatformContext.h>

#elif defined( AK_LINUX_DESKTOP )
#include <AK/SoundEngine/Platforms/Linux/AkLinuxSoundEngine.h>
#include <AK/SoundEngine/Platforms/Linux/AkPlatformContext.h>

#elif defined( AK_EMSCRIPTEN )
#include <AK/SoundEngine/Platforms/Emscripten/AkEmscriptenSoundEngine.h>
#include <AK/SoundEngine/Platforms/Emscripten/AkPlatformContext.h>

#elif defined( AK_QNX  )
#include <AK/SoundEngine/Platforms/QNX/AkQNXSoundEngine.h>
#include <AK/SoundEngine/Platforms/QNX/AkPlatformContext.h>

#elif defined( AK_NX )
#include <AK/SoundEngine/Platforms/NX/AkNXSoundEngine.h>
#include <AK/SoundEngine/Platforms/NX/AkPlatformContext.h>

#elif defined( AK_OUNCE )
#include <AK/SoundEngine/Platforms/Ounce/AkOunceSoundEngine.h>
#include <AK/SoundEngine/Platforms/Ounce/AkPlatformContext.h>

#else
#error AkSoundEngine.h: Undefined platform
#endif

#ifndef AK_ASSERT_HOOK
	/// Function called on assert handling, optional
	/// \sa 
	/// - AkInitSettings
	AK_CALLBACK( void, AkAssertHook)( 
		const char * in_pszExpression,	///< Expression
		const char * in_pszFileName,	///< File Name
		int in_lineNumber				///< Line Number
		);
	#define AK_ASSERT_HOOK
#endif

/// Callback function prototype for User Music notifications
///	It is useful for reacting to user music playback.
///
/// \sa
/// - \ref AkGlobalCallbackFunc
/// - \ref AkPlatformInitSettings
/// - \ref background_music_and_dvr
///
typedef AKRESULT ( * AkBackgroundMusicChangeCallbackFunc )(
	bool in_bBackgroundMusicMuted,	///< Flag indicating whether the busses tagged as "background music" in the project are muted or not.
	void* in_pCookie ///< User-provided data, e.g. a user structure.
	);

/// Define the orientation of the the floor plane with respect to the X,Y,Z axes, and which axes represent the side, front and up vectors as a basis for rotations in Wwise.
/// AkFloorPlane is used in to orient the Game Object 3D Viewer in Wwise, and in the transformation of geometry instances in Wwise Spatial Audio.
enum AkFloorPlane
{
	AkFloorPlane_XZ = 0,					///< The floor is oriented along the ZX-plane. The front vector points towards +Z, the up vector towards +Y, and the side vector towards +X.
	AkFloorPlane_XY,						///< The floor is oriented along the XY-plane. The front vector points towards +X, the up vector towards +Z, and the side vector towards +Y.
	AkFloorPlane_YZ,						///< The floor is oriented along the YZ-plane. The front vector points towards +Y, the up vector towards +X, and the side vector towards +Z.
	AkFloorPlane_Last,						///< End of enum, invalid value.
	AkFloorPlane_Default = AkFloorPlane_XZ	///< The Wwise default floor plane is ZX.
};

// Function that the host runtime must call to allow for jobs to execute.
// in_jobType is the type originally provided by AkJobMgrSettings::FuncRequestJobWorker.
// in_uExecutionTimeUsec is the number of microseconds that the function should execute for before terminating.
// Note that the deadline is only checked after each individual job completes execution, so the function may run slightly
// longer than intended. The "in_uExecutionTimeUsec" should be considered a suggestion or guideline, not a strict rule.
// A value of 0 means that the function will run until there are no more jobs ready to be immediately executed.
AK_CALLBACK(void, AkJobWorkerFunc)(
	AkJobType in_jobType,
	AkUInt32 in_uExecutionTimeUsec
	);

/// Settings for the Sound Engine's internal job manager
struct AkJobMgrSettings
{
	/// Callback function prototype definition used for handling requests from JobMgr for new workers to perform work.
	AK_CALLBACK(void, FuncRequestJobWorker)(
		AkJobWorkerFunc in_fnJobWorker, ///< Function passed to host runtime that should be executed. Note that the function provided will exist for as long as the soundengine code is loaded, and will always be the same.
		AkJobType in_jobType,           ///< The type of job worker that has been requested. This should be passed forward to in_fnJobWorker
		AkUInt32 in_uNumWorkers,        ///< Number of workers requested
		void * in_pClientData           ///< Data provided by client in AkJobMgrSettings
		);

	FuncRequestJobWorker fnRequestJobWorker; ///< Function called by the job manager when a new worker needs to be requested. When null, all jobs will be executed on the same thread that calls RenderAudio().

	AkUInt32 uMaxActiveWorkers[AK_NUM_JOB_TYPES]; ///< The maximum number of concurrent workers that will be requested. Must be >= 1 for each jobType.

	AkUInt32 uNumMemorySlabs; ///< Number of memory slabs to pre-allocate for job manager memory. At least one slab per worker thread should be pre-allocated. Default is 1.
	AkUInt32 uMemorySlabSize; ///< Size of each memory slab used for job manager memory. Must be a power of two. Default is 8K.

	void* pClientData; ///< Arbitrary data that will be passed back to the client when calling FuncRequestJobWorker
};

/// External (optional) callback for tracking performance of the sound engine that is called when a timer starts. (only called in Debug and Profile binaries; this is not called in Release)
/// in_uPluginID may be non-zero when this function is called, to provide extra data about what context this Timer was started in.
/// in_pszZoneName will point to a static string, so the pointer can be stored for later use, not just the contents of the string itself.
AK_CALLBACK(void, AkProfilerPushTimerFunc)(
	AkPluginID in_uPluginID,
	const char* in_pszZoneName
	);

/// External (optional) function for tracking performance of the sound engine that is called when a timer stops. (only called in Debug and Profile binaries; this is not called in Release)
AK_CALLBACK(void, AkProfilerPopTimerFunc)();
	
///< External (optional) function for tracking notable events in the sound engine, to act as a marker or bookmark. (only called in Debug and Profile binaries; this is not called in Release)
/// in_uPluginID may be non-zero when this function is called, to provide extra data about what context this Marker was posted in.
/// in_pszMarkerName will point to a static string, so the pointer can be stored for later use, not just the contents of the string itself.
AK_CALLBACK(void, AkProfilerPostMarkerFunc)(
	AkPluginID in_uPluginID,
	const char* in_pszMarkerName
	);

/// Platform-independent initialization settings of the sound engine
/// \sa 
/// - <tt>AK::SoundEngine::Init()</tt>
/// - <tt>AK::SoundEngine::GetDefaultInitSettings()</tt>
/// - \ref soundengine_integration_init_advanced
struct AkInitSettings
{
    AkAssertHook        pfnAssertHook;				///< External assertion handling function (optional)

    AkUInt32            uMaxNumPaths;				///< Maximum number of paths for positioning
	AkUInt32            uCommandQueueSize;			///< Size of the command queue, in bytes
	bool				bEnableGameSyncPreparation;	///< Sets to true to enable AK::SoundEngine::PrepareGameSync usage.
	AkUInt32			uContinuousPlaybackLookAhead;	///< Number of quanta ahead when continuous containers should instantiate a new voice before which next sounds should start playing. This look-ahead time allows I/O to occur, and is especially useful to reduce the latency of continuous containers with trigger rate or sample-accurate transitions. 
													///< Default is 1 audio quantum, also known as an audio frame. Its size is equal to AkInitSettings::uNumSamplesPerFrame / AkPlatformInitSettings::uSampleRate. For many platforms the default values - which can be overridden - are respectively 1,024 samples and 48 kHz. This gives a default 21.3 ms for an audio quantum, which is adequate if you have a RAM-based streaming device that completes transfers within 20 ms. With 1 look-ahead quantum, voices spawned by continuous containers are more likely to be ready when they are required to play, thereby improving the overall precision of sound scheduling. If your device completes transfers in 30 ms instead, you might consider increasing this value to 2 because it will grant new voices 2 audio quanta (~43 ms) to fetch data. 
	AkReal32			fStreamingLookAheadRatio;	///< Multiplication factor for all streaming look-ahead heuristic values, for music streams.

	AkUInt32			uNumSamplesPerFrame;		///< Number of samples per audio frame (256, 512, 1024, or 2048).

    AkUInt32            uMonitorQueuePoolSize;		///< Size of the monitoring queue, in bytes. This parameter is not used in Release build.
	AkUInt32            uCpuMonitorQueueMaxSize;	///< Maximum size of the CPU monitoring queue, per thread, in bytes. This parameter is not used in Release build.
	
	AkOutputSettings	settingsMainOutput;			///< Main output device settings.
	AkJobMgrSettings    settingsJobManager;         ///< Settings to configure the behavior of the Sound Engine's internal job manager

	AkUInt32			uMaxHardwareTimeoutMs;		///< Amount of time to wait for HW devices to trigger an audio interrupt. If there is no interrupt after that time, the sound engine will revert to  silent mode and continue operating until the HW finally comes back. Default value: 2000 (2 seconds)

	bool                bUseSoundBankMgrThread;     ///< Use a Wwise-owned thread for loading sound banks. If set to false, bank-loading will occur only inside a call to AK::SoundEngine::ProcessBanks(). \sa \ref goingfurther_bankmgrthread
	bool                bUseLEngineThread;          ///< Use a Wwise-owned thread for processing audio. If set to false, audio processing will occur only inside a call to AK::SoundEngine::RenderAudio(). \sa \ref goingfurther_eventmgrthread

	AkBackgroundMusicChangeCallbackFunc BGMCallback; ///< Application-defined audio source change event callback function.
	void*				BGMCallbackCookie;			///< Application-defined user data for the audio source change event callback function.
	const AkOSChar *	szPluginDLLPath;			///< When using DLLs for plugins, specify their path. Leave NULL if DLLs are in the same folder as the game executable.

	AkFloorPlane		eFloorPlane;				///< Define the orientation of the the floor plane with respect to the X,Y,Z axes, and which axes represent the side, front and up vectors as a basis for rotations in Wwise.
													///< AkFloorPlane is used in to orient the Game Object 3D Viewer in Wwise, and in the transformation of geometry instances in Wwise Spatial Audio.

	AkReal32			fGameUnitsToMeters;			///< The number of game units in a meter.
													///< This setting is used to adapt the size of elements in the Authoring's Game Object 3D Viewer and Audio Object 3D Viewer to meters.
													///< This setting is also used to simulate real-world positioning of System Audio Objects, to improve the HRTF in some cases.
													///< Note that for legacy reasons, this setting's name is misleading. Its value should represent the number of game units *per* meter.
													///< For example, if your game engine sends positions in centimeters, the value of fGameUnitsToMeters should be 100.

	AkUInt32			uBankReadBufferSize;		///< The number of bytes read by the BankReader when new data needs to be loaded from disk during serialization. Increasing this trades memory usage for larger, but fewer, file-read events during bank loading.

	AkReal32			fDebugOutOfRangeLimit;		///< Debug setting: Only used when bDebugOutOfRangeCheckEnabled is true.  This defines the maximum values samples can have.  Normal audio must be contained within +1/-1.  This limit should be set higher to allow temporary or short excursions out of range.  Default is 16.

	bool				bDebugOutOfRangeCheckEnabled;	///< Debug setting: Enable checks for out-of-range (and NAN) floats in the processing code.  This incurs a small performance hit, but can be enabled in most scenarios.  Will print error messages in the log if invalid values are found at various point in the pipeline. Contact AK Support with the new error messages for more information.

	bool				bOfflineRendering;			///< Enables/disables offline rendering. \ref goingfurther_offlinerendering

	AkProfilerPushTimerFunc fnProfilerPushTimer;	///< External (optional) function for tracking performance of the sound engine that is called when a timer starts. (only called in Debug and Profile binaries; this is not called in Release)
	AkProfilerPopTimerFunc fnProfilerPopTimer;		///< External (optional) function for tracking performance of the sound engine that is called when a timer stops. (only called in Debug and Profile binaries; this is not called in Release)
	AkProfilerPostMarkerFunc fnProfilerPostMarker;	///< External (optional) function for tracking significant events in the sound engine, to act as a marker or bookmark. (only called in Debug and Profile binaries; this is not called in Release)
};

/// Necessary settings for setting externally-loaded sources
struct AkSourceSettings
{
	AkUniqueID	sourceID;							///< Source ID (available in the SoundBank content files)
	AkUInt8*	pMediaMemory;						///< Pointer to the data to be set for the source
	AkUInt32	uMediaSize;							///< Size, in bytes, of the data to be set for the source
};

/// Return values for GetSourcePlayPositions.
struct AkSourcePosition
{
	AkUniqueID	audioNodeID;						///< Audio Node ID of playing item
	AkUniqueID	mediaID;							///< Media ID of playing item. (corresponds to 'ID' attribute of 'File' element in SoundBank metadata file)
	AkTimeMs	msTime;								///< Position of the source (in ms) associated with that playing item
	AkUInt32	samplePosition;						///< Position of the source (in samples) associated with that playing item
	AkUInt32	updateBufferTick;					///< Value of GetBufferTick() at the time the position was updated
};

/// Audiokinetic namespace
namespace AK
{
	class IReadBytes;
	class IWriteBytes;

	/// Audiokinetic sound engine namespace
	/// \remarks The functions in this namespace are thread-safe, unless stated otherwise.
	namespace SoundEngine
	{
        ///////////////////////////////////////////////////////////////////////
		/// @name Initialization
		//@{

		/// Query whether or not the sound engine has been successfully initialized.
		/// \warning This function is not thread-safe. It should not be called at the same time as \c SoundEngine::Init() or \c SoundEngine::Term().
		/// \return \c True if the sound engine has been initialized, \c False otherwise.
		/// \sa
		/// - \ref soundengine_integration_init_advanced
		/// - <tt>AK::SoundEngine::Init()</tt>
		/// - <tt>AK::SoundEngine::Term()</tt>
		extern   bool __cdecl  IsInitialized  ();

		/// Initialize the sound engine.
		/// \warning This function is not thread-safe.
		/// \remark The initial settings should be initialized using <tt>AK::SoundEngine::GetDefaultInitSettings()</tt>
		///			and <tt>AK::SoundEngine::GetDefaultPlatformInitSettings()</tt> to fill the structures with their 
		///			default settings. This is not mandatory, but it helps avoid backward compatibility problems.
		///		
		/// \return 
		/// - \c AK_Success if the initialization was successful
		/// - \c AK_MemManagerNotInitialized if the memory manager is not available or not properly initialized
		/// - \c AK_StreamMgrNotInitialized if the stream manager is not available or not properly initialized
		/// - \c AK_SSEInstructionsNotSupported if the machine does not support SSE instruction (only on the PC)
		/// - \c AK_InsufficientMemory if there is not enough memory available to initialize the sound engine properly
		/// - \c AK_InvalidParameter if some parameters are invalid
		/// - \c AK_AlreadyInitialized if the sound engine is already initialized, or if the provided settings result in insufficient 
		/// - \c AK_Fail for unknown errors, check with AK Support.
		/// resources for the initialization.
		/// \sa
		/// - \ref soundengine_integration_init_advanced
		/// - \ref workingwithsdks_initialization
		/// - <tt>AK::SoundEngine::Term()</tt>
		/// - <tt>AK::SoundEngine::GetDefaultInitSettings()</tt>
		/// - <tt>AK::SoundEngine::GetDefaultPlatformInitSettings()</tt>
        extern   AKRESULT __cdecl  Init  (
            AkInitSettings *			in_pSettings,   		///< Initialization settings (can be NULL, to use the default values)
            AkPlatformInitSettings *	in_pPlatformSettings  	///< Platform-specific settings (can be NULL, to use the default values)
		    );

		/// Gets the default values of the platform-independent initialization settings.
		/// \warning This function is not thread-safe.
		/// \sa
		/// - \ref soundengine_integration_init_advanced
		/// - <tt>AK::SoundEngine::Init()</tt>
		/// - <tt>AK::SoundEngine::GetDefaultPlatformInitSettings()</tt>
		extern   void __cdecl  GetDefaultInitSettings  (
            AkInitSettings &			out_settings   			///< Returned default platform-independent sound engine settings
		    );

		/// Gets the default values of the platform-specific initialization settings.
		///
		/// \warning This function is not thread-safe.
		/// \sa 
		/// - \ref soundengine_integration_init_advanced
		/// - <tt>AK::SoundEngine::Init()</tt>
		/// - <tt>AK::SoundEngine::GetDefaultInitSettings()</tt>
		extern   void __cdecl  GetDefaultPlatformInitSettings  (
            AkPlatformInitSettings &	out_platformSettings  	///< Returned default platform-specific sound engine settings
		    );
		
        /// Terminates the sound engine.
		/// If some sounds are still playing or events are still being processed when this function is 
		///	called, they will be stopped.
		/// \warning This function is not thread-safe.
		/// \warning Before calling Term, you must ensure that no other thread is accessing the sound engine.
		/// \sa 
		/// - \ref soundengine_integration_init_advanced
		/// - <tt>AK::SoundEngine::Init()</tt>
        extern   void __cdecl  Term  ();

		/// Gets the configured audio settings.
		/// Call this function to get the configured audio settings.
		/// 
		/// \warning This function is not thread-safe.
		/// \warning Call this function only after the sound engine has been properly initialized.
		/// \return
		/// - \c AK_NotInitialized if <tt>AK::SoundEngine::Init()</tt> was not called
		/// - \c AK_Success otherwise.		
		extern   AKRESULT __cdecl  GetAudioSettings  (
			AkAudioSettings &	out_audioSettings  	///< Returned audio settings
			);

		/// Gets the output speaker configuration of the specified output.
		/// Call this function to get the speaker configuration of the output (which may not correspond
		/// to the physical output format of the platform, in the case of downmixing provided by the platform itself). 
		/// You may initialize the sound engine with a user-specified configuration, but the resulting 
		/// configuration is determined by the sound engine, based on the platform, output type and
		/// platform settings (for e.g. system menu or control panel option).
		/// If the speaker configuration of the output is object-based, the speaker configuration of the
		/// main mix is returned. To query more information on object-based output devices, see AK::SoundEngine::GetOutputDeviceConfiguration.
		///
		/// It is recommended to call GetSpeakerConfiguration anytime after receiving a callback from RegisterAudioDeviceStatusCallback to know if the channel configuration has changed.
		/// 
		/// \warning Call this function only after the sound engine has been properly initialized. 
		/// If you are initializing the sound engine with AkInitSettings::bUseLEngineThread to false, it is required to call RenderAudio() at least once before calling this function to complete the sound engine initialization. 
		/// The Init.bnk must be loaded prior to this call.
		/// \return The output configuration. An empty AkChannelConfig not AkChannelConfig::IsValid() if device does not exist or if the Init.bnk was not loaded yet.		
		/// \sa 
		/// - AkSpeakerConfig.h
		/// - AkOutputSettings
		/// - <tt>AK::SoundEngine::GetOutputDeviceConfiguration()</tt>
		extern   AkChannelConfig __cdecl  GetSpeakerConfiguration  (
			AkOutputDeviceID	in_idOutput = 0				///< Output ID to set the bus on.  As returned from AddOutput or GetOutputID.  You can pass 0 for the main (default) output	
			);

		/// Gets the configuration of the specified output device.
		/// Call this function to get the channel configuration of the output device as well as its 3D audio capabilities.
		/// If the configuration of the output device is object-based (io_channelConfig.eConfigType == AK_ChannelConfigType_Objects),
		/// io_capabilities can be inspected to determine the channel configuration of the main mix (Ak3DAudioSinkCapabilities::channelConfig),
		/// whether or not the output device uses a passthrough mix (Ak3DAudioSinkCapabilities::bPassthrough) and the maximum number of objects
		/// that can play simultaneously on this output device (Ak3DAudioSinkCapabilities::uMax3DAudioObjects). Note that if
		/// Ak3DAudioSinkCapabilities::bMultiChannelObjects is false, multi-channel objects will be split into multiple mono objects
		/// before being sent to the output device.
		///
		/// \warning Call this function only after the sound engine has been properly initialized. If you are initializing the sound engine with AkInitSettings::bUseLEngineThread to false, it is required to call RenderAudio() at least once before calling this function to complete the sound engine initialization.
		/// \return
		/// - \c AK_Success if successful
		/// - \c AK_IDNotFound is the output was not found in the system.
		/// - \c AK_NotInitialized if the sound engine is not initialized
		/// \sa
		/// - AkSpeakerConfig.h
		/// - AkOutputSettings
		/// - <tt>AK::SoundEngine::GetSpeakerConfiguration()</tt>
		extern  AKRESULT __cdecl  GetOutputDeviceConfiguration (
			AkOutputDeviceID in_idOutput,
			AkChannelConfig& io_channelConfig,
			Ak3DAudioSinkCapabilities& io_capabilities
			);

		/// Gets the panning rule of the specified output.
		/// \warning Call this function only after the sound engine has been properly initialized.
		/// Returns the supported configuration in out_ePanningRule: 
		/// - AkPanningRule_Speakers
		/// - AkPanningRule_Headphone
		/// \return
		/// - \c AK_Success if successful
		/// - \c AK_IDNotFound is the output was not found in the system.
		/// - \c AK_NotInitialized if the sound engine is not initialized
		/// \sa 
		/// - AkSpeakerConfig.h
		extern   AKRESULT __cdecl  GetPanningRule  (
			AkPanningRule &		out_ePanningRule,			///< Returned panning rule (AkPanningRule_Speakers or AkPanningRule_Headphone) for given output.
			AkOutputDeviceID	in_idOutput = 0				///< Output ID to set the bus on.  As returned from AddOutput or GetOutputID.  You can pass 0 for the main (default) output	
			);

		/// Sets the panning rule of the specified output.
		/// This may be changed anytime once the sound engine is initialized.
		/// \warning This function posts a message through the sound engine's internal message queue, whereas GetPanningRule() queries the current panning rule directly.
		/// \aknote 
		///	The specified panning rule will only impact the sound if the processing format is downmixing to Stereo in the mixing process. It
		/// will not impact the output if the audio stays in 5.1 until the end, for example.
		/// \endaknote
		extern   AKRESULT __cdecl  SetPanningRule  ( 
			AkPanningRule		in_ePanningRule,			///< Panning rule.
			AkOutputDeviceID	in_idOutput = 0				///< Output ID to set the bus on.  As returned from AddOutput or GetOutputID.  You can pass 0 for the main (default) output	
			);

		/// Gets speaker angles of the specified device. Speaker angles are used for 3D positioning of sounds over standard configurations.
		/// Note that the current version of Wwise only supports positioning on the plane.
		/// The speaker angles are expressed as an array of loudspeaker pairs, in degrees, relative to azimuth ]0,180].
		/// Supported loudspeaker setups are always symmetric; the center speaker is always in the middle and thus not specified by angles.
		/// Angles must be set in ascending order. 
		/// You may call this function with io_pfSpeakerAngles set to NULL to get the expected number of angle values in io_uNumAngles, 
		/// in order to allocate your array correctly. You may also obtain this number by calling
		/// AK::GetNumberOfAnglesForConfig( AK_SPEAKER_SETUP_DEFAULT_PLANE ).
		/// If io_pfSpeakerAngles is not NULL, the array is filled with up to io_uNumAngles.
		/// Typical usage:
		/// - AkUInt32 uNumAngles;
		/// - GetSpeakerAngles( NULL, uNumAngles, AkOutput_Main );
		/// - AkReal32 * pfSpeakerAngles = AkAlloca( uNumAngles * sizeof(AkReal32) );
		/// - GetSpeakerAngles( pfSpeakerAngles, uNumAngles, AkOutput_Main );
		/// \aknote 
		///	On most platforms, the angle set on the plane consists of 3 angles, to account for 7.1. 
		/// - When panning to stereo (speaker mode, see <tt>AK::SoundEngine::SetPanningRule()</tt>), only angle[0] is used, and 3D sounds in the back of the listener are mirrored to the front. 
		/// - When panning to 5.1, the front speakers use angle[0], and the surround speakers use (angle[2] + angle[1]) / 2.
		/// \endaknote
		/// \warning Call this function only after the sound engine has been properly initialized.
		/// \return AK_Success if device exists.
		/// \sa SetSpeakerAngles()
		extern   AKRESULT __cdecl  GetSpeakerAngles  (
			AkReal32 *			io_pfSpeakerAngles,			///< Returned array of loudspeaker pair angles, in degrees relative to azimuth [0,180]. Pass NULL to get the required size of the array.
			AkUInt32 &			io_uNumAngles,				///< Returned number of angles in io_pfSpeakerAngles, which is the minimum between the value that you pass in, and the number of angles corresponding to AK::GetNumberOfAnglesForConfig( AK_SPEAKER_SETUP_DEFAULT_PLANE ), or just the latter if io_pfSpeakerAngles is NULL.
			AkReal32 &			out_fHeightAngle,			///< Elevation of the height layer, in degrees relative to the plane [-90,90].
			AkOutputDeviceID	in_idOutput = 0				///< Output ID to set the bus on.  As returned from AddOutput or GetOutputID.  You can pass 0 for the main (default) output			
			);
		
		/// Sets speaker angles of the specified device. Speaker angles are used for 3D positioning of sounds over standard configurations.
		/// Note that the current version of Wwise only supports positioning on the plane.
		/// The speaker angles are expressed as an array of loudspeaker pairs, in degrees, relative to azimuth ]0,180], for a 7.1 speaker configuration.
		/// Supported loudspeaker setups are always symmetric; the center speaker is always in the middle and thus not specified by angles.
		/// Angles must be set in ascending order. 
		/// Note:
		/// - This function requires the minimum speaker angle between any pair of speakers to be at least 5 degrees.
		/// - When setting angles for a 5.1 speaker layout, we recommend that you select an angle for the SL and SR channels, then subtract 15 degrees for in_pfSpeakerAngles[1] and add 15 degrees for in_pfSpeakerAngles[2] to set the arc appropriately.
		///
		/// Typical usage: 
		/// - Initialize the sound engine and/or add secondary output(s).
		/// - Get number of speaker angles and their value into an array using GetSpeakerAngles().
		/// - Modify the angles and call SetSpeakerAngles().
		/// This function posts a message to the audio thread through the command queue, so it is thread safe. However the result may not be immediately read with GetSpeakerAngles().
		/// \warning This function only applies to configurations (or subset of these configurations) that are standard and whose speakers are on the plane (2D).
		/// \return 
		/// - \c AK_Success if successful.
		/// - \c AK_InvalidFloatValue if the value specified was NaN or Inf
		/// - \c AK_InsufficientMemory if there wasn't enough memory in the message queue
		/// - \c AK_InvalidParameter one of the parameter is invalid, check the debug log. Speaker angles must be [0, 180] and height angle must be [-90, 90]. Height angle must not be 0, regardless if height channels are used.
		/// \sa GetSpeakerAngles()
		extern   AKRESULT __cdecl  SetSpeakerAngles  (
			const AkReal32 *	in_pfSpeakerAngles,			///< Array of loudspeaker pair angles, in degrees relative to azimuth ]0,180].
			AkUInt32			in_uNumAngles,				///< Number of elements in in_pfSpeakerAngles. It must correspond to AK::GetNumberOfAnglesForConfig( AK_SPEAKER_SETUP_DEFAULT_PLANE ) (the value returned by GetSpeakerAngles()).
			AkReal32 			in_fHeightAngle = AK_DEFAULT_HEIGHT_ANGLE,	///< Elevation of the height layer, in degrees relative to the plane [-90,90], but it cannot be 0.
			AkOutputDeviceID	in_idOutput = 0				///< Output ID to set the bus on.  As returned from AddOutput or GetOutputID.  You can pass 0 for the main (default) output
			);

		/// Allows the game to set the volume threshold to be used by the sound engine to determine if a voice must go virtual.
		/// This may be changed anytime once the sound engine was initialized.
		/// If this function is not called, the used value will be the value specified in the platform specific project settings.
		/// \return 
		/// - \c AK_Success if successful
		/// - \c AK_InvalidParameter if the threshold was not between 0 and -96.3 dB.
		/// - \c AK_InvalidFloatValue if the value specified was NaN or Inf
		extern   AKRESULT __cdecl  SetVolumeThreshold  ( 
			AkReal32 in_fVolumeThresholdDB ///< Volume Threshold, must be a value between 0 and -96.3 dB
			);

		/// Allows the game to set the maximum number of non virtual voices to be played simultaneously.
		/// This may be changed anytime once the sound engine was initialized.
		/// If this function is not called, the used value will be the value specified in the platform specific project settings.
		/// \return 
		/// - \c AK_InvalidParameter if the threshold was not between 1 and MaxUInt16.
		/// - \c AK_Success if successful
		extern   AKRESULT __cdecl  SetMaxNumVoicesLimit  ( 
			AkUInt16 in_maxNumberVoices ///< Maximum number of non-virtual voices.
			);
				
		/// Allows the game to set new values for the maximum active workers for the job manager
		/// This may be changed anytime once the sound engine was initialized.
		/// This function may only be used if a worker function has been set via \ref AkJobMgrSettings.
		/// \return 
		/// - \c AK_InvalidParameter in_jobType is not a valid jobtype, or in_uNewMaxActiveWorkers is not greater than 0
		/// - \c AK_NotInitialized JobMgr system was not already initialized with a function to request for workers
		/// - \c AK_Success if successful
		extern   AKRESULT __cdecl  SetJobMgrMaxActiveWorkers  ( 
			AkJobType in_jobType,            ///< JobType to set new uMaxActiveWorkers for
			AkUInt32 in_uNewMaxActiveWorkers ///< New maximum number of active workers will request
			);
				
        //@}

		////////////////////////////////////////////////////////////////////////
		/// @name Rendering Audio
		//@{

		/// Processes all commands in the sound engine's command queue.
		/// This method has to be called periodically (once per game frame).
		/// \sa 
		/// - \ref concept_events
		/// - \ref soundengine_events
		/// - <tt>AK::SoundEngine::PostEvent()</tt>
		/// \return Always returns AK_Success
        extern   AKRESULT __cdecl  RenderAudio  ( 
			bool in_bAllowSyncRender = true				///< When AkInitSettings::bUseLEngineThread is false, RenderAudio may generate an audio buffer -- unless in_bAllowSyncRender is set to false. Use in_bAllowSyncRender=false when calling RenderAudio from a Sound Engine callback.
			);

		//@}

		////////////////////////////////////////////////////////////////////////
		/// @name Component Registration
		//@{

		/// Query interface to global plug-in context used for plug-in registration/initialization.
		/// \return Global plug-in context.
		AK_EXTERNAPIFUNC(AK::IAkGlobalPluginContext *, GetGlobalPluginContext)();

		/// Registers a plug-in with the sound engine and sets the callback functions to create the 
		/// plug-in and its parameter node.  
		/// \aknote 
		///	This function is deprecated. Registration is now automatic if you link plug-ins statically. If plug-ins are dynamic libraries (such as DLLs or SOs), use \c RegisterPluginDLL.
		/// \endaknote
		/// \sa
		/// - \ref register_effects
		/// - \ref plugin_xml
		/// \return 
		/// - \c AK_Success if successful
		/// - \c AK_InvalidParameter if invalid parameters were provided
		/// - \c AK_InsufficientMemory if there isn't enough memory to register the plug-in
		/// \remarks
		/// Codecs and plug-ins must be registered before loading banks that use them.\n
		/// Loading a bank referencing an unregistered plug-in or codec will result in a load bank success,
		/// but the plug-ins will not be used. More specifically, playing a sound that uses an unregistered effect plug-in 
		/// will result in audio playback without applying the said effect. If an unregistered source plug-in is used by an event's audio objects, 
		/// posting the event will fail.
        extern   AKRESULT __cdecl  RegisterPlugin  ( 
			AkPluginType in_eType,								///< Plug-in type (for example, source or effect)
			AkUInt32 in_ulCompanyID,							///< Company identifier (as declared in the plug-in description XML file)
			AkUInt32 in_ulPluginID,								///< Plug-in identifier (as declared in the plug-in description XML file)
			AkCreatePluginCallback in_pCreateFunc,				///< Pointer to the plug-in's creation function
			AkCreateParamCallback in_pCreateParamFunc,			///< Pointer to the plug-in's parameter node creation function
			AkGetDeviceListCallback in_pGetDeviceList = NULL	///< Optional pointer to the plug-in's device enumeration function. Specify for a sink plug-in to support \ref AK::SoundEngine::GetDeviceList.
            );

		/// Loads a plug-in dynamic library and registers it with the sound engine.  
		/// With dynamic linking, all plugins are automatically registered.
		/// The plug-in DLL must be in the OS-specific library path or in the same location as the executable. If not, set AkInitSettings.szPluginDLLPath.
		/// \return 
		/// - \c AK_Success if successful.  
		/// - \c AK_FileNotFound if the DLL is not found in the OS path or if it has extraneous dependencies not found.  
		/// - \c AK_InsufficientMemory if the system ran out of resources while loading the dynamic library
		/// - \c AK_NotCompatible if the file was found but is not binary-compatible with the system's expected executable format
		/// - \c AK_InvalidFile if the symbol g_pAKPluginList is not exported by the dynamic library
		/// - \c AK_Fail if an unexpected system error was encountered
		extern   AKRESULT __cdecl  RegisterPluginDLL   (
			const AkOSChar* in_DllName,					///< Name of the DLL to load, without "lib" prefix or extension.  
			const AkOSChar* in_DllPath = NULL			///< Optional path to the DLL. Will override szPluginDLLPath that was set in AkInitSettings.
			);
		
		/// Query whether plug-in is registered with the sound engine.
		/// \return true when plug-in is registered, false otherwise.
		extern   bool __cdecl  IsPluginRegistered  (
			AkPluginType in_eType,								///< Plug-in type (for example, source or effect)
			AkUInt32 in_ulCompanyID,							///< Company identifier (as declared in the plug-in description XML file)
			AkUInt32 in_ulPluginID								///< Plug-in identifier (as declared in the plug-in description XML file)
			);

		/// Registers a codec type with the sound engine and set the callback functions to create the 
		/// codec's file source and bank source nodes.
		/// \aknote 
		///	This function is deprecated. Registration is now automatic if you link plugins statically. If plugins are dynamic libraries (such as DLLs or SOs), use RegisterPluginDLL.
		/// \endaknote		
		/// \sa 
		/// - \ref register_effects
		/// \return 
		/// - \c AK_Success if successful
		/// - \c AK_InvalidParameter if invalid parameters were provided
		/// - \c AK_InsufficientMemory if there isn't enough memory to register the plug-in
		/// \remarks
		/// Codecs and plug-ins must be registered before loading banks that use them.\n
		/// Loading a bank referencing an unregistered plug-in or codec will result in a load bank success,
		/// but the plug-ins will not be used. More specifically, playing a sound that uses an unregistered effect plug-in 
		/// will result in audio playback without applying the said effect. If an unregistered source plug-in is used by an event's audio objects, 
		/// posting the Event will fail.
        extern   AKRESULT __cdecl  RegisterCodec  ( 
			AkUInt32 in_ulCompanyID,						///< Company identifier (as declared in the plug-in description XML file)
			AkUInt32 in_ulCodecID,							///< Codec identifier (as declared in the plug-in description XML file)
			AkCreateFileSourceCallback in_pFileCreateFunc,	///< Pointer to the codec's file source node creation function
            AkCreateBankSourceCallback in_pBankCreateFunc	///< Pointer to the codec's bank source node creation function
            );

		/// Registers a global callback function. This function will be called from the audio rendering thread, at the
		/// location specified by in_eLocation. This function will also be called from the thread calling 			
		/// AK::SoundEngine::Term with in_eLocation set to AkGlobalCallbackLocation_Term.
		/// For example, in order to be called at every audio rendering pass, and once during teardown for releasing resources, you would call 
		/// RegisterGlobalCallback(myCallback, AkGlobalCallbackLocation_BeginRender  | AkGlobalCallbackLocation_Term, myCookie, AkPluginTypeNone, 0, 0);  
		/// \remarks
		/// A Plugin Type, Company ID and Plugin ID can be provided to this function to enable timing in the performance monitor.
		/// If the callback is being timed, it will contribute to the Total Plug-in CPU measurement, and also appear in the Plug-ins tab of the Advanced Profiler by plug-in type and ID. 
		/// It is illegal to call this function while already inside of a global callback.
		/// This function might stall for several milliseconds before returning.
		/// \return
		/// - \c AK_Success if successful
		/// - \c AK_InvalidParameter if parameters are out of range (check debug console or Wwise Profiler)
		/// \sa 
		/// - <tt>AK::SoundEngine::UnregisterGlobalCallback()</tt>
		/// - AkGlobalCallbackFunc
		/// - AkGlobalCallbackLocation
		extern  AKRESULT __cdecl  RegisterGlobalCallback (
			AkGlobalCallbackFunc in_pCallback,				///< Function to register as a global callback.
			AkUInt32 in_eLocation = AkGlobalCallbackLocation_BeginRender, ///< Callback location defined in AkGlobalCallbackLocation. Bitwise OR multiple locations if needed.
			void * in_pCookie = NULL,						///< User cookie.
			AkPluginType in_eType = AkPluginTypeNone,		///< Plug-in type (for example, source or effect). AkPluginTypeNone for no timing.
			AkUInt32 in_ulCompanyID = 0,					///< Company identifier (as declared in the plug-in description XML file). 0 for no timing. 
			AkUInt32 in_ulPluginID = 0						///< Plug-in identifier (as declared in the plug-in description XML file). 0 for no timing. 
			);

		/// Unregisters a global callback function, previously registered using RegisterGlobalCallback.
		/// \remarks
		/// It is legal to call this function while already inside of a global callback, If it is unregistering itself and not
		/// another callback.
		/// This function might stall for several milliseconds before returning.
		/// \return
		/// - \c AK_Success if successful
		/// - \c AK_InvalidParameter if parameters are out of range (check debug console or Wwise Profiler)
		/// \sa 
		/// - <tt>AK::SoundEngine::RegisterGlobalCallback()</tt>
		/// - AkGlobalCallbackFunc
		/// - AkGlobalCallbackLocation
		extern  AKRESULT __cdecl  UnregisterGlobalCallback (
			AkGlobalCallbackFunc in_pCallback,				///< Function to unregister as a global callback.
			AkUInt32 in_eLocation = AkGlobalCallbackLocation_BeginRender ///< Must match in_eLocation as passed to RegisterGlobalCallback for this callback.
			);

		/// Registers a resource monitor callback function that gets all of the resource usage data contained in the 
		/// AkResourceMonitorDataSummary structure. This includes general information about the system, such as CPU usage, 
		/// active Voices, and Events. This function will be called from the audio rendering thread at the end of each frame.
		/// \remarks
		/// If the callback is being timed, it will contribute to the Total Plug-in CPU measurement, and also appear in the Plug-ins tab of the Advanced Profiler by plug-in type and ID. 
		/// It is illegal to call this function while already inside of a resource callback.
		/// This function might stall for several milliseconds before returning.
		/// This function will return AK_Fail in Release
		/// \sa 
		/// - <tt>AK::SoundEngine::UnregisterResourceMonitorCallback()</tt>
		/// - AkResourceMonitorCallbackFunc
		extern  AKRESULT __cdecl  RegisterResourceMonitorCallback (
			AkResourceMonitorCallbackFunc in_pCallback				///< Function to register as a resource monitor callback.
			);

		/// Unregisters a resource monitor callback function, previously registered using RegisterResourceMonitorCallback.
		/// \remarks
		/// It is legal to call this function while already inside of a resource monitor callback, If it is unregistering itself and not
		/// another callback.
		/// This function might stall for several milliseconds before returning.
		/// \sa 
		/// - <tt>AK::SoundEngine::RegisterResourceMonitorCallback()</tt>
		/// - AkResourceMonitorCallbackFunc
		extern  AKRESULT __cdecl  UnregisterResourceMonitorCallback (
			AkResourceMonitorCallbackFunc in_pCallback				///< Function to unregister as a resource monitor callback.
			);

		/// Registers a callback for the Audio Device status changes.  
		/// The callback will be called from the audio thread
		/// Can be called prior to AK::SoundEngine::Init
		/// \sa AK::SoundEngine::AddOutput
		extern  AKRESULT __cdecl  RegisterAudioDeviceStatusCallback (
			AK::AkDeviceStatusCallbackFunc in_pCallback				///< Function to register as a status callback.						
			);

		/// Unregisters the callback for the Audio Device status changes, registered by RegisterAudioDeviceStatusCallback				
		extern  AKRESULT __cdecl  UnregisterAudioDeviceStatusCallback ();
		//@}

#ifdef AK_SUPPORT_WCHAR
		////////////////////////////////////////////////////////////////////////
		/// @name Getting ID from strings
		//@{

		/// Universal converter from Unicode string to ID for the sound engine.
		/// This function will hash the name based on a algorithm ( provided at : /AK/Tools/Common/AkFNVHash.h )
		/// Note:
		///		This function does return a AkUInt32, which is totally compatible with:
		///		AkUniqueID, AkStateGroupID, AkStateID, AkSwitchGroupID, AkSwitchStateID, AkRtpcID, and so on...
		/// \sa
		/// - <tt>AK::SoundEngine::PostEvent</tt>
		/// - <tt>AK::SoundEngine::SetRTPCValue</tt>
		/// - <tt>AK::SoundEngine::SetSwitch</tt>
		/// - <tt>AK::SoundEngine::SetState</tt>
		/// - <tt>AK::SoundEngine::PostTrigger</tt>
		/// - <tt>AK::SoundEngine::SetGameObjectAuxSendValues</tt>
		/// - <tt>AK::SoundEngine::LoadBank</tt>
		/// - <tt>AK::SoundEngine::UnloadBank</tt>
		/// - <tt>AK::SoundEngine::PrepareEvent</tt>
		/// - <tt>AK::SoundEngine::PrepareGameSyncs</tt>
		extern   AkUInt32 __cdecl  GetIDFromString  ( const wchar_t* in_pszString );
#endif //AK_SUPPORT_WCHAR

		/// Universal converter from string to ID for the sound engine.
		/// This function will hash the name based on a algorithm ( provided at : /AK/Tools/Common/AkFNVHash.h )
		/// Note:
		///		This function does return a AkUInt32, which is totally compatible with:
		///		AkUniqueID, AkStateGroupID, AkStateID, AkSwitchGroupID, AkSwitchStateID, AkRtpcID, and so on...
		/// \sa
		/// - <tt>AK::SoundEngine::PostEvent</tt>
		/// - <tt>AK::SoundEngine::SetRTPCValue</tt>
		/// - <tt>AK::SoundEngine::SetSwitch</tt>
		/// - <tt>AK::SoundEngine::SetState</tt>
		/// - <tt>AK::SoundEngine::PostTrigger</tt>
		/// - <tt>AK::SoundEngine::SetGameObjectAuxSendValues</tt>
		/// - <tt>AK::SoundEngine::LoadBank</tt>
		/// - <tt>AK::SoundEngine::UnloadBank</tt>
		/// - <tt>AK::SoundEngine::PrepareEvent</tt>
		/// - <tt>AK::SoundEngine::PrepareGameSyncs</tt>
		extern   AkUInt32 __cdecl  GetIDFromString  ( const char* in_pszString );

		//@}

		////////////////////////////////////////////////////////////////////////
		/// @name Event Management
		//@{

		/// Asynchronously posts an Event to the sound engine (by event ID).\n
		/// The Event must be loaded through a SoundBank before the call to PostEvent.
		/// The callback function can be used to be notified when markers are reached or when the event is finished (see \ref AkCallbackType).
		/// An array of wave file sources can be provided to resolve External Sources triggered by the event. 
        /// \return The playing ID of the event launched, or AK_INVALID_PLAYING_ID if posting the event failed and an error will be displayed in the debug console and the Wwise Profiler.
		/// \remarks
		/// If used, the array of external sources should contain the information for each external source triggered by the
		/// event. When triggering an event with multiple external sources, you need to differentiate each source 
		/// by using the cookie property in the External Source in the Wwise project and in AkExternalSourceInfo.
		/// \aknote If an event triggers the playback of more than one external source, they must be named uniquely in the project 
		/// (therefore have a unique cookie) in order to tell them apart when filling the AkExternalSourceInfo structures.
		/// \endaknote
		/// \aknote
		/// If Wwise Authoring is connected to the game and "Profile And Edit (Sync All)" is used, the required Event doesn't have to be loaded before this function is called.
		/// If the Event is missing, it will be requested from Wwise Authoring directly, which might cause additional latency.
		/// \endaknote
		/// \sa 
		/// - \ref concept_events
		/// - \ref integrating_external_sources
		/// - <tt>AK::SoundEngine::RenderAudio()</tt>
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
		/// - <tt>AK::SoundEngine::GetSourcePlayPosition()</tt>
        extern   AkPlayingID __cdecl  PostEvent  (
	        AkUniqueID in_eventID,							///< Unique ID of the event
	        AkGameObjectID in_gameObjectID,					///< Associated game object ID
			AkUInt32 in_uFlags = 0,							///< Bitmask: see \ref AkCallbackType
			AkCallbackFunc in_pfnCallback = NULL,			///< Callback function
			void * in_pCookie = NULL,						///< Callback cookie that will be sent to the callback function along with additional information
			AkUInt32 in_cExternals = 0,						///< Optional count of external source structures
			AkExternalSourceInfo *in_pExternalSources = NULL,///< Optional array of external source resolution information
			AkPlayingID	in_PlayingID = AK_INVALID_PLAYING_ID///< Optional (advanced users only) Specify the playing ID to target with the event. Will Cause active actions in this event to target an existing Playing ID. Let it be AK_INVALID_PLAYING_ID or do not specify any for normal playback.
	        );

#ifdef AK_SUPPORT_WCHAR
		/// Posts an Event to the sound engine (by Event name)\n
		/// The Event must be loaded through a SoundBank before the call to PostEvent.
		/// The callback function can be used to be notified when markers are reached or when the event is finished (see \ref AkCallbackType).
		/// An array of wave file sources can be provided to resolve External Sources triggered by the event. 
		/// \return The playing ID of the event launched, or AK_INVALID_PLAYING_ID if posting the event failed and an error will be displayed in the debug console and the Wwise Profiler.
		/// \remarks
		/// If used, the array of external sources should contain the information for each external source triggered by the
		/// event. When triggering an event with multiple external sources, you need to differentiate each source 
		/// by using the cookie property in the External Source in the Wwise project and in AkExternalSourceInfo.
		/// \aknote If an event triggers the playback of more than one external source, they must be named uniquely in the project 
		/// (therefore have a unique cookie) in order to tell them apart when filling the AkExternalSourceInfo structures.
		/// \endaknote
		/// \aknote
		/// If Wwise Authoring is connected to the game and "Profile And Edit (Sync All)" is used, the required Event doesn't have to be loaded before this function is called.
		/// If the Event is missing, it will be requested from Wwise Authoring directly, which might cause additional latency.
		/// \endaknote
		/// \sa 
		/// - \ref concept_events
		/// - \ref integrating_external_sources
		/// - <tt>AK::SoundEngine::RenderAudio()</tt>
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
		/// - <tt>AK::SoundEngine::GetSourcePlayPosition()</tt>
        extern   AkPlayingID __cdecl  PostEvent  (
	        const wchar_t* in_pszEventName,					///< Name of the event
	        AkGameObjectID in_gameObjectID,					///< Associated game object ID
			AkUInt32 in_uFlags = 0,							///< Bitmask: see \ref AkCallbackType
			AkCallbackFunc in_pfnCallback = NULL,			///< Callback function
			void * in_pCookie = NULL,						///< Callback cookie that will be sent to the callback function along with additional information.
			AkUInt32 in_cExternals = 0,						///< Optional count of external source structures
			AkExternalSourceInfo *in_pExternalSources = NULL,///< Optional array of external source resolution information
			AkPlayingID	in_PlayingID = AK_INVALID_PLAYING_ID///< Optional (advanced users only) Specify the playing ID to target with the event. Will Cause active actions in this event to target an existing Playing ID. Let it be AK_INVALID_PLAYING_ID or do not specify any for normal playback.
	        );
#endif //AK_SUPPORT_WCHAR

		/// Posts an Event to the sound engine (by Event name)\n
		/// The Event must be loaded through a SoundBank before the call to PostEvent.
		/// The callback function can be used to be notified when markers are reached or when the event is finished (see \ref AkCallbackType).
		/// An array of wave file sources can be provided to resolve External Sources triggered by the event. 
		/// \return The playing ID of the event launched, or AK_INVALID_PLAYING_ID if posting the event failed and an error will be displayed in the debug console and the Wwise Profiler.
		/// \remarks
		/// If used, the array of external sources should contain the information for each external source triggered by the
		/// event. When triggering an event with multiple external sources, you need to differentiate each source 
		/// by using the cookie property in the External Source in the Wwise project and in AkExternalSourceInfo.
		/// \aknote If an event triggers the playback of more than one external source, they must be named uniquely in the project 
		/// (therefore have a unique cookie) in order to tell them apart when filling the AkExternalSourceInfo structures.
		/// \endaknote
		/// \aknote
		/// If Wwise Authoring is connected to the game and "Profile And Edit (Sync All)" is used, the required Event doesn't have to be loaded before this function is called.
		/// If the Event is missing, it will be requested from Wwise Authoring directly, which might cause additional latency.
		/// \endaknote
		/// \sa 
		/// - \ref concept_events
		/// - \ref integrating_external_sources
		/// - <tt>AK::SoundEngine::RenderAudio()</tt>
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
		/// - <tt>AK::SoundEngine::GetSourcePlayPosition()</tt>
        extern   AkPlayingID __cdecl  PostEvent  (
	        const char* in_pszEventName,					///< Name of the event
	        AkGameObjectID in_gameObjectID,					///< Associated game object ID
			AkUInt32 in_uFlags = 0,							///< Bitmask: see \ref AkCallbackType
			AkCallbackFunc in_pfnCallback = NULL,			///< Callback function
			void * in_pCookie = NULL,						///< Callback cookie that will be sent to the callback function along with additional information.
			AkUInt32 in_cExternals = 0,						///< Optional count of external source structures
			AkExternalSourceInfo *in_pExternalSources = NULL,///< Optional array of external source resolution information
			AkPlayingID	in_PlayingID = AK_INVALID_PLAYING_ID///< Optional (advanced users only) Specify the playing ID to target with the event. Will Cause active actions in this event to target an existing Playing ID. Let it be AK_INVALID_PLAYING_ID or do not specify any for normal playback.
	        );

		// \deprecated Use AkActionOnEventType in the global scope.
		using ::AkActionOnEventType;
		const ::AkActionOnEventType AkActionOnEventType_Stop = ::AkActionOnEventType_Stop;
		const ::AkActionOnEventType AkActionOnEventType_Pause = ::AkActionOnEventType_Pause;
		const ::AkActionOnEventType AkActionOnEventType_Resume = ::AkActionOnEventType_Resume;
		const ::AkActionOnEventType AkActionOnEventType_Break = ::AkActionOnEventType_Break;
		const ::AkActionOnEventType AkActionOnEventType_ReleaseEnvelope = ::AkActionOnEventType_ReleaseEnvelope;
		const ::AkActionOnEventType AkActionOnEventType_Last = ::AkActionOnEventType_Last;

		/// Executes an Action on all nodes that are referenced in the specified Event in an Action of type play.
		/// \return
		/// - \c AK_Success if the action was successfully queued.
		/// - \c AK_IDNotFound if the Event was not found (not loaded or there is a typo in the ID)
		/// \sa
		/// - <tt>AkActionOnEventType</tt>
		extern   AKRESULT __cdecl  ExecuteActionOnEvent  (
			AkUniqueID in_eventID,												///< Unique ID of the event
			AkActionOnEventType in_ActionType,									///< Action to execute on all the elements that were played using the specified event.
	        AkGameObjectID in_gameObjectID = AK_INVALID_GAME_OBJECT,			///< Associated game object ID
			AkTimeMs in_uTransitionDuration = 0,								///< Fade duration
			AkCurveInterpolation in_eFadeCurve = AkCurveInterpolation_Linear,	///< Curve type to be used for the transition
			AkPlayingID in_PlayingID = AK_INVALID_PLAYING_ID					///< Associated PlayingID
			);

#ifdef AK_SUPPORT_WCHAR
		/// Executes an Action on all nodes that are referenced in the specified Event in an Action of type play.
		/// \return
		/// - \c AK_Success if the action was successfully queued.
		/// - \c AK_IDNotFound if the Event was not found (not loaded or there is a typo in the ID)
		/// \sa
		/// - <tt>AkActionOnEventType</tt>
		extern   AKRESULT __cdecl  ExecuteActionOnEvent  (
			const wchar_t* in_pszEventName,										///< Name of the event
			AkActionOnEventType in_ActionType,									///< Action to execute on all the elements that were played using the specified event.
	        AkGameObjectID in_gameObjectID = AK_INVALID_GAME_OBJECT,			///< Associated game object ID
			AkTimeMs in_uTransitionDuration = 0,								///< Fade duration
			AkCurveInterpolation in_eFadeCurve = AkCurveInterpolation_Linear,	///< Curve type to be used for the transition
			AkPlayingID in_PlayingID = AK_INVALID_PLAYING_ID					///< Associated PlayingID
			);
#endif //AK_SUPPORT_WCHAR

		/// Executes an Action on all nodes that are referenced in the specified Event in an Action of type play.
		/// \return
		/// - \c AK_Success if the action was successfully queued.
		/// - \c AK_IDNotFound if the Event was not found (not loaded or there is a typo in the ID)
		/// \sa
		/// - <tt>AkActionOnEventType</tt>
		extern   AKRESULT __cdecl  ExecuteActionOnEvent  (
			const char* in_pszEventName,										///< Name of the event
			AkActionOnEventType in_ActionType,									///< Action to execute on all the elements that were played using the specified event.
	        AkGameObjectID in_gameObjectID = AK_INVALID_GAME_OBJECT,			///< Associated game object ID
			AkTimeMs in_uTransitionDuration = 0,								///< Fade duration
			AkCurveInterpolation in_eFadeCurve = AkCurveInterpolation_Linear,	///< Curve type to be used for the transition
			AkPlayingID in_PlayingID = AK_INVALID_PLAYING_ID					///< Associated PlayingID
			);


		/// Executes a number of MIDI Events on all nodes that are referenced in the specified Event in an Action of type Play.
		/// The time at which a MIDI Event is posted is determined by in_bAbsoluteOffsets. If false, each MIDI event will be
		/// posted in AkMIDIPost::uOffset samples from the start of the current frame. If true, each MIDI event will be posted
		/// at the absolute time AkMIDIPost::uOffset samples.
		/// To obtain the current absolute time, see AK::SoundEngine::GetSampleTick.
		/// The duration of a sample can be determined from the sound engine's audio settings, via a call to AK::SoundEngine::GetAudioSettings.
		/// If a playing ID is specified then that playing ID must be active. Otherwise a new playing ID will be assigned.
		/// \return The playing ID of the event launched, or AK_INVALID_PLAYING_ID if posting the event failed and an error will be displayed in the debug console and the Wwise Profiler.
		/// \sa
		/// - <tt>AK::SoundEngine::GetAudioSettings</tt>
		/// - <tt>AK::SoundEngine::GetSampleTick</tt>
		/// - <tt>AK::SoundEngine::StopMIDIOnEvent</tt>
		/// - \ref soundengine_midi_event_playing_id
		extern   AkPlayingID __cdecl  PostMIDIOnEvent  (
			AkUniqueID in_eventID,												///< Unique ID of the Event
	        AkGameObjectID in_gameObjectID,										///< Associated game object ID
			AkMIDIPost* in_pPosts,												///< MIDI Events to post
			AkUInt16 in_uNumPosts,												///< Number of MIDI Events to post
			bool in_bAbsoluteOffsets = false,									///< Set to true when AkMIDIPost::uOffset are absolute, false when relative to current frame
			AkUInt32 in_uFlags = 0,												///< Bitmask: see \ref AkCallbackType
			AkCallbackFunc in_pfnCallback = NULL,								///< Callback function
			void * in_pCookie = NULL,											///< Callback cookie that will be sent to the callback function along with additional information
			AkPlayingID in_playingID = AK_INVALID_PLAYING_ID					///< Target playing ID
			);

		/// Stops MIDI notes on all nodes that are referenced in the specified event in an action of type play,
		/// with the specified Game Object. Invalid parameters are interpreted as wildcards. For example, calling
		/// this function with in_eventID set to AK_INVALID_UNIQUE_ID will stop all MIDI notes for Game Object
		/// in_gameObjectID.
		/// \return 
		/// - \c AK_Success if the stop command was queued
		/// - \c AK_IDNotFound if the Event ID is unknown (not loaded or typo in the ID)
		/// \sa
		/// - <tt>AK::SoundEngine::PostMIDIOnEvent</tt>
		/// - \ref soundengine_midi_event_playing_id
		extern   AKRESULT __cdecl  StopMIDIOnEvent  (
			AkUniqueID in_eventID = AK_INVALID_UNIQUE_ID,						///< Unique ID of the Event
	        AkGameObjectID in_gameObjectID = AK_INVALID_GAME_OBJECT,			///< Associated game object ID
			AkPlayingID in_playingID = AK_INVALID_PLAYING_ID					///< Target playing ID
			);


		/// Starts streaming the first part of all streamed files referenced by an Event into a cache buffer. Caching streams are serviced when no other streams require the 
		/// available bandwidth. The files will remain cached until UnpinEventInStreamCache is called, or a higher priority pinned file needs the space and the limit set by 
		/// uMaxCachePinnedBytes is exceeded.  
		/// \remarks The amount of data from the start of the file that will be pinned to cache is determined by the prefetch size. The prefetch size is set via the authoring tool and stored in the sound banks.  
		/// \remarks It is possible to override the prefetch size stored in the sound bank via the low level IO. For more information see <tt>AK::StreamMgr::IAkLowLevelIOHook::BatchOpen()</tt> and AkFileSystemFlags.
		/// \remarks If this function is called additional times with the same event, then the priority of the caching streams are updated. Note however that priority is passed down to the stream manager 
		///	on a file-by-file basis, and if another event is pinned to cache that references the same file but with a different priority, then the first priority will be updated with the most recent value.
		/// \remarks If the event references files that are chosen based on a State Group (via a switch container), all files in all states will be cached. Those in the current active state
		/// will get cached with active priority, while all other files will get cached with inactive priority.
		/// \remarks in_uInactivePriority is only relevant for events that reference switch containers that are assigned to State Groups. This parameter is ignored for all other events, including events that only reference
		/// switch containers that are assigned to Switch Groups. Files that are chosen based on a Switch Group have a different switch value per game object, and are all effectively considered active by the pin-to-cache system.
		/// \return 
		/// - \c AK_Success if command was queued
		/// - \c AK_IDNotFound if the Event ID is unknown (not loaded or typo in the ID)
		/// \sa
		/// - <tt>AK::SoundEngine::GetBufferStatusForPinnedEvent</tt>
		/// - <tt>AK::SoundEngine::UnpinEventInStreamCache</tt>
		/// - <tt>AK::StreamMgr::IAkLowLevelIOHook::BatchOpen</tt>
		/// - AkFileSystemFlags
		extern   AKRESULT __cdecl  PinEventInStreamCache  (
			AkUniqueID in_eventID,											///< Unique ID of the event
			AkPriority in_uActivePriority,									///< Priority of active stream caching I/O
			AkPriority in_uInactivePriority 								///< Priority of inactive stream caching I/O
			);

#ifdef AK_SUPPORT_WCHAR
		/// Starts streaming the first part of all streamed files referenced by an event into a cache buffer. Caching streams are serviced when no other streams require the 
		/// available bandwidth. The files will remain cached until UnpinEventInStreamCache is called, or a higher priority pinned file needs the space and the limit set by 
		/// uMaxCachePinnedBytes is exceeded.  
		/// \remarks The amount of data from the start of the file that will be pinned to cache is determined by the prefetch size. The prefetch size is set via the authoring tool and stored in the sound banks.  
		/// \remarks It is possible to override the prefetch size stored in the sound bank via the low level IO. For more information see <tt>AK::StreamMgr::IAkLowLevelIOHook::BatchOpen()</tt> and AkFileSystemFlags.
		/// \remarks If this function is called additional times with the same event, then the priority of the caching streams are updated. Note however that priority is passed down to the stream manager 
		///	on a file-by-file basis, and if another event is pinned to cache that references the same file but with a different priority, then the first priority will be updated with the most recent value.
		/// \remarks If the event references files that are chosen based on a State Group (via a Switch Container), all files in all states will be cached. Those in the current active state
		/// will get cached with active priority, while all other files will get cached with inactive priority.
		/// \remarks in_uInactivePriority is only relevant for events that reference switch containers that are assigned to State Groups. This parameter is ignored for all other events, including events that only reference
		/// switch containers that are assigned to Switch Groups. Files that are chosen based on a Switch Group have a different switch value per game object, and are all effectively considered active by the pin-to-cache system.
		/// \return 
		/// - \c AK_Success if command was queued
		/// - \c AK_IDNotFound if the Event ID is unknown (not loaded or typo in the ID)
		/// \sa
		/// - <tt>AK::SoundEngine::GetBufferStatusForPinnedEvent</tt>
		/// - <tt>AK::SoundEngine::UnpinEventInStreamCache</tt>
		/// - <tt>AK::StreamMgr::IAkLowLevelIOHook::BatchOpen</tt>
		/// - AkFileSystemFlags
		extern   AKRESULT __cdecl  PinEventInStreamCache  (
			const wchar_t* in_pszEventName,									///< Name of the event
			AkPriority in_uActivePriority,									///< Priority of active stream caching I/O
			AkPriority in_uInactivePriority 								///< Priority of inactive stream caching I/O
			);
#endif //AK_SUPPORT_WCHAR

		/// Starts streaming the first part of all streamed files referenced by an event into a cache buffer. Caching streams are serviced when no other streams require the 
		/// available bandwidth. The files will remain cached until UnpinEventInStreamCache is called, or a higher priority pinned file needs the space and the limit set by 
		/// uMaxCachePinnedBytes is exceeded.  
		/// \remarks The amount of data from the start of the file that will be pinned to cache is determined by the prefetch size. The prefetch size is set via the authoring tool and stored in the sound banks.  
		/// \remarks It is possible to override the prefetch size stored in the sound bank via the low level IO. For more information see <tt>AK::StreamMgr::IAkLowLevelIOHook::BatchOpen()</tt> and AkFileSystemFlags.
		/// \remarks If this function is called additional times with the same event, then the priority of the caching streams are updated. Note however that priority is passed down to the stream manager 
		/// on a file-by-file basis, and if another event is pinned to cache that references the same file but with a different priority, then the first priority will be updated with the most recent value.
		/// \remarks If the event references files that are chosen based on a State Group (via a switch container), all files in all states will be cached. Those in the current active state
		/// will get cached with active priority, while all other files will get cached with inactive priority.
		/// \remarks in_uInactivePriority is only relevant for events that reference switch containers that are assigned to State Groups. This parameter is ignored for all other events, including events that only reference
		/// switch containers that are assigned to Switch Groups. Files that are chosen based on a Switch Group have a different switch value per game object, and are all effectively considered active by the pin-to-cache system.
		/// \return 
		/// - \c AK_Success if command was queued
		/// - \c AK_IDNotFound if the Event ID is unknown (not loaded or typo in the ID)
		/// \sa
		/// - <tt>AK::SoundEngine::GetBufferStatusForPinnedEvent</tt>
		/// - <tt>AK::SoundEngine::UnpinEventInStreamCache</tt>
		/// - <tt>AK::StreamMgr::IAkLowLevelIOHook::BatchOpen</tt>
		/// - AkFileSystemFlags
		extern   AKRESULT __cdecl  PinEventInStreamCache  (
			const char* in_pszEventName,									///< Name of the event
			AkPriority in_uActivePriority,									///< Priority of active stream caching I/O
			AkPriority in_uInactivePriority 								///< Priority of inactive stream caching I/O
			);

		/// Releases the set of files that were previously requested to be pinned into cache via <tt>AK::SoundEngine::PinEventInStreamCache()</tt>. The file may still remain in stream cache
		/// after <tt>AK::SoundEngine::UnpinEventInStreamCache()</tt> is called, until the memory is reused by the streaming memory manager in accordance with to its cache management algorithm.
		/// \return 
		/// - \c AK_Success if command was queued
		/// - \c AK_IDNotFound if the Event ID is unknown (not loaded or typo in the ID)
		/// \sa
		/// - <tt>AK::SoundEngine::PinEventInStreamCache</tt>
		/// - <tt>AK::SoundEngine::GetBufferStatusForPinnedEvent</tt>
		extern   AKRESULT __cdecl  UnpinEventInStreamCache  (
			AkUniqueID in_eventID											///< Unique ID of the event
			);	

#ifdef AK_SUPPORT_WCHAR
		/// Releases the set of files that were previously requested to be pinned into cache via <tt>AK::SoundEngine::PinEventInStreamCache()</tt>. The file may still remain in stream cache
		/// after <tt>AK::SoundEngine::UnpinEventInStreamCache()</tt> is called, until the memory is reused by the streaming memory manager in accordance with to its cache management algorithm.
		/// \return 
		/// - \c AK_Success if command was queued
		/// - \c AK_IDNotFound if the Event ID is unknown (not loaded or typo in the ID)
		/// \sa
		/// - <tt>AK::SoundEngine::PinEventInStreamCache</tt>
		/// - <tt>AK::SoundEngine::GetBufferStatusForPinnedEvent</tt>
		extern   AKRESULT __cdecl  UnpinEventInStreamCache  (
			const wchar_t* in_pszEventName									///< Name of the event
			);
#endif //AK_SUPPORT_WCHAR

		/// Releases the set of files that were previously requested to be pinned into cache via <tt>AK::SoundEngine::PinEventInStreamCache()</tt>. The file may still remain in stream cache
		/// after <tt>AK::SoundEngine::UnpinEventInStreamCache()</tt> is called, until the memory is reused by the streaming memory manager in accordance with to its cache management algorithm.
		/// \return 
		/// - \c AK_Success if command was queued
		/// - \c AK_IDNotFound if the Event ID is unknown (not loaded or typo in the ID)
		/// \sa
		/// - <tt>AK::SoundEngine::PinEventInStreamCache</tt>
		/// - <tt>AK::SoundEngine::GetBufferStatusForPinnedEvent</tt>
		extern   AKRESULT __cdecl  UnpinEventInStreamCache  (
			const char* in_pszEventName										///< Name of the event
			);

		/// Returns information about an Event that was requested to be pinned into cache via <tt>AK::SoundEngine::PinEventInStreamCache()</tt>.
		/// Retrieves the smallest buffer fill-percentage for each file referenced by the event, and whether 
		/// the cache-pinned memory limit is preventing any of the files from filling up their buffer.
		/// \remarks To set the limit for the maximum number of bytes that can be pinned to cache, see \c AkDeviceSettings 
		/// \return 
		/// - \c AK_Success if command was queued
		/// - \c AK_IDNotFound if the Event ID is unknown (not loaded or typo in the ID)
		/// \sa
		/// - <tt>AK::SoundEngine::PinEventInStreamCache</tt>
		/// - <tt>AK::SoundEngine::UnpinEventInStreamCache</tt>
		/// - AkDeviceSettings
		extern   AKRESULT __cdecl  GetBufferStatusForPinnedEvent  (
			AkUniqueID in_eventID,											///< Unique ID of the event
			AkReal32& out_fPercentBuffered,									///< Fill-percentage (out of 100) of requested buffer size for least buffered file in the event.
			bool& out_bCachePinnedMemoryFull								///< True if at least one file can not complete buffering because the cache-pinned memory limit would be exceeded.
			);

		/// Returns information about an Event that was requested to be pinned into cache via <tt>AK::SoundEngine::PinEventInStreamCache()</tt>.
		/// Retrieves the smallest buffer fill-percentage for each file referenced by the event, and whether 
		/// the cache-pinned memory limit is preventing any of the files from filling up their buffer.
		/// \remarks To set the limit for the maximum number of bytes that can be pinned to cache, see AkDeviceSettings 
		/// \return 
		/// - \c AK_Success if command was queued
		/// - \c AK_IDNotFound if the Event ID is unknown (not loaded or typo in the ID)
		/// \sa
		/// - <tt>AK::SoundEngine::PinEventInStreamCache</tt>
		/// - <tt>AK::SoundEngine::UnpinEventInStreamCache</tt>
		/// - AkDeviceSettings
		extern   AKRESULT __cdecl  GetBufferStatusForPinnedEvent  (
			const char* in_pszEventName,									///< Name of the event
			AkReal32& out_fPercentBuffered,									///< Fill-percentage (out of 100) of requested buffer size for least buffered file in the event.
			bool& out_bCachePinnedMemoryFull								///< True if at least one file can not complete buffering because the cache-pinned memory limit would be exceeded.
			);

#ifdef AK_SUPPORT_WCHAR
		/// Returns information about an Event that was requested to be pinned into cache via <tt>AK::SoundEngine::PinEventInStreamCache()</tt>.
		/// Retrieves the smallest buffer fill-percentage for each file referenced by the event, and whether 
		/// the cache-pinned memory limit is preventing any of the files from filling up their buffer.
		/// \remarks To set the limit for the maximum number of bytes that can be pinned to cache, see AkDeviceSettings 
		/// \return 
		/// - \c AK_Success if command was queued
		/// - \c AK_IDNotFound if the Event ID is unknown (not loaded or typo in the ID)
		/// \sa
		/// - <tt>AK::SoundEngine::PinEventInStreamCache</tt>
		/// - <tt>AK::SoundEngine::UnpinEventInStreamCache</tt>
		/// - AkDeviceSettings
		extern   AKRESULT __cdecl  GetBufferStatusForPinnedEvent  (
			const wchar_t* in_pszEventName,									///< Name of the event
			AkReal32& out_fPercentBuffered,									///< Fill-percentage (out of 100) of requested buffer size for least buffered file in the event.
			bool& out_bCachePinnedMemoryFull								///< True if at least one file can not complete buffering because the cache-pinned memory limit would be exceeded.
			);
#endif

		/// Seeks inside all playing objects that are referenced in Play Actions of the specified Event.
		///
		/// Notes:
		///		- This works with all objects of the Containers hierarchy, including Music Segments and Music Switch Containers. 
		///		- There is a restriction with sounds that play within a continuous sequence. Seeking is ignored 
		///			if one of their ancestors is a continuous (random or sequence) container with crossfade or 
		///			trigger rate transitions. Seeking is also ignored with sample-accurate transitions, unless
		///			the sound that is currently playing is the first sound of the sequence.
		///		- Seeking is also ignored with voices that can go virtual with "From Beginning" behavior. 
		///		- Sounds/segments are stopped if in_iPosition is greater than their duration.
		///		- in_iPosition is clamped internally to the beginning of the sound/segment.
		///		- If the option "Seek to nearest marker" is used, the seeking position snaps to the nearest marker.
		///			With objects of the Containers hierarchy, markers are embedded in wave files by an external wave editor.
		///			Note that looping regions ("sampler loop") are not considered as markers. Also, the "add file name marker" of the 
		///			conversion settings dialog adds a marker at the beginning of the file, which is considered when seeking
		///			to nearest marker. In the case of interactive music objects (Music Segments, Music Switch Containers, and Music Playlist Containers), user (wave) markers are ignored:
		///			seeking occurs to the nearest segment cue (authored in the segment editor), including the Entry Cue, but excluding the Exit Cue.
		///		- This method posts a command in the sound engine queue, thus seeking will not occur before 
		///			the audio thread consumes it (after a call to RenderAudio()). 
		///
		///	Notes specific to Music Segments:
		///		- With Music Segments, in_iPosition is relative to the Entry Cue, in milliseconds. Use a negative
		///			value to seek within the Pre-Entry.
		///		- Music segments cannot be looped. You may want to listen to the AK_EndOfEvent notification 
		///			in order to restart them if required.
		///		- In order to restart at the correct location, with all their tracks synchronized, Music Segments 
		///			take the "look-ahead time" property of their streamed tracks into account for seeking. 
		///			Consequently, the resulting position after a call to SeekOnEvent() might be earlier than the 
		///			value that was passed to the method. Use <tt>AK::SoundEngine::GetPlayingSegmentInfo()</tt> to query 
		///			the exact position of a segment. Also, the segment will be silent during that time
		///			(so that it restarts precisely at the position that you specified). <tt>AK::SoundEngine::GetPlayingSegmentInfo()</tt> 
		///			also informs you about the remaining look-ahead time.  
		///
		/// Notes specific to Music Switch Containers:
		///		- Seeking triggers a music transition towards the current (or target) segment.
		///			This transition is subject to the container's transition rule that matches the current and defined in the container,
		///			so the moment when seeking occurs depends on the rule's "Exit At" property. On the other hand, the starting position 
		///			in the target segment depends on both the desired seeking position and the rule's "Sync To" property.
		///		- If the specified time is greater than the destination segment's length, the modulo is taken.
		///
		/// \return 
		/// - \c AK_Success if command was queued
		/// - \c AK_IDNotFound if the Event ID is unknown (not loaded or typo in the ID)
		/// \sa
		/// - <tt>AK::SoundEngine::RenderAudio()</tt>
		/// - <tt>AK::SoundEngine::PostEvent()</tt>
		/// - <tt>AK::SoundEngine::GetSourcePlayPosition()</tt>
		/// - <tt>AK::SoundEngine::GetPlayingSegmentInfo()</tt>
		extern   AKRESULT __cdecl  SeekOnEvent  ( 
			AkUniqueID in_eventID,										///< Unique ID of the event
			AkGameObjectID in_gameObjectID,								///< Associated game object ID; use AK_INVALID_GAME_OBJECT to affect all game objects
			AkTimeMs in_iPosition,										///< Desired position where playback should restart, in milliseconds
			bool in_bSeekToNearestMarker = false,						///< If true, the final seeking position will be made equal to the nearest marker (see note above)
			AkPlayingID in_PlayingID = AK_INVALID_PLAYING_ID			///< Specify the playing ID for the seek to be applied to. Will result in the seek happening only on active actions of the playing ID. Let it be AK_INVALID_PLAYING_ID or do not specify any, to seek on all active actions of this event ID.
			);

#ifdef AK_SUPPORT_WCHAR
		/// Seeks inside all playing objects that are referenced in Play Actions of the specified Event.
		///
		/// Notes:
		///		- This works with all objects of the Containers hierarchy, and also with Music Segments and Music Switch Containers. 
		///		- There is a restriction with sounds that play within a continuous sequence. Seeking is ignored 
		///			if one of their ancestors is a continuous (random or sequence) container with crossfade or 
		///			trigger rate transitions. Seeking is also ignored with sample-accurate transitions, unless
		///			the sound that is currently playing is the first sound of the sequence.
		///		- Seeking is also ignored with voices that can go virtual with "From Beginning" behavior. 
		///		- With Music Segments, in_iPosition is relative to the Entry Cue, in milliseconds. Use a negative
		///			value to seek within the Pre-Entry.
		///		- Sounds/segments are stopped if in_iPosition is greater than their duration.
		///		- in_iPosition is clamped internally to the beginning of the sound/segment.
		///		- If the option "Seek to nearest marker" is used, the seeking position snaps to the nearest marker.
		///			With objects of the Containers hierarchy, markers are embedded in wave files by an external wave editor.
		///			Note that looping regions ("sampler loop") are not considered as markers. Also, the "add file name marker" of the 
		///			conversion settings dialog adds a marker at the beginning of the file, which is considered when seeking
		///			to nearest marker. In the case of interactive music objects (Music Segments, Music Switch Containers, and Music Playlist Containers), user (wave) markers are ignored:
		///			seeking occurs to the nearest segment cue (authored in the segment editor), including the Entry Cue, but excluding the Exit Cue.
		///		- This method posts a command in the sound engine queue, thus seeking will not occur before 
		///			the audio thread consumes it (after a call to RenderAudio()). 
		///
		///	Notes specific to Music Segments:
		///		- With Music Segments, in_iPosition is relative to the Entry Cue, in milliseconds. Use a negative
		///			value to seek within the Pre-Entry.
		///		- Music segments cannot be looped. You may want to listen to the AK_EndOfEvent notification 
		///			in order to restart them if required.
		///		- In order to restart at the correct location, with all their tracks synchronized, Music Segments 
		///			take the "look-ahead time" property of their streamed tracks into account for seeking. 
		///			Consequently, the resulting position after a call to SeekOnEvent() might be earlier than the 
		///			value that was passed to the method. Use <tt>AK::SoundEngine::GetPlayingSegmentInfo()</tt> to query 
		///			the exact position of a segment. Also, the segment will be silent during that time
		///			(so that it restarts precisely at the position that you specified). <tt>AK::SoundEngine::GetPlayingSegmentInfo()</tt> 
		///			also informs you about the remaining look-ahead time. 
		///
		/// Notes specific to Music Switch Containers:
		///		- Seeking triggers a music transition towards the current (or target) segment.
		///			This transition is subject to the container's transition rule that matches the current and defined in the container,
		///			so the moment when seeking occurs depends on the rule's "Exit At" property. On the other hand, the starting position 
		///			in the target segment depends on both the desired seeking position and the rule's "Sync To" property.
		///		- If the specified time is greater than the destination segment's length, the modulo is taken.
		/// \return 
		/// - \c AK_Success if command was queued
		/// - \c AK_IDNotFound if the Event ID is unknown (not loaded or typo in the ID)
		/// \sa
		/// - <tt>AK::SoundEngine::RenderAudio()</tt>
		/// - <tt>AK::SoundEngine::PostEvent()</tt>
		/// - <tt>AK::SoundEngine::GetSourcePlayPosition()</tt>
		/// - <tt>AK::SoundEngine::GetPlayingSegmentInfo()</tt>
		extern   AKRESULT __cdecl  SeekOnEvent  ( 
			const wchar_t* in_pszEventName,								///< Name of the event
			AkGameObjectID in_gameObjectID,								///< Associated game object ID; use AK_INVALID_GAME_OBJECT to affect all game objects
			AkTimeMs in_iPosition,										///< Desired position where playback should restart, in milliseconds
			bool in_bSeekToNearestMarker = false,						///< If true, the final seeking position will be made equal to the nearest marker (see note above)
			AkPlayingID in_PlayingID = AK_INVALID_PLAYING_ID			///< Specify the playing ID for the seek to be applied to. Will result in the seek happening only on active actions of the playing ID. Let it be AK_INVALID_PLAYING_ID or do not specify any, to seek on all active actions of this event ID.	
			);
#endif //AK_SUPPORT_WCHAR

		/// Seeks inside all playing objects that are referenced in Play Actions of the specified Event.
		///
		/// Notes:
		///		- This works with all objects of the Containers hierarchy, and also with Music Segments and Music Switch Containers. 
		///		- There is a restriction with sounds that play within a continuous sequence. Seeking is ignored 
		///			if one of their ancestors is a continuous (random or sequence) container with crossfade or 
		///			trigger rate transitions. Seeking is also ignored with sample-accurate transitions, unless
		///			the sound that is currently playing is the first sound of the sequence.
		///		- Seeking is also ignored with voices that can go virtual with "From Beginning" behavior. 
		///		- With Music Segments, in_iPosition is relative to the Entry Cue, in milliseconds. Use a negative
		///			value to seek within the Pre-Entry.
		///		- Sounds/segments are stopped if in_iPosition is greater than their duration.
		///		- in_iPosition is clamped internally to the beginning of the sound/segment.
		///		- If the option "Seek to nearest marker" is used, the seeking position snaps to the nearest marker.
		///			With objects of the Containers hierarchy, markers are embedded in wave files by an external wave editor.
		///			Note that looping regions ("sampler loop") are not considered as markers. Also, the "add file name marker" of the 
		///			conversion settings dialog adds a marker at the beginning of the file, which is considered when seeking
		///			to nearest marker. In the case of interactive music objects (Music Segments, Music Switch Containers, and Music Playlist Containers), user (wave) markers are ignored:
		///			seeking occurs to the nearest segment cue (authored in the segment editor), including the Entry Cue, but excluding the Exit Cue.
		///		- This method posts a command in the sound engine queue, thus seeking will not occur before 
		///			the audio thread consumes it (after a call to RenderAudio()). 
		///
		///	Notes specific to Music Segments:
		///		- With Music Segments, in_iPosition is relative to the Entry Cue, in milliseconds. Use a negative
		///			value to seek within the Pre-Entry.
		///		- Music segments cannot be looped. You may want to listen to the AK_EndOfEvent notification 
		///			in order to restart them if required.
		///		- In order to restart at the correct location, with all their tracks synchronized, Music Segments 
		///			take the "look-ahead time" property of their streamed tracks into account for seeking. 
		///			Consequently, the resulting position after a call to SeekOnEvent() might be earlier than the 
		///			value that was passed to the method. Use <tt>AK::SoundEngine::GetPlayingSegmentInfo()</tt> to query 
		///			the exact position of a segment. Also, the segment will be silent during that time
		///			(so that it restarts precisely at the position that you specified). <tt>AK::SoundEngine::GetPlayingSegmentInfo()</tt> 
		///			also informs you about the remaining look-ahead time. 
		///
		/// Notes specific to Music Switch Containers:
		///		- Seeking triggers a music transition towards the current (or target) segment.
		///			This transition is subject to the container's transition rule that matches the current and defined in the container,
		///			so the moment when seeking occurs depends on the rule's "Exit At" property. On the other hand, the starting position 
		///			in the target segment depends on both the desired seeking position and the rule's "Sync To" property.
		///		- If the specified time is greater than the destination segment's length, the modulo is taken.
		///
		/// \return 
		/// - \c AK_Success if command was queued
		/// - \c AK_IDNotFound if the Event ID is unknown (not loaded or typo in the ID)
		/// \sa
		/// - <tt>AK::SoundEngine::RenderAudio()</tt>
		/// - <tt>AK::SoundEngine::PostEvent()</tt>
		/// - <tt>AK::SoundEngine::GetSourcePlayPosition()</tt>
		/// - <tt>AK::SoundEngine::GetPlayingSegmentInfo()</tt>
		extern   AKRESULT __cdecl  SeekOnEvent  ( 
			const char* in_pszEventName,								///< Name of the event
			AkGameObjectID in_gameObjectID,								///< Associated game object ID; use AK_INVALID_GAME_OBJECT to affect all game objects
			AkTimeMs in_iPosition,										///< Desired position where playback should restart, in milliseconds
			bool in_bSeekToNearestMarker = false,						///< If true, the final seeking position will be made equal to the nearest marker (see note above)	
			AkPlayingID in_PlayingID = AK_INVALID_PLAYING_ID			///< Specify the playing ID for the seek to be applied to. Will result in the seek happening only on active actions of the playing ID. Let it be AK_INVALID_PLAYING_ID or do not specify any, to seek on all active actions of this event ID.	
			);

		/// Seeks inside all playing objects that are referenced in Play Actions of the specified Event.
		/// Seek position is specified as a percentage of the sound's total duration, and takes looping into account. 
		///
		/// Notes:
		///		- This works with all objects of the Containers hierarchy, and also with Music Segments and Music Switch Containers. 
		///		- There is a restriction with sounds that play within a continuous sequence. Seeking is ignored 
		///			if one of their ancestors is a continuous (random or sequence) container with crossfade or 
		///			trigger rate transitions. Seeking is also ignored with sample-accurate transitions, unless
		///			the sound that is currently playing is the first sound of the sequence.
		///		- Seeking is also ignored with voices that can go virtual with "From Beginning" behavior. 
		///		- in_iPosition is clamped internally to the beginning of the sound/segment.
		///		- If the option "Seek to nearest marker" is used, the seeking position snaps to the nearest marker.
		///			With objects of the Containers hierarchy, markers are embedded in wave files by an external wave editor.
		///			Note that looping regions ("sampler loop") are not considered as markers. Also, the "add file name marker" of the 
		///			conversion settings dialog adds a marker at the beginning of the file, which is considered when seeking
		///			to nearest marker. In the case of interactive music objects (Music Segments, Music Switch Containers, and Music Playlist Containers), user (wave) markers are ignored:
		///			seeking occurs to the nearest segment cue (authored in the segment editor), including the Entry Cue, but excluding the Exit Cue.
		///		- This method posts a command in the sound engine queue, thus seeking will not occur before 
		///			the audio thread consumes it (after a call to RenderAudio()). 
		///
		///	Notes specific to Music Segments:
		///		- With Music Segments, \c in_fPercent is relative to the Entry Cue, and the segment's duration is the 
		///			duration between its entry and exit cues. Consequently, you cannot seek within the pre-entry or
		///			post-exit of a segment using this method. Use absolute values instead.
		///		- Music Segments cannot be looped. You may want to listen to the \c AK_EndOfEvent notification 
		///			in order to restart them if required.
		///		- In order to restart at the correct location, with all their tracks synchronized, Music Segments 
		///			take the "look-ahead time" property of their streamed tracks into account for seeking. 
		///			Consequently, the resulting position after a call to SeekOnEvent() might be earlier than the 
		///			value that was passed to the method. Use <tt>AK::SoundEngine::GetPlayingSegmentInfo()</tt> to query 
		///			the exact position of a segment. Also, the segment will be silent during the time that period
		///			(so that it restarts precisely at the position that you specified). <tt>AK::SoundEngine::GetPlayingSegmentInfo()</tt> 
		///			also informs you about the remaining look-ahead time. 
		///
		/// Notes specific to Music Switch Containers:
		///		- Seeking triggers a music transition towards the current (or target) segment.
		///			This transition is subject to the container's transition rule that matches the current and defined in the container,
		///			so the moment when seeking occurs depends on the rule's "Exit At" property. On the other hand, the starting position 
		///			in the target segment depends on both the desired seeking position and the rule's "Sync To" property.
		///		- If the specified time is greater than the destination segment's length, the modulo is taken.
		///
		/// \sa
		/// - <tt>AK::SoundEngine::RenderAudio()</tt>
		/// - <tt>AK::SoundEngine::PostEvent()</tt>
		/// - <tt>AK::SoundEngine::GetSourcePlayPosition()</tt>
		/// - <tt>AK::SoundEngine::GetPlayingSegmentInfo()</tt>
		extern   AKRESULT __cdecl  SeekOnEvent  ( 
			AkUniqueID in_eventID,										///< Unique ID of the event
			AkGameObjectID in_gameObjectID ,							///< Associated game object ID; use AK_INVALID_GAME_OBJECT to affect all game objects
			AkReal32 in_fPercent,										///< Desired position where playback should restart, expressed in a percentage of the file's total duration, between 0 and 1.f (see note above about infinite looping sounds)
			bool in_bSeekToNearestMarker = false,						///< If true, the final seeking position will be made equal to the nearest marker (see note above)	
			AkPlayingID in_PlayingID = AK_INVALID_PLAYING_ID			///< Specify the playing ID for the seek to be applied to. Will result in the seek happening only on active actions of the playing ID. Let it be AK_INVALID_PLAYING_ID or do not specify any, to seek on all active actions of this event ID.	
			);

#ifdef AK_SUPPORT_WCHAR
		/// Seeks inside all playing objects that are referenced in Play Actions of the specified Event.
		/// Seek position is specified as a percentage of the sound's total duration, and takes looping into account. 
		///
		/// Notes:
		///		- This works with all objects of the Containers hierarchy, and also with Music Segments and Music Switch Containers. 
		///		- There is a restriction with sounds that play within a continuous sequence. Seeking is ignored 
		///			if one of their ancestors is a continuous (random or sequence) container with crossfade or 
		///			trigger rate transitions. Seeking is also ignored with sample-accurate transitions, unless
		///			the sound that is currently playing is the first sound of the sequence.
		///		- Seeking is also ignored with voices that can go virtual with "From Beginning" behavior. 
		///		- If the option "Seek to nearest marker" is used, the seeking position snaps to the nearest marker.
		///			With objects of the Containers hierarchy, markers are embedded in wave files by an external wave editor.
		///			Note that looping regions ("sampler loop") are not considered as markers. Also, the "add file name marker" of the 
		///			conversion settings dialog adds a marker at the beginning of the file, which is considered when seeking
		///			to nearest marker. In the case of interactive music objects (Music Segments, Music Switch Containers, and Music Playlist Containers), user (wave) markers are ignored:
		///			seeking occurs to the nearest segment cue (authored in the segment editor), including the Entry Cue, but excluding the Exit Cue.
		///		- This method posts a command in the sound engine queue, thus seeking will not occur before 
		///			the audio thread consumes it (after a call to RenderAudio()). 
		///
		///	Notes specific to Music Segments:
		///		- With Music Segments, \c in_fPercent is relative to the Entry Cue, and the segment's duration is the 
		///			duration between its entry and exit cues. Consequently, you cannot seek within the pre-entry or
		///			post-exit of a segment using this method. Use absolute values instead.
		///		- Music Segments cannot be looped. You may want to listen to the \c AK_EndOfEvent notification 
		///			in order to restart them if required.
		///		- In order to restart at the correct location, with all their tracks synchronized, Music Segments 
		///			take the "look-ahead time" property of their streamed tracks into account for seeking. 
		///			Consequently, the resulting position after a call to SeekOnEvent() might be earlier than the 
		///			value that was passed to the method. Use <tt>AK::SoundEngine::GetPlayingSegmentInfo()</tt> to query 
		///			the exact position of a segment. Also, the segment will be silent during the time that period
		///			(so that it restarts precisely at the position that you specified). <tt>AK::SoundEngine::GetPlayingSegmentInfo()</tt> 
		///			also informs you about the remaining look-ahead time. 
		///
		/// Notes specific to Music Switch Containers:
		///		- Seeking triggers a music transition towards the current (or target) segment.
		///			This transition is subject to the container's transition rule that matches the current and defined in the container,
		///			so the moment when seeking occurs depends on the rule's "Exit At" property. On the other hand, the starting position 
		///			in the target segment depends on both the desired seeking position and the rule's "Sync To" property.
		///		- If the specified time is greater than the destination segment's length, the modulo is taken.
		///
		/// \sa
		/// - <tt>AK::SoundEngine::RenderAudio()</tt>
		/// - <tt>AK::SoundEngine::PostEvent()</tt>
		/// - <tt>AK::SoundEngine::GetSourcePlayPosition()</tt>
		/// - <tt>AK::SoundEngine::GetPlayingSegmentInfo()</tt>
		extern   AKRESULT __cdecl  SeekOnEvent  ( 
			const wchar_t* in_pszEventName,								///< Name of the event
			AkGameObjectID in_gameObjectID ,							///< Associated game object ID; use AK_INVALID_GAME_OBJECT to affect all game objects
			AkReal32 in_fPercent ,										///< Desired position where playback should restart, expressed in a percentage of the file's total duration, between 0 and 1.f (see note above about infinite looping sounds)
			bool in_bSeekToNearestMarker = false,						///< If true, the final seeking position will be made equal to the nearest marker (see note above)	
			AkPlayingID in_PlayingID = AK_INVALID_PLAYING_ID			///< Specify the playing ID for the seek to be applied to. Will result in the seek happening only on active actions of the playing ID. Let it be AK_INVALID_PLAYING_ID or do not specify any, to seek on all active actions of this event ID.	
			);
#endif //AK_SUPPORT_WCHAR

		/// Seeks inside all playing objects that are referenced in Play Actions of the specified Event.
		/// Seek position is specified as a percentage of the sound's total duration, and takes looping into account. 
		///
		/// Notes:
		///		- This works with all objects of the Containers hierarchy, and also with Music Segments and Music Switch Containers. 
		///		- There is a restriction with sounds that play within a continuous sequence. Seeking is ignored 
		///			if one of their ancestors is a continuous (random or sequence) container with crossfade or 
		///			trigger rate transitions. Seeking is also ignored with sample-accurate transitions, unless
		///			the sound that is currently playing is the first sound of the sequence.
		///		- Seeking is also ignored with voices that can go virtual with "From Beginning" behavior. 
		///		- If the option "Seek to nearest marker" is used, the seeking position snaps to the nearest marker.
		///			With objects of the Containers hierarchy, markers are embedded in wave files by an external wave editor.
		///			Note that looping regions ("sampler loop") are not considered as markers. Also, the "add file name marker" of the 
		///			conversion settings dialog adds a marker at the beginning of the file, which is considered when seeking
		///			to nearest marker. In the case of interactive music objects (Music Segments, Music Switch Containers, and Music Playlist Containers), user (wave) markers are ignored:
		///			seeking occurs to the nearest segment cue (authored in the segment editor), including the Entry Cue, but excluding the Exit Cue.
		///		- This method posts a command in the sound engine queue, thus seeking will not occur before 
		///			the audio thread consumes it (after a call to RenderAudio()). 
		///
		///	Notes specific to Music Segments:
		///		- With Music Segments, \c in_fPercent is relative to the Entry Cue, and the segment's duration is the 
		///			duration between its entry and exit cues. Consequently, you cannot seek within the pre-entry or
		///			post-exit of a segment using this method. Use absolute values instead.
		///		- Music segments cannot be looped. You may want to listen to the \c AK_EndOfEvent notification 
		///			in order to restart them if required.
		///		- In order to restart at the correct location, with all their tracks synchronized, Music Segments 
		///			take the "look-ahead time" property of their streamed tracks into account for seeking. 
		///			Consequently, the resulting position after a call to SeekOnEvent() might be earlier than the 
		///			value that was passed to the method. Use <tt>AK::SoundEngine::GetPlayingSegmentInfo()</tt> to query 
		///			the exact position of a segment. Also, the segment will be silent during the time that period
		///			(so that it restarts precisely at the position that you specified). <tt>AK::SoundEngine::GetPlayingSegmentInfo()</tt> 
		///			also informs you about the remaining look-ahead time. 
		///
		/// Notes specific to Music Switch Containers:
		///		- Seeking triggers a music transition towards the current (or target) segment.
		///			This transition is subject to the container's transition rule that matches the current and defined in the container,
		///			so the moment when seeking occurs depends on the rule's "Exit At" property. On the other hand, the starting position 
		///			in the target segment depends on both the desired seeking position and the rule's "Sync To" property.
		///		- If the specified time is greater than the destination segment's length, the modulo is taken.
		///
		/// \sa
		/// - <tt>AK::SoundEngine::RenderAudio()</tt>
		/// - <tt>AK::SoundEngine::PostEvent()</tt>
		/// - <tt>AK::SoundEngine::GetSourcePlayPosition()</tt>
		/// - <tt>AK::SoundEngine::GetPlayingSegmentInfo()</tt>
		extern   AKRESULT __cdecl  SeekOnEvent  ( 
			const char* in_pszEventName,								///< Name of the event
			AkGameObjectID in_gameObjectID,								///< Associated game object ID; use AK_INVALID_GAME_OBJECT to affect all game objects
			AkReal32 in_fPercent,										///< Desired position where playback should restart, expressed in a percentage of the file's total duration, between 0 and 1.f (see note above about infinite looping sounds)
			bool in_bSeekToNearestMarker = false,						///< If true, the final seeking position will be made equal to the nearest marker (see notes above).	
			AkPlayingID in_PlayingID = AK_INVALID_PLAYING_ID			///< Specify the playing ID for the seek to be applied to. Will result in the seek happening only on active actions of the playing ID. Let it be AK_INVALID_PLAYING_ID or do not specify any, to seek on all active actions of this event ID.	
			);

		/// Cancels all Event callbacks associated with a specific callback cookie.\n
		/// \sa 
		/// - <tt>AK::SoundEngine::PostEvent()</tt>
		extern   void __cdecl  CancelEventCallbackCookie  ( 
			void * in_pCookie 							///< Callback cookie to be cancelled
			);

		/// Cancels all Event callbacks associated with a specific game object.\n
		/// \sa 
		/// - <tt>AK::SoundEngine::PostEvent()</tt>
		extern   void __cdecl  CancelEventCallbackGameObject  (
			AkGameObjectID in_gameObjectID 				///< ID of the game object to be cancelled
			);

		/// Cancels all Event callbacks for a specific playing ID.
		/// \sa 
		/// - <tt>AK::SoundEngine::PostEvent()</tt>
		extern   void __cdecl  CancelEventCallback  ( 
			AkPlayingID in_playingID 					///< Playing ID of the event that must not use callbacks
			);

		/// Gets the current position of the source associated with this playing ID, obtained from PostEvent(). If more than one source is playing,
		/// the first to play is returned.
		/// Notes:
		/// - The source's position is updated at every audio frame, and the time at which this occurs is stored. 
		///		When you call this function from your thread, you therefore query the position that was updated in the previous audio frame.
		///		If in_bExtrapolate is true (default), the returned position is extrapolated using the elapsed time since last 
		///		sound engine update and the source's playback rate.
		/// \return 
		/// - \c AK_Success if successful.
		/// - \c AK_InvalidParameter if the provided pointer is not valid.
		/// - \c AK_PlayingIDNotFound if the playing ID is invalid (not playing yet, or finished playing).
		/// \sa 
		/// - \ref soundengine_query_pos
		/// - \ref concept_events
		extern   AKRESULT __cdecl  GetSourcePlayPosition  (
			AkPlayingID		in_PlayingID,				///< Playing ID returned by <tt>AK::SoundEngine::PostEvent()</tt>
			AkTimeMs*		out_puPosition,				///< Position of the source (in ms) associated with the specified playing ID
			bool			in_bExtrapolate = true		///< Position is extrapolated based on time elapsed since last sound engine update.
			);

		/// Gets the current position of the sources associated with this playing ID, obtained from PostEvent().
		/// Notes:
		/// - The source's position is updated at every audio frame, and the time at which this occurs is stored. 
		///		When you call this function from your thread, you therefore query the position that was updated in the previous audio frame.
		///		If in_bExtrapolate is true (default), the returned position is extrapolated using the elapsed time since last 
		///		sound engine update and the source's playback rate.
		/// - If 0 is passed in for the number of entries (*in_pcPositions == 0) then only the number of positions will be returned and the
		///     position array (out_puPositions) will not be updated.
		/// - The io_pcPositions pointer must be non-NULL.
		///     out_puPositions may be NULL if *io_pcPositions == 0, otherwise it must be non-NULL.
		/// - Positions are returned in no particular order.
		/// \return 
		/// - \c AK_Success if successful.
		/// - \c AK_InvalidParameter if the provided pointer is not valid.
		/// - \c AK_PlayingIDNotFound if the playing ID is invalid (not playing yet, or finished playing).
		/// \sa 
		/// - \ref soundengine_query_pos
		/// - \ref concept_events
		extern  AKRESULT __cdecl  GetSourcePlayPositions (
			AkPlayingID		in_PlayingID,				///< Playing ID returned by <tt>AK::SoundEngine::PostEvent()</tt>
			AkSourcePosition* out_puPositions,			///< Audio Node IDs and positions of sources associated with the specified playing ID
			AkUInt32 *		io_pcPositions,				///< Number of entries in out_puPositions. Needs to be set to the size of the array: it is adjusted to the actual number of returned entries
			bool			in_bExtrapolate = true		///< Position is extrapolated based on time elapsed since last sound engine update
			);

		/// Query information on the active segment of a music object that is playing. Use the playing ID 
		/// that was returned from AK::SoundEngine::PostEvent(), provided that the event contained a play
		/// action that was targeting a music object. For any interactive music object (Music Segments, Music Switch Containers, and Music Playlist Containers), 
		/// there is only one segment that is active at a time. 
		/// To be able to query segment information, you must pass the AK_EnableGetMusicPlayPosition flag 
		/// to the AK::SoundEngine::PostEvent() method. This informs the sound engine that the source associated 
		/// with this event should be given special consideration because GetPlayingSegmentInfo() can be called 
		/// at any time for this AkPlayingID.
		/// Notes:
		/// - If the music object is a single segment, you will get negative values for AkSegmentInfo::iCurrentPosition
		///		during the pre-entry. This will never occur with other types of music objects because the 
		///		pre-entry of a segment always overlaps another active segment.
		///	- The active segment during the pre-entry of the first segment of a Playlist Container or a Music Switch 
		///		Container is "nothing", as well as during the post-exit of the last segment of a Playlist (and beyond).
		///	- When the active segment is "nothing", out_uSegmentInfo is filled with zeros.
		/// - If in_bExtrapolate is true (default), AkSegmentInfo::iCurrentPosition is corrected by the amount of time elapsed
		///		since the beginning of the audio frame. It is thus possible that it slightly overshoots the total segment length.
		/// \return AK_Success if there is a playing music structure associated with the specified playing ID.
		/// \sa
		/// - AK::SoundEngine::PostEvent
		/// - AkSegmentInfo
		extern  AKRESULT __cdecl  GetPlayingSegmentInfo (
			AkPlayingID		in_PlayingID,			///< Playing ID returned by AK::SoundEngine::PostEvent().
			AkSegmentInfo& out_segmentInfo,		///< Structure containing information about the active segment of the music structure that is playing.
			bool			in_bExtrapolate = true	///< Position is extrapolated based on time elapsed since last sound engine update.
			);

		/// Gets the stream buffering of the sources associated with this playing ID, obtained from PostEvent().
		/// Notes:
		/// - You need to pass AK_EnableGetSourceStreamBuffering to PostEvent() in order to use this function, otherwise
		/// 	it returns AK_Fail, even if the playing ID is valid.
		/// - The sources stream buffering is updated at every audio frame. If there are multiple sources associated with this playing ID,
		///		the value returned corresponds to the least buffered source. 
		/// - The returned buffering status out_bIsBuffering will be true If any of the sources associated with the playing ID are actively being buffered.
		///		It will be false if all of them have reached the end of file, or have reached a state where they are buffered enough and streaming is temporarily idle.
		/// - Purely in-memory sources are excluded from this database. If all sources are in-memory, GetSourceStreamBuffering() will return AK_PlayingIDNotFound.
		/// - The returned buffering amount and state is not completely accurate with some hardware-accelerated codecs. In such cases, the amount of stream buffering is generally underestimated.
		///		On the other hand, it is not guaranteed that the source will be ready to produce data at the next audio frame even if out_bIsBuffering has turned to false.
		/// \return 
		/// - \c AK_Success if successful.
		/// - \c AK_PlayingIDNotFound if the source data associated with this playing ID is not found, for example if PostEvent() was not called with AK_EnableGetSourceStreamBuffering, or if the header was not parsed.
		/// \sa 
		/// - \ref concept_events
		extern   AKRESULT __cdecl  GetSourceStreamBuffering  (
			AkPlayingID		in_PlayingID,				///< Playing ID returned by <tt>AK::SoundEngine::PostEvent()</tt>
			AkTimeMs &		out_buffering,				///< Returned amount of buffering (in ms) of the source (or one of the sources) associated with that playing ID
			bool &			out_bIsBuffering			///< Returned buffering status of the source(s) associated with that playing ID
			);

		/// Stops the current content playing associated to the specified game object ID.
		/// If no game object is specified, all sounds will be stopped.
		extern   void __cdecl  StopAll  ( 
			AkGameObjectID in_gameObjectID = AK_INVALID_GAME_OBJECT ///< (Optional)Specify a game object to stop only playback associated to the provided game object ID.
			);

		/// Stop the current content playing associated to the specified playing ID.
		/// \aknote 
		///	This function is deprecated. It is functionally identical to ExecuteActionOnPlayingID() using AkActionOnEventType_Stop.
		/// API call in Wwise Capture Log will show ExecuteActionOnPlayingID when calling this function.
		/// Use ExecuteActionOnPlayingID() instead.
		/// \endaknote
		/// \sa
		///  - <tt>AK::SoundEngine::ExecuteActionOnPlayingID()</tt>
		extern   void __cdecl  StopPlayingID  (
			AkPlayingID in_playingID,											///< Playing ID to be stopped.
			AkTimeMs in_uTransitionDuration = 0,								///< Fade duration
			AkCurveInterpolation in_eFadeCurve = AkCurveInterpolation_Linear	///< Curve type to be used for the transition
			);

		/// Executes an Action on the content associated to the specified playing ID.
		/// \sa
		/// - <tt>AkActionOnEventType</tt>
		extern  void __cdecl  ExecuteActionOnPlayingID (
			AkActionOnEventType in_ActionType,									///< Action to execute on the specified playing ID.
			AkPlayingID in_playingID,											///< Playing ID on which to execute the action.
			AkTimeMs in_uTransitionDuration = 0,								///< Fade duration
			AkCurveInterpolation in_eFadeCurve = AkCurveInterpolation_Linear	///< Curve type to be used for the transition
			);

		/// Sets the random seed value. Can be used to synchronize randomness
		/// across instances of the Sound Engine.
		/// \remark This seeds the number generator used for all container randomization 
		///         and the plug-in RNG; since it acts globally, this should be called right 
		///         before any PostEvent call where randomness synchronization is required,
		///         and cannot guarantee similar results for continuous containers.
		/// \sa
		/// - <tt>AK::IAkPluginServiceRNG</tt>
		extern   void __cdecl  SetRandomSeed  ( 
			AkUInt32 in_uSeed													///< Random seed.
			);

		/// Mutes/Unmutes the busses tagged as background music.  
		/// This is automatically called for platforms that have user-music support.
		/// This function is provided to give the same behavior on platforms that don't have user-music support.
		extern   void __cdecl  MuteBackgroundMusic   (
			bool in_bMute ///< Sets true to mute, false to unmute.
			);
		//@}

		/// Gets the state of the Background Music busses. This state is either set directly 
		/// with \c AK::SoundEngine::MuteBackgroundMusic or by the OS, if it has User Music services.
		/// \return true if the background music busses are muted, false if not.
		extern  bool __cdecl  GetBackgroundMusicMute  ();
		//@}


		/// Sends custom game data to a plug-in that resides on a bus (effect plug-in) or a voice (source plug-in).
		/// Data will be copied and stored into a separate list.
		/// Previous entry is deleted when a new one is sent.
		/// Sets the data pointer to NULL to clear item from the list.
		/// \aknote The plug-in type and ID are passed and matched with plugins set on the desired bus. 
		/// This means that multiple instances of the same plug-in on a given bus' effect chain will always receive the same data.
		/// \endaknote
		/// \return AK_Success if data was sent successfully.
		extern   AKRESULT __cdecl  SendPluginCustomGameData   (
			AkUniqueID in_busID,			///< Bus ID. For source plug-ins, specify AK_INVALID_UNIQUE_ID.
			AkGameObjectID in_gameObjectID,	///< Game Object ID. Pass AK_INVALID_GAME_OBJECT to send custom data with global scope. Game object scope supersedes global scope, as with RTPCs. 
			AkPluginType in_eType,			///< Plug-in type (for example, source or effect)
			AkUInt32 in_uCompanyID,		///< Company identifier (as declared in the plug-in description XML file)
			AkUInt32 in_uPluginID,			///< Plug-in identifier (as declared in the plug-in description XML file)
			const void* in_pData,			///< The data blob
			AkUInt32 in_uSizeInBytes		///< Size of data
			);
		//@}

		////////////////////////////////////////////////////////////////////////
		/// @name Game Objects
		//@{
		
        /// Registers a game object.
		/// \return
		/// - \c AK_Success if successful
		/// - \c AK_InvalidParameter if the specified AkGameObjectID is invalid. Range 0xFFFFFFFFFFFFFFE0 (-32) to 0xFFFFFFFFFFFFFFFF (-1) are invalid inclusively.
		/// \remark Registering a game object twice does nothing. Unregistering it once unregisters it no 
		///			matter how many times it has been registered.
		/// \sa 
		/// - <tt>AK::SoundEngine::UnregisterGameObj()</tt>
		/// - <tt>AK::SoundEngine::UnregisterAllGameObj()</tt>
		/// - \ref concept_gameobjects
		extern  AKRESULT __cdecl  RegisterGameObj (
			AkGameObjectID in_gameObjectID								///< ID of the game object to be registered. Valid range is [0 to 0xFFFFFFFFFFFFFFDF].
	        );

		/// Registers a game object.
		/// \return
		/// - \c AK_Success if successful
		/// - \c AK_InvalidParameter if the specified AkGameObjectID is invalid. Range 0xFFFFFFFFFFFFFFE0 (-32) to 0xFFFFFFFFFFFFFFFF (-1) are invalid inclusively.
		/// \remark Registering a game object twice does nothing. Unregistering it once unregisters it no 
		///			matter how many times it has been registered.
		/// \sa 
		/// - <tt>AK::SoundEngine::UnregisterGameObj()</tt>
		/// - <tt>AK::SoundEngine::UnregisterAllGameObj()</tt>
		/// - \ref concept_gameobjects
        extern   AKRESULT __cdecl  RegisterGameObj  (
	        AkGameObjectID in_gameObjectID,							///< ID of the game object to be registered. Valid range is [0 to 0xFFFFFFFFFFFFFFDF].
			const char * in_pszObjName								///< Name of the game object (for monitoring purpose)
	        );

        /// Unregisters a game object.
		/// \return 
		/// - \c AK_Success if successful
		/// - \c AK_InvalidParameter if the specified AkGameObjectID is invalid. Range 0xFFFFFFFFFFFFFFE0 (-32) to 0xFFFFFFFFFFFFFFFF (-1) are invalid inclusively.
		/// \remark Registering a game object twice does nothing. Unregistering it once unregisters it no 
		///			matter how many times it has been registered. Unregistering a game object while it is 
		///			in use is allowed, but the control over the parameters of this game object is lost.
		///			For example, say a sound associated with this game object is a 3D moving sound. This sound will 
		///			stop moving when the game object is unregistered, and there will be no way to regain control over the game object.
		/// \sa 
		/// - <tt>AK::SoundEngine::RegisterGameObj()</tt>
		/// - <tt>AK::SoundEngine::UnregisterAllGameObj()</tt>
		/// - \ref concept_gameobjects
        extern   AKRESULT __cdecl  UnregisterGameObj  (
	        AkGameObjectID in_gameObjectID				///< ID of the game object to be unregistered. Valid range is [0 to 0xFFFFFFFFFFFFFFDF]. Use
	        											///< AK_INVALID_GAME_OBJECT to unregister all game objects.
	        );

		/// Unregister all game objects, or all game objects with a particular matching set of property flags.
		/// This function to can be used to unregister all game objects.  
		/// \return
		/// - \c AK_Success if successful
		/// \remark Registering a game object twice does nothing. Unregistering it once unregisters it no 
		///			matter how many times it has been registered. Unregistering a game object while it is 
		///			in use is allowed, but the control over the parameters of this game object is lost.
		///			For example, if a sound associated with this game object is a 3D moving sound, it will 
		///			stop moving once the game object is unregistered, and there will be no way to recover 
		///			the control over this game object.
		/// \sa 
		/// - <tt>AK::SoundEngine::RegisterGameObj()</tt>
		/// - <tt>AK::SoundEngine::UnregisterGameObj()</tt>
		/// - \ref concept_gameobjects
		extern   AKRESULT __cdecl  UnregisterAllGameObj  (
			);

       	/// Sets the position of a game object.
		/// \warning The object's orientation vector (in_Position.Orientation) must be normalized.
		/// \return 
		/// - \c AK_Success when successful
		/// - \c AK_InvalidParameter if parameters are not valid, for example: 
		/// + in_Position makes an invalid transform 
		/// + in_eFlags is not one of the valid enum values 
		/// + the game object ID is in the reserved ID range.
		/// \sa 
		/// - \ref soundengine_3dpositions
        extern   AKRESULT __cdecl  SetPosition  ( 
			AkGameObjectID in_GameObjectID,		///< Game Object identifier
			const AkSoundPosition & in_Position,///< Position to set; in_Position.Orientation must be normalized.
			AkSetPositionFlags in_eFlags = AkSetPositionFlags_Default ///< Optional flags to independently set the position of the emitter or listener component.
		    );

		/// Sets multiple positions to a single game object.
		/// Setting multiple positions on a single game object is a way to simulate multiple emission sources while using the resources of only one voice.
		/// This can be used to simulate wall openings, area sounds, or multiple objects emitting the same sound in the same area.
		/// \aknote
		/// - Calling <tt>AK::SoundEngine::SetMultiplePositions()</tt> with only one position is the same as calling <tt>AK::SoundEngine::SetPosition()</tt> 
		/// - If a sound has diffraction enabled, it is treated as <tt>AkMultiPositionType_MultiDirections</tt>. <tt>AkMultiPositionType_MultiSources</tt> is not supported in this case.
		/// \endaknote
		/// \return 
		/// - \c AK_Success when successful
		/// - \c AK_CommandTooLarge if the number of positions is too large for the command queue. Reduce the number of positions.
		/// - \c AK_InvalidParameter if parameters are not valid, for example: 
		/// + in_Position makes an invalid transform 
		/// + in_eFlags is not one of the valid enum values 
		/// + the game object ID is in the reserved ID range.
		/// \sa 
		/// - \ref soundengine_3dpositions
		/// - \ref soundengine_3dpositions_multiplepos
		/// - \ref AkMultiPositionType
        extern   AKRESULT __cdecl  SetMultiplePositions  ( 
			AkGameObjectID in_GameObjectID,						///< Game Object identifier.
			const AkSoundPosition * in_pPositions,				///< Array of positions to apply.
			AkUInt16 in_NumPositions,							///< Number of positions specified in the provided array.
			AkMultiPositionType in_eMultiPositionType = AkMultiPositionType_MultiDirections, ///< \ref AkMultiPositionType
			AkSetPositionFlags in_eFlags = AkSetPositionFlags_Default ///< Optional flags to independently set the position of the emitter or listener component.
		    );

		/// Sets multiple positions to a single game object, with flexible assignment of input channels.
		/// Setting multiple positions on a single game object is a way to simulate multiple emission sources while using the resources of only one voice.
		/// This can be used to simulate wall openings, area sounds, or multiple objects emitting the same sound in the same area.
		/// \aknote Calling <tt>AK::SoundEngine::SetMultiplePositions()</tt> with only one position is the same as calling <tt>AK::SoundEngine::SetPosition()</tt> \endaknote
		/// \return 
		/// - \c AK_Success when successful
		/// - \c AK_CommandTooLarge if the number of positions is too large for the command queue. Reduce the number of positions.
		/// - \c AK_InvalidParameter if parameters are not valid.
		/// \sa 
		/// - \ref soundengine_3dpositions
		/// - \ref soundengine_3dpositions_multiplepos
		/// - \ref AkMultiPositionType
        extern   AKRESULT __cdecl  SetMultiplePositions  ( 
			AkGameObjectID in_GameObjectID,						///< Game Object identifier.
			const AkChannelEmitter * in_pPositions,			///< Array of positions to apply, each using its own channel mask.
			AkUInt16 in_NumPositions,							///< Number of positions specified in the provided array.
			AkMultiPositionType in_eMultiPositionType = AkMultiPositionType_MultiDirections, ///< \ref AkMultiPositionType
			AkSetPositionFlags in_eFlags = AkSetPositionFlags_Default ///< Optional flags to independently set the position of the emitter or listener component.
		    );

        /// Sets the scaling factor of a Game Object.
		/// Modify the attenuation computations on this Game Object to simulate sounds with a larger or smaller area of effect.
		/// \return 
		/// - \c AK_Success when successful
		/// - \c AK_InvalidParameter if the scaling factor specified was 0 or negative.
		/// - \c AK_InvalidFloatValue if the value specified was NaN or Inf
		extern   AKRESULT __cdecl  SetScalingFactor  (
			AkGameObjectID in_GameObjectID,				///< Game object identifier
			AkReal32 in_fAttenuationScalingFactor		///< Scaling Factor, 1 means 100%, 0.5 means 50%, 2 means 200%, and so on.
			);

		/// Use the position of a separate game object for distance calculations for a specified listener.
		/// When <tt>AK::SoundEngine::SetDistanceProbe()</tt> is called, Wwise calculates distance attenuation and filtering 
		/// based on the distance between the distance probe Game Object (\c in_distanceProbeGameObjectID) and the emitter Game Object's position.
		/// In third-person perspective applications, the distance probe Game Object may be set to the player character's position, 
		/// and the listener Game Object's position to that of the camera. In this scenario, attenuation is based on 
		/// the distance between the character and the sound, whereas panning, spatialization, and spread and focus calculations are base on the camera.
		/// Both Game Objects, \c in_listenerGameObjectID and \c in_distanceProbeGameObjectID must have been previously registered using <tt>AK::SoundEngine::RegisterGameObj</tt>.
		/// This function is optional. if <tt>AK::SoundEngine::SetDistanceProbe()</tt> is never called, distance calculations are based on the listener Game Object position.
		/// To clear the distance probe, and revert to using the listener position for distance calculations, pass \c AK_INVALID_GAME_OBJECT to \c in_distanceProbeGameObjectID.
		/// \aknote If the distance probe Game Object is assigned multiple positions, then the first position is used for distance calculations by the listener. \endaknote
		/// \return 
		/// - \c AK_Success when successful
		/// \sa 
		/// - <tt>AK::SoundEngine::SetPosition()</tt>
		extern  AKRESULT __cdecl  SetDistanceProbe (
			AkGameObjectID in_listenerGameObjectID,		///< Game object identifier for the listener. Must have been previously registered via RegisterGameObj.
			AkGameObjectID in_distanceProbeGameObjectID	///< Game object identifier for the distance probe, or \c AK_INVALID_GAME_OBJECT to reset distance probe.  If valid, must have been previously registered via RegisterGameObj.
			);

        //@}

		////////////////////////////////////////////////////////////////////////
		/// @name Bank Management
		//@{

		/// Processes all pending asynchronous bank operations.
		/// This function must be called periodically (once per game frame, for example) when using asynchronous bank-loading functions. Its purpose is to 
		/// process all queued asynchronous LoadBank, UnloadBank, PrepareBank, etc (functions using a callback). The work done in this function can be slow or blocking.
		/// Therefore, the calling thread must not be a critical thread. It is recommended to have a dedicated thread for this, or a job/task if such a system exists.
		/// 
		/// When <tt>AkInitSettings::bUseSoundBankMgrThread</tt> is <tt>false</tt>, this function processes all pending operations immediately on the calling thread.
		/// 
		/// \akcaution
		/// Do not call this function from multiple different threads.
		/// Synchronous Bank and Prepare (functions without callbacks) calls implicitely call this function. Therefore, synchronous Bank and Prepare
		/// calls should always be called from the same thread that is calling <tt>AK::SoundEngine::ProcessBanks</tt>.
		/// \endakcaution
		/// 
		/// \sa
		/// - <tt>AK::SoundEngine::LoadBank</tt>
		/// - <tt>AK::SoundEngine::PrepareEvent</tt>
		/// - \ref goingfurther_eventmgrthread
		extern  void __cdecl  ProcessBanks ();

		/// Unload all currently loaded banks.
		/// It also internally calls ClearPreparedEvents() since at least one bank must have been loaded to allow preparing events.
		/// \return 
		/// - \c AK_Success if successful
		/// - \c AK_NotInitialized if the sound engine was not correctly initialized or if there is not enough memory to handle the command
		/// \sa 
		/// - <tt>AK::SoundEngine::UnloadBank()</tt>
		/// - <tt>AK::SoundEngine::LoadBank()</tt>
		/// - \ref soundengine_banks
		extern   AKRESULT __cdecl  ClearBanks  ();

		/// Sets the I/O settings of the bank load and prepare event processes.
        /// The sound engine uses default values unless explicitly set by calling this method.
		/// \warning This function must be called before loading banks.
		/// \return 
		/// - \c AK_Success if successful
		/// - \c AK_NotInitialized if the sound engine was not correctly initialized
		/// - \c AK_InvalidParameter if some parameters are invalid, check the debug console
		/// \sa 
		/// - \ref soundengine_banks
        /// - \ref streamingdevicemanager
        extern   AKRESULT __cdecl  SetBankLoadIOSettings  (
            AkReal32            in_fThroughput,         ///< Average throughput of bank data streaming (bytes/ms) (the default value is AK_DEFAULT_BANK_THROUGHPUT)
            AkPriority          in_priority             ///< Priority of bank streaming (the default value is AK_DEFAULT_PRIORITY)
            );

#ifdef AK_SUPPORT_WCHAR
		/// Load a bank synchronously (by Unicode string).\n
		/// The bank name and type are passed to the Stream Manager.
		/// See \ref soundengine_banks_general for a discussion on using strings and IDs.
		/// A bank load request will be posted, and consumed by the Bank Manager thread.
		/// The function returns when the request has been completely processed.
		/// \return 
		/// The bank ID, which is obtained by hashing the bank name (see GetIDFromString()). 
		/// You may use this ID with UnloadBank().
		/// - \c AK_Success: Load or unload successful.
		/// - \c AK_BankAlreadyLoaded: This bank is already loaded, nothing done.
		/// - \c AK_InsufficientMemory: Insufficient memory to store bank data.
		/// - \c AK_BankReadError: I/O error.
		/// - \c AK_WrongBankVersion: Invalid bank version: make sure the version of Wwise that you used to generate the SoundBanks matches that of the SDK you are currently using.
		/// - \c AK_InvalidFile: File specified could not be opened.
		/// - \c AK_InvalidParameter: Invalid parameter, invalid memory alignment.		
		/// - \c AK_NotInitialized if the sound engine was not correctly initialized
		/// - \c AK_InvalidParameter if some parameters are invalid, check the debug console
		/// - \c AK_InvalidBankType if the bank type parameter is out of range.
		/// - \c AK_Fail: Load or unload failed for any other reason. (Most likely small allocation failure, check the debug console)
		/// \remarks
		/// - The initialization bank (Init.bnk) must be loaded first.
		/// - All SoundBanks subsequently loaded must come from the same Wwise project as the
		///   initialization bank. If you need to load SoundBanks from a different project, you
		///   must first unload ALL banks, including the initialization bank, then load the
		///   initialization bank from the other project, and finally load banks from that project.
		/// - Codecs and plug-ins must be registered before loading banks that use them.
		/// - Loading a bank referencing an unregistered plug-in or codec will result in a load bank success,
		/// but the plug-ins will not be used. More specifically, playing a sound that uses an unregistered effect plug-in 
		/// will result in audio playback without applying the said effect. If an unregistered source plug-in is used by an event's audio objects, 
		/// posting the event will fail.
		/// - The sound engine internally calls GetIDFromString(in_pszString) to return the correct bank ID.
		/// Therefore, in_pszString should be the real name of the SoundBank (with or without the BNK extension - it is trimmed internally),
		/// not the name of the file (if you changed it), nor the full path of the file. The path should be resolved in 
		/// your implementation of the Stream Manager, or in the Low-Level I/O module if you use the default Stream Manager's implementation.
		/// \aknote
		/// When Wwise Authoring is connected to the game in "Profile And Edit (Sync All)" mode, LoadBank will return AK_Success even if the file is not found.
		/// The Capture Log will still report the missing file. For example, this will be the case when SoundBanks were not generated.
		/// If this option is selected, you can work without SoundBanks as long as Wwise Authoring is connected. 
		/// The Init.bnk is still required to initialize the sound engine properly.
		/// \endaknote
		/// \sa 
		/// - <tt>AK::SoundEngine::UnloadBank()</tt>
		/// - <tt>AK::SoundEngine::ClearBanks()</tt>
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
		/// - \ref soundengine_banks
		/// - \ref integrating_elements_plugins
		/// - \ref streamingdevicemanager
		/// - \ref streamingmanager_lowlevel
		/// - \ref sdk_bank_training
		extern   AKRESULT __cdecl  LoadBank  (
	        const wchar_t*      in_pszString,					///< Name of the bank to load
            AkBankID &          out_bankID,						///< Returned bank ID
			AkBankType			in_bankType = AkBankType_User	///< Type of the bank to load
	        );
#endif //AK_SUPPORT_WCHAR

		/// Loads a bank synchronously.\n
		/// The bank name and type are passed to the Stream Manager.
		/// See \ref soundengine_banks_general for a discussion on using strings and IDs.
		/// A bank load request will be posted, and consumed by the Bank Manager thread.
		/// The function returns when the request has been completely processed.
		/// \return 
		/// The bank ID, which is obtained by hashing the bank name (see GetIDFromString()). 
		/// You may use this ID with UnloadBank().
		/// - \c AK_Success: Load or unload successful.
		/// - \c AK_BankAlreadyLoaded: This bank is already loaded, nothing done.
		/// - \c AK_InsufficientMemory: Insufficient memory to store bank data.
		/// - \c AK_BankReadError: I/O error.
		/// - \c AK_WrongBankVersion: Invalid bank version: make sure the version of Wwise that 
		/// you used to generate the SoundBanks matches that of the SDK you are currently using.
		/// - \c AK_InvalidFile: File specified could not be opened.
		/// - \c AK_NotInitialized if the sound engine was not correctly initialized
		/// - \c AK_InvalidParameter if some parameters are invalid, check the debug console
		/// - \c AK_InvalidBankType if the bank type parameter is out of range.
		/// - \c AK_Fail: Load or unload failed for any other reason. (Most likely small allocation failure)
		/// \remarks
		/// - The initialization bank (Init.bnk) must be loaded first.
		/// - All SoundBanks subsequently loaded must come from the same Wwise project as the
		///   initialization bank. If you need to load SoundBanks from a different project, you
		///   must first unload ALL banks, including the initialization bank, then load the
		///   initialization bank from the other project, and finally load banks from that project.
		/// - Codecs and plug-ins must be registered before loading banks that use them.
		/// - Loading a bank referencing an unregistered plug-in or codec will result in a load bank success,
		/// but the plug-ins will not be used. More specifically, playing a sound that uses an unregistered effect plug-in 
		/// will result in audio playback without applying the said effect. If an unregistered source plug-in is used by an event's audio objects, 
		/// posting the event will fail.
		/// - The sound engine internally calls GetIDFromString(in_pszString) to return the correct bank ID.
		/// Therefore, in_pszString should be the real name of the SoundBank (with or without the BNK extension - it is trimmed internally),
		/// not the name of the file (if you changed it), nor the full path of the file. The path should be resolved in 
		/// your implementation of the Stream Manager, or in the Low-Level I/O module if you use the default Stream Manager's implementation.
		/// \aknote
		/// When Wwise Authoring is connected to the game in "Profile And Edit (Sync All)" mode, LoadBank will return AK_Success even if the file is not found.
		/// The Capture Log will still report the missing file. For example, this will be the case when SoundBanks were not generated.
		/// If this option is selected, you can work without SoundBanks as long as Wwise Authoring is connected. 
		/// The Init.bnk is still required to initialize the sound engine properly.
		/// \endaknote
		/// \sa 
		/// - <tt>AK::SoundEngine::UnloadBank()</tt>
		/// - <tt>AK::SoundEngine::ClearBanks()</tt>
		/// - <tt>AK::SoundEngine::GetIDFromString</tt>
		/// - \ref soundengine_banks
		/// - \ref integrating_elements_plugins
		/// - \ref streamingdevicemanager
		/// - \ref streamingmanager_lowlevel
		/// - \ref sdk_bank_training
		extern   AKRESULT __cdecl  LoadBank  (
	        const char*         in_pszString,					///< Name of the bank to load
            AkBankID &          out_bankID,						///< Returned bank ID
			AkBankType			in_bankType = AkBankType_User	///< Type of the bank to load
	        );

        /// Loads a bank synchronously (by ID).\n
		/// \aknote Requires that the "Use SoundBank names" option be unchecked in the Wwise Project Settings. \endaknote
		/// The bank ID is passed to the Stream Manager.
		/// See \ref soundengine_banks_general for a discussion on using strings and IDs.
		/// A bank load request will be posted, and consumed by the Bank Manager thread.
		/// The function returns when the request has been completely processed.
		/// \return 
		/// - \c AK_Success: Load or unload successful.
		/// - \c AK_BankAlreadyLoaded: This bank is already loaded, nothing done.
		/// - \c AK_InsufficientMemory: Insufficient memory to store bank data.
		/// - \c AK_BankReadError: I/O error. The bank is either shorter than expected or its data corrupted.
		/// - \c AK_WrongBankVersion: Invalid bank version: make sure the version of Wwise that 
		/// you used to generate the SoundBanks matches that of the SDK you are currently using.
		/// - \c AK_InvalidFile: File specified could not be opened.
		/// - \c AK_NotInitialized if the sound engine was not correctly initialized
		/// - \c AK_InvalidParameter if some parameters are invalid, check the debug console or Wwise Profiler
		/// - \c AK_InvalidBankType if the bank type parameter is out of range.
		/// - \c AK_FileNotFound if the bank file was not found on disk.
		/// - \c AK_FilePermissionError if the file permissions are wrong for the file
		/// - \c AK_Fail: Load or unload failed for any other reason. , check the debug console or Wwise Profiler
		/// \remarks
		/// - The initialization bank (Init.bnk) must be loaded first.
		/// - All SoundBanks subsequently loaded must come from the same Wwise project as the
		///   initialization bank. If you need to load SoundBanks from a different project, you
		///   must first unload ALL banks, including the initialization bank, then load the
		///   initialization bank from the other project, and finally load banks from that project.
		/// - Codecs and plug-ins must be registered before loading banks that use them.
		/// - Loading a bank referencing an unregistered plug-in or codec will result in a load bank success,
		/// but the plug-ins will not be used. More specifically, playing a sound that uses an unregistered effect plug-in 
		/// will result in audio playback without applying the said effect. If an unregistered source plug-in is used by an event's audio objects, 
		/// posting the event will fail.
		/// \aknote
		/// When Wwise Authoring is connected to the game in "Profile And Edit (Sync All)" mode, LoadBank will return AK_Success even if the file is not found.
		/// The Capture Log will still report the missing file. For example, this will be the case when SoundBanks were not generated.
		/// If this option is selected, you can work without SoundBanks as long as Wwise Authoring is connected. 
		/// The Init.bnk is still required to initialize the sound engine properly.
		/// \endaknote
		/// \sa 
		/// - <tt>AK::SoundEngine::UnloadBank()</tt>
		/// - <tt>AK::SoundEngine::ClearBanks()</tt>
		/// - \ref soundengine_banks
		/// - \ref integrating_elements_plugins
		/// - \ref sdk_bank_training
		extern   AKRESULT __cdecl  LoadBank  (
	        AkBankID			in_bankID,						///< Bank ID of the bank to load
			AkBankType			in_bankType = AkBankType_User	///< Type of the bank to load
			);

		/// Loads a bank synchronously (from in-memory data, in-place, user bank only).\n
		///
		/// IMPORTANT: Banks loaded from memory with in-place data MUST be unloaded using the UnloadBank function
		/// providing the same memory pointer. Make sure you are using the correct UnloadBank(...) overload
		///
		/// Use LoadBankMemoryView when you want to manage I/O on your side. Load the bank file
		/// in a buffer and pass its address to the sound engine.
		/// In-memory loading is in-place: *** the memory must be valid until the bank is unloaded. ***
		/// A bank load request will be posted, and consumed by the Bank Manager thread.
		/// The function returns when the request has been completely processed.
		/// \return 
		/// The bank ID, which is stored in the first few bytes of the bank file. You may use this 
		/// ID with UnloadBank().
		/// - \c AK_Success: Load or unload successful.
		/// - \c AK_BankAlreadyLoaded: This bank is already loaded, nothing done.
		/// - \c AK_InsufficientMemory: Insufficient memory to store bank data.
		/// - \c AK_BankReadError: I/O error.
		/// - \c AK_WrongBankVersion: Invalid bank version: make sure the version of Wwise that 
		/// you used to generate the SoundBanks matches that of the SDK you are currently using.
		/// - \c AK_InvalidFile: File specified could not be opened.
		/// - \c AK_NotInitialized if the sound engine was not correctly initialized
		/// - \c AK_InvalidParameter if some parameters are invalid, check the debug console 
		/// - \c AK_InvalidBankType if the bank is not a user-defined bank.
		/// - \c AK_DataAlignmentError if the data pointer is not aligned properly 
		/// - \c AK_Fail: Load or unload failed for any other reason. (Most likely small allocation failure)
		/// \remarks
		/// - The initialization bank (Init.bnk) must be loaded first.
		/// - All SoundBanks subsequently loaded must come from the same Wwise project as the
		///   initialization bank. If you need to load SoundBanks from a different project, you
		///   must first unload ALL banks, including the initialization bank, then load the
		///   initialization bank from the other project, and finally load banks from that project.
		/// - Codecs and plug-ins must be registered before loading banks that use them.
		/// - Loading a bank referencing an unregistered plug-in or codec will result in a load bank success,
		/// but the plug-ins will not be used. More specifically, playing a sound that uses an unregistered effect plug-in 
		/// will result in audio playback without applying the said effect. If an unregistered source plug-in is used by an event's audio objects, 
		/// posting the event will fail.
		/// - The memory must be aligned on platform-specific AK_BANK_PLATFORM_DATA_ALIGNMENT bytes (see AkTypes.h).
		/// - Avoid using this function for banks containing a lot of events or structure data: this data will be unpacked into the sound engine heap,
		///   making the supplied bank memory redundant. For event/structure-only banks, prefer LoadBankMemoryCopy().
		/// \sa 
		/// - <tt>AK::SoundEngine::UnloadBank()</tt>
		/// - <tt>AK::SoundEngine::ClearBanks()</tt>
		/// - \ref soundengine_banks
		/// - \ref integrating_elements_plugins
		/// - \ref sdk_bank_training
		extern   AKRESULT __cdecl  LoadBankMemoryView  (
	        const void *		in_pInMemoryBankPtr,	///< Pointer to the in-memory bank to load (pointer is stored in sound engine, memory must remain valid)
			AkUInt32			in_uInMemoryBankSize,	///< Size of the in-memory bank to load
            AkBankID &          out_bankID				///< Returned bank ID
	        );

		/// Loads a bank synchronously (from in-memory data, in-place, any bank type).\n
		///
		/// IMPORTANT: Banks loaded from memory with in-place data MUST be unloaded using the UnloadBank function
		/// providing the same memory pointer. Make sure you are using the correct UnloadBank(...) overload
		///
		/// Use LoadBankMemoryView when you want to manage I/O on your side. Load the bank file
		/// in a buffer and pass its address to the sound engine.
		/// In-memory loading is in-place: *** the memory must be valid until the bank is unloaded. ***
		/// A bank load request will be posted, and consumed by the Bank Manager thread.
		/// The function returns when the request has been completely processed.
		/// \return 
		/// The bank ID, which is stored in the first few bytes of the bank file. You may use this 
		/// ID with UnloadBank().
		/// - \c AK_Success: Load or unload successful.
		/// - \c AK_BankAlreadyLoaded: This bank is already loaded, nothing done.
		/// - \c AK_InsufficientMemory: Insufficient memory to store bank data.
		/// - \c AK_BankReadError: I/O error.
		/// - \c AK_WrongBankVersion: Invalid bank version: make sure the version of Wwise that 
		/// you used to generate the SoundBanks matches that of the SDK you are currently using.
		/// - \c AK_InvalidFile: File specified could not be opened.
		/// - \c AK_NotInitialized if the sound engine was not correctly initialized
		/// - \c AK_InvalidParameter if some parameters are invalid, check the debug console 
		/// - \c AK_DataAlignmentError if the data pointer is not aligned properly 
		/// - \c AK_Fail: Load or unload failed for any other reason. (Most likely small allocation failure)
		/// \remarks
		/// - The initialization bank (Init.bnk) must be loaded first.
		/// - All SoundBanks subsequently loaded must come from the same Wwise project as the
		///   initialization bank. If you need to load SoundBanks from a different project, you
		///   must first unload ALL banks, including the initialization bank, then load the
		///   initialization bank from the other project, and finally load banks from that project.
		/// - Codecs and plug-ins must be registered before loading banks that use them.
		/// - Loading a bank referencing an unregistered plug-in or codec will result in a load bank success,
		/// but the plug-ins will not be used. More specifically, playing a sound that uses an unregistered effect plug-in 
		/// will result in audio playback without applying the said effect. If an unregistered source plug-in is used by an event's audio objects, 
		/// posting the event will fail.
		/// - The memory must be aligned on platform-specific AK_BANK_PLATFORM_DATA_ALIGNMENT bytes (see AkTypes.h).
		/// - Avoid using this function for banks containing a lot of events or structure data: this data will be unpacked into the sound engine heap,
		///   making the supplied bank memory redundant. For event/structure-only banks, prefer LoadBankMemoryCopy().
		/// \sa 
		/// - <tt>AK::SoundEngine::UnloadBank()</tt>
		/// - <tt>AK::SoundEngine::ClearBanks()</tt>
		/// - \ref soundengine_banks
		/// - \ref integrating_elements_plugins
		/// - \ref sdk_bank_training
		extern   AKRESULT __cdecl  LoadBankMemoryView  (
	        const void *		in_pInMemoryBankPtr,	///< Pointer to the in-memory bank to load (pointer is stored in sound engine, memory must remain valid)
			AkUInt32			in_uInMemoryBankSize,	///< Size of the in-memory bank to load
            AkBankID &          out_bankID,				///< Returned bank ID
			AkBankType &		out_bankType			///< Returned bank type
			);

		/// Loads a bank synchronously (from in-memory data, out-of-place, user bank only).\n
		///
		/// NOTE: Banks loaded from in-memory with out-of-place data must be unloaded using the standard UnloadBank function
		/// (with no memory pointer). Make sure you are using the correct UnloadBank(...) overload
		///
		/// Use LoadBankMemoryCopy when you want to manage I/O on your side. Load the bank file
		/// in a buffer and pass its address to the sound engine, the media section of the bank will be copied into newly 
		/// allocated memory.  
		/// In-memory loading is out-of-place: the buffer can be release as soon as the function returns. The advantage of using this
		/// over the in-place version is that there is no duplication of bank structures.
		/// A bank load request will be posted, and consumed by the Bank Manager thread.
		/// The function returns when the request has been completely processed.
		/// \return 
		/// The bank ID, which is stored in the first few bytes of the bank file. You may use this 
		/// ID with UnloadBank().
		/// - \c AK_Success: Load or unload successful.
		/// - \c AK_BankAlreadyLoaded: This bank is already loaded, nothing done.
		/// - \c AK_InsufficientMemory: Insufficient memory to store bank data.
		/// - \c AK_BankReadError: I/O error.
		/// - \c AK_WrongBankVersion: Invalid bank version: make sure the version of Wwise that 
		/// you used to generate the SoundBanks matches that of the SDK you are currently using.
		/// - \c AK_InvalidFile: File specified could not be opened.
		/// - \c AK_NotInitialized if the sound engine was not correctly initialized
		/// - \c AK_InvalidParameter if some parameters are invalid, check the debug console
		/// - \c AK_InvalidBankType if the bank is not a user-defined bank.
		/// - \c AK_DataAlignmentError if the data pointer is not aligned properly 
		/// - \c AK_Fail: Load or unload failed for any other reason. (Most likely small allocation failure)
		/// \remarks
		/// - The initialization bank (Init.bnk) must be loaded first.
		/// - All SoundBanks subsequently loaded must come from the same Wwise project as the
		///   initialization bank. If you need to load SoundBanks from a different project, you
		///   must first unload ALL banks, including the initialization bank, then load the
		///   initialization bank from the other project, and finally load banks from that project.
		/// - Codecs and plug-ins must be registered before loading banks that use them.
		/// - Loading a bank referencing an unregistered plug-in or codec will result in a load bank success,
		/// but the plug-ins will not be used. More specifically, playing a sound that uses an unregistered effect plug-in 
		/// will result in audio playback without applying the said effect. If an unregistered source plug-in is used by an event's audio objects, 
		/// posting the event will fail.
		/// \sa 
		/// - <tt>AK::SoundEngine::UnloadBank()</tt>
		/// - <tt>AK::SoundEngine::ClearBanks()</tt>
		/// - \ref soundengine_banks
		/// - \ref integrating_elements_plugins
		/// - \ref sdk_bank_training
		extern   AKRESULT __cdecl  LoadBankMemoryCopy  (
			const void *		in_pInMemoryBankPtr,	///< Pointer to the in-memory bank to load (pointer is not stored in sound engine, memory can be released after return)
			AkUInt32			in_uInMemoryBankSize,	///< Size of the in-memory bank to load
			AkBankID &          out_bankID				///< Returned bank ID
			);

		/// Loads a bank synchronously (from in-memory data, out-of-place, any bank type).\n
		///
		/// NOTE: Banks loaded from in-memory with out-of-place data must be unloaded using the standard UnloadBank function
		/// (with no memory pointer). Make sure you are using the correct UnloadBank(...) overload
		///
		/// Use LoadBankMemoryCopy when you want to manage I/O on your side. Load the bank file
		/// in a buffer and pass its address to the sound engine, the media section of the bank will be copied into newly 
		/// allocated memory.  
		/// In-memory loading is out-of-place: the buffer can be release as soon as the function returns. The advantage of using this
		/// over the in-place version is that there is no duplication of bank structures.
		/// A bank load request will be posted, and consumed by the Bank Manager thread.
		/// The function returns when the request has been completely processed.
		/// \return 
		/// The bank ID, which is stored in the first few bytes of the bank file. You may use this 
		/// ID with UnloadBank().
		/// - \c AK_Success: Load or unload successful.
		/// - \c AK_BankAlreadyLoaded: This bank is already loaded, nothing done.
		/// - \c AK_InsufficientMemory: Insufficient memory to store bank data.
		/// - \c AK_BankReadError: I/O error.
		/// - \c AK_WrongBankVersion: Invalid bank version: make sure the version of Wwise that 
		/// you used to generate the SoundBanks matches that of the SDK you are currently using.
		/// - \c AK_InvalidFile: File specified could not be opened.
		/// - \c AK_NotInitialized if the sound engine was not correctly initialized
		/// - \c AK_InvalidParameter if some parameters are invalid, check the debug console
		/// - \c AK_DataAlignmentError if the data pointer is not aligned properly 
		/// - \c AK_Fail: Load or unload failed for any other reason. (Most likely small allocation failure)
		/// \remarks
		/// - The initialization bank (Init.bnk) must be loaded first.
		/// - All SoundBanks subsequently loaded must come from the same Wwise project as the
		///   initialization bank. If you need to load SoundBanks from a different project, you
		///   must first unload ALL banks, including the initialization bank, then load the
		///   initialization bank from the other project, and finally load banks from that project.
		/// - Codecs and plug-ins must be registered before loading banks that use them.
		/// - Loading a bank referencing an unregistered plug-in or codec will result in a load bank success,
		/// but the plug-ins will not be used. More specifically, playing a sound that uses an unregistered effect plug-in 
		/// will result in audio playback without applying the said effect. If an unregistered source plug-in is used by an event's audio objects, 
		/// posting the event will fail.
		/// \sa 
		/// - <tt>AK::SoundEngine::UnloadBank()</tt>
		/// - <tt>AK::SoundEngine::ClearBanks()</tt>
		/// - \ref soundengine_banks
		/// - \ref integrating_elements_plugins
		/// - \ref sdk_bank_training
		extern   AKRESULT __cdecl  LoadBankMemoryCopy  (
			const void *		in_pInMemoryBankPtr,	///< Pointer to the in-memory bank to load (pointer is not stored in sound engine, memory can be released after return)
			AkUInt32			in_uInMemoryBankSize,	///< Size of the in-memory bank to load
			AkBankID &          out_bankID,				///< Returned bank ID
			AkBankType &		out_bankType			///< Returned bank type
			);

		/// Synchronously decodes Vorbis-encoded and Opus-encoded (Software version) media in a SoundBank. The file should already be read in memory before the decode operation. The out_pDecodedBankPtr can then be used with variants of LoadBank that load from in-memory data.
		/// \n
		/// CPU usage, RAM size, storage size and Internet bandwidth must be accounted for when developing a game, especially when it is aimed at mobile platforms. The DecodeBank function makes it possible to decode media at load time instead of decoding them every time they are played.
		extern   AKRESULT __cdecl  DecodeBank  (
			const void *		in_pInMemoryBankPtr,	///< Pointer to the in-memory bank to decode (pointer is not stored in sound engine, memory can be released after return)
			AkUInt32			in_uInMemoryBankSize,	///< Size of the in-memory bank to decode
			AkMemPoolId			in_uPoolForDecodedBank,	///< Memory pool to allocate decoded bank into. Specify AK_INVALID_POOL_ID and out_pDecodedBankPtr=NULL to obtain decoded bank size without performing the decode operation. Pass AK_INVALID_POOL_ID and out_pDecodedBankPtr!=NULL to decode bank into specified pointer.
			void * &			out_pDecodedBankPtr,	///< Decoded bank memory location.
			AkUInt32 &			out_uDecodedBankSize	///< Decoded bank memory size.
			);

#ifdef AK_SUPPORT_WCHAR
        /// Loads a bank asynchronously (by Unicode string).\n
		/// The bank name is passed to the Stream Manager.
		/// See \ref soundengine_banks_general for a discussion on using strings and IDs.
		/// A bank load request will be posted to the Bank Manager consumer thread.
		/// The function returns immediately.
		/// \return 
		/// - \c AK_Success if the scheduling was successful,
		/// - \c AK_InvalidBankType if in_bankType was invalid
		/// Use a callback to be notified when completed, and get the status of the request.
		/// The bank ID, which is obtained by hashing the bank name (see GetIDFromString()). 
		/// You may use this ID with UnloadBank().
		/// \remarks
		/// - The initialization bank (Init.bnk) must be loaded first.
		/// - All SoundBanks subsequently loaded must come from the same Wwise project as the
		///   initialization bank. If you need to load SoundBanks from a different project, you
		///   must first unload ALL banks, including the initialization bank, then load the
		///   initialization bank from the other project, and finally load banks from that project.
		/// - Codecs and plug-ins must be registered before loading banks that use them.
		/// - Loading a bank referencing an unregistered plug-in or codec will result in a load bank success,
		/// but the plug-ins will not be used. More specifically, playing a sound that uses an unregistered effect plug-in 
		/// will result in audio playback without applying the said effect. If an unregistered source plug-in is used by an event's audio objects, 
		/// posting the event will fail.
		/// - The sound engine internally calls GetIDFromString(in_pszString) to return the correct bank ID.
		/// Therefore, \c in_pszString should be the real name of the SoundBank (with or without the BNK extension - it is trimmed internally),
		/// not the name of the file (if you changed it), nor the full path of the file. The path should be resolved in 
		/// your implementation of the Stream Manager (<tt>AK::IAkStreamMgr::CreateStd()</tt>), or in the Low-Level I/O module 
		/// (<tt>AK::StreamMgr::IAkLowLevelIOHook::BatchOpen()</tt>) if you use the default Stream Manager's implementation.
		/// - The cookie (in_pCookie) is passed to the Low-Level I/O module for your convenience, in <tt>AK::StreamMgr::IAkLowLevelIOHook::BatchOpen()</tt> 
		/// as AkFileSystemFlags::pCustomParam.
		/// \aknote
		/// When Wwise Authoring is connected to the game in "Profile And Edit (Sync All)" mode, LoadBank will succeed even if the file is not found.
		/// This might be the case when sound banks were not generated, for example. The Capture Log will still report the missing file. 
		/// This allows working without sound banks, as long as the Wwise Authoring is connected. 
		/// The Init.bnk is still required to initialize the sound engine properly though.
		/// \endaknote
		/// \sa 
		/// - <tt>AK::SoundEngine::UnloadBank()</tt>
		/// - <tt>AK::SoundEngine::ClearBanks()</tt>
		/// - AkBankCallbackFunc
		/// - \ref soundengine_banks
		/// - \ref integrating_elements_plugins
		/// - \ref streamingdevicemanager
		/// - \ref streamingmanager_lowlevel
		/// - \ref sdk_bank_training
		extern   AKRESULT __cdecl  LoadBank  (
	        const wchar_t*      in_pszString,					///< Name/path of the bank to load
			AkBankCallbackFunc  in_pfnBankCallback,				///< Callback function
			void *              in_pCookie,						///< Callback cookie (reserved to user, passed to the callback function, and also to  <tt>AK::StreamMgr::IAkLowLevelIOHook::BatchOpen()</tt> as AkFileSystemFlags::pCustomParam)
			AkBankID &          out_bankID,						///< Returned bank ID
			AkBankType			in_bankType = AkBankType_User	///< Type of the bank to load
	        );
#endif //AK_SUPPORT_WCHAR

        /// Loads a bank asynchronously.\n
		/// The bank name is passed to the Stream Manager.
		/// See \ref soundengine_banks_general for a discussion on using strings and IDs.
		/// A bank load request will be posted to the Bank Manager consumer thread.
		/// The function returns immediately.
		/// \return 
		/// - \c AK_Success if the scheduling was successful,
		/// - \c AK_InvalidBankType if in_bankType was invalid
		/// Use a callback to be notified when completed, and get the status of the request.
		/// The bank ID, which is obtained by hashing the bank name (see GetIDFromString()). 
		/// You may use this ID with UnloadBank().
		/// \remarks
		/// - The initialization bank (Init.bnk) must be loaded first.
		/// - All SoundBanks subsequently loaded must come from the same Wwise project as the
		///   initialization bank. If you need to load SoundBanks from a different project, you
		///   must first unload ALL banks, including the initialization bank, then load the
		///   initialization bank from the other project, and finally load banks from that project.
		/// - Codecs and plug-ins must be registered before loading banks that use them.
		/// - Loading a bank referencing an unregistered plug-in or codec will result in a load bank success,
		/// but the plug-ins will not be used. More specifically, playing a sound that uses an unregistered effect plug-in 
		/// will result in audio playback without applying the said effect. If an unregistered source plug-in is used by an event's audio objects, 
		/// posting the Event will fail.
		/// - The sound engine internally calls GetIDFromString(in_pszString) to return the correct bank ID.
		/// Therefore, \c in_pszString should be the real name of the SoundBank (with or without the BNK extension - it is trimmed internally),
		/// not the name of the file (if you changed it), nor the full path of the file. The path should be resolved in 
		/// your implementation of the Stream Manager (<tt>AK::IAkStreamMgr::CreateStd()</tt>), or in the Low-Level I/O module 
		/// (<tt>AK::StreamMgr::IAkLowLevelIOHook::BatchOpen()</tt>) if you use the default Stream Manager's implementation.
		/// - The cookie (in_pCookie) is passed to the Low-Level I/O module for your convenience, in <tt>AK::StreamMgr::IAkLowLevelIOHook::BatchOpen()</tt> 
		/// as <tt>AkFileSystemFlags::pCustomParam</tt>.
		/// \aknote
		/// When Wwise Authoring is connected to the game in "Profile And Edit (Sync All)" mode, LoadBank will succeed even if the file is not found.
		/// This might be the case when sound banks were not generated, for example. The Capture Log will still report the missing file. 
		/// This allows working without sound banks, as long as Wwise Authoring is connected. 
		/// The Init.bnk is still required to initialize the sound engine properly though.
		/// \endaknote
		/// \sa 
		/// - <tt>AK::SoundEngine::UnloadBank()</tt>
		/// - <tt>AK::SoundEngine::ClearBanks()</tt>
		/// - AkBankCallbackFunc
		/// - \ref soundengine_banks
		/// - \ref integrating_elements_plugins
		/// - \ref streamingdevicemanager
		/// - \ref streamingmanager_lowlevel
		/// - \ref sdk_bank_training
		extern   AKRESULT __cdecl  LoadBank  (
	        const char*         in_pszString,					///< Name/path of the bank to load
			AkBankCallbackFunc  in_pfnBankCallback,				///< Callback function
			void *              in_pCookie,						///< Callback cookie (reserved to user, passed to the callback function, and also to  <tt>AK::StreamMgr::IAkLowLevelIOHook::BatchOpen()</tt> as AkFileSystemFlags::pCustomParam)
			AkBankID &          out_bankID,						///< Returned bank ID
			AkBankType			in_bankType = AkBankType_User	///< Type of the bank to load
	        );

        /// Loads a bank asynchronously (by ID).\n
		/// \aknote Requires that the "Use SoundBank names" option be unchecked in the Wwise Project Settings. \endaknote
		/// The bank ID is passed to the Stream Manager.
		/// See \ref soundengine_banks_general for a discussion on using strings and IDs.
		/// A bank load request will be posted to the Bank Manager consumer thread.
		/// The function returns immediately.
		/// \return 
		/// - \c AK_Success if the scheduling was successful,
		/// - \c AK_InvalidBankType if in_bankType was invalid
		/// Use a callback to be notified when completed, and get the status of the request.
		/// The bank ID, which is obtained by hashing the bank name (see GetIDFromString()). 
		/// You may use this ID with \c UnloadBank().
		/// \remarks
		/// - The initialization bank (Init.bnk) must be loaded first.
		/// - All SoundBanks subsequently loaded must come from the same Wwise project as the
		///   initialization bank. If you need to load SoundBanks from a different project, you
		///   must first unload ALL banks, including the initialization bank, then load the
		///   initialization bank from the other project, and finally load banks from that project.
		/// - Codecs and plug-ins must be registered before loading banks that use them.
		/// - Loading a bank referencing an unregistered plug-in or codec will result in a load bank success,
		/// but the plug-ins will not be used. More specifically, playing a sound that uses an unregistered effect plug-in 
		/// will result in audio playback without applying the said effect. If an unregistered source plug-in is used by an event's audio objects, 
		/// posting the event will fail.
		/// - The file path should be resolved in your implementation of the Stream Manager, or in the Low-Level I/O module if 
		/// you use the default Stream Manager's implementation. The ID overload of <tt>AK::IAkStreamMgr::CreateStd()</tt> and <tt>AK::StreamMgr::IAkLowLevelIOHook::BatchOpen()</tt> are called.
		/// - The cookie (in_pCookie) is passed to the Low-Level I/O module for your convenience, in <tt>AK::StreamMgr::IAkLowLevelIOHook::BatchOpen()</tt> 
		/// as AkFileSystemFlags::pCustomParam.
		/// \aknote
		/// When Wwise Authoring is connected to the game in "Profile And Edit (Sync All)" mode, LoadBank will succeed even if the file is not found.
		/// This might be the case when sound banks were not generated, for example. The Capture Log will still report the missing file. 
		/// This allows working without sound banks, as long as Wwise Authoring is connected. 
		/// The Init.bnk is still required to initialize the sound engine properly though.
		/// \endaknote
		/// \sa 
		/// - <tt>AK::SoundEngine::UnloadBank()</tt>
		/// - <tt>AK::SoundEngine::ClearBanks()</tt>
		/// - AkBankCallbackFunc
		/// - \ref soundengine_banks
		/// - \ref integrating_elements_plugins
		/// - \ref sdk_bank_training
		extern   AKRESULT __cdecl  LoadBank  (
	        AkBankID			in_bankID,						///< Bank ID of the bank to load
			AkBankCallbackFunc  in_pfnBankCallback,				///< Callback function
			void *              in_pCookie,						///< Callback cookie (reserved to user, passed to the callback function, and also to  <tt>AK::StreamMgr::IAkLowLevelIOHook::BatchOpen()</tt> as AkFileSystemFlags::pCustomParam)
			AkBankType			in_bankType = AkBankType_User	///< Type of the bank to load
 	        );

		/// Loads a bank asynchronously (from in-memory data, in-place, user bank only).\n
		///
		/// IMPORTANT: Banks loaded from memory with in-place data MUST be unloaded using the UnloadBank function
		/// providing the same memory pointer. Make sure you are using the correct UnloadBank(...) overload
		///
		/// Use LoadBankMemoryView when you want to manage I/O on your side. Load the bank file
		/// in a buffer and pass its address to the sound engine.
		/// In-memory loading is in-place: *** the memory must be valid until the bank is unloaded. ***
		/// A bank load request will be posted to the Bank Manager consumer thread.
		/// The function returns immediately.
		/// \return 
		/// - \c AK_Success if the scheduling was successful,
		/// - \c AK_InvalidBankType if the bank is not a user-defined bank.
		/// - \c AK_DataAlignmentError if the data pointer is not aligned properly
		/// Use a callback to be notified when completed, and get the status of the request.
		/// The bank ID, which is obtained by hashing the bank name (see GetIDFromString()). 
		/// You may use this ID with UnloadBank().
		/// \remarks
		/// - The initialization bank (Init.bnk) must be loaded first.
		/// - All SoundBanks subsequently loaded must come from the same Wwise project as the
		///   initialization bank. If you need to load SoundBanks from a different project, you
		///   must first unload ALL banks, including the initialization bank, then load the
		///   initialization bank from the other project, and finally load banks from that project.
		/// - Codecs and plug-ins must be registered before loading banks that use them.
		/// - Loading a bank referencing an unregistered plug-in or codec will result in a load bank success,
		/// but the plug-ins will not be used. More specifically, playing a sound that uses an unregistered effect plug-in 
		/// will result in audio playback without applying the said effect. If an unregistered source plug-in is used by an event's audio objects, 
		/// posting the event will fail.
		/// - The memory must be aligned on platform-specific AK_BANK_PLATFORM_DATA_ALIGNMENT bytes (see AkTypes.h).
		/// - Avoid using this function for banks containing a lot of events or structure data: this data will be unpacked into the sound engine heap,
		///   making the supplied bank memory redundant. For event/structure-only banks, prefer LoadBankMemoryCopy().
		/// \sa 
		/// - <tt>AK::SoundEngine::UnloadBank()</tt>
		/// - <tt>AK::SoundEngine::ClearBanks()</tt>
		/// - AkBankCallbackFunc
		/// - \ref soundengine_banks
		/// - \ref integrating_elements_plugins
		/// - \ref sdk_bank_training
		extern   AKRESULT __cdecl  LoadBankMemoryView  (
			const void *		in_pInMemoryBankPtr,	///< Pointer to the in-memory bank to load (pointer is stored in sound engine, memory must remain valid)
			AkUInt32			in_uInMemoryBankSize,	///< Size of the in-memory bank to load
			AkBankCallbackFunc  in_pfnBankCallback,	    ///< Callback function
			void *              in_pCookie,				///< Callback cookie
			AkBankID &          out_bankID				///< Returned bank ID
	        );

		/// Loads a bank asynchronously (from in-memory data, in-place, any bank type).\n
		///
		/// IMPORTANT: Banks loaded from memory with in-place data MUST be unloaded using the UnloadBank function
		/// providing the same memory pointer. Make sure you are using the correct UnloadBank(...) overload
		///
		/// Use LoadBankMemoryView when you want to manage I/O on your side. Load the bank file
		/// in a buffer and pass its address to the sound engine.
		/// In-memory loading is in-place: *** the memory must be valid until the bank is unloaded. ***
		/// A bank load request will be posted to the Bank Manager consumer thread.
		/// The function returns immediately.
		/// \return 
		/// - \c AK_Success if the scheduling was successful,
		/// - \c AK_DataAlignmentError if the data pointer is not aligned properly
		/// Use a callback to be notified when completed, and get the status of the request.
		/// The bank ID, which is obtained by hashing the bank name (see GetIDFromString()). 
		/// You may use this ID with UnloadBank().
		/// \remarks
		/// - The initialization bank (Init.bnk) must be loaded first.
		/// - All SoundBanks subsequently loaded must come from the same Wwise project as the
		///   initialization bank. If you need to load SoundBanks from a different project, you
		///   must first unload ALL banks, including the initialization bank, then load the
		///   initialization bank from the other project, and finally load banks from that project.
		/// - Codecs and plug-ins must be registered before loading banks that use them.
		/// - Loading a bank referencing an unregistered plug-in or codec will result in a load bank success,
		/// but the plug-ins will not be used. More specifically, playing a sound that uses an unregistered effect plug-in 
		/// will result in audio playback without applying the said effect. If an unregistered source plug-in is used by an event's audio objects, 
		/// posting the event will fail.
		/// - The memory must be aligned on platform-specific AK_BANK_PLATFORM_DATA_ALIGNMENT bytes (see AkTypes.h).
		/// - Avoid using this function for banks containing a lot of events or structure data: this data will be unpacked into the sound engine heap,
		///   making the supplied bank memory redundant. For event/structure-only banks, prefer LoadBankMemoryCopy().
		/// \sa 
		/// - <tt>AK::SoundEngine::UnloadBank()</tt>
		/// - <tt>AK::SoundEngine::ClearBanks()</tt>
		/// - AkBankCallbackFunc
		/// - \ref soundengine_banks
		/// - \ref integrating_elements_plugins
		/// - \ref sdk_bank_training
		extern   AKRESULT __cdecl  LoadBankMemoryView  (
			const void *		in_pInMemoryBankPtr,	///< Pointer to the in-memory bank to load (pointer is stored in sound engine, memory must remain valid)
			AkUInt32			in_uInMemoryBankSize,	///< Size of the in-memory bank to load
			AkBankCallbackFunc  in_pfnBankCallback,	    ///< Callback function
			void *              in_pCookie,				///< Callback cookie
			AkBankID &          out_bankID,				///< Returned bank ID
			AkBankType &		out_bankType			///< Returned bank type
	        );

		/// Loads a bank asynchronously (from in-memory data, out-of-place, user bank only).\n
		///
		/// NOTE: Banks loaded from in-memory with out-of-place data must be unloaded using the standard UnloadBank function
		/// (with no memory pointer). Make sure you are using the correct UnloadBank(...) overload
		///
		/// Use LoadBankMemoryCopy when you want to manage I/O on your side. Load the bank file
		/// in a buffer and pass its address to the sound engine, the media section of the bank will be copied into newly allocated 
		/// memory.
		/// In-memory loading is out-of-place: the buffer can be released after the callback function is called. The advantage of using this
		/// over the in-place version is that there is no duplication of bank structures.
		/// A bank load request will be posted to the Bank Manager consumer thread.
		/// The function returns immediately.
		/// \return 
		/// - \c AK_Success if the scheduling was successful,
		/// - \c AK_InvalidBankType if the bank is not a user-defined bank.
		/// - \c AK_DataAlignmentError if the data pointer is not aligned properly 
		/// Use a callback to be notified when completed, and get the status of the request.
		/// The bank ID, which is obtained by hashing the bank name (see GetIDFromString()). 
		/// You may use this ID with UnloadBank().
		/// \remarks
		/// - The initialization bank (Init.bnk) must be loaded first.
		/// - All SoundBanks subsequently loaded must come from the same Wwise project as the
		///   initialization bank. If you need to load SoundBanks from a different project, you
		///   must first unload ALL banks, including the initialization bank, then load the
		///   initialization bank from the other project, and finally load banks from that project.
		/// - Codecs and plug-ins must be registered before loading banks that use them.
		/// - Loading a bank referencing an unregistered plug-in or codec will result in a load bank success,
		/// but the plug-ins will not be used. More specifically, playing a sound that uses an unregistered effect plug-in 
		/// will result in audio playback without applying the said effect. If an unregistered source plug-in is used by an event's audio objects, 
		/// posting the event will fail.
		/// \sa 
		/// - <tt>AK::SoundEngine::UnloadBank()</tt>
		/// - <tt>AK::SoundEngine::ClearBanks()</tt>
		/// - AkBankCallbackFunc
		/// - \ref soundengine_banks
		/// - \ref integrating_elements_plugins
		/// - \ref sdk_bank_training
		extern  AKRESULT __cdecl  LoadBankMemoryCopy (
			const void*			in_pInMemoryBankPtr,	///< Pointer to the in-memory bank to load (pointer is not stored in sound engine, memory can be released after callback)
			AkUInt32			in_uInMemoryBankSize,	///< Size of the in-memory bank to load
			AkBankCallbackFunc  in_pfnBankCallback,	    ///< Callback function
			void*				in_pCookie,				///< Callback cookie
			AkBankID&			out_bankID				///< Returned bank ID
			);

		/// Loads a bank asynchronously (from in-memory data, out-of-place, any bank type).\n
		///
		/// NOTE: Banks loaded from in-memory with out-of-place data must be unloaded using the standard UnloadBank function
		/// (with no memory pointer). Make sure you are using the correct UnloadBank(...) overload
		///
		/// Use LoadBankMemoryCopy when you want to manage I/O on your side. Load the bank file
		/// in a buffer and pass its address to the sound engine, the media section of the bank will be copied into newly allocated 
		/// memory.
		/// In-memory loading is out-of-place: the buffer can be released after the callback function is called. The advantage of using this
		/// over the in-place version is that there is no duplication of bank structures.
		/// A bank load request will be posted to the Bank Manager consumer thread.
		/// The function returns immediately.
		/// \return 
		/// - \c AK_Success if the scheduling was successful,
		/// - \c AK_InvalidBankType if in_bankType was invalid
		/// - \c AK_DataAlignmentError if the data pointer is not aligned properly 
		/// Use a callback to be notified when completed, and get the status of the request.
		/// The bank ID, which is obtained by hashing the bank name (see GetIDFromString()). 
		/// You may use this ID with UnloadBank().
		/// \remarks
		/// - The initialization bank (Init.bnk) must be loaded first.
		/// - All SoundBanks subsequently loaded must come from the same Wwise project as the
		///   initialization bank. If you need to load SoundBanks from a different project, you
		///   must first unload ALL banks, including the initialization bank, then load the
		///   initialization bank from the other project, and finally load banks from that project.
		/// - Codecs and plug-ins must be registered before loading banks that use them.
		/// - Loading a bank referencing an unregistered plug-in or codec will result in a load bank success,
		/// but the plug-ins will not be used. More specifically, playing a sound that uses an unregistered effect plug-in 
		/// will result in audio playback without applying the said effect. If an unregistered source plug-in is used by an event's audio objects, 
		/// posting the event will fail.
		/// \sa 
		/// - <tt>AK::SoundEngine::UnloadBank()</tt>
		/// - <tt>AK::SoundEngine::ClearBanks()</tt>
		/// - AkBankCallbackFunc
		/// - \ref soundengine_banks
		/// - \ref integrating_elements_plugins
		/// - \ref sdk_bank_training
		extern   AKRESULT __cdecl  LoadBankMemoryCopy  (
			const void *		in_pInMemoryBankPtr,	///< Pointer to the in-memory bank to load (pointer is not stored in sound engine, memory can be released after callback)
			AkUInt32			in_uInMemoryBankSize,	///< Size of the in-memory bank to load
			AkBankCallbackFunc  in_pfnBankCallback,	    ///< Callback function
			void *              in_pCookie,				///< Callback cookie
			AkBankID &          out_bankID,				///< Returned bank ID
			AkBankType &		out_bankType			///< Returned bank type
			);

#ifdef AK_SUPPORT_WCHAR
        /// Unloads a bank synchronously (by Unicode string).\n
		/// See \ref soundengine_banks_general for a discussion on using strings and IDs.
		/// \return AK_Success if successful, AK_Fail otherwise. AK_Success is returned when the bank was not loaded.
		/// \remarks
		/// - The sound engine internally calls GetIDFromString(in_pszString) to retrieve the bank ID, 
		/// then it calls the synchronous version of UnloadBank() by ID.
		/// Therefore, in_pszString should be the real name of the SoundBank (with or without the BNK extension - it is trimmed internally),
		/// not the name of the file (if you changed it), nor the full path of the file. 
		/// - In order to force the memory deallocation of the bank, sounds that use media from this bank will be stopped. 
		/// This means that streamed sounds or generated sounds will not be stopped.
		/// \sa 
		/// - <tt>AK::SoundEngine::LoadBank()</tt>
		/// - <tt>AK::SoundEngine::ClearBanks()</tt>
		/// - \ref soundengine_banks
        extern   AKRESULT __cdecl  UnloadBank  (
	        const wchar_t*      in_pszString,					///< Name of the bank to unload
			const void *		in_pInMemoryBankPtr,			///< Memory pointer from where the bank was initially loaded from. (REQUIRED to determine which bank associated to a memory pointer must be unloaded). Pass NULL if NULL was passed when loading the bank or if LoadBankMemoryCopy was used to load the bank.
			AkBankType			in_bankType = AkBankType_User	///< Type of the bank to unload
	        );
#endif //AK_SUPPORT_WCHAR

        /// Unloads a bank synchronously.\n
		/// See \ref soundengine_banks_general for a discussion on using strings and IDs.
		/// \return AK_Success if successful, AK_Fail otherwise. AK_Success is returned when the bank was not loaded.
		/// \remarks
		/// - The sound engine internally calls GetIDFromString(in_pszString) to retrieve the bank ID, 
		/// then it calls the synchronous version of UnloadBank() by ID.
		/// Therefore, in_pszString should be the real name of the SoundBank (with or without the BNK extension - it is trimmed internally),
		/// not the name of the file (if you changed it), nor the full path of the file. 
		/// - In order to force the memory deallocation of the bank, sounds that use media from this bank will be stopped. 
		/// This means that streamed sounds or generated sounds will not be stopped.
		/// \sa 
		/// - <tt>AK::SoundEngine::LoadBank()</tt>
		/// - <tt>AK::SoundEngine::ClearBanks()</tt>
		/// - \ref soundengine_banks
        extern   AKRESULT __cdecl  UnloadBank  (
	        const char*         in_pszString,					///< Name of the bank to unload
			const void *		in_pInMemoryBankPtr,			///< Memory pointer from where the bank was initially loaded from. (REQUIRED to determine which bank associated to a memory pointer must be unloaded). Pass NULL if NULL was passed when loading the bank or if LoadBankMemoryCopy was used to load the bank.
			AkBankType			in_bankType = AkBankType_User	///< Type of the bank to unload
	        );

		/// Unloads a bank synchronously (by ID and memory pointer).\n
		/// \return AK_Success if successful, AK_Fail otherwise. AK_Success is returned when the bank was not loaded.
		/// \remarks
		/// - In order to force the memory deallocation of the bank, sounds that use media from this bank will be stopped. 
		/// This means that streamed sounds or generated sounds will not be stopped.
		/// \sa 
		/// - <tt>AK::SoundEngine::LoadBank()</tt>
		/// - <tt>AK::SoundEngine::ClearBanks()</tt>
		/// - \ref soundengine_banks
        extern   AKRESULT __cdecl  UnloadBank  (
	        AkBankID            in_bankID,						///< ID of the bank to unload
			const void *		in_pInMemoryBankPtr,			///< Memory pointer from where the bank was initially loaded from. (REQUIRED to determine which bank associated to a memory pointer must be unloaded). Pass NULL if NULL was passed when loading the bank or if LoadBankMemoryCopy was used to load the bank.
			AkBankType			in_bankType = AkBankType_User	///< Type of the bank to unload
 	        );

#ifdef AK_SUPPORT_WCHAR
		/// Unloads a bank asynchronously (by Unicode string).\n
		/// See \ref soundengine_banks_general for a discussion on using strings and IDs.
		/// \return AK_Success if scheduling successful (use a callback to be notified when completed)
		/// \remarks
		/// The sound engine internally calls GetIDFromString(in_pszString) to retrieve the bank ID, 
		/// then it calls the synchronous version of UnloadBank() by ID.
		/// Therefore, in_pszString should be the real name of the SoundBank (with or without the BNK extension - it is trimmed internally),
		/// not the name of the file (if you changed it), nor the full path of the file. 
		/// - In order to force the memory deallocation of the bank, sounds that use media from this bank will be stopped. 
		/// This means that streamed sounds or generated sounds will not be stopped.
		/// \sa 
		/// - <tt>AK::SoundEngine::LoadBank()</tt>
		/// - <tt>AK::SoundEngine::ClearBanks()</tt>
		/// - AkBankCallbackFunc
		/// - \ref soundengine_banks
		extern   AKRESULT __cdecl  UnloadBank  (
	        const wchar_t*      in_pszString,					///< Name of the bank to unload
			const void *		in_pInMemoryBankPtr,			///< Memory pointer from where the bank was initially loaded from. (REQUIRED to determine which bank associated to a memory pointer must be unloaded). Pass NULL if NULL was passed when loading the bank or if LoadBankMemoryCopy was used to load the bank.
			AkBankCallbackFunc  in_pfnBankCallback,				///< Callback function
			void *              in_pCookie,						///< Callback cookie (reserved to user, passed to the callback function)
			AkBankType			in_bankType = AkBankType_User	///< Type of the bank to unload
	        );
#endif //AK_SUPPORT_WCHAR

		/// Unloads a bank asynchronously.\n
		/// See \ref soundengine_banks_general for a discussion on using strings and IDs.
		/// \return AK_Success if scheduling successful (use a callback to be notified when completed)
		/// \remarks
		/// The sound engine internally calls GetIDFromString(in_pszString) to retrieve the bank ID, 
		/// then it calls the synchronous version of UnloadBank() by ID.
		/// Therefore, in_pszString should be the real name of the SoundBank (with or without the BNK extension - it is trimmed internally),
		/// not the name of the file (if you changed it), nor the full path of the file. 
		/// - In order to force the memory deallocation of the bank, sounds that use media from this bank will be stopped. 
		/// This means that streamed sounds or generated sounds will not be stopped.
		/// \sa 
		/// - <tt>AK::SoundEngine::LoadBank()</tt>
		/// - <tt>AK::SoundEngine::ClearBanks()</tt>
		/// - AkBankCallbackFunc
		/// - \ref soundengine_banks
		extern   AKRESULT __cdecl  UnloadBank  (
	        const char*         in_pszString,				    ///< Name of the bank to unload
			const void *		in_pInMemoryBankPtr,			///< Memory pointer from where the bank was initially loaded from. (REQUIRED to determine which bank associated to a memory pointer must be unloaded). Pass NULL if NULL was passed when loading the bank or if LoadBankMemoryCopy was used to load the bank.
			AkBankCallbackFunc  in_pfnBankCallback,				///< Callback function
			void *              in_pCookie,						///< Callback cookie (reserved to user, passed to the callback function)
			AkBankType			in_bankType = AkBankType_User	///< Type of the bank to unload
	        );

		/// Unloads a bank asynchronously (by ID and memory pointer).\n
		/// See \ref soundengine_banks_general for a discussion on using strings and IDs.
		/// \return AK_Success if scheduling successful (use a callback to be notified when completed)
		/// \remarks
		/// - In order to force the memory deallocation of the bank, sounds that use media from this bank will be stopped. 
		/// This means that streamed sounds or generated sounds will not be stopped.
		/// \sa 
		/// - <tt>AK::SoundEngine::LoadBank()</tt>
		/// - <tt>AK::SoundEngine::ClearBanks()</tt>
		/// - AkBankCallbackFunc
		/// - \ref soundengine_banks
		extern   AKRESULT __cdecl  UnloadBank  (
	        AkBankID            in_bankID,						///< ID of the bank to unload
			const void *		in_pInMemoryBankPtr,			///< Memory pointer from where the bank was initially loaded from. (REQUIRED to determine which bank associated to a memory pointer must be unloaded). Pass NULL if NULL was passed when loading the bank or if LoadBankMemoryCopy was used to load the bank.
			AkBankCallbackFunc  in_pfnBankCallback,				///< Callback function
			void *              in_pCookie,						///< Callback cookie (reserved to user, passed to the callback function)
			AkBankType			in_bankType = AkBankType_User	///< Type of the bank to unload
	        );

		/// Cancels all Event callbacks associated with a specific callback cookie specified while loading Banks of preparing Events.\n
		/// \sa 
		/// - <tt>AK::SoundEngine::LoadBank()</tt>
		/// - <tt>AK::SoundEngine::PrepareEvent()</tt>
		/// - <tt>AK::SoundEngine::UnloadBank()</tt>
		/// - <tt>AK::SoundEngine::ClearBanks()</tt>
		/// - AkBankCallbackFunc
		extern   void __cdecl  CancelBankCallbackCookie  ( 
			void * in_pCookie 							///< Callback cookie to be canceled
			);

		/// Preparation type.
		/// \sa
		/// - <tt>AK::SoundEngine::PrepareEvent()</tt>
		/// - <tt>AK::SoundEngine::PrepareGameSyncs()</tt>
		/// - <tt>AK::SoundEngine::PrepareBank()</tt>
		enum PreparationType
		{
			Preparation_Load,	///< \c PrepareEvent() will load required information to play the specified event.
			Preparation_Unload,	///< \c PrepareEvent() will unload required information to play the specified event.
			Preparation_LoadAndDecode, ///< Vorbis media is decoded when loading, and an uncompressed PCM version is used for playback.
			Preparation_Last,	///< End of enum, invalid value.
		};

		/// Parameter to be passed to <tt>AK::SoundEngine::PrepareBank()</tt>.
		/// Use AkBankContent_All to load both the media and structural content from the bank. 
		/// Use AkBankContent_StructureOnly to load only the structural content, including events, from the bank and then later use the PrepareEvent() functions to load media on demand from loose files on the disk.
		/// \sa 
		/// - <tt>AK::SoundEngine::PrepareBank()</tt>
		/// - \ref soundengine_banks_preparingbanks
		enum AkBankContent
		{
			AkBankContent_StructureOnly,	///< Use AkBankContent_StructureOnly to load only the structural content, including Events, and then later use the PrepareEvent() functions to load media on demand from loose files on the disk.
			AkBankContent_All,				///< Use AkBankContent_All to load both the media and structural content.
			AkBankContent_Last				///< End of enum, invalid value.
		};

#ifdef AK_SUPPORT_WCHAR
		/// This function will load the Events, structural content, and optionally, the media content from the SoundBank. If the flag AkBankContent_All is specified, PrepareBank() will load the media content from 
		/// the bank; but, as opposed to LoadBank(), the media will be loaded one media item at a time instead of in one contiguous memory block. Using PrepareBank(), alone or in combination with PrepareEvent(), 
		/// will prevent in-memory duplication of media at the cost of some memory fragmentation.
		/// Calling this function specifying the flag AkBankContent_StructureOnly will load only the structural part (including events) of this bank, 
		/// allowing using PrepareEvent() to load media on demand.  
		/// \sa 
		/// - \ref soundengine_banks_preparingbanks
		/// - <tt>AK::SoundEngine::LoadBank()</tt>
		/// - <tt>AK::SoundEngine::PreparationType</tt>
		/// \remarks
		/// \c PrepareBank(), when called with the flag \c AkBankContent_StructureOnly, requires additional calls to \c PrepareEvent() to load the media for each event. \c PrepareEvent(), however, is unable to 
		///		access media content contained within SoundBanks and requires that the media be available as loose files in the file system. This flag may be useful to implement multiple loading configurations;
		///	for example, a game may have a tool mode that uses \c PrepareEvent() to load loose files on-demand and, also, a game mode that uses \c LoadBank() to load the bank in entirety.
		extern   AKRESULT __cdecl  PrepareBank  (
			AK::SoundEngine::PreparationType	in_PreparationType,			///< Preparation type ( Preparation_Load or Preparation_Unload )
			const wchar_t*						in_pszString,				///< Name of the bank to Prepare/Unprepare.
			AK::SoundEngine::AkBankContent	in_uFlags = AkBankContent_All,	///< Structures only (including events) or all content.
			AkBankType						in_bankType = AkBankType_User	///< Type of the bank to Prepare/Unprepare.
			);
#endif //AK_SUPPORT_WCHAR

		/// This function will load the Events, structural content, and optionally, the media content from the SoundBank. If the flag AkBankContent_All is specified, PrepareBank() will load the media content from 
		/// the bank; but, as opposed to LoadBank(), the media will be loaded one media item at a time instead of in one contiguous memory block. Using PrepareBank(), alone or in combination with PrepareEvent(), 
		/// will prevent in-memory duplication of media at the cost of some memory fragmentation.
		/// Calling this function specifying the flag AkBankContent_StructureOnly will load only the structural part (including events) of this bank, 
		/// allowing using PrepareEvent() to load media on demand.  
		/// \sa 
		/// - \ref soundengine_banks_preparingbanks
		/// - <tt>AK::SoundEngine::LoadBank()</tt>
		/// - <tt>AK::SoundEngine::PreparationType</tt>
		/// \remarks
		/// \c PrepareBank(), when called with the flag \c AkBankContent_StructureOnly, requires additional calls to \c PrepareEvent() to load the media for each event. \c PrepareEvent(), however, is unable to 
		///		access media content contained within SoundBanks and requires that the media be available as loose files in the file system. This flag may be useful to implement multiple loading configurations;
		///		for example, a game may have a tool mode that uses \c PrepareEvent() to load loose files on-demand and, also, a game mode that uses \c LoadBank() to load the bank in entirety.
		extern   AKRESULT __cdecl  PrepareBank  (
			AK::SoundEngine::PreparationType	in_PreparationType,			///< Preparation type ( Preparation_Load or Preparation_Unload )
			const char*							in_pszString,				///< Name of the bank to Prepare/Unprepare.
			AK::SoundEngine::AkBankContent	in_uFlags = AkBankContent_All,	///< Structures only (including events) or all content.
			AkBankType						in_bankType = AkBankType_User	///< Type of the bank to Prepare/Unprepare.
			);

		/// \n\aknote Requires that the "Use SoundBank names" option be unchecked in the Wwise Project Settings. \endaknote
		/// This function will load the events, structural content, and optionally, the media content from the SoundBank. If the flag AkBankContent_All is specified, PrepareBank() will load the media content from 
		/// the bank, but as opposed to LoadBank(), the media will be loaded one media item at a time instead of in one contiguous memory block. Using PrepareBank(), alone or in combination with PrepareEvent(), 
		/// will prevent in-memory duplication of media at the cost of some memory fragmentation.
		/// Calling this function specifying the flag AkBankContent_StructureOnly will load only the structural part (including events) of this bank, 
		/// allowing using PrepareEvent() to load media on demand.  
		/// \sa 
		/// - \ref soundengine_banks_preparingbanks
		/// - <tt>AK::SoundEngine::LoadBank()</tt>
		/// - <tt>AK::SoundEngine::PreparationType</tt>
		/// \remarks
		/// \c PrepareBank(), when called with the flag AkBankContent_StructureOnly, requires additional calls to PrepareEvent() to load the media for each event. PrepareEvent(), however, is unable to 
		///		access media content contained within SoundBanks and requires that the media be available as loose files in the file system. This flag may be useful to implement multiple loading configurations;
		///		for example, a game may have a tool mode that uses PrepareEvent() to load loose files on-demand and, also, a game mode that uses LoadBank() to load the bank in entirety.
		extern   AKRESULT __cdecl  PrepareBank  (
			AK::SoundEngine::PreparationType	in_PreparationType,				///< Preparation type ( Preparation_Load or Preparation_Unload )
			AkBankID							in_bankID,						///< ID of the bank to Prepare/Unprepare.
			AK::SoundEngine::AkBankContent		in_uFlags = AkBankContent_All,	///< Structures only (including events) or all content.
			AkBankType							in_bankType = AkBankType_User	///< Type of the bank to Prepare/Unprepare.
			);

#ifdef AK_SUPPORT_WCHAR
		/// This function will load the Events, structural content, and optionally, the media content from the SoundBank. If the flag \c AkBankContent_All is specified, \c PrepareBank() will load the media content from 
		/// the bank, but as opposed to \c LoadBank(), the media will be loaded one media item at a time instead of in one contiguous memory block. Using \c PrepareBank(), alone or in combination with \c PrepareEvent(), 
		/// will prevent in-memory duplication of media at the cost of some memory fragmentation.
		/// Calling this function specifying the flag \c AkBankContent_StructureOnly will load only the structural part (including events) of this bank, 
		/// allowing using \c PrepareEvent() to load media on demand.  
		/// \sa 
		/// - \ref soundengine_banks_preparingbanks
		/// - <tt>AK::SoundEngine::LoadBank()</tt>
		/// - <tt>AK::SoundEngine::PreparationType</tt>
		/// \remarks
		/// \c PrepareBank(), when called with the flag \c AkBankContent_StructureOnly, requires additional calls to \c PrepareEvent() to load the media for each event. \c PrepareEvent(), however, is unable to 
		///		access media content contained within SoundBanks and requires that the media be available as loose files in the file system. This flag may be useful to implement multiple loading configurations;
		///		for example, a game may have a tool mode that uses \c PrepareEvent() to load loose files on-demand and, also, a game mode that uses \c LoadBank() to load the bank in entirety.
		extern   AKRESULT __cdecl  PrepareBank  (
			AK::SoundEngine::PreparationType	in_PreparationType,			///< Preparation type ( Preparation_Load or Preparation_Unload )
			const wchar_t*		in_pszString,								///< Name of the bank to Prepare/Unprepare.
			AkBankCallbackFunc	in_pfnBankCallback,							///< Callback function
			void *              in_pCookie,									///< Callback cookie (reserved to user, passed to the callback function)
			AK::SoundEngine::AkBankContent	in_uFlags = AkBankContent_All,	///< Structures only (including events) or all content.
			AkBankType						in_bankType = AkBankType_User	///< Type of the bank to Prepare/Unprepare.
			);
#endif //AK_SUPPORT_WCHAR

		/// This function will load the events, structural content, and optionally, the media content from the SoundBank. If the flag \c AkBankContent_All is specified, \c PrepareBank() will load the media content from 
		/// the bank, but as opposed to \c LoadBank(), the media will be loaded one media item at a time instead of in one contiguous memory block. Using \c PrepareBank(), alone or in combination with PrepareEvent(), 
		/// will prevent in-memory duplication of media at the cost of some memory fragmentation.
		/// Calling this function specifying the flag AkBankContent_StructureOnly will load only the structural part (including events) of this bank, 
		/// allowing using PrepareEvent() to load media on demand.  
		/// \sa 
		/// - \ref soundengine_banks_preparingbanks
		/// - <tt>AK::SoundEngine::LoadBank()</tt>
		/// - <tt>AK::SoundEngine::PreparationType()</tt>
		/// \remarks
		/// PrepareBank(), when called with the flag AkBankContent_StructureOnly, requires additional calls to PrepareEvent() to load the media for each event. PrepareEvent(), however, is unable to 
		///		access media content contained within SoundBanks and requires that the media be available as loose files in the file system. This flag may be useful to implement multiple loading configurations;
		///		for example, a game may have a tool mode that uses PrepareEvent() to load loose files on-demand and, also, a game mode that uses LoadBank() to load the bank in entirety.
		extern   AKRESULT __cdecl  PrepareBank  (
			AK::SoundEngine::PreparationType	in_PreparationType,			///< Preparation type ( Preparation_Load or Preparation_Unload )
			const char*			in_pszString,								///< Name of the bank to Prepare/Unprepare.
			AkBankCallbackFunc	in_pfnBankCallback,							///< Callback function
			void *              in_pCookie,									///< Callback cookie (reserved to user, passed to the callback function)
			AK::SoundEngine::AkBankContent	in_uFlags = AkBankContent_All,	///< Structures only (including events) or all content.
			AkBankType						in_bankType = AkBankType_User	///< Type of the bank to Prepare/Unprepare.
			);

		/// \n\aknote Requires that the "Use SoundBank names" option be unchecked in the Wwise Project Settings. \endaknote
		/// This function will load the events, structural content, and optionally, the media content from the SoundBank. If the flag AkBankContent_All is specified, \c PrepareBank() will load the media content from 
		/// the bank, but as opposed to \c LoadBank(), the media will be loaded one media item at a time instead of in one contiguous memory block. Using \c PrepareBank(), alone or in combination with \c PrepareEvent(), 
		/// will prevent in-memory duplication of media at the cost of some memory fragmentation.
		/// Calling this function specifying the flag AkBankContent_StructureOnly will load only the structural part (including events) of this bank, 
		/// allowing using PrepareEvent() to load media on demand.  
		/// \sa 
		/// - \ref soundengine_banks_preparingbanks
		/// - <tt>AK::SoundEngine::LoadBank()</tt>
		/// - <tt>AK::SoundEngine::PreparationType()</tt>
		/// \remarks
		/// \c PrepareBank(), when called with the flag AkBankContent_StructureOnly, requires additional calls to PrepareEvent() to load the media for each event. \c PrepareEvent(), however, is unable to 
		///		access media content contained within SoundBanks and requires that the media be available as loose files in the file system. This flag may be useful to implement multiple loading configurations;
		///		for example, a game may have a tool mode that uses \c PrepareEvent() to load loose files on-demand and, also, a game mode that uses \c LoadBank() to load the bank in entirety.
		extern   AKRESULT __cdecl  PrepareBank  (
			AK::SoundEngine::PreparationType		in_PreparationType,			///< Preparation type ( Preparation_Load or Preparation_Unload )
			AkBankID			in_bankID,						///< ID of the bank to Prepare/Unprepare.
			AkBankCallbackFunc	in_pfnBankCallback,				///< Callback function
			void *              in_pCookie,						///< Callback cookie (reserved to user, passed to the callback function)
			AK::SoundEngine::AkBankContent	in_uFlags = AkBankContent_All,	///< Structures only (including events) or all content.
			AkBankType						in_bankType = AkBankType_User	///< Type of the bank to Prepare/Unprepare.
			);
		
		/// Clear all previously prepared events.\n
		/// \return
		/// - \c AK_Success if successful.
		/// - \c AK_Fail if the sound engine was not correctly initialized or if there is not enough memory to handle the command.
		/// \remarks
		/// The function \c ClearBanks() also clears all prepared events.
		/// \sa
		/// - <tt>AK::SoundEngine::PrepareEvent()</tt>
		/// - <tt>AK::SoundEngine::ClearBanks()</tt>
		extern   AKRESULT __cdecl  ClearPreparedEvents  ();

#ifdef AK_SUPPORT_WCHAR
		/// Prepares or unprepares Events synchronously (by Unicode string).\n
		/// The Events are identified by strings, and converted to IDs internally
		/// (see \ref soundengine_banks_general for a discussion on using strings and IDs).
		/// Before invoking \c PrepareEvent(), use \c LoadBank() to explicitly load the SoundBank(s) 
		/// that contain the Events and structures. When a request is posted to the
		/// Bank Manager consumer thread, it will resolve all dependencies needed to 
		/// successfully post the specified Events and load the required loose media files. 
		/// \aknote Before version 2015.1, the required media files could be included
		/// in a separate media SoundBank. As described in \ref whatsnew_2015_1_migration,
		/// however,\c  \c PrepareEvent() now only looks for loose media files.
		/// \endaknote
		/// The function returns when the request is completely processed.
		/// \return 
		/// - \c AK_Success: Prepare/un-prepare successful.
		/// - \c AK_IDNotFound: At least one of the event/game sync identifiers passed to PrepareEvent() does not exist.
		/// - \c AK_InsufficientMemory: Insufficient memory to store bank data.
		/// - \c AK_BankReadError: I/O error.
		/// - \c AK_WrongBankVersion: Invalid bank version: make sure the version of Wwise that 
		/// you used to generate the SoundBanks matches that of the SDK you are currently using.
		/// - \c AK_InvalidFile: File specified could not be opened.
		/// - \c AK_InvalidParameter: Invalid parameter, invalid memory alignment.		
		/// - \c AK_Fail: Load or unload failed for any other reason. (Most likely small allocation failure)
		/// \remarks
		/// Whenever at least one event fails to be resolved, the actions performed for all 
		/// other events are cancelled.
		/// \aknote  The use of \c PrepareEvent() is incompatible with \c LoadBank(), using in-memory data.
		/// \endaknote
		/// \sa
		/// - <tt>AK::SoundEngine::PrepareEvent()</tt>
		/// - <tt>AK::SoundEngine::ClearPreparedEvents()</tt>
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
		/// - <tt>AK::SoundEngine::LoadBank()</tt>
		/// - \ref soundengine_banks
		/// - \ref sdk_bank_training
		extern   AKRESULT __cdecl  PrepareEvent  ( 
			PreparationType		in_PreparationType,		///< Preparation type ( Preparation_Load or Preparation_Unload )
			const wchar_t**		in_ppszString,			///< Array of event names
			AkUInt32			in_uNumEvent			///< Number of events in the array
			);
#endif //AK_SUPPORT_WCHAR

		/// Prepares or unprepares events synchronously.\n
		/// The Events are identified by strings and converted to IDs internally
		/// (see \ref soundengine_banks_general for a discussion on using strings and IDs).
		/// Before invoking \c PrepareEvent(), use \c LoadBank() to explicitly load the SoundBank(s) 
		/// that contain the Events and structures. When a request is posted to the
		/// Bank Manager consumer thread, it will resolve all dependencies needed to 
		/// successfully post the specified Events and load the required loose media files. 
		/// \aknote Before version 2015.1, the required media files could be included
		/// in a separate media SoundBank. As described in \ref whatsnew_2015_1_migration,
		/// however, \c PrepareEvent() now only looks for loose media files.
		/// \endaknote
		/// The function returns when the request is completely processed.
		/// \return 
		/// - \c AK_Success: Prepare/un-prepare successful.
		/// - \c AK_IDNotFound: At least one of the event/game sync identifiers passed to PrepareEvent() does not exist.
		/// - \c AK_InsufficientMemory: Insufficient memory to store bank data.
		/// - \c AK_BankReadError: I/O error.
		/// - \c AK_WrongBankVersion: Invalid bank version: make sure the version of Wwise that 
		/// you used to generate the SoundBanks matches that of the SDK you are currently using.
		/// - \c AK_InvalidFile: File specified could not be opened.
		/// - \c AK_InvalidParameter: Invalid parameter, invalid memory alignment.		
		/// - \c AK_Fail: Load or unload failed for any other reason. (Most likely small allocation failure)
		/// \remarks
		/// Whenever at least one event fails to be resolved, the actions performed for all 
		/// other events are cancelled.
		/// \aknote  The use of \c PrepareEvent() is incompatible with \c LoadBank(), using in-memory data.
		/// \endaknote
		/// \sa
		/// - <tt>AK::SoundEngine::PrepareEvent()</tt>
		/// - <tt>AK::SoundEngine::ClearPreparedEvents()</tt>
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
		/// - <tt>AK::SoundEngine::LoadBank()</tt>
		/// - \ref soundengine_banks
		/// - \ref sdk_bank_training
		extern   AKRESULT __cdecl  PrepareEvent  ( 
			PreparationType		in_PreparationType,		///< Preparation type ( Preparation_Load or Preparation_Unload )
			const char**		in_ppszString,			///< Array of event names
			AkUInt32			in_uNumEvent			///< Number of events in the array
			);

		/// Prepares or unprepares events synchronously (by ID).
		/// The Events are identified by their ID (see \ref soundengine_banks_general for a discussion on using strings and IDs).
		/// Before invoking PrepareEvent(), use LoadBank() to explicitly load the SoundBank(s) 
		/// that contain the Events and structures. When a request is posted to the
		/// Bank Manager consumer thread, it will resolve all dependencies needed to 
		/// successfully post the specified Events and load the required loose media files. 
		/// \aknote Before version 2015.1, the required media files could be included
		/// in a separate media SoundBank. As described in \ref whatsnew_2015_1_migration,
		/// however, PrepareEvent() now only looks for loose media files.
		/// \endaknote
		/// The function returns when the request is completely processed.
		/// \return 
		/// - \c AK_Success: Prepare/un-prepare successful.
		/// - \c AK_IDNotFound: At least one of the event/game sync identifiers passed to PrepareEvent() does not exist.
		/// - \c AK_InsufficientMemory: Insufficient memory to store bank data.
		/// - \c AK_BankReadError: I/O error.
		/// - \c AK_WrongBankVersion: Invalid bank version: make sure the version of Wwise that 
		/// you used to generate the SoundBanks matches that of the SDK you are currently using.
		/// - \c AK_InvalidFile: File specified could not be opened.
		/// - \c AK_InvalidParameter: Invalid parameter, invalid memory alignment.		
		/// - \c AK_Fail: Load or unload failed for any other reason. (Most likely small allocation failure)
		/// \remarks
		/// Whenever at least one event fails to be resolved, the actions performed for all 
		/// other events are cancelled.
		/// \sa
		/// - <tt>AK::SoundEngine::PrepareEvent()</tt>
		/// - <tt>AK::SoundEngine::ClearPreparedEvents()</tt>
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
		/// - <tt>AK::SoundEngine::LoadBank()</tt>
		/// - \ref soundengine_banks
		/// - \ref sdk_bank_training
		extern   AKRESULT __cdecl  PrepareEvent  ( 
			PreparationType		in_PreparationType,		///< Preparation type ( Preparation_Load or Preparation_Unload )
			AkUniqueID*			in_pEventID,			///< Array of event IDs
			AkUInt32			in_uNumEvent			///< Number of event IDs in the array
			);

#ifdef AK_SUPPORT_WCHAR
		/// Prepares or unprepares an event asynchronously (by Unicode string).
		/// The Events are identified by string (see \ref soundengine_banks_general for a discussion on using strings and IDs).
		/// Before invoking \c PrepareEvent(), use \c LoadBank() to explicitly load the SoundBank(s) 
		/// that contain the Events and structures. When a request is posted to the
		/// Bank Manager consumer thread, it will resolve all dependencies needed to 
		/// successfully post the specified Events and load the required loose media files. 
		/// \aknote Before version 2015.1, the required media files could be included
		/// in a separate media SoundBank. As described in \ref whatsnew_2015_1_migration,
		/// however, \c PrepareEvent() now only looks for loose media files.
		/// \endaknote
		/// The function returns immediately. Use a callback to be notified when the request has finished being processed.
		/// \return AK_Success if scheduling is was successful, AK_Fail otherwise.
		/// \remarks
		/// Whenever at least one Event fails to be resolved, the actions performed for all 
		/// other Events are cancelled.
		/// \sa
		/// - <tt>AK::SoundEngine::PrepareEvent()</tt>
		/// - <tt>AK::SoundEngine::ClearPreparedEvents()</tt>
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
		/// - <tt>AK::SoundEngine::LoadBank()</tt>
		/// - AkBankCallbackFunc
		/// - \ref soundengine_banks
		/// - \ref sdk_bank_training
		extern   AKRESULT __cdecl  PrepareEvent  ( 
			PreparationType		in_PreparationType,		///< Preparation type ( Preparation_Load or Preparation_Unload )
			const wchar_t**		in_ppszString,			///< Array of event names
			AkUInt32			in_uNumEvent,			///< Number of events in the array
			AkBankCallbackFunc	in_pfnBankCallback,		///< Callback function
			void *              in_pCookie				///< Callback cookie (reserved to user, passed to the callback function)
			);
#endif //AK_SUPPORT_WCHAR

		/// Prepares or unprepares an event asynchronously.
		/// The Events are identified by string (see \ref soundengine_banks_general for a discussion on using strings and IDs).
		/// Before invoking \c PrepareEvent(), use \c LoadBank() to explicitly load the SoundBank(s) 
		/// that contain the Events and structures. When a request is posted to the
		/// Bank Manager consumer thread, it will resolve all dependencies needed to 
		/// successfully post the specified Events and load the required loose media files. 
		/// \aknote Before version 2015.1, the required media files could be included
		/// in a separate media SoundBank. As described in \ref whatsnew_2015_1_migration,
		/// however, \c PrepareEvent() now only looks for loose media files.
		/// \endaknote
		/// The function returns immediately. Use a callback to be notified when the request has finished being processed.
		/// \return AK_Success if scheduling is was successful, AK_Fail otherwise.
		/// \remarks
		/// Whenever at least one event fails to be resolved, the actions performed for all 
		/// other events are cancelled.
		/// \sa
		/// - <tt>AK::SoundEngine::PrepareEvent()</tt>
		/// - <tt>AK::SoundEngine::ClearPreparedEvents()</tt>
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
		/// - <tt>AK::SoundEngine::LoadBank()</tt>
		/// - AkBankCallbackFunc
		/// - \ref soundengine_banks
		/// - \ref sdk_bank_training
		extern   AKRESULT __cdecl  PrepareEvent  ( 
			PreparationType		in_PreparationType,		///< Preparation type ( Preparation_Load or Preparation_Unload )
			const char**		in_ppszString,			///< Array of event names 
			AkUInt32			in_uNumEvent,			///< Number of events in the array
			AkBankCallbackFunc	in_pfnBankCallback,		///< Callback function
			void *              in_pCookie				///< Callback cookie (reserved to user, passed to the callback function)
			);

		/// Prepares or unprepares events asynchronously (by ID).\n
		/// The Events are identified by their ID (see \ref soundengine_banks_general for a discussion on using strings and IDs).
		/// Before invoking PrepareEvent(), use LoadBank() to explicitly load the SoundBank(s) 
		/// that contain the Events and structures. When a request is posted to the
		/// Bank Manager consumer thread, it will resolve all dependencies needed to 
		/// successfully post the specified Events and load the required loose media files. 
		/// \aknote Before version 2015.1, the required media files could be included
		/// in a separate media SoundBank. As described in \ref whatsnew_2015_1_migration,
		/// however, PrepareEvent() now only looks for loose media files.
		/// \endaknote
		/// The function returns immediately. Use a callback to be notified when the request has finished being processed.
		/// \return AK_Success if scheduling is was successful, AK_Fail otherwise.
		/// \remarks
		/// Whenever at least one event fails to be resolved, the actions performed for all 
		/// other events are cancelled.
		/// \sa
		/// - <tt>AK::SoundEngine::PrepareEvent()</tt>
		/// - <tt>AK::SoundEngine::ClearPreparedEvents()</tt>
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
		/// - <tt>AK::SoundEngine::LoadBank()</tt>
		/// - AkBankCallbackFunc
		/// - \ref soundengine_banks
		/// - \ref sdk_bank_training
		extern   AKRESULT __cdecl  PrepareEvent  ( 
			PreparationType		in_PreparationType,		///< Preparation type ( Preparation_Load or Preparation_Unload )
			AkUniqueID*			in_pEventID,			///< Array of event IDs
			AkUInt32			in_uNumEvent,			///< Number of event IDs in the array
			AkBankCallbackFunc	in_pfnBankCallback,		///< Callback function
			void *              in_pCookie				///< Callback cookie (reserved to user, passed to the callback function)
			);

#ifdef AK_SUPPORT_WCHAR
		/// Prepares or unprepares busses synchronously (by Unicode string).\n
		/// The Busses are identified by strings and converted to IDs internally
		/// (see \ref soundengine_banks_general for a discussion on using strings and IDs).
		/// Before invoking PrepareBus(), use LoadBank() to explicitly load the SoundBank(s) 
		/// that contain the Bus structures. When a request is posted to the
		/// Bank Manager consumer thread, it will load the required loose media files for effects 
		/// placed on the specified busses.
		/// The function returns when the request is completely processed.
		/// \return 
		/// - \c AK_Success: Prepare/un-prepare successful.
		/// - \c AK_IDNotFound: At least one of the bus IDs does not exist.
		/// - \c AK_InsufficientMemory: Insufficient memory to store bank data.
		/// - \c AK_BankReadError: I/O error.
		/// - \c AK_WrongBankVersion: Invalid bank version: make sure the version of Wwise that 
		/// you used to generate the SoundBanks matches that of the SDK you are currently using.
		/// - \c AK_InvalidFile: File specified could not be opened.
		/// - \c AK_InvalidParameter: Invalid parameter, invalid memory alignment.		
		/// - \c AK_Fail: Load or unload failed for any other reason.
		/// \remarks
		/// Whenever at least one bus fails to be resolved, the actions performed for all 
		/// other busses are cancelled.
		/// \sa
		/// - <tt>AK::SoundEngine::PrepareBus()</tt>
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
		/// - <tt>AK::SoundEngine::LoadBank()</tt>
		/// - AkBankCallbackFunc
		/// - \ref soundengine_banks
		/// - \ref sdk_bank_training
		extern  AKRESULT __cdecl  PrepareBus (
			PreparationType     in_PreparationType, ///< Preparation type ( Preparation_Load or Preparation_Unload )
			const wchar_t**     in_ppszString,      ///< Array of bus names
			AkUInt32            in_uBusses          ///< Number of bus names in the array
			);
#endif //AK_SUPPORT_WCHAR

		/// Prepares or unprepares busses synchronously.\n
		/// The Busses are identified by strings and converted to IDs internally
		/// (see \ref soundengine_banks_general for a discussion on using strings and IDs).
		/// Before invoking PrepareBus(), use LoadBank() to explicitly load the SoundBank(s) 
		/// that contain the Bus structures. When a request is posted to the
		/// Bank Manager consumer thread, it will load the required loose media files for effects 
		/// placed on the specified busses.
		/// The function returns when the request is completely processed.
		/// \return 
		/// - \c AK_Success: Prepare/un-prepare successful.
		/// - \c AK_IDNotFound: At least one of the bus IDs does not exist.
		/// - \c AK_InsufficientMemory: Insufficient memory to store bank data.
		/// - \c AK_BankReadError: I/O error.
		/// - \c AK_WrongBankVersion: Invalid bank version: make sure the version of Wwise that 
		/// you used to generate the SoundBanks matches that of the SDK you are currently using.
		/// - \c AK_InvalidFile: File specified could not be opened.
		/// - \c AK_InvalidParameter: Invalid parameter, invalid memory alignment.		
		/// - \c AK_Fail: Load or unload failed for any other reason.
		/// \remarks
		/// Whenever at least one bus fails to be resolved, the actions performed for all 
		/// other busses are cancelled.
		/// \sa
		/// - <tt>AK::SoundEngine::PrepareBus()</tt>
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
		/// - <tt>AK::SoundEngine::LoadBank()</tt>
		/// - AkBankCallbackFunc
		/// - \ref soundengine_banks
		/// - \ref sdk_bank_training
		extern  AKRESULT __cdecl  PrepareBus (
			PreparationType     in_PreparationType, ///< Preparation type ( Preparation_Load or Preparation_Unload )
			const char**        in_ppszString,      ///< Array of bus names
			AkUInt32            in_uBusses          ///< Number of bus names in the array
			);

		/// Prepares or unprepares busses synchronously (by ID).\n
		/// The Busses are identified by their ID (see \ref soundengine_banks_general for a discussion on using strings and IDs).
		/// Before invoking PrepareBus(), use LoadBank() to explicitly load the SoundBank(s) 
		/// that contain the Bus structures. When a request is posted to the
		/// Bank Manager consumer thread, it will load the required loose media files for effects 
		/// placed on the specified busses.
		/// The function returns when the request is completely processed.
		/// \return 
		/// - \c AK_Success: Prepare/un-prepare successful.
		/// - \c AK_IDNotFound: At least one of the bus IDs does not exist.
		/// - \c AK_InsufficientMemory: Insufficient memory to store bank data.
		/// - \c AK_BankReadError: I/O error.
		/// - \c AK_WrongBankVersion: Invalid bank version: make sure the version of Wwise that 
		/// you used to generate the SoundBanks matches that of the SDK you are currently using.
		/// - \c AK_InvalidFile: File specified could not be opened.
		/// - \c AK_InvalidParameter: Invalid parameter, invalid memory alignment.		
		/// - \c AK_Fail: Load or unload failed for any other reason.
		/// \remarks
		/// Whenever at least one bus fails to be resolved, the actions performed for all 
		/// other busses are cancelled.
		/// \sa
		/// - <tt>AK::SoundEngine::PrepareBus()</tt>
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
		/// - <tt>AK::SoundEngine::LoadBank()</tt>
		/// - AkBankCallbackFunc
		/// - \ref soundengine_banks
		/// - \ref sdk_bank_training
		extern  AKRESULT __cdecl  PrepareBus (
			PreparationType     in_PreparationType, ///< Preparation type ( Preparation_Load or Preparation_Unload )
			AkUniqueID*         in_pBusID,          ///< Array of bus IDs
			AkUInt32            in_uBusses          ///< Number of bus IDs in the array
			);

#ifdef AK_SUPPORT_WCHAR
		/// Prepares or unprepares busses asynchronously (by Unicode string).\n
		/// The Busses are identified by string (see \ref soundengine_banks_general for a discussion on using strings and IDs).
		/// Before invoking PrepareBus(), use LoadBank() to explicitly load the SoundBank(s) 
		/// that contain the Bus structures. When a request is posted to the
		/// Bank Manager consumer thread, it will load the required loose media files for effects 
		/// placed on the specified busses.
		/// The function returns immediately. Use a callback to be notified when the request has finished being processed.
		/// \return AK_Success if scheduling is was successful, AK_Fail otherwise.
		/// \remarks
		/// Whenever at least one bus fails to be resolved, the actions performed for all 
		/// other busses are cancelled.
		/// \sa
		/// - <tt>AK::SoundEngine::PrepareBus()</tt>
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
		/// - <tt>AK::SoundEngine::LoadBank()</tt>
		/// - AkBankCallbackFunc
		/// - \ref soundengine_banks
		/// - \ref sdk_bank_training
		extern  AKRESULT __cdecl  PrepareBus (
			PreparationType     in_PreparationType, ///< Preparation type ( Preparation_Load or Preparation_Unload )
			const wchar_t**     in_ppszString,      ///< Array of bus names
			AkUInt32            in_uBusses,         ///< Number of bus names in the array
			AkBankCallbackFunc  in_pfnBankCallback, ///< Callback function
			void*               in_pCookie          ///< Callback cookie (reserved to user, passed to the callback function)
			);
#endif //AK_SUPPORT_WCHAR

		/// Prepares or unprepares busses asynchronously.\n
		/// The Busses are identified by string (see \ref soundengine_banks_general for a discussion on using strings and IDs).
		/// Before invoking PrepareBus(), use LoadBank() to explicitly load the SoundBank(s) 
		/// that contain the Bus structures. When a request is posted to the
		/// Bank Manager consumer thread, it will load the required loose media files for effects 
		/// placed on the specified busses.
		/// The function returns immediately. Use a callback to be notified when the request has finished being processed.
		/// \return AK_Success if scheduling is was successful, AK_Fail otherwise.
		/// \remarks
		/// Whenever at least one bus fails to be resolved, the actions performed for all 
		/// other busses are cancelled.
		/// \sa
		/// - <tt>AK::SoundEngine::PrepareBus()</tt>
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
		/// - <tt>AK::SoundEngine::LoadBank()</tt>
		/// - AkBankCallbackFunc
		/// - \ref soundengine_banks
		/// - \ref sdk_bank_training
		extern  AKRESULT __cdecl  PrepareBus (
			PreparationType     in_PreparationType, ///< Preparation type ( Preparation_Load or Preparation_Unload )
			const char**        in_ppszString,      ///< Array of bus names 
			AkUInt32            in_uBusses,         ///< Number of bus names in the array
			AkBankCallbackFunc  in_pfnBankCallback, ///< Callback function
			void*               in_pCookie          ///< Callback cookie (reserved to user, passed to the callback function)
			);

		/// Prepares or unprepares busses asynchronously (by ID).\n
		/// The Busses are identified by their ID (see \ref soundengine_banks_general for a discussion on using strings and IDs).
		/// Before invoking PrepareBus(), use LoadBank() to explicitly load the SoundBank(s) 
		/// that contain the Bus structures. When a request is posted to the
		/// Bank Manager consumer thread, it will load the required loose media files for effects 
		/// placed on the specified busses.
		/// The function returns immediately. Use a callback to be notified when the request has finished being processed.
		/// \return AK_Success if scheduling is was successful, AK_Fail otherwise.
		/// \remarks
		/// Whenever at least one bus fails to be resolved, the actions performed for all 
		/// other busses are cancelled.
		/// \sa
		/// - <tt>AK::SoundEngine::PrepareBus()</tt>
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
		/// - <tt>AK::SoundEngine::LoadBank()</tt>
		/// - AkBankCallbackFunc
		/// - \ref soundengine_banks
		/// - \ref sdk_bank_training
		extern  AKRESULT __cdecl  PrepareBus (
			PreparationType     in_PreparationType, ///< Preparation type ( Preparation_Load or Preparation_Unload )
			AkUniqueID*         in_pBusID,          ///< Array of bus IDs
			AkUInt32            in_uBusses,         ///< Number of bus IDs in the array
			AkBankCallbackFunc  in_pfnBankCallback, ///< Callback function
			void*               in_pCookie          ///< Callback cookie (reserved to user, passed to the callback function)
			);

		/// Indicates the location of a specific Media ID in memory
		/// The sources are identified by their ID (see \ref soundengine_banks_general for a discussion on using strings and IDs).
		/// \return AK_Success if operation was successful, AK_InvalidParameter if in_pSourceSettings is invalid or media sizes are 0.
		extern   AKRESULT __cdecl  SetMedia  ( 
			AkSourceSettings *	in_pSourceSettings,		///< Array of Source Settings
			AkUInt32			in_uNumSourceSettings	///< Number of Source Settings in the array
			);

		/// Removes the specified source from the list of loaded media, only if this media is not already in use.
		/// The sources are identified by their ID (see \ref soundengine_banks_general for a discussion on using strings and IDs).
		/// \aknote Media that is still in use by the sound engine should not be unset. It is marked for removal to prevent additional use.
		/// If this function returns AK_ResourceInUse, then the client must not release memory for this media.
		/// Instead, the client should retry the TryUnsetMedia operation later with the same parameters and check for AK_Success.
		/// \endaknote
		/// If out_pUnsetResults is not null, then it is assumed to point to an array of result codes of the same length as in_pSourceSettings.
		/// out_pUnsetResults will be filled with either AK_Success or AK_ResourceInUse, indicating which media was still in use and not unset.
		/// \return
		/// - \c AK_Success: Operation was successful, and the memory can be released on the client side.
		/// - \c AK_ResourceInUse: Specified media is still in use by the sound engine, and the media was not unset. Do not release memory, and try again later.
		/// - \c AK_InvalidParameter: in_pSourceSettings is invalid
		extern  AKRESULT __cdecl  TryUnsetMedia (
			AkSourceSettings* in_pSourceSettings,       ///< Array of Source Settings
			AkUInt32          in_uNumSourceSettings,    ///< Number of Source Settings in the array
			AKRESULT*         out_pUnsetResults         ///< (optional, can be null) Array of result codes
			);

#ifdef AK_SUPPORT_WCHAR
		/// Prepares or unprepares game syncs synchronously (by Unicode string).\n
		/// The group and game syncs are specified by string (see \ref soundengine_banks_general for a discussion on using strings and IDs).
		/// The game syncs definitions must already exist in the sound engine by having
		/// explicitly loaded the bank(s) that contain them (with LoadBank()).
		/// A request is posted to the Bank Manager consumer thread. It will resolve all 
		/// dependencies needed to successfully set this game sync group to one of the
		/// game sync values specified, and load the required banks, if applicable. 
		/// The function returns when the request has been completely processed. 
		/// \return 
		/// - \c AK_Success: Prepare/un-prepare successful.
		/// - \c AK_IDNotFound: At least one of the event/game sync identifiers passed to PrepareGameSyncs() does not exist.
		/// - \c AK_InsufficientMemory: Insufficient memory to store bank data.
		/// - \c AK_BankReadError: I/O error.
		/// - \c AK_WrongBankVersion: Invalid bank version: make sure the version of Wwise that 
		/// you used to generate the SoundBanks matches that of the SDK you are currently using.
		/// - \c AK_InvalidFile: File specified could not be opened.
		/// - \c AK_InvalidParameter: Invalid parameter, invalid memory alignment.		
		/// - \c AK_Fail: Load or unload failed for any other reason. (Most likely small allocation failure)
		/// \remarks
		/// You need to call PrepareGameSyncs() if the sound engine was initialized with AkInitSettings::bEnableGameSyncPreparation 
		/// set to true. When set to false, the sound engine automatically prepares all game syncs when preparing events,
		/// so you never need to call this function.
		/// \sa 
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
		/// - <tt>AK::SoundEngine::PrepareEvent()</tt>
		/// - <tt>AK::SoundEngine::LoadBank()</tt>
		/// - \c AkInitSettings
		/// - \ref soundengine_banks
		/// - \ref sdk_bank_training
		extern   AKRESULT __cdecl  PrepareGameSyncs  (
			PreparationType	in_PreparationType,			///< Preparation type ( Preparation_Load or Preparation_Unload )
			AkGroupType		in_eGameSyncType,			///< The type of game sync.
			const wchar_t*	in_pszGroupName,			///< The State Group Name or the Switch Group Name.
			const wchar_t**	in_ppszGameSyncName,		///< The specific ID of the state to either support or not support.
			AkUInt32		in_uNumGameSyncs			///< The number of game sync in the string array.
			);
#endif //AK_SUPPORT_WCHAR

		/// Prepares or unprepares game syncs synchronously.\n
		/// The group and game syncs are specified by string (see \ref soundengine_banks_general for a discussion on using strings and IDs).
		/// The game syncs definitions must already exist in the sound engine by having
		/// explicitly loaded the bank(s) that contain them (with LoadBank()).
		/// A request is posted to the Bank Manager consumer thread. It will resolve all 
		/// dependencies needed to successfully set this game sync group to one of the
		/// game sync values specified, and load the required banks, if applicable. 
		/// The function returns when the request has been completely processed. 
		/// \return 
		/// - \c AK_Success: Prepare/un-prepare successful.
		/// - \c AK_IDNotFound: At least one of the event/game sync identifiers passed to PrepareGameSyncs() does not exist.
		/// - \c AK_InsufficientMemory: Insufficient memory to store bank data.
		/// - \c AK_BankReadError: I/O error.
		/// - \c AK_WrongBankVersion: Invalid bank version: make sure the version of Wwise that 
		/// you used to generate the SoundBanks matches that of the SDK you are currently using.
		/// - \c AK_InvalidFile: File specified could not be opened.
		/// - \c AK_InvalidParameter: Invalid parameter, invalid memory alignment.		
		/// - \c AK_Fail: Load or unload failed for any other reason. (Most likely small allocation failure)
		/// \remarks
		/// You need to call PrepareGameSyncs() if the sound engine was initialized with AkInitSettings::bEnableGameSyncPreparation 
		/// set to true. When set to false, the sound engine automatically prepares all game syncs when preparing events,
		/// so you never need to call this function.
		/// \sa 
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
		/// - <tt>AK::SoundEngine::PrepareEvent()</tt>
		/// - <tt>AK::SoundEngine::LoadBank()</tt>
		/// - AkInitSettings
		/// - \ref soundengine_banks
		/// - \ref sdk_bank_training
		extern   AKRESULT __cdecl  PrepareGameSyncs  (
			PreparationType	in_PreparationType,			///< Preparation type ( Preparation_Load or Preparation_Unload )
			AkGroupType		in_eGameSyncType,			///< The type of game sync.
			const char*		in_pszGroupName,			///< The State Group Name or the Switch Group Name.
			const char**	in_ppszGameSyncName,		///< The specific ID of the state to either support or not support.
			AkUInt32		in_uNumGameSyncs			///< The number of game sync in the string array.
			);

		/// Prepares or unprepares game syncs synchronously (by ID).\n
		/// The group and game syncs are specified by ID (see \ref soundengine_banks_general for a discussion on using strings and IDs).
		/// The game syncs definitions must already exist in the sound engine by having
		/// explicitly loaded the bank(s) that contain them (with LoadBank()).
		/// A request is posted to the Bank Manager consumer thread. It will resolve all 
		/// dependencies needed to successfully set this game sync group to one of the
		/// game sync values specified, and load the required banks, if applicable. 
		/// The function returns when the request has been completely processed. 
		/// \return 
		/// - \c AK_Success: Prepare/un-prepare successful.
		/// - \c AK_IDNotFound: At least one of the event/game sync identifiers passed to PrepareGameSyncs() does not exist.
		/// - \c AK_InsufficientMemory: Insufficient memory to store bank data.
		/// - \c AK_BankReadError: I/O error.
		/// - \c AK_WrongBankVersion: Invalid bank version: make sure the version of Wwise that 
		/// you used to generate the SoundBanks matches that of the SDK you are currently using.
		/// - \c AK_InvalidFile: File specified could not be opened.
		/// - \c AK_InvalidParameter: Invalid parameter, invalid memory alignment.		
		/// - \c AK_Fail: Load or unload failed for any other reason. (Most likely small allocation failure)
		/// \remarks
		/// You need to call \c PrepareGameSyncs() if the sound engine was initialized with \c AkInitSettings::bEnableGameSyncPreparation 
		/// set to \c true. When set to \c false, the sound engine automatically prepares all game syncs when preparing Events,
		/// so you never need to call this function.
		/// \sa 
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
		/// - <tt>AK::SoundEngine::PrepareEvent()</tt>
		/// - <tt>AK::SoundEngine::LoadBank()</tt>
		/// - AkInitSettings
		/// - \ref soundengine_banks
		/// - \ref sdk_bank_training
		extern   AKRESULT __cdecl  PrepareGameSyncs  (
			PreparationType	in_PreparationType,			///< Preparation type ( Preparation_Load or Preparation_Unload )
			AkGroupType		in_eGameSyncType,			///< The type of game sync.
			AkUInt32		in_GroupID,					///< The State Group ID or the Switch Group ID.
			AkUInt32*		in_paGameSyncID,			///< Array of ID of the game syncs to either support or not support.
			AkUInt32		in_uNumGameSyncs			///< The number of game sync ID in the array.
			);

#ifdef AK_SUPPORT_WCHAR
		/// Prepares or unprepares game syncs asynchronously (by Unicode string).\n
		/// The group and game syncs are specified by string (see \ref soundengine_banks_general for a discussion on using strings and IDs).
		/// The game syncs definitions must already exist in the sound engine by having
		/// explicitly loaded the bank(s) that contain them (with \c LoadBank()).
		/// A request is posted to the Bank Manager consumer thread. It will resolve all 
		/// dependencies needed to successfully set this game sync group to one of the
		/// game sync values specified, and load the required banks, if applicable. 
		/// The function returns immediately. Use a callback to be notified when the request has finished being processed.
		/// \return AK_Success if scheduling is was successful, AK_Fail otherwise.
		/// \remarks
		/// You need to call \c PrepareGameSyncs() if the sound engine was initialized with \c AkInitSettings::bEnableGameSyncPreparation 
		/// set to \c true. When set to \c false, the sound engine automatically prepares all game syncs when preparing Events,
		/// so you never need to call this function.
		/// \sa 
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
		/// - <tt>AK::SoundEngine::PrepareEvent()</tt>
		/// - <tt>AK::SoundEngine::LoadBank()</tt>
		/// - \c AkInitSettings
		/// - \c AkBankCallbackFunc
		/// - \ref soundengine_banks
		/// - \ref sdk_bank_training
		extern   AKRESULT __cdecl  PrepareGameSyncs  (
			PreparationType		in_PreparationType,		///< Preparation type ( Preparation_Load or Preparation_Unload )
			AkGroupType			in_eGameSyncType,		///< The type of game sync.
			const wchar_t*		in_pszGroupName,		///< The State Group Name or the Switch Group Name.
			const wchar_t**		in_ppszGameSyncName,	///< The specific ID of the state to either support or not support.
			AkUInt32			in_uNumGameSyncs,		///< The number of game sync in the string array.
			AkBankCallbackFunc	in_pfnBankCallback,		///< Callback function
			void *				in_pCookie				///< Callback cookie (reserved to user, passed to the callback function)
			);
#endif //AK_SUPPORT_WCHAR

		/// Prepares or unprepares game syncs asynchronously.\n
		/// The group and game syncs are specified by string (see \ref soundengine_banks_general for a discussion on using strings and IDs).
		/// The game syncs definitions must already exist in the sound engine by having
		/// explicitly loaded the bank(s) that contain them (with \c LoadBank()).
		/// A request is posted to the Bank Manager consumer thread. It will resolve all 
		/// dependencies needed to successfully set this game sync group to one of the
		/// game sync values specified, and load the required banks, if applicable. 
		/// The function returns immediately. Use a callback to be notified when the request has finished being processed.
		/// \return AK_Success if scheduling is was successful, AK_Fail otherwise.
		/// \remarks
		/// You need to call \c PrepareGameSyncs() if the sound engine was initialized with \c AkInitSettings::bEnableGameSyncPreparation 
		/// set to \c true. When set to \c false, the sound engine automatically prepares all game syncs when preparing events,
		/// so you never need to call this function.
		/// \sa 
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
		/// - <tt>AK::SoundEngine::PrepareEvent()</tt>
		/// - <tt>AK::SoundEngine::LoadBank()</tt>
		/// - AkInitSettings
		/// - AkBankCallbackFunc
		/// - \ref soundengine_banks
		/// - \ref sdk_bank_training
		extern   AKRESULT __cdecl  PrepareGameSyncs  (
			PreparationType		in_PreparationType,		///< Preparation type ( Preparation_Load or Preparation_Unload )
			AkGroupType			in_eGameSyncType,		///< The type of game sync.
			const char*			in_pszGroupName,		///< The State Group Name or the Switch Group Name.
			const char**		in_ppszGameSyncName,	///< The specific ID of the state to either support or not support.
			AkUInt32			in_uNumGameSyncs,		///< The number of game sync in the string array.
			AkBankCallbackFunc	in_pfnBankCallback,		///< Callback function
			void *				in_pCookie				///< Callback cookie (reserved to user, passed to the callback function)
			);

		/// Prepares or un-prepare game syncs asynchronously (by ID).\n
		/// The group and game syncs are specified by ID (see \ref soundengine_banks_general for a discussion on using strings and IDs).
		/// The game syncs definitions must already exist in the sound engine by having
		/// explicitly loaded the bank(s) that contain them (with LoadBank()).
		/// A request is posted to the Bank Manager consumer thread. It will resolve all 
		/// dependencies needed to successfully set this game sync group to one of the
		/// game sync values specified, and load the required banks, if applicable. 
		/// The function returns immediately. Use a callback to be notified when the request has finished being processed.
		/// \return AK_Success if scheduling is was successful, AK_Fail otherwise.
		/// \remarks
		/// You need to call PrepareGameSyncs() if the sound engine was initialized with AkInitSettings::bEnableGameSyncPreparation 
		/// set to true. When set to false, the sound engine automatically prepares all Game Syncs when preparing Events,
		/// so you never need to call this function.
		/// \sa 
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
		/// - <tt>AK::SoundEngine::PrepareEvent()</tt>
		/// - <tt>AK::SoundEngine::LoadBank()</tt>
		/// - AkInitSettings
		/// - AkBankCallbackFunc
		/// - \ref soundengine_banks
		/// - \ref sdk_bank_training
		extern   AKRESULT __cdecl  PrepareGameSyncs  (
			PreparationType		in_PreparationType,		///< Preparation type ( Preparation_Load or Preparation_Unload )
			AkGroupType			in_eGameSyncType,		///< The type of game sync.
			AkUInt32			in_GroupID,				///< The State Group ID or the Switch Group ID.
			AkUInt32*			in_paGameSyncID,		///< Array of ID of the Game Syncs to either support or not support.
			AkUInt32			in_uNumGameSyncs,		///< The number of game sync ID in the array.
			AkBankCallbackFunc	in_pfnBankCallback,		///< Callback function
			void *				in_pCookie				///< Callback cookie (reserved to user, passed to the callback function)
			);

	    //@}


		////////////////////////////////////////////////////////////////////////
		/// @name Listeners
		//@{

		/// Sets a game object's associated listeners.
		/// All listeners that have previously been added via AddListener or set via SetListeners will be removed and replaced with the listeners in the array in_pListenerGameObjs.
		/// Calling this function will override the default set of listeners and in_emitterGameObj will now reference its own, unique set of listeners.
		/// \return 
		/// - \c AK_Success if successful
		/// - \c AK_CommandTooLarge if the number of positions is too large for the command queue. Reduce the number of positions.
		/// \sa 
		/// - <tt>AK::SoundEngine::AddListener</tt>
		/// - <tt>AK::SoundEngine::RemoveListener</tt>
		/// - <tt>AK::SoundEngine::SetDefaultListeners</tt>
		/// - \ref soundengine_listeners
		extern   AKRESULT __cdecl  SetListeners  (
			AkGameObjectID in_emitterGameObj,			///< Emitter game object.  Must have been previously registered via RegisterGameObj.
			const AkGameObjectID* in_pListenerGameObjs,	///< Array of listener game object IDs that will be activated for in_emitterGameObj. 
			AkUInt32 in_uNumListeners					///< Length of array
			);

		/// Add a single listener to a game object's set of associated listeners.
		/// Any listeners that have previously been added or set via AddListener or SetListeners will remain as listeners and in_listenerGameObj will be added as an additional listener.
		/// Calling this function will override the default set of listeners and in_emitterGameObj will now reference its own, unique set of listeners.
		/// \sa 
		/// - <tt>AK::SoundEngine::SetListeners</tt>
		/// - <tt>AK::SoundEngine::RemoveListener</tt>
		/// - <tt>AK::SoundEngine::SetDefaultListeners</tt>
		/// - \ref soundengine_listeners
		extern  AKRESULT __cdecl  AddListener (
			AkGameObjectID in_emitterGameObj,			///< Emitter game object.  Must have been previously registered via RegisterGameObj.
			AkGameObjectID in_listenerGameObj			///< Listener game object IDs that will be activated for in_emitterGameObj. 
			);

		/// Remove a single listener from a game object's set of active listeners.
		/// Calling this function will override the default set of listeners and in_emitterGameObj will now reference its own, unique set of listeners.
		/// \sa 
		/// - <tt>AK::SoundEngine::SetListeners</tt>
		/// - <tt>AK::SoundEngine::AddListener</tt>
		/// - <tt>AK::SoundEngine::SetDefaultListeners</tt>
		/// - \ref soundengine_listeners
		extern  AKRESULT __cdecl  RemoveListener (
			AkGameObjectID in_emitterGameObj,			///< Emitter game object.
			AkGameObjectID in_listenerGameObj			///< Listener game object IDs that will be deactivated for in_emitterGameObj. Game objects must have been previously registered.
			);

		/// Sets the default set of associated listeners for game objects that have not explicitly overridden their listener sets.  Upon registration, all game objects reference the default listener set, until
		/// a call to AddListener, RemoveListener, SetListeners or SetGameObjectOutputBusVolume is made on that game object.
		/// All default listeners that have previously been added via AddDefaultListener or set via SetDefaultListeners will be removed and replaced with the listeners in the array in_pListenerGameObjs.
		/// \return Always returns AK_Success
		/// \sa 
		/// - \ref soundengine_listeners
		extern  AKRESULT __cdecl  SetDefaultListeners (
			const AkGameObjectID* in_pListenerObjs,	///< Array of listener game object IDs that will be activated for subsequent registrations. Game objects must have been previously registered.
			AkUInt32 in_uNumListeners				///< Length of array
			);

		/// Add a single listener to the default set of listeners.  Upon registration, all game objects reference the default listener set, until
		/// a call to AddListener, RemoveListener, SetListeners or SetGameObjectOutputBusVolume is made on that game object.
		/// \sa 
		/// - <tt>AK::SoundEngine::SetDefaultListeners</tt>
		/// - <tt>AK::SoundEngine::RemoveDefaultListener</tt>
		/// - \ref soundengine_listeners
		extern  AKRESULT __cdecl  AddDefaultListener (
			AkGameObjectID in_listenerGameObj			///< Listener game object IDs that will be added to the default set of listeners.
			);

		/// Remove a single listener from the default set of listeners.  Upon registration, all game objects reference the default listener set, until
		/// a call to AddListener, RemoveListener, SetListeners or SetGameObjectOutputBusVolume is made on that game object.
		/// \sa 
		/// - <tt>AK::SoundEngine::SetDefaultListeners</tt>
		/// - <tt>AK::SoundEngine::AddDefaultListener</tt>
		/// - \ref soundengine_listeners
		extern  AKRESULT __cdecl  RemoveDefaultListener (
			AkGameObjectID in_listenerGameObj			///< Listener game object IDs that will be removed from the default set of listeners.
			);

		/// Resets the listener associations to the default listener(s), as set by <tt>SetDefaultListeners</tt>. This will also reset per-listener gains that have been set using <tt>SetGameObjectOutputBusVolume</tt>.
		/// \return Always returns AK_Success
		/// \sa 
		/// - <tt>AK::SoundEngine::SetListeners</tt>
		/// - <tt>AK::SoundEngine::SetDefaultListeners</tt>
		/// - <tt>AK::SoundEngine::SetGameObjectOutputBusVolume</tt>
		/// - \ref soundengine_listeners
		extern  AKRESULT __cdecl  ResetListenersToDefault (
			AkGameObjectID in_emitterGameObj	///< Emitter game object.
			);

		/// Sets a listener's spatialization parameters. This lets you define listener-specific 
		/// volume offsets for each audio channel.
		/// If \c in_bSpatialized is false, only \c in_pVolumeOffsets is used for this listener (3D positions 
		/// have no effect on the speaker distribution). Otherwise, \c in_pVolumeOffsets is added to the speaker
		/// distribution computed for this listener.
		/// Use helper functions of \c AK::SpeakerVolumes to manipulate the vector of volume offsets in_pVolumeOffsets.
		/// 
		/// \remarks
		/// - If a sound is mixed into a bus that has a different speaker configuration than in_channelConfig,
		/// standard up/downmix rules apply.
		/// - Sounds with 3D Spatialization set to None will not be affected by these parameters.
		/// \return 
		/// - \c AK_Success if message was successfully posted to sound engine queue
		/// - \c AK_InvalidFloatValue if the value specified was NaN or Inf
		/// - \c AK_InsufficientMemory if there wasn't enough memory in the message queue		
		/// \sa 
		/// - \ref soundengine_listeners_spatial
		extern   AKRESULT __cdecl  SetListenerSpatialization  (
			AkGameObjectID in_uListenerID,				///< Listener game object ID
			bool in_bSpatialized,						///< Spatialization toggle (True : enable spatialization, False : disable spatialization)
			AkChannelConfig in_channelConfig,			///< Channel configuration associated with volumes in_pVolumeOffsets. Ignored if in_pVolumeOffsets is NULL.
			AK::SpeakerVolumes::VectorPtr in_pVolumeOffsets = NULL	///< Per-speaker volume offset, in dB. See AkSpeakerVolumes.h for how to manipulate this vector.
			);

		//@}


		////////////////////////////////////////////////////////////////////////
		/// @name Game Syncs
		//@{

		/// Sets the value of a real-time parameter control (by ID).
		/// With this function, you may set a game parameter value with global scope or with game object scope. 
		/// Game object scope supersedes global scope. (Once a value is set for the game object scope, it will not be affected by changes to the global scope value.) Game parameter values set with a global scope are applied to all 
		/// game objects that not yet registered, or already registered but not overridden with a value with game object scope.
		/// To set a game parameter value with global scope, pass \c AK_INVALID_GAME_OBJECT as the game object. 
		/// With this function, you may also change the value of a game parameter over time. To do so, specify a non-zero 
		/// value for \c in_uValueChangeDuration. At each audio frame, the game parameter value will be updated internally 
		/// according to the interpolation curve. If you call <tt>SetRTPCValue()</tt> with <tt>in_uValueChangeDuration = 0</tt> in the 
		/// middle of an interpolation, the interpolation stops and the new value is set directly. Thus, if you call this 
		/// function at every game frame, you should not use \c in_uValueChangeDuration, as it would have no effect and it is less efficient.
		/// See \ref soundengine_rtpc_pergameobject, \ref soundengine_rtpc_buses and 
		/// \ref soundengine_rtpc_effects for more details on RTPC scope.
		/// \return 
		/// - \c AK_Success if the value was successfully set
		/// - \c AK_InvalidFloatValue if the value specified was NaN, Inf or FLT_MAX (3.402823e+38)
		/// - \c AK_InvalidID if in_rtpcID is AK_INVALID_UNIQUE_ID (0)
		/// \sa 
		/// - \ref soundengine_rtpc
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
        extern   AKRESULT __cdecl  SetRTPCValue  ( 
			AkRtpcID in_rtpcID, 									///< ID of the game parameter
			AkRtpcValue in_value, 									///< Value to set
			AkGameObjectID in_gameObjectID = AK_INVALID_GAME_OBJECT,///< Associated game object ID
			AkTimeMs in_uValueChangeDuration = 0,					///< Duration during which the game parameter is interpolated towards in_value
			AkCurveInterpolation in_eFadeCurve = AkCurveInterpolation_Linear,	///< Curve type to be used for the game parameter interpolation
			bool in_bBypassInternalValueInterpolation = false		///< True if you want to bypass the internal "slew rate" or "over time filtering" specified by the sound designer. This is meant to be used when for example loading a level and you dont want the values to interpolate.
		    );

#ifdef AK_SUPPORT_WCHAR
		/// Sets the value of a real-time parameter control (by Unicode string name).
		/// With this function, you may set a game parameter value to global scope or to game object scope. 
		/// Game object scope supersedes global scope. (Once a value is set for the game object scope, it will not be affected by changes to the global scope value.) Game parameter values set with global scope are applied to all 
		/// game objects that not yet registered, or already registered but not overridden with a value with game object scope.
		/// To set a game parameter value with global scope, pass AK_INVALID_GAME_OBJECT as the game object. 
		/// With this function, you may also change the value of a game parameter over time. To do so, specify a non-zero 
		/// value for \c in_uValueChangeDuration. At each audio frame, the game parameter value will be updated internally 
		/// according to the interpolation curve. If you call SetRTPCValue() with in_uValueChangeDuration = 0 in the 
		/// middle of an interpolation, the interpolation stops and the new value is set directly. Thus, if you call this 
		/// function at every game frame, you should not use in_uValueChangeDuration, as it would have no effect and it is less efficient.
		/// See \ref soundengine_rtpc_pergameobject, \ref soundengine_rtpc_buses and 
		/// \ref soundengine_rtpc_effects for more details on RTPC scope.
		/// \return 
		/// - \c AK_Success if the value was successfully set
		/// - \c AK_InvalidFloatValue if the value specified was NaN, Inf or FLT_MAX (3.402823e+38)		
		/// - \c AK_InvalidID if in_pszRtpcName is NULL.
		/// \aknote Strings are case-insensitive. \endaknote
		/// \sa 
		/// - \ref soundengine_rtpc
        extern   AKRESULT __cdecl  SetRTPCValue  ( 
			const wchar_t* in_pszRtpcName,							///< Name of the game parameter
			AkRtpcValue in_value, 									///< Value to set
			AkGameObjectID in_gameObjectID = AK_INVALID_GAME_OBJECT,///< Associated game object ID
			AkTimeMs in_uValueChangeDuration = 0,					///< Duration during which the game parameter is interpolated towards in_value
			AkCurveInterpolation in_eFadeCurve = AkCurveInterpolation_Linear,	///< Curve type to be used for the game parameter interpolation
			bool in_bBypassInternalValueInterpolation = false		///< True if you want to bypass the internal "slew rate" or "over time filtering" specified by the sound designer. This is meant to be used when for example loading a level and you dont want the values to interpolate.
		    );
#endif //AK_SUPPORT_WCHAR

		/// Sets the value of a real-time parameter control.
		/// With this function, you may set a game parameter value with global scope or with game object scope. 
		/// Game object scope supersedes global scope. (Once a value is set for the game object scope, it will not be affected by changes to the global scope value.) Game parameter values set with global scope are applied to all 
		/// game objects that not yet registered, or already registered but not overridden with a value with game object scope.
		/// To set a game parameter value with global scope, pass AK_INVALID_GAME_OBJECT as the game object. 
		/// With this function, you may also change the value of a game parameter over time. To do so, specify a non-zero 
		/// value for \c in_uValueChangeDuration. At each audio frame, the game parameter value will be updated internally 
		/// according to the interpolation curve. If you call SetRTPCValue() with in_uValueChangeDuration = 0 in the 
		/// middle of an interpolation, the interpolation stops and the new value is set directly. Thus, if you call this 
		/// function at every game frame, you should not use in_uValueChangeDuration, as it would have no effect and it is less efficient.
		/// See \ref soundengine_rtpc_pergameobject, \ref soundengine_rtpc_buses and 
		/// \ref soundengine_rtpc_effects for more details on RTPC scope.
		/// \return 
		/// - \c AK_Success if the value was successfully set
		/// - \c AK_InvalidFloatValue if the value specified was NaN, Inf or FLT_MAX (3.402823e+38)	
		/// - \c AK_InvalidID if in_pszRtpcName is NULL.
		/// \aknote Strings are case-insensitive. \endaknote
		/// \sa 
		/// - \ref soundengine_rtpc
        extern   AKRESULT __cdecl  SetRTPCValue  ( 
			const char* in_pszRtpcName,								///< Name of the game parameter
			AkRtpcValue in_value, 									///< Value to set
			AkGameObjectID in_gameObjectID = AK_INVALID_GAME_OBJECT,///< Associated game object ID
			AkTimeMs in_uValueChangeDuration = 0,					///< Duration during which the game parameter is interpolated towards in_value
			AkCurveInterpolation in_eFadeCurve = AkCurveInterpolation_Linear,	///< Curve type to be used for the game parameter interpolation
			bool in_bBypassInternalValueInterpolation = false		///< True if you want to bypass the internal "slew rate" or "over time filtering" specified by the sound designer. This is meant to be used when for example loading a level and you dont want the values to interpolate.
		    );

		/// Sets the value of a real-time parameter control (by ID).
		/// With this function, you may set a game parameter value on playing id scope. 
		/// Playing id scope supersedes both game object scope and global scope. 
		/// With this function, you may also change the value of a game parameter over time. To do so, specify a non-zero 
		/// value for in_uValueChangeDuration. At each audio frame, the game parameter value will be updated internally 
		/// according to the interpolation curve. If you call SetRTPCValueByPlayingID() with in_uValueChangeDuration = 0 in the 
		/// middle of an interpolation, the interpolation stops and the new value is set directly. Thus, if you call this 
		/// function at every game frame, you should not use in_uValueChangeDuration, as it would have no effect and it is less efficient.
		/// See \ref soundengine_rtpc_pergameobject, \ref soundengine_rtpc_buses and 
		/// \ref soundengine_rtpc_effects for more details on RTPC scope.
		/// - \c AK_Success if successful
		/// - \c AK_PlayingIDNotFound if in_playingID is not found.
		/// - \c AK_InvalidID if in_pszRtpcName is NULL.
		/// - \c AK_InvalidFloatValue if the value specified was NaN, Inf or FLT_MAX (3.402823e+38)
		/// \sa 
		/// - \ref soundengine_rtpc
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
		extern   AKRESULT __cdecl  SetRTPCValueByPlayingID  ( 
			AkRtpcID in_rtpcID, 									///< ID of the game parameter
			AkRtpcValue in_value, 									///< Value to set
			AkPlayingID in_playingID,								///< Associated playing ID
			AkTimeMs in_uValueChangeDuration = 0,					///< Duration during which the game parameter is interpolated towards in_value
			AkCurveInterpolation in_eFadeCurve = AkCurveInterpolation_Linear,	///< Curve type to be used for the game parameter interpolation
			bool in_bBypassInternalValueInterpolation = false		///< True if you want to bypass the internal "slew rate" or "over time filtering" specified by the sound designer. This is meant to be used when, for example, loading a level and you don't want the values to interpolate.
			);

#ifdef AK_SUPPORT_WCHAR
		/// Sets the value of a real-time parameter control (by Unicode string name).
		/// With this function, you may set a game parameter value on playing ID scope. 
		/// Playing id scope supersedes both game object scope and global scope. 
		/// With this function, you may also change the value of a game parameter over time. To do so, specify a non-zero 
		/// value for in_uValueChangeDuration. At each audio frame, the game parameter value will be updated internally 
		/// according to the interpolation curve. If you call SetRTPCValueByPlayingID() with in_uValueChangeDuration = 0 in the 
		/// middle of an interpolation, the interpolation stops and the new value is set directly. Thus, if you call this 
		/// function at every game frame, you should not use in_uValueChangeDuration, as it would have no effect and it is less efficient.
		/// See \ref soundengine_rtpc_pergameobject, \ref soundengine_rtpc_buses and 
		/// \ref soundengine_rtpc_effects for more details on RTPC scope.
		/// - \c AK_Success if successful
		/// - \c AK_PlayingIDNotFound if in_playingID is not found.
		/// - \c AK_InvalidID if in_pszRtpcName is NULL.
		/// - \c AK_InvalidFloatValue if the value specified was NaN, Inf or FLT_MAX (3.402823e+38)
		/// \sa 
		/// - \ref soundengine_rtpc
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
		extern   AKRESULT __cdecl  SetRTPCValueByPlayingID  ( 
			const wchar_t* in_pszRtpcName,							///< Name of the game parameter
			AkRtpcValue in_value, 									///< Value to set
			AkPlayingID in_playingID,								///< Associated playing ID
			AkTimeMs in_uValueChangeDuration = 0,					///< Duration during which the game parameter is interpolated towards in_value
			AkCurveInterpolation in_eFadeCurve = AkCurveInterpolation_Linear,	///< Curve type to be used for the game parameter interpolation
			bool in_bBypassInternalValueInterpolation = false		///< True if you want to bypass the internal "slew rate" or "over time filtering" specified by the sound designer. This is meant to be used when, for example, loading a level and you don't want the values to interpolate.
			);
#endif //AK_SUPPORT_WCHAR
		
		/// Sets the value of a real-time parameter control (by string name).
		/// With this function, you may set a game parameter value on playing id scope. 
		/// Playing id scope supersedes both game object scope and global scope. 
		/// With this function, you may also change the value of a game parameter over time. To do so, specify a non-zero 
		/// value for in_uValueChangeDuration. At each audio frame, the game parameter value will be updated internally 
		/// according to the interpolation curve. If you call SetRTPCValueByPlayingID() with in_uValueChangeDuration = 0 in the 
		/// middle of an interpolation, the interpolation stops and the new value is set directly. Thus, if you call this 
		/// function at every game frame, you should not use in_uValueChangeDuration, as it would have no effect and it is less efficient.
		/// See \ref soundengine_rtpc_pergameobject, \ref soundengine_rtpc_buses and 
		/// \ref soundengine_rtpc_effects for more details on RTPC scope.
		/// - \c AK_Success if successful
		/// - \c AK_PlayingIDNotFound if in_playingID is not found.
		/// - \c AK_InvalidID if in_pszRtpcName is NULL.
		/// - \c AK_InvalidFloatValue if the value specified was NaN, Inf or FLT_MAX (3.402823e+38)
		/// \sa 
		/// - \ref soundengine_rtpc
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
		extern   AKRESULT __cdecl  SetRTPCValueByPlayingID  ( 
			const char* in_pszRtpcName,								///< Name of the game parameter
			AkRtpcValue in_value, 									///< Value to set
			AkPlayingID in_playingID,								///< Associated playing ID
			AkTimeMs in_uValueChangeDuration = 0,					///< Duration during which the game parameter is interpolated towards in_value
			AkCurveInterpolation in_eFadeCurve = AkCurveInterpolation_Linear,	///< Curve type to be used for the game parameter interpolation
			bool in_bBypassInternalValueInterpolation = false		///< True if you want to bypass the internal "slew rate" or "over time filtering" specified by the sound designer. This is meant to be used when for example loading a level and you dont want the values to interpolate.
			);

		/// Resets the value of the game parameter to its default value, as specified in the Wwise project.
		/// With this function, you may reset a game parameter to its default value with global scope or with game object scope. 
		/// Game object scope supersedes global scope. Game parameter values reset with global scope are applied to all 
		/// game objects that were not overridden with a value with game object scope.
		/// To reset a game parameter value with global scope, pass AK_INVALID_GAME_OBJECT as the game object. 
		/// With this function, you may also reset the value of a game parameter over time. To do so, specify a non-zero 
		/// value for in_uValueChangeDuration. At each audio frame, the game parameter value will be updated internally 
		/// according to the interpolation curve. If you call SetRTPCValue() or ResetRTPCValue() with in_uValueChangeDuration = 0 in the 
		/// middle of an interpolation, the interpolation stops and the new value is set directly. 
		/// See \ref soundengine_rtpc_pergameobject, \ref soundengine_rtpc_buses and 
		/// \ref soundengine_rtpc_effects for more details on RTPC scope.
		/// \return 
		/// - \c AK_Success when successful
		/// - \c AK_InvalidID if in_rtpcID is AK_INVALID_UNIQUE_ID (0)
		/// \sa 
		/// - \ref soundengine_rtpc
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
		/// - <tt>AK::SoundEngine::SetRTPCValue()</tt>
		extern   AKRESULT __cdecl  ResetRTPCValue  (
			AkRtpcID in_rtpcID, 									///< ID of the game parameter
			AkGameObjectID in_gameObjectID = AK_INVALID_GAME_OBJECT,///< Associated game object ID
			AkTimeMs in_uValueChangeDuration = 0,					///< Duration during which the game parameter is interpolated towards its default value
			AkCurveInterpolation in_eFadeCurve = AkCurveInterpolation_Linear,	///< Curve type to be used for the game parameter interpolation
			bool in_bBypassInternalValueInterpolation = false		///< True if you want to bypass the internal "slew rate" or "over time filtering" specified by the sound designer. This is meant to be used when for example loading a level and you dont want the values to interpolate.
			);

#ifdef AK_SUPPORT_WCHAR
		/// Resets the value of the game parameter to its default value, as specified in the Wwise project.
		/// With this function, you may reset a game parameter to its default value with global scope or with game object scope. 
		/// Game object scope supersedes global scope. Game parameter values reset with global scope are applied to all 
		/// game objects that were not overridden with a value with game object scope.
		/// To reset a game parameter value with global scope, pass AK_INVALID_GAME_OBJECT as the game object. 
		/// With this function, you may also reset the value of a game parameter over time. To do so, specify a non-zero 
		/// value for in_uValueChangeDuration. At each audio frame, the game parameter value will be updated internally 
		/// according to the interpolation curve. If you call SetRTPCValue() or ResetRTPCValue() with in_uValueChangeDuration = 0 in the 
		/// middle of an interpolation, the interpolation stops and the new value is set directly. 
		/// See \ref soundengine_rtpc_pergameobject, \ref soundengine_rtpc_buses and 
		/// \ref soundengine_rtpc_effects for more details on RTPC scope.
		/// \return 
		/// - \c AK_Success if successful
		/// - \c AK_InvalidID if in_pszParamName is NULL.
		/// \aknote Strings are case-insensitive. \endaknote
		/// \sa 
		/// - \ref soundengine_rtpc
		/// - <tt>AK::SoundEngine::SetRTPCValue()</tt>
		extern   AKRESULT __cdecl  ResetRTPCValue  (
			const wchar_t* in_pszRtpcName,							///< Name of the game parameter
			AkGameObjectID in_gameObjectID = AK_INVALID_GAME_OBJECT,///< Associated game object ID
			AkTimeMs in_uValueChangeDuration = 0,					///< Duration during which the game parameter is interpolated towards its default value
			AkCurveInterpolation in_eFadeCurve = AkCurveInterpolation_Linear,	///< Curve type to be used for the game parameter interpolation
			bool in_bBypassInternalValueInterpolation = false		///< True if you want to bypass the internal "slew rate" or "over time filtering" specified by the sound designer. This is meant to be used when for example loading a level and you dont want the values to interpolate.
			);
#endif //AK_SUPPORT_WCHAR

		/// Resets the value of the game parameter to its default value, as specified in the Wwise project.
		/// With this function, you may reset a game parameter to its default value with global scope or with game object scope. 
		/// Game object scope supersedes global scope. Game parameter values reset with global scope are applied to all 
		/// game objects that were not overridden with a value with game object scope.
		/// To reset a game parameter value with global scope, pass AK_INVALID_GAME_OBJECT as the game object. 
		/// With this function, you may also reset the value of a game parameter over time. To do so, specify a non-zero 
		/// value for in_uValueChangeDuration. At each audio frame, the game parameter value will be updated internally 
		/// according to the interpolation curve. If you call SetRTPCValue() or ResetRTPCValue() with in_uValueChangeDuration = 0 in the 
		/// middle of an interpolation, the interpolation stops and the new value is set directly. 
		/// See \ref soundengine_rtpc_pergameobject, \ref soundengine_rtpc_buses and 
		/// \ref soundengine_rtpc_effects for more details on RTPC scope.
		/// \return 
		/// - \c AK_Success if successful
		/// - \c AK_InvalidID if in_pszParamName is NULL.
		/// \aknote Strings are case-insensitive. \endaknote
		/// \sa 
		/// - \ref soundengine_rtpc
		/// - <tt>AK::SoundEngine::SetRTPCValue()</tt>
		extern   AKRESULT __cdecl  ResetRTPCValue  (
			const char* in_pszRtpcName,								///< Name of the game parameter
			AkGameObjectID in_gameObjectID = AK_INVALID_GAME_OBJECT,///< Associated game object ID
			AkTimeMs in_uValueChangeDuration = 0,					///< Duration during which the game parameter is interpolated towards its default value
			AkCurveInterpolation in_eFadeCurve = AkCurveInterpolation_Linear,	///< Curve type to be used for the game parameter interpolation
			bool in_bBypassInternalValueInterpolation = false		///< True if you want to bypass the internal "slew rate" or "over time filtering" specified by the sound designer. This is meant to be used when for example loading a level and you dont want the values to interpolate.
			);

		/// Resets the value of the game parameter to its default value, as specified in the Wwise project.
		/// With this function, you may reset a game parameter to its default value on playing id scope.
		/// Playing id scope supersedes both game object scope and global scope. 
		/// With this function, you may also reset the value of a game parameter over time. To do so, specify a non-zero 
		/// value for in_uValueChangeDuration. At each audio frame, the game parameter value will be updated internally 
		/// according to the interpolation curve.
		///  If you call SetRTPCValueByPlayingID() or ReetRTPCValueByPlayingID() with in_uValueChangeDuration = 0 in the 
		/// middle of an interpolation, the interpolation stops and the new value is set directly. 
		/// See \ref soundengine_rtpc_pergameobject, \ref soundengine_rtpc_buses and 
		/// \ref soundengine_rtpc_effects for more details on RTPC scope.
		/// \return 
		/// - \c AK_Success when successful
		/// - \c AK_InvalidID if in_playingID is AK_INVALID_PLAYING_ID (0)
		/// \sa 
		/// - \ref soundengine_rtpc
		/// - <tt>AK::SoundEngine::SetRTPCValueByPlayingID()</tt>
		extern  AKRESULT __cdecl  ResetRTPCValueByPlayingID (
			AkRtpcID in_rtpcID, 									///< ID of the game parameter
			AkPlayingID in_playingID,								///< Associated playing ID
			AkTimeMs in_uValueChangeDuration = 0,					///< Duration during which the game parameter is interpolated towards its default value
			AkCurveInterpolation in_eFadeCurve = AkCurveInterpolation_Linear,	///< Curve type to be used for the game parameter interpolation
			bool in_bBypassInternalValueInterpolation = false		///< True if you want to bypass the internal "slew rate" or "over time filtering" specified by the sound designer. This is meant to be used when for example loading a level and you dont want the values to interpolate.
			);

#ifdef AK_SUPPORT_WCHAR
		/// Resets the value of the game parameter to its default value, as specified in the Wwise project.
		/// With this function, you may reset a game parameter to its default value on playing id scope.
		/// Playing id scope supersedes both game object scope and global scope. 
		/// With this function, you may also reset the value of a game parameter over time. To do so, specify a non-zero 
		/// value for in_uValueChangeDuration. At each audio frame, the game parameter value will be updated internally 
		/// according to the interpolation curve.
		///  If you call SetRTPCValueByPlayingID() or ReetRTPCValueByPlayingID() with in_uValueChangeDuration = 0 in the 
		/// middle of an interpolation, the interpolation stops and the new value is set directly. 
		/// See \ref soundengine_rtpc_pergameobject, \ref soundengine_rtpc_buses and 
		/// \ref soundengine_rtpc_effects for more details on RTPC scope.
		/// \return 
		/// - \c AK_Success when successful
		/// - \c AK_InvalidID if in_playingID is AK_INVALID_PLAYING_ID (0) or if if in_pszParamName is NULL.
		/// \aknote Strings are case-insensitive. \endaknote
		/// \sa 
		/// - \ref soundengine_rtpc
		/// - <tt>AK::SoundEngine::SetRTPCValueByPlayingID()</tt>
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
		extern  AKRESULT __cdecl  ResetRTPCValueByPlayingID (
			const wchar_t* in_pszRtpcName,							///< Name of the game parameter
			AkPlayingID in_playingID,								///< Associated playing ID
			AkTimeMs in_uValueChangeDuration = 0,					///< Duration during which the game parameter is interpolated towards in_value
			AkCurveInterpolation in_eFadeCurve = AkCurveInterpolation_Linear,	///< Curve type to be used for the game parameter interpolation
			bool in_bBypassInternalValueInterpolation = false		///< True if you want to bypass the internal "slew rate" or "over time filtering" specified by the sound designer. This is meant to be used when for example loading a level and you dont want the values to interpolate.
			);
#endif //AK_SUPPORT_WCHAR

		/// Resets the value of the game parameter to its default value, as specified in the Wwise project.
		/// With this function, you may reset a game parameter to its default value on playing id scope.
		/// Playing id scope supersedes both game object scope and global scope. 
		/// With this function, you may also reset the value of a game parameter over time. To do so, specify a non-zero 
		/// value for in_uValueChangeDuration. At each audio frame, the game parameter value will be updated internally 
		/// according to the interpolation curve.
		///  If you call SetRTPCValueByPlayingID() or ReetRTPCValueByPlayingID() with in_uValueChangeDuration = 0 in the 
		/// middle of an interpolation, the interpolation stops and the new value is set directly. 
		/// See \ref soundengine_rtpc_pergameobject, \ref soundengine_rtpc_buses and 
		/// \ref soundengine_rtpc_effects for more details on RTPC scope.
		/// \return 
		/// - \c AK_Success when successful
		/// - \c AK_InvalidID if in_playingID is AK_INVALID_PLAYING_ID (0) or if if in_pszParamName is NULL.
		/// \aknote Strings are case-insensitive. \endaknote
		/// \sa 
		/// - \ref soundengine_rtpc
		/// - <tt>AK::SoundEngine::SetRTPCValueByPlayingID()</tt>
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt> 
		extern  AKRESULT __cdecl  ResetRTPCValueByPlayingID (
			const char* in_pszRtpcName,								///< Name of the game parameter
			AkPlayingID in_playingID,								///< Associated playing ID
			AkTimeMs in_uValueChangeDuration = 0,					///< Duration during which the game parameter is interpolated towards its default value
			AkCurveInterpolation in_eFadeCurve = AkCurveInterpolation_Linear,	///< Curve type to be used for the game parameter interpolation
			bool in_bBypassInternalValueInterpolation = false		///< True if you want to bypass the internal "slew rate" or "over time filtering" specified by the sound designer. This is meant to be used when for example loading a level and you dont want the values to interpolate.
			);

		/// Sets the State of a Switch Group (by IDs).
		/// \return Always returns AK_Success
		/// \sa 
		/// - \ref soundengine_switch
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
        extern   AKRESULT __cdecl  SetSwitch  ( 
			AkSwitchGroupID in_switchGroup, 			///< ID of the Switch Group
			AkSwitchStateID in_switchState, 			///< ID of the Switch
			AkGameObjectID in_gameObjectID				///< Associated game object ID
		    );

#ifdef AK_SUPPORT_WCHAR
		/// Sets the State of a Switch Group (by Unicode string names).
		/// \return 
		/// - \c AK_Success if successful
		/// - \c AK_InvalidID if the switch or Switch Group name was not resolved to an existing ID\n
		/// Make sure that the banks were generated with the "include string" option.
		/// \aknote Strings are case-insensitive. \endaknote
		/// \sa 
		/// - \ref soundengine_switch
        extern   AKRESULT __cdecl  SetSwitch  ( 
			const wchar_t* in_pszSwitchGroup,			///< Name of the Switch Group
			const wchar_t* in_pszSwitchState, 			///< Name of the Switch
			AkGameObjectID in_gameObjectID				///< Associated game object ID
			);
#endif //AK_SUPPORT_WCHAR

		/// Sets the state of a Switch Group.
		/// \return 
		/// - \c AK_Success if successful
		/// - \c AK_InvalidID if the switch or Switch Group name was not resolved to an existing ID\n
		/// Make sure that the banks were generated with the "include string" option.
		/// \aknote Strings are case-insensitive. \endaknote
		/// \sa 
		/// - \ref soundengine_switch
        extern   AKRESULT __cdecl  SetSwitch  ( 
			const char* in_pszSwitchGroup,				///< Name of the Switch Group
			const char* in_pszSwitchState, 				///< Name of the Switch
			AkGameObjectID in_gameObjectID				///< Associated game object ID
			);

		/// Post the specified trigger (by IDs).
		/// \return Always returns AK_Success
		/// \sa 
		/// - \ref soundengine_triggers
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
		extern   AKRESULT __cdecl  PostTrigger  ( 
			AkTriggerID 	in_triggerID, 				///< ID of the trigger
			AkGameObjectID 	in_gameObjectID				///< Associated game object ID
		    );

#ifdef AK_SUPPORT_WCHAR
		/// Posts the specified trigger (by Unicode string name).
		/// \return 
		/// - \c AK_Success if successful
		/// - \c AK_InvalidID if the trigger name was null
		/// Make sure that the banks were generated with the "include string" option.
		/// \aknote Strings are case-insensitive. \endaknote
		/// \sa 
		/// - \ref soundengine_triggers
        extern   AKRESULT __cdecl  PostTrigger  ( 
			const wchar_t* in_pszTrigger,				///< Name of the trigger
			AkGameObjectID in_gameObjectID				///< Associated game object ID
			);
#endif //AK_SUPPORT_WCHAR

		/// Posts the specified trigger.
		/// \return 
		/// - \c AK_Success if successful
		/// - \c AK_InvalidID if the trigger name was null
		/// Make sure that the banks were generated with the "include string" option.
		/// \aknote Strings are case-insensitive. \endaknote
		/// \sa 
		/// - \ref soundengine_triggers
        extern   AKRESULT __cdecl  PostTrigger  ( 
			const char* in_pszTrigger,			 	    ///< Name of the trigger
			AkGameObjectID in_gameObjectID				///< Associated game object ID
			);

		/// Sets the state of a State Group (by IDs).
		/// \return Always returns AK_Success
		/// \sa 
		/// - \ref soundengine_states
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
        extern   AKRESULT __cdecl  SetState  ( 
			AkStateGroupID in_stateGroup, 				///< ID of the State Group
			AkStateID in_state 							///< ID of the state
		    );

#ifdef AK_SUPPORT_WCHAR
		/// Sets the state of a State Group (by Unicode string names).
		/// \return 
		/// - \c AK_Success if successful
		/// - \c AK_InvalidID if the state or State Group name was null
		/// Make sure that the banks were generated with the "include string" option.
		/// \aknote Strings are case-insensitive. \endaknote
		/// \sa 
		/// - \ref soundengine_states
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
        extern   AKRESULT __cdecl  SetState  ( 
			const wchar_t* in_pszStateGroup,				///< Name of the State Group
			const wchar_t* in_pszState 						///< Name of the state
			);
#endif //AK_SUPPORT_WCHAR

		/// Sets the state of a State Group.
		/// \return 
		/// - \c AK_Success if successful
		/// - \c AK_InvalidID if the state or State Group name was null
		/// Make sure that the banks were generated with the "include string" option.
		/// \aknote Strings are case-insensitive. \endaknote
		/// \sa 
		/// - \ref soundengine_states
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
        extern   AKRESULT __cdecl  SetState  ( 
			const char* in_pszStateGroup,					///< Name of the State Group
			const char* in_pszState 						///< Name of the state
			);

		//@}

		////////////////////////////////////////////////////////////////////////
		/// @name Environments
		//@{

		/// Sets the Auxiliary Busses to route the specified game object
		/// To clear the game object's auxiliary sends, \c in_uNumSendValues must be 0.
		/// \sa 
		/// - \ref soundengine_environments
		/// - \ref soundengine_environments_dynamic_aux_bus_routing
		/// - \ref soundengine_environments_id_vs_string
		/// - <tt>AK::SoundEngine::GetIDFromString()</tt>
		/// \return 
		/// - \c AK_Success if successful
		/// - \c AK_InvalidParameter if a duplicated environment is found in the array
		/// - \c AK_InvalidFloatValue if the value specified was NaN or Inf
		extern   AKRESULT __cdecl  SetGameObjectAuxSendValues  ( 
			AkGameObjectID		in_gameObjectID,		///< Associated game object ID
			AkAuxSendValue*		in_aAuxSendValues,		///< Variable-size array of AkAuxSendValue structures
														///< (it may be NULL if no environment must be set)
			AkUInt32			in_uNumSendValues		///< The number of auxiliary busses at the pointer's address
														///< (it must be 0 if no environment is set)
			);

		/// Registers a callback to allow the game to modify or override the volume to be applied at the output of an audio bus.
		/// The callback must be registered once per bus ID.
		/// Call with <tt>in_pfnCallback = NULL</tt> to unregister.
		/// \aknote The bus in_busID needs to be a mixing bus.\endaknote
		/// \aknote The callback function will not be called for the Main Audio Bus, since the output of this bus is not a bus, but is instead an Audio Device.\endaknote
		/// \sa 
		/// - \ref goingfurther_speakermatrixcallback
		/// - \ref soundengine_environments
		/// - AkSpeakerVolumeMatrixCallbackInfo
		/// - <tt>AK::IAkMixerInputContext</tt>
		/// - <tt>AK::IAkMixerPluginContext</tt>
		/// \return 
		/// - \c AK_Success if successful
		/// - \c AK_IDNotFound if the bus is not found
		/// - \c AK_NotInitialized if the sound engine is not initialized
		/// - \c AK_InsufficientMemory if there is not enough memory to complete the operation
		extern   AKRESULT __cdecl  RegisterBusVolumeCallback  ( 
			AkUniqueID in_busID,						///< Bus ID, as obtained by GetIDFromString( bus_name ).
			AkBusCallbackFunc in_pfnCallback,			///< Callback function.
			void* in_pCookie = NULL						///< User cookie.
			);

		/// Registers a callback to be called to allow the game to access metering data from any mixing bus. You may use this to monitor loudness at any point of the mixing hierarchy 
		/// by accessing the peak, RMS, True Peak and K-weighted power (according to loudness standard ITU BS.1770). See \ref goingfurther_speakermatrixcallback for an example.
		/// The callback must be registered once per bus ID.
		/// Call with in_pfnCallback = NULL or in_eMeteringFlags = AK_NoMetering to unregister.
		/// \aknote The bus in_busID needs to be a mixing bus.\endaknote
		/// \sa 
		/// - \ref goingfurther_speakermatrixcallback
		/// - AkBusMeteringCallbackFunc
		/// - <tt>AK::AkMetering</tt>
		/// \return 
		/// - \c AK_Success if successful
		/// - \c AK_IDNotFound if the bus is not found
		/// - \c AK_NotInitialized if the sound engine is not initialized
		/// - \c AK_InsufficientMemory if there is not enough memory to complete the operation
		extern   AKRESULT __cdecl  RegisterBusMeteringCallback  ( 
			AkUniqueID in_busID,						///< Bus ID, as obtained by GetIDFromString( bus_name ).
			AkBusMeteringCallbackFunc in_pfnCallback,	///< Callback function.
			AkMeteringFlags in_eMeteringFlags,			///< Metering flags.
			void* in_pCookie = NULL						///< User cookie.
			);

		/// Registers a callback to be called to allow the game to access metering data from any output device. You may use this to monitor loudness as sound leaves the Wwise sound engine 
		/// by accessing the peak, RMS, True Peak and K-weighted power (according to loudness standard ITU BS.1770). See \ref goingfurther_speakermatrixcallback for an example.
		/// The callback must be registered once per device shareset ID.
		/// Call with in_pfnCallback = NULL or in_eMeteringFlags = AK_NoMetering to unregister.
		/// \sa 
		/// - \ref goingfurther_speakermatrixcallback
		/// - AkOutputDeviceMeteringCallbackFunc
		/// - <tt>AK::AkMetering</tt>
		/// \return 
		/// - \c AK_Success if successful
		/// - \c AK_DeviceNotFound if the device is not found
		/// - \c AK_NotInitialized if the sound engine is not initialized
		/// - \c AK_InsufficientMemory if there is not enough memory to complete the operation
		extern   AKRESULT __cdecl  RegisterOutputDeviceMeteringCallback  ( 
			AkOutputDeviceID in_idOutput,				///< Output ID, as returned from AddOutput or GetOutputID.  You can pass 0 for the main (default) output
			AkOutputDeviceMeteringCallbackFunc in_pfnCallback,	///< Callback function.
			AkMeteringFlags in_eMeteringFlags,			///< Metering flags.
			void* in_pCookie = NULL						///< User cookie.
			);

		/// Sets the Output Bus Volume (direct) to be used for the specified game object.
		/// The control value is a number ranging from 0.0f to 1.0f.
		/// Output Bus Volumes are stored per listener association, so calling this function will override the default set of listeners. The game object in_emitterObjID will now reference its own set of listeners which will 
		/// be the same as the old set of listeners, but with the new associated gain.  Future changes to the default listener set will not be picked up by this game object unless ResetListenersToDefault() is called.
		/// \sa 
		/// - \ref AK::SoundEngine::ResetListenersToDefault
		/// - \ref soundengine_environments
		/// - \ref soundengine_environments_setting_dry_environment
		/// - \ref soundengine_environments_id_vs_string
		/// \return 
		/// - \c AK_Success when successful
		/// - \c AK_InvalidFloatValue if the value specified was NaN or Inf
		extern   AKRESULT __cdecl  SetGameObjectOutputBusVolume  ( 
			AkGameObjectID		in_emitterObjID,		///< Associated emitter game object ID
			AkGameObjectID		in_listenerObjID,		///< Associated listener game object ID. Pass AK_INVALID_GAME_OBJECT to set the Output Bus Volume for all connected listeners.
			AkReal32			in_fControlValue		///< A multiplier in the range [0.0f:16.0f] ( -∞ dB to +24 dB).																			
														///< A value greater than 1.0f will amplify the sound. 
			);

		/// Sets an Effect ShareSet at the specified audio node and Effect slot index.
		/// \aknote
		/// Replacing effects is preferably done through a Set Effect Event Action.
		/// \endaknote
		/// The target node cannot be a Bus, to set effects on a bus, use SetBusEffect() instead.
		/// \aknote The option "Override Parent" in 
		/// the Effect section in Wwise must be enabled for this node, otherwise the parent's effect will 
		/// still be the one in use and the call to SetContainerEffect will have no impact.
		/// \endaknote
		/// \return Always returns AK_Success
		extern   AKRESULT __cdecl  SetContainerEffect (
			AkUniqueID in_audioNodeID,					///< Can be a member of the Containers Hierarchy (not a bus).
			AkUInt32 in_uFXIndex,						///< Effect slot index (0-254)
			AkUniqueID in_shareSetID					///< ShareSets ID; pass AK_INVALID_UNIQUE_ID to clear the effect slot
			);


		/// Sets an Effect ShareSet at the specified audio node and Effect slot index.
		/// \aknote 
		///	This function is deprecated. Use SetContainerEffect instead.
		/// \endaknote
		/// \aknote
		/// Replacing effects is preferably done through a Set Effect Event Action.
		/// \endaknote
		/// The target node cannot be a Bus, to set effects on a bus, use SetBusEffect() instead.
		/// \aknote The option "Override Parent" in 
		/// the Effect section in Wwise must be enabled for this node, otherwise the parent's effect will 
		/// still be the one in use and the call to SetActorMixerEffect will have no impact.
		/// \endaknote
		/// \return Always returns AK_Success
		extern   AKRESULT __cdecl  SetActorMixerEffect  (
			AkUniqueID in_audioNodeID,					///< Can be a member of the Containers Hierarchy (not a bus).
			AkUInt32 in_uFXIndex,						///< Effect slot index (0-254)
			AkUniqueID in_shareSetID					///< ShareSets ID; pass AK_INVALID_UNIQUE_ID to clear the effect slot
			);

		/// Sets an Effect ShareSet at the specified bus and Effect slot index.
		/// \aknote
		/// Replacing effects is preferably done through a Set Effect Event Action.
		/// \endaknote
		/// The Bus can either be an Audio Bus or an Auxiliary Bus.
		/// This adds a reference on the audio node to an existing ShareSet.
		/// \aknote This function has unspecified behavior when adding an Effect to a currently playing
		/// Bus which does not have any Effects, or removing the last Effect on a currently playing bus.
		/// \endaknote
		/// \aknote This function will replace existing Effects on the node. If the target node is not at 
		/// the top of the hierarchy and is in the Containers hierarchy, the option "Override Parent" in 
		/// the Effect section in Wwise must be enabled for this node, otherwise the parent's Effect will 
		/// still be the one in use and the call to SetBusEffect will have no impact.
		/// \endaknote
		/// \return
		/// - \c AK_Success when successfully posted.
		/// - \c AK_IDNotFound if the Bus isn't found by in_audioNodeID
		/// - \c AK_InvalidParameter if in_uFXIndex isn't in range
		extern   AKRESULT __cdecl  SetBusEffect  ( 
			AkUniqueID in_audioNodeID,					///< Bus Short ID.
			AkUInt32 in_uFXIndex,						///< Effect slot index (0-254)
			AkUniqueID in_shareSetID					///< ShareSets ID; pass AK_INVALID_UNIQUE_ID to clear the Effect slot
			);

#ifdef AK_SUPPORT_WCHAR
		/// Sets an Effect ShareSet at the specified Bus and Effect slot index.
		/// \aknote
		/// Replacing effects is preferably done through a Set Effect Event Action.
		/// \endaknote
		/// The Bus can either be an Audio Bus or an Auxiliary Bus.
		/// This adds a reference on the audio node to an existing ShareSet.
		/// \aknote This function has unspecified behavior when adding an Effect to a currently playing
		/// bus which does not have any Effects, or removing the last Effect on a currently playing Bus.
		/// \endaknote
		/// \aknote Make sure the new effect shareset is included in a soundbank, and that sound bank is
		/// loaded. Otherwise you will see errors in the Capture Log.
		/// \endaknote
		/// \aknote This function will replace existing Effects on the node. If the target node is not at 
		/// the top of the hierarchy and is in the Containers, the option "Override Parent" in 
		/// the Effect section in Wwise must be enabled for this node, otherwise the parent's Effect will 
		/// still be the one in use and the call to \c SetBusEffect will have no impact.
		/// \endaknote
		/// \returns 
		/// - \c AK_Success when successfully posted.
		/// - \c AK_IDNotFound if the Bus name doesn't point to a valid bus.
		/// - \c AK_InvalidParameter if in_uFXIndex isn't in range
		/// - \c AK_InvalidID if in_pszBusName is null
		extern   AKRESULT __cdecl  SetBusEffect  ( 
			const wchar_t* in_pszBusName,				///< Bus name
			AkUInt32 in_uFXIndex,						///< Effect slot index (0-254)
			AkUniqueID in_shareSetID					///< ShareSets ID; pass AK_INVALID_UNIQUE_ID to clear the effect slot
			);
#endif //AK_SUPPORT_WCHAR

		/// Sets an Effect ShareSet at the specified Bus and Effect slot index.
		/// \aknote
		/// Replacing effects is preferably done through a Set Effect Event Action.
		/// \endaknote
		/// The Bus can either be an Audio Bus or an Auxiliary Bus.
		/// This adds a reference on the audio node to an existing ShareSet.
		/// \aknote This function has unspecified behavior when adding an Effect to a currently playing
		/// Bus which does not have any effects, or removing the last Effect on a currently playing bus.
		/// \endaknote
		/// \aknote Make sure the new effect shareset is included in a soundbank, and that sound bank is
		/// loaded. Otherwise you will see errors in the Capture Log.
		/// \endaknote
		/// \aknote This function will replace existing Effects on the node. If the target node is not at 
		/// the top of the hierarchy and is in the Containers, the option "Override Parent" in 
		/// the Effect section in Wwise must be enabled for this node, otherwise the parent's Effect will 
		/// still be the one in use and the call to \c SetBusEffect will have no impact.
		/// \endaknote
		/// \returns 
		/// - \c AK_Success when successfully posted.
		/// - \c AK_IDNotFound if the Bus name doesn't point to a valid bus.
		/// - \c AK_InvalidParameter if in_uFXIndex isn't in range
		/// - \c AK_InvalidID if in_pszBusName is null
		extern   AKRESULT __cdecl  SetBusEffect  ( 
			const char* in_pszBusName,		///< Bus name
			AkUInt32 in_uFXIndex,			///< Effect slot index (0-254)
			AkUniqueID in_shareSetID		///< ShareSets ID; pass AK_INVALID_UNIQUE_ID to clear the effect slot
			);

		/// Sets an audio device effect shareset on the specified output device and effect slot index.
		/// \aknote
		/// Replacing effects is preferably done through a Set Effect Event Action.
		/// \endaknote
		/// \aknote Make sure the new effect shareset is included in a soundbank, and that sound bank is loaded. Otherwise you will see errors in the Capture Log.\endaknote
		/// \aknote This function will replace existing effects of the audio device shareset. \endaknote
		/// \aknote Audio device effects support is limited to one shareset per plug-in type at any time. \endaknote
		/// \aknote Errors are reported in the Wwise Capture Log if the effect cannot be set on the output device. \endaknote

		/// \returns Always returns AK_Success
		extern  AKRESULT __cdecl  SetOutputDeviceEffect (
			AkOutputDeviceID in_outputDeviceID,				///< Output ID, as returned from AddOutput or GetOutputID. Most of the time this should be 0 to designate the main (default) output
			AkUInt32 in_uFXIndex,							///< Effect slot index (0-254)
			AkUniqueID in_FXShareSetID						///< Effect ShareSet ID
			);

		/// Forces channel configuration for the specified bus.
		/// \aknote You cannot change the configuration of the master bus.\endaknote
		/// 
		/// \return Always returns AK_Success
		extern  AKRESULT __cdecl  SetBusConfig (
			AkUniqueID in_audioNodeID,					///< Bus Short ID.
			AkChannelConfig in_channelConfig			///< Desired channel configuration. An invalid configuration (from default constructor) means "as parent".
			);

#ifdef AK_SUPPORT_WCHAR
		/// Forces channel configuration for the specified bus.
		/// \aknote You cannot change the configuration of the master bus.\endaknote
		/// 
		/// \returns 
		/// - \c AK_Success when successful
		/// - \c AK_InvalidID if in_pszBusName is null
		extern  AKRESULT __cdecl  SetBusConfig (
			const wchar_t* in_pszBusName,				///< Bus name
			AkChannelConfig in_channelConfig			///< Desired channel configuration. An invalid configuration (from default constructor) means "as parent".
			);
#endif //AK_SUPPORT_WCHAR

		/// Forces channel configuration for the specified bus.
		/// \aknote You cannot change the configuration of the master bus.\endaknote
		/// 
		/// \returns 
		/// - \c AK_Success when successful
		/// - \c AK_InvalidID if in_pszBusName is null
		extern  AKRESULT __cdecl  SetBusConfig (
			const char* in_pszBusName,					///< Bus name
			AkChannelConfig in_channelConfig			///< Desired channel configuration. An invalid configuration (from default constructor) means "as parent".
			);

		/// Resets the channel configuration for the specified bus back to the value loaded from soundbanks.
		/// Useful to clear any value set by AK::SoundEngine::SetBusConfig.
		/// \aknote You cannot change the configuration of the master bus.\endaknote
		/// 
		/// \return Always returns AK_Success
		extern  AKRESULT __cdecl  ResetBusConfig (
			AkUniqueID in_audioNodeID					///< Bus Short ID.
			);

#ifdef AK_SUPPORT_WCHAR
		/// Resets the channel configuration for the specified bus back to the value loaded from soundbanks.
		/// Useful to clear any value set by AK::SoundEngine::SetBusConfig.
		/// \aknote You cannot change the configuration of the master bus.\endaknote
		/// 
		/// \returns 
		/// - \c AK_Success when successful
		/// - \c AK_InvalidID if in_pszBusName is null
		extern  AKRESULT __cdecl  ResetBusConfig (
			const wchar_t* in_pszBusName				///< Bus name
			);
#endif //AK_SUPPORT_WCHAR

		/// Resets the channel configuration for the specified bus back to the value loaded from soundbanks.
		/// Useful to clear any value set by AK::SoundEngine::SetBusConfig.
		/// \aknote You cannot change the configuration of the master bus.\endaknote
		/// 
		/// \returns 
		/// - \c AK_Success when successful
		/// - \c AK_InvalidID if in_pszBusName is null
		extern  AKRESULT __cdecl  ResetBusConfig (
			const char* in_pszBusName					///< Bus name
			);
		
		/// Forces channel configuration for the specified Sidechain Mix at run-time.
		///  
		/// Standard, ambisonic, and anonymous channel configs are supported.
		/// If a channel config of type AK_ChannelConfigType_UseDeviceMain or AK_ChannelConfigType_UseDevicePassthrough are specified,
		/// then the Primary Device's channel config for its Main Mix or Passthrough Mix is used, as appropriate.
		/// Audio Object channel configs are not supported for Sidechain Mixes.
		/// 
		/// An invalid channel configuration (from default constructor) can be used to clear any previously-set config set via the soundengine API, and revert to what was loaded from soundbanks.
		/// 
		/// \return Always returns AK_Success
		extern  AKRESULT __cdecl  SetSidechainMixConfig (
			AkUniqueID in_sidechainMixId,				///< SidechainMix Short ID.
			AkChannelConfig in_channelConfig			///< Desired channel configuration. 
			);

#ifdef AK_SUPPORT_WCHAR
		/// Forces channel configuration for the specified Sidechain Mix at run-time.
		///  
		/// Standard, ambisonic, and anonymous channel configs are supported.
		/// If a channel config of type AK_ChannelConfigType_UseDeviceMain or AK_ChannelConfigType_UseDevicePassthrough are specified,
		/// then the Primary Device's channel config for its Main Mix or Passthrough Mix is used, as appropriate.
		/// Audio Object channel configs are not supported for Sidechain Mixes.
		/// 
		/// An invalid channel configuration (from default constructor) can be used to clear any previously-set config set via the soundengine API, and revert to what was loaded from soundbanks.
		/// 
		/// \returns 
		/// - \c AK_Success when successful
		/// - \c AK_InvalidID if in_pszSidechainMixName is null
		extern  AKRESULT __cdecl  SetSidechainMixConfig (
			const wchar_t* in_pszSidechainMixName,		///< SidechainMix name
			AkChannelConfig in_channelConfig			///< Desired channel configuration. An invalid configuration (from default constructor) means "as parent".
			);
#endif //AK_SUPPORT_WCHAR

		/// Forces channel configuration for the specified Sidechain Mix at run-time.
		///  
		/// Standard, ambisonic, and anonymous channel configs are supported.
		/// If a channel config of type AK_ChannelConfigType_UseDeviceMain or AK_ChannelConfigType_UseDevicePassthrough are specified,
		/// then the Primary Device's channel config for its Main Mix or Passthrough Mix is used, as appropriate.
		/// Audio Object channel configs are not supported for Sidechain Mixes.
		/// 
		/// An invalid channel configuration (from default constructor) can be used to clear any previously-set config set via the soundengine API, and revert to what was loaded from soundbanks.
		/// 
		/// \returns 
		/// - \c AK_Success when successful
		/// - \c AK_InvalidID if in_pszSidechainMixName is null
		extern  AKRESULT __cdecl  SetSidechainMixConfig (
			const char* in_pszSidechainMixName,			///< SidechainMix name
			AkChannelConfig in_channelConfig			///< Desired channel configuration. An invalid configuration (from default constructor) means "as parent".
			);

		/// Sets a game object's obstruction and occlusion levels. If SetMultiplePositions were used, values are set for all positions.
		/// To assign a unique obstruction and occlusion value to each sound position, instead use AK::SoundEngine::SetMultipleObstructionAndOcclusion.
		/// This function is used to affect how an object should be heard by a specific listener.
		/// \sa 
		/// - \ref soundengine_obsocc
		/// - \ref soundengine_environments
		/// \return Always returns AK_Success
		extern   AKRESULT __cdecl  SetObjectObstructionAndOcclusion  (  
			AkGameObjectID in_EmitterID,		///< Emitter game object ID
			AkGameObjectID in_ListenerID,		///< Listener game object ID
			AkReal32 in_fObstructionLevel,		///< ObstructionLevel: [0.0f..1.0f]
			AkReal32 in_fOcclusionLevel			///< OcclusionLevel: [0.0f..1.0f]
			);

		/// Sets a game object's obstruction and occlusion value for each position defined by AK::SoundEngine::SetMultiplePositions.
		/// This function differs from AK::SoundEngine::SetObjectObstructionAndOcclusion as a list of obstruction/occlusion pair is provided
		/// and each obstruction/occlusion pair will affect the corresponding position defined at the same index.
		/// \aknote In the case the number of obstruction/occlusion pairs is smaller than the number of positions, remaining positions' 
		/// obstruction/occlusion values are set to 0.0. \endaknote
		/// \return
		/// - \c AK_Success if successful
		/// - \c AK_CommandTooLarge if the number of obstruction values is too large for the command queue.
		/// - \c AK_InvalidParameter if one of the parameter is out of range (check the debug console)
		/// - \c AK_InvalidFloatValue if one of the occlusion/obstruction values is NaN or Inf.
		/// \sa
		/// - \ref soundengine_obsocc
		/// - \ref soundengine_environments
		/// \return AK_Success if occlusion/obstruction values are successfully stored for this emitter
		extern   AKRESULT __cdecl  SetMultipleObstructionAndOcclusion  (
			AkGameObjectID in_EmitterID,										///< Emitter game object ID
			AkGameObjectID in_uListenerID,										///< Listener game object ID
			AkObstructionOcclusionValues* in_fObstructionOcclusionValues,		///< Array of obstruction/occlusion pairs to apply
																				///< ObstructionLevel: [0.0f..1.0f]
																				///< OcclusionLevel: [0.0f..1.0f]
			AkUInt32 in_uNumOcclusionObstruction								///< Number of obstruction/occlusion pairs specified in the provided array
			);

		/// Saves the playback history of container structures.
		/// This function will write history data for all currently loaded containers and instantiated game
		/// objects (for example, current position in Sequence Containers and previously played elements in
		/// Random Containers). 
		/// \remarks
		/// This function acquires the main audio lock, and may block the caller for several milliseconds.
		/// \return
		/// - \c AK_Success when successful
		/// - \c AK_Fail is in_pBytes could not be parsed (corruption or data is truncated)
		/// \sa 
		/// - <tt>AK::SoundEngine::SetContainerHistory()</tt>
		extern   AKRESULT __cdecl  GetContainerHistory (
			AK::IWriteBytes * in_pBytes			///< Pointer to IWriteBytes interface used to save the history.
			);

		/// Restores the playback history of container structures.
		/// This function will read history data from the passed-in stream reader interface, and apply it to all
		/// currently loaded containers and instantiated game objects. Game objects are matched by
		/// ID. History for unloaded structures and unknown game objects will be skipped.
		/// \remarks
		/// This function acquires the main audio lock, and may block the caller for several milliseconds.
		/// \return 
		/// - \c AK_Success if successful
		/// - \c AK_InsufficientMemory if not enough memory is available for IReadBytes operation
		/// \sa 
		/// - <tt>AK::SoundEngine::GetContainerHistory()</tt>
		extern  AKRESULT __cdecl  SetContainerHistory (
			AK::IReadBytes * in_pBytes 			///< Pointer to IReadBytes interface used to load the history.
			);

		//@}
        
        ////////////////////////////////////////////////////////////////////////
		/// @name Capture
		//@{

		/// Starts recording the sound engine audio output. 
		/// StartOutputCapture outputs a wav file per current output device of the sound engine.
		/// If more than one device is active, the system will create multiple files in the same output 
		/// directory and will append numbers at the end of the provided filename.
		///
		/// If no device is running yet, the system will return success AK_Success despite doing nothing.
		/// Use RegisterAudioDeviceStatusCallback to get notified when devices are created/destructed.
		///
		/// \return AK_Success if successful, AK_Fail if there was a problem starting the output capture.
		/// \remark
		///		- The sound engine opens a stream for writing using <tt>AK::IAkStreamMgr::CreateStd()</tt>. If you are using the
		///			default implementation of the Stream Manager, file opening is executed in your implementation of 
		///			the Low-Level IO interface <tt>AK::StreamMgr::IAkLowLevelIOHook::BatchOpen()</tt>. The following 
		///			AkFileSystemFlags are passed: uCompanyID = AKCOMPANYID_AUDIOKINETIC and uCodecID = AKCODECID_PCM,
		///			and the AkOpenMode is AK_OpenModeWriteOvrwr. See \ref streamingmanager_lowlevel_location for
		///			more details on managing the deployment of your Wwise generated data.
		/// \return
		/// - \c AK_Success when successful
		/// - \c AK_InvalidParameter if in_CaptureFileName is null.
		/// - \c AK_InsufficientMemory if not enough memory is available.
		/// \sa 
		/// - <tt>AK::SoundEngine::StopOutputCapture()</tt>
		/// - <tt>AK::StreamMgr::SetFileLocationResolver()</tt>
		/// - \ref streamingdevicemanager
		/// - \ref streamingmanager_lowlevel_location
		/// - RegisterAudioDeviceStatusCallback
		extern   AKRESULT __cdecl  StartOutputCapture  ( 
			const AkOSChar* in_CaptureFileName				///< Name of the output capture file
			);

		/// Stops recording the sound engine audio output. 
		/// \return AK_Success if successful, AK_Fail if there was a problem stopping the output capture.
		/// \sa 
		/// - <tt>AK::SoundEngine::StartOutputCapture()</tt>
		extern   AKRESULT __cdecl  StopOutputCapture  ();

		/// Adds text marker in audio output file.
		/// \return
		/// - \c AK_Success when successful
		/// - \c AK_InvalidParameter if in_MarkerText is null.
		/// - \c AK_InsufficientMemory if not enough memory is available.
		/// \sa
		/// - <tt>AK::SoundEngine::StartOutputCapture()</tt>
		extern   AKRESULT __cdecl  AddOutputCaptureMarker  (
			const char* in_MarkerText,							///< Text of the marker
			AkUInt32 in_uSamplePos = AK_INVALID_SAMPLE_POS		///< Sample to attach the marker to. If set to AK_INVALID_SAMPLE_POS, marker is added at the current recording time.
			);

		/// Adds binary data to a marker in audio output file.
		/// \return
		/// - \c AK_Success when successful
		/// - \c AK_InvalidParameter if in_pMarkerData is null or in_uMarkerDataSize is zero.
		/// - \c AK_InsufficientMemory if not enough memory is available.
		/// \sa
		/// - <tt>AK::SoundEngine::StartOutputCapture()</tt>
		extern  AKRESULT __cdecl  AddOutputCaptureBinaryMarker (
			void* in_pMarkerData,								///< Marker data
			AkUInt32 in_uMarkerDataSize,						///< Size of the marker data
			AkUInt32 in_uSamplePos = AK_INVALID_SAMPLE_POS		///< Sample to attach the marker to. If set to AK_INVALID_SAMPLE_POS, marker is added at the current recording time.
			);

		/// Gets the system sample rate.
		/// \return The sample rate.
		extern  AkUInt32 __cdecl  GetSampleRate ();

		/// Registers a callback used for retrieving audio samples.
		/// The callback will be called from the audio thread during real-time rendering and from the main thread during offline rendering.
		/// \return
		/// - \c AK_Success when successful
		/// - \c AK_DeviceNotFound if the audio device ID doesn't match to a device in use.
		/// - \c AK_InvalidParameter when in_pfnCallback is null
		/// - \c AK_NotInitialized if the sound engine is not initialized at this time
		/// \sa
		/// - <tt>AK::SoundEngine::AddOutput()</tt>
		/// - <tt>AK::SoundEngine::GetOutputID()</tt>
		/// - <tt>AK::SoundEngine::UnregisterCaptureCallback()</tt>
		extern  AKRESULT __cdecl  RegisterCaptureCallback (
			AkCaptureCallbackFunc in_pfnCallback,								///< Capture callback function to register.
			AkOutputDeviceID in_idOutput = AK_INVALID_OUTPUT_DEVICE_ID,			///< The audio device specific id, return by AK::SoundEngine::AddOutput or AK::SoundEngine::GetOutputID
			void* in_pCookie = NULL												///< Callback cookie that will be sent to the callback function along with additional information
			);

		/// Unregisters a callback used for retrieving audio samples.
		/// \return
		/// - \c AK_Success when successful
		/// - \c AK_DeviceNotFound if the audio device ID doesn't match to a device in use.
		/// - \c AK_InvalidParameter when in_pfnCallback is null
		/// - \c AK_NotInitialized if the sound engine is not initialized at this time
		/// \sa
		/// - <tt>AK::SoundEngine::AddOutput()</tt>
		/// - <tt>AK::SoundEngine::GetOutputID()</tt>
		/// - <tt>AK::SoundEngine::RegisterCaptureCallback()</tt>
		extern  AKRESULT __cdecl  UnregisterCaptureCallback (
			AkCaptureCallbackFunc in_pfnCallback,								///< Capture callback function to unregister.
			AkOutputDeviceID in_idOutput = AK_INVALID_OUTPUT_DEVICE_ID,			///< The audio device specific id, return by AK::SoundEngine::AddOutput or AK::SoundEngine::GetOutputID
			void* in_pCookie = NULL												///< Callback cookie that will be sent to the callback function along with additional information
			);

		/// Starts recording the sound engine profiling information into a file. This file can be read
		/// by Wwise Authoring. The file is created at the base path. If you have integrated Wwise I/O,
		/// you can use <tt>CAkDefaultIOHookDeferred::SetBasePath()</tt> (or <tt>CAkDefaultIOHookDeferred::AddBasePath()</tt>)
		/// to change the location where the file is saved. The profiling session records all data types possible.
		/// Note that this call captures peak metering for all the busses loaded and mixing
		/// while this call is invoked.
		/// \remark This function is provided as a utility tool only. It does nothing if it is 
		///			called in the release configuration and returns AK_NotCompatible.
		extern   AKRESULT __cdecl  StartProfilerCapture  ( 
			const AkOSChar* in_CaptureFileName				///< Name of the output profiler file (.prof extension recommended)
			);

		/// Stops recording the sound engine profiling information. 
		/// \remark This function is provided as a utility tool only. It does nothing if it is 
		///			called in the release configuration and returns AK_NotCompatible.
		extern   AKRESULT __cdecl  StopProfilerCapture  ();

		//@}

		////////////////////////////////////////////////////////////////////////
		/// @name Offline Rendering
		//@{

		/// Sets the offline rendering frame time in seconds.
		/// When offline rendering is enabled, every call to \ref RenderAudio() will generate sample data as if this much time has elapsed. If the frame time argument is less than or equal to zero, every call to RenderAudio() will generate one audio buffer.
		/// 
		/// \sa \ref goingfurther_offlinerendering
		/// \return Always returns AK_Success
		extern  AKRESULT __cdecl  SetOfflineRenderingFrameTime (
			AkReal32 in_fFrameTimeInSeconds					///< frame time in seconds used during offline rendering
			);

		/// Enables/disables offline rendering. 
		/// 
		/// \sa \ref goingfurther_offlinerendering
		/// \return Always returns AK_Success
		extern  AKRESULT __cdecl  SetOfflineRendering (
			bool in_bEnableOfflineRendering					///< enables/disables offline rendering
			);

		//@}

		////////////////////////////////////////////////////////////////////////
		/// @name Secondary Outputs
		//@{

		/// Adds an output to the sound engine. Use this to add controller-attached headphones, controller speakers, DVR output, etc.  
		/// The in_Settings parameter contains an Audio Device shareset to specify the output plugin to use and a device ID to specify the instance, if necessary (e.g. which game controller).
		/// 
		/// Like most functions of AK::SoundEngine, AddOutput is asynchronous. A successful return code merely indicates that the request is properly queued.
		/// Error codes returned by this function indicate various invalid parameters. To know if this function succeeds or not, and the failure code,
		/// register an AkDeviceStatusCallbackFunc callback with RegisterAudioDeviceStatusCallback.
		/// 
		/// \sa AkOutputSettings for more details.
		/// \sa \ref integrating_secondary_outputs
		/// \sa \ref default_audio_devices
		/// \sa AK::SoundEngine::RegisterAudioDeviceStatusCallback
		/// \sa AK::AkDeviceStatusCallbackFunc
		/// \return 
		/// The following codes are returned directly from the function, as opposed to the AkDeviceStatusCallback
		/// - \c AK_NotImplemented: Feature not supported, some platforms don't have other outputs.
		/// - \c AK_InvalidParameter: Out of range parameters or unsupported parameter combinations (see parameter list below).
		/// - \c AK_IDNotFound: The audioDeviceShareset on in_settings doesn't exist.  Possibly, the Init bank isn't loaded yet or was not updated with latest changes.
		/// - \c AK_DeviceNotReady: The idDevice on in_settings doesn't match with a valid hardware device.  Either the device doesn't exist or is disabled.  Disconnected devices (headphones) are not considered "not ready" therefore won't cause this error.
		/// - \c AK_NotInitialized: If AK::SoundEngine::Init was not called or if the Init.bnk was not loaded before the call.
		/// - \c AK_Success: Parameters are valid.
		/// 
		/// The following codes are returned from the callback.
		/// - \c AK_InsufficientMemory : Not enough memory to complete the operation.
		/// - \c AK_IDNotFound: The audioDeviceShareset on in_settings doesn't exist.  Possibly, the Init bank isn't loaded yet or was not updated with latest changes.
		/// - \c AK_PluginNotRegistered: The audioDeviceShareset exists but the plug-in it refers to is not installed or statically linked with the game.
		/// - \c AK_NotCompatible: The hardware does not support this type of output. Wwise will try to use the System output instead, and a separate callback will fire when that completes.
		/// - \c AK_DeviceNotCompatible: The hardware does not support this type of output. Wwise will NOT fallback to any other type of output.
		/// - \c AK_Fail: Generic code for any non-permanent conditions (e.g. disconnection) that prevent the use of the output. Wwise has created the output and sounds will be routed to it, but this output is currently silent until the temporary condition resolves.
		/// - \c AK_NoDistinctListener: Outputs of the same type (same ShareSet, like controller speakers) must have distinct Listeners to make a proper routing. This doesn't happen if there is only one output of that type.
		extern  AKRESULT __cdecl  AddOutput (			
			const AkOutputSettings & in_Settings,			///< Creation parameters for this output.  \ref AkOutputSettings							
			AkOutputDeviceID *out_pDeviceID = NULL,			///< (Optional) Output ID to use with all other Output management functions.  Leave to NULL if not required. \ref AK::SoundEngine::GetOutputID
			const AkGameObjectID* in_pListenerIDs = NULL,	///< Specific listener(s) to attach to this device. 
															///< If specified, only the sounds routed to game objects linked to those listeners will play in this device.  
															///< It is necessary to have separate listeners if multiple devices of the same type can coexist (e.g. controller speakers)
															///< If not specified, sound routing simply obey the associations between Master Busses and Audio Devices setup in the Wwise Project.
			AkUInt32 in_uNumListeners = 0					///< The number of elements in the in_pListenerIDs array.
			);

		/// Removes one output added through AK::SoundEngine::AddOutput
		/// If a listener was associated with the device, you should consider unregistering the listener prior to call RemoveOutput 
		/// so that Game Object/Listener routing is properly updated according to your game scenario.			
		/// \sa \ref integrating_secondary_outputs
		/// \sa AK::SoundEngine::AddOutput
		/// \return AK_Success: Parameters are valid.
		extern  AKRESULT __cdecl  RemoveOutput (
			AkOutputDeviceID in_idOutput	///< ID of the output to remove.  Use the returned ID from AddOutput, GetOutputID, or ReplaceOutput
			);

		/// Replaces an output device previously created during engine initialization or from AddOutput, with a new output device.
		/// In addition to simply removing one output device and adding a new one, the new output device will also be used on all of the master buses
		/// that the old output device was associated with, and preserve all listeners that were attached to the old output device.
		/// 
		/// Like most functions of AK::SoundEngine, AddOutput is asynchronous. A successful return code merely indicates that the request is properly queued.
		/// Error codes returned by this function indicate various invalid parameters. To know if this function succeeds or not, and the failure code,
		/// register an AkDeviceStatusCallbackFunc callback with RegisterAudioDeviceStatusCallback.
		/// 
		/// \sa AK::SoundEngine::AddOutput
		/// \sa AK::SoundEngine::RegisterAudioDeviceStatusCallback
		/// \sa AK::AkDeviceStatusCallbackFunc
		/// \return 
		/// - \c AK_InvalidID: The audioDeviceShareset on in_settings was not valid.
		/// - \c AK_IDNotFound: The audioDeviceShareset on in_settings doesn't exist.  Possibly, the Init bank isn't loaded yet or was not updated with latest changes.
		/// - \c AK_DeviceNotReady: The idDevice on in_settings doesn't match with a valid hardware device.  Either the device doesn't exist or is disabled.  Disconnected devices (headphones) are not considered "not ready" therefore won't cause this error.
		/// - \c AK_DeviceNotFound: The in_outputDeviceId provided does not match with any of the output devices that the sound engine is currently using.
		/// - \c AK_InvalidParameter: Out of range parameters or unsupported parameter combinations on in_settings
		/// - \c AK_Success: parameters were valid, and the remove and add will occur.
		extern  AKRESULT __cdecl  ReplaceOutput (
			const AkOutputSettings & in_Settings,				///< Creation parameters for this output.  \ref AkOutputSettings
			AkOutputDeviceID in_outputDeviceId,					///< AkOutputDeviceID of the output to replace. Use 0 to target the current main output, regardless of its id.  Otherwise, use the AkOutputDeviceID returned from AddOutput() or ReplaceOutput(), or generated by GetOutputID()
			AkOutputDeviceID *out_pOutputDeviceId = NULL		///< (Optional) Pointer into which the method writes the AkOutputDeviceID of the new output device. If the call fails, the value pointed to will not be modified.
			);

		/// Gets the compounded output ID from shareset and device id.
		/// Outputs are defined by their type (Audio Device shareset) and their specific system ID.  A system ID could be reused for other device types on some OS or platforms, hence the compounded ID.
		/// Use 0 for in_idShareset & in_idDevice to get the Main Output ID (the one usually initialized during AK::SoundEngine::Init)
		/// \return The id of the output					
		extern  AkOutputDeviceID __cdecl  GetOutputID (
			AkUniqueID in_idShareset,		///< Audio Device ShareSet ID, as defined in the Wwise Project.  If needed, use AK::SoundEngine::GetIDFromString() to convert from a string.  Set to AK_INVALID_UNIQUE_ID to use the default.
			AkUInt32 in_idDevice			///< Device specific identifier, when multiple devices of the same type are possible.  If only one device is possible, leave to 0.
											///< \sa \ref obtaining_device_id
			);

		extern  AkOutputDeviceID __cdecl  GetOutputID (
			const char* in_szShareSet,			///< Audio Device ShareSet Name, as defined in the Wwise Project.  If Null, will select the Default Output shareset (always available)
			AkUInt32 in_idDevice				///< Device specific identifier, when multiple devices of the same type are possible.  If only one device is possible, leave to 0.
												///< \sa \ref obtaining_device_id
			);

		#ifdef AK_SUPPORT_WCHAR
		extern  AkOutputDeviceID __cdecl  GetOutputID (
			const wchar_t* in_szShareSet,			///< Audio Device ShareSet Name, as defined in the Wwise Project.  If Null, will select the Default Output shareset (always available)
			AkUInt32 in_idDevice			///< Device specific identifier, when multiple devices of the same type are possible.  If only one device is possible, leave to 0.
											///< \sa \ref obtaining_device_id
			);
		#endif

		/// Sets the Audio Device to which a master bus outputs.  This overrides the setting in the Wwise project.	
		/// Can only be set on top-level busses. The Init bank should be successfully loaded prior to this call.
		/// \aknote This function is useful only if used before the creation of an output, at the beginning of the sound engine setup. 
		/// Once active outputs using this Bus have been created, it is imperative to use AK::SoundEngine:ReplaceOutput instead to change the type of output.
		/// \return 
		/// AK_IDNotFound when either the Bus ID or the Device ID are not present in the Init bank or the bank was not loaded
		/// AK_InvalidParameter when the specified bus is not a Master Bus.  This function can be called only on busses that have no parent bus.
		extern  AKRESULT __cdecl  SetBusDevice (
			AkUniqueID in_idBus,			///< Id of the master bus
			AkUniqueID in_idNewDevice		///< New device shareset to replace with.
			);

		/// Sets the Audio Device to which a master bus outputs.  This overrides the setting in the Wwise project.	
		/// Can only be set on top-level busses. The Init bank should be successfully loaded prior to this call.
		/// \aknote This function is useful only if used before the creation of an output, at the beginning of the sound engine setup. 
		/// Once active outputs using this Bus have been created, it is imperative to use AK::SoundEngine:ReplaceOutput instead to change the type of output.
		/// \return 
		/// AK_IDNotFound when either the Bus ID or the Device ID are not present in the Init bank or the bank was not loaded
		/// AK_InvalidParameter when the specified bus is not a Master Bus.  This function can be called only on busses that have no parent bus.
		extern  AKRESULT __cdecl  SetBusDevice (
			const char*  in_BusName,		///< Name of the master bus
			const char*  in_DeviceName		///< New device shareset to replace with.
			);

		#ifdef AK_SUPPORT_WCHAR
		/// Sets the Audio Device to which a master bus outputs.  This overrides the setting in the Wwise project.	
		/// Can only be set on top-level busses. The Init bank should be successfully loaded prior to this call.
		/// SetBusDevice must be preceded by a call to AddOutput for the new device shareset to be registered as an output.
		/// \return 
		/// AK_IDNotFound when either the Bus ID or the Device ID are not present in the Init bank or the bank was not loaded
		/// AK_InvalidParameter when the specified bus is not a Master Bus.  This function can be called only on busses that have no parent bus.
		extern  AKRESULT __cdecl  SetBusDevice (
			const wchar_t*  in_BusName,		///< Name of the master bus
			const wchar_t*  in_DeviceName	///< New device shareset to replace with.
			);
		#endif
		
		/// Returns a listing of the current devices for a given sink plug-in, including Device ID, friendly name, and state.
		/// \remarks
		/// This call is only valid for sink plug-ins that support device enumeration.
		/// Prerequisites:
		///  * The plug-in must have been initialized by loading the init bank or by calling \ref AK::SoundEngine::RegisterPlugin.
		///  * A physical device recognized by this plug-in must exist in the system.
		/// 
		/// The built-in audio devices (System, Communication, Headphones, Personal, Pad Speaker) all support enumeration, on all platforms.
		/// The only Wwise plug-in that support device enumeration is Motion, for the Windows platform only. 
		/// Note that it is optional to implement device enumeration on custom sink plug-ins.
		/// \return
		/// - \c AK_NotImplemented if the sink plug-in does not implement device enumeration
		/// - \c AK_PluginNotRegistered if the plug-in has not been registered yet either by loading the init bank or by calling RegisterPluginDLL.
		/// - \c AK_NotCompatible if no device of this type are supported on the current platform
		/// - \c AK_Fail in case of system device manager failure (OS related)
		/// 
		extern  AKRESULT __cdecl  GetDeviceList  (
			AkUInt32 in_ulCompanyID,					///< Company identifier (as declared in the plug-in description XML file)
			AkUInt32 in_ulPluginID,						///< Plug-in identifier (as declared in the plug-in description XML file)
			AkUInt32& io_maxNumDevices,					///< In: The length of the out_deviceDescriptions array. Out: If out_deviceDescriptions is not-null, this will be set to the number of entries in out_deviceDescriptions that was populated. If out_deviceDescriptions is null, this will be set to the number of devices that may be available.
			AkDeviceDescription* out_deviceDescriptions ///< The output array of device descriptions.
			);

		/// Returns a listing of the current devices for a given sink plug-in, including Device ID, friendly name, and state.
		/// \remarks
		/// This call is only valid for sink plug-ins that support device enumeration.
		/// Prerequisites:
		///  * The plug-in must have been initialized by loading the init bank or by calling \ref AK::SoundEngine::RegisterPlugin.
		///  * The audio device shareset must have been loaded from a soundbank, and a physical device recognized by this plug-in must exist in the system.
		/// 
		/// The built-in audio devices (System, Communication, Headphones, Personal, Pad Speaker) all support enumeration, on all platforms.
		/// The only Wwise plug-in that support device enumeration is Motion, for the Windows platform only. 
		/// Note that it is optional to implement device enumeration on custom sink plug-ins.
		/// \return
		/// AK_NotImplemented if the sink plug-in does not implement device enumeration
		/// AK_PluginNotRegistered if the plug-in has not been registered yet either by loading the init bank or by calling RegisterPluginDLL.
		extern  AKRESULT __cdecl  GetDeviceList  (
			AkUniqueID in_audioDeviceShareSetID,		///< In: The audio device shareset ID for which to list the sink plug-in devices.
			AkUInt32& io_maxNumDevices,					///< In: The length of the out_deviceDescriptions array. Out: If out_deviceDescriptions is not-null, this will be set to the number of entries in out_deviceDescriptions that was populated. If out_deviceDescriptions is null, this will be set to the number of devices that may be available.
			AkDeviceDescription* out_deviceDescriptions ///< The output array of device descriptions.
			);

		/// Sets the volume of a output device.		
		/// \return
		/// - \c AK_Success if successful		
		/// - \c AK_InvalidFloatValue if the value specified was NaN or Inf
		extern   AKRESULT __cdecl  SetOutputVolume  (
			AkOutputDeviceID in_idOutput,	///< Output ID to set the volume on.  As returned from AddOutput or GetOutputID
			AkReal32 in_fVolume				///< Volume (0.0 = Muted, 1.0 = Volume max)
			);

		/// Returns whether or not the audio device matching the device ID provided supports spatial audio (i.e. the functionality is enabled, and more than 0 dynamic objects are supported).
		/// If Spatial Audio is supported, then you can call Init, AddOutput, or ReplaceOutput with an Audio Device Shareset corresponding to the respective platform-specific plug-in that
		/// provides spatial audio, such as the Microsoft Spatial Sound Platform for Windows. Note that on Xbox One, you need to call EnableSpatialAudio() before the sound engine is
		/// initialized, or initialize the sound engine with AkPlatformInitSettings::bEnableSpatialAudio set to true if you want spatial audio support; otherwise this will always return AK_NotCompatible.
		/// \return
		/// AK_NotCompatible when the device ID provided does not support spatial audio, or the platform does not support spatial audio
		/// AK_Fail when there is some other miscellaneous failure, or the device ID provided does not match a device that the system knows about
		/// AK_Success when the device ID provided does support spatial audio
		extern   AKRESULT __cdecl  GetDeviceSpatialAudioSupport   (
			AkUInt32 in_idDevice			///< Device specific identifier, when multiple devices of the same type are possible.  If only one device is possible, leave to 0.
											///< \sa \ref obtaining_device_id
			);


		//@}

		/// This function should be called to put the sound engine in background mode, where audio isn't processed anymore. This needs to be called if the console has a background mode or some suspended state.
		/// Call \c WakeupFromSuspend when your application receives the message from the OS that the process is back in foreground.
		/// When suspended, the sound engine will process API messages (like PostEvent and SetSwitch) only when \ref RenderAudio() is called. 
		/// It is recommended to match the <b>in_bRenderAnyway</b> parameter with the behavior of the rest of your game: 
		/// if your game still runs in background and you must keep some kind of coherent state between the audio engine and game, then allow rendering.
		/// If you want to minimize CPU when in background, then don't allow rendering and never call RenderAudio from the game.
		///
		/// Consult \ref workingwithsdks_system_calls to learn when it is appropriate to call this function for each platform.
		/// \sa WakeupFromSuspend
		/// \sa \ref workingwithsdks_system_calls
		extern   AKRESULT __cdecl  Suspend  (
			bool in_bRenderAnyway = false, ///< If set to true, audio processing will still occur, but not outputted. When set to false, no audio will be processed at all, even upon reception of RenderAudio().
			bool in_bFadeOut = true        ///< Delay the suspend by one audio frame in order to fade-out. When false, the suspend takes effect immediately but audio may glitch.
			);

		/// This function should be called to wake up the sound engine and start processing audio again. This needs to be called if the console has a background mode or some suspended state.
		///
		/// Consult \ref workingwithsdks_system_calls to learn when it is appropriate to call this function for each platform.
		/// \sa Suspend
		/// \sa \ref workingwithsdks_system_calls
		extern   AKRESULT __cdecl  WakeupFromSuspend  (
			AkUInt32 in_uDelayMs = 0      /// Delay (in milliseconds) before the wake up occurs. Rounded up to audio frame granularity. Adding a delay is useful if there is a possibility that another OS event may override the wake-up in the near future.
		);

		/// Obtains the current audio output buffer tick. This corresponds to the number of buffers produced by
		/// the sound engine since initialization. 
		/// \return Tick count.
		extern  AkUInt32 __cdecl  GetBufferTick ();

		/// Obtains the current audio output sample tick. This corresponds to the number of samples produced by
		/// the sound engine since initialization. 
		/// \return Sample count.
		extern  AkUInt64 __cdecl  GetSampleTick ();

		/// Resets all global changes made to the sound engine.
		/// This includes:
		/// - States
		/// - RTPCs in the global scope
		/// - Changes made on sound object by Event Actions like Set Volume, Set Pitch, etc or equivalent API calls.
		/// - Mute/solo status
		/// - Effects set dynamically through SetEffect or a Set Effect Action.
		/// - Random and Sequence containers histories (last played, etc)
		/// \note
		/// To reset Game Object specific values, use AK::SoundEngine::UnregisterGameObj or AK::SoundEngine::UnregisterAllGameObj
		/// then AK::SoundEngine::RegisterGameObj if the game object is still needed.
		AK_EXTERNAPIFUNC(AKRESULT, ResetGlobalValues());

		/// Sets the assertion handling function that the Sound Engine and other modules use.
		/// The assertion hook can be set via this function prior to Sound Engine initialization in order to catch early initialization assertion failures.
		/// However, during Sound Engine initialization, the assertion handler is replaced with the value of <tt>AkInitSettings::pfnAssertHook</tt>.
		/// After Sound Engine initialization, the assertion handler cannot be replaced.
		/// 
		/// \aknote
		/// This function has no effect on Release builds.
		/// \endaknote
		/// 
		/// \return AK_Success when called before Sound Engine initialization, or AK_AlreadyInitialized if Sound Engine was already initialized
		extern  AKRESULT __cdecl  SetAssertHook (AkAssertHook in_pfnAssertHook);
	}
}

#endif // _AK_SOUNDENGINE_H_
/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Apache License Usage

Alternatively, this file may be used under the Apache License, Version 2.0 (the 
"Apache License"); you may not use this file except in compliance with the 
Apache License. You may obtain a copy of the Apache License at 
http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing, software distributed
under the Apache License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
OR CONDITIONS OF ANY KIND, either express or implied. See the Apache License for
the specific language governing permissions and limitations under the License.

  Copyright (c) 2026 Audiokinetic Inc.
*******************************************************************************/

// AkCommonDefs.h

/// \file 
/// AudioLib common defines, enums, and structs.


#ifndef _AK_COMMON_DEFS_H_
#define _AK_COMMON_DEFS_H_

#include <AK/SoundEngine/Common/AkSpeakerConfig.h>
#include <AK/SoundEngine/Common/AkSpeakerVolumes.h>
#include <AK/Tools/Common/AkBitFuncs.h>
#include <AK/Tools/Common/AkAssert.h>

//-----------------------------------------------------------------------------
// AUDIO DATA FORMAT
//-----------------------------------------------------------------------------

// Audio data format.
// ------------------------------------------------

const AkDataTypeID		AK_INT				= 0;		///< Integer data type (uchar, short, and so on)
const AkDataTypeID		AK_FLOAT			= 1;		///< Float data type

const AkDataInterleaveID AK_INTERLEAVED		= 0;		///< Interleaved data
const AkDataInterleaveID AK_NONINTERLEAVED	= 1;		///< Non-interleaved data

// Native format currently the same on all supported platforms, may become platform specific in the future
const AkUInt32 AK_LE_NATIVE_BITSPERSAMPLE  = 32;					///< Native number of bits per sample.
const AkUInt32 AK_LE_NATIVE_SAMPLETYPE = AK_FLOAT;					///< Native data type.
const AkUInt32 AK_LE_NATIVE_INTERLEAVE = AK_NONINTERLEAVED;			///< Native interleaved setting.

/// Defines the parameters of an audio buffer format.
struct AkAudioFormat
{
	AkUInt32	uSampleRate;		///< Number of samples per second

	AkChannelConfig channelConfig;	///< Channel configuration.

	AkUInt32	uBitsPerSample	:6; ///< Number of bits per sample.
	AkUInt32	uBlockAlign		:10;///< Number of bytes per sample frame. (For example a 5.1 PCM 16bit should have a uBlockAlign equal to 6(5.1 channels)*2(16 bits per sample) = 12.
	AkUInt32	uTypeID			:2; ///< Data type ID (AkDataTypeID). 
	AkUInt32	uInterleaveID	:1; ///< Interleave ID (AkDataInterleaveID). 
	
	/// Get the number of channels.
	/// \return The number of channels
	 AkUInt32 GetNumChannels() const
	{
		return channelConfig.uNumChannels;
	}

	/// Query if LFE channel is present.
	/// \return True when LFE channel is present
	 bool HasLFE() const
	{ 
		return channelConfig.HasLFE(); 
	}

	/// Query if center channel is present.
	/// Note that mono configurations have one channel which is arbitrary set to AK_SPEAKER_FRONT_CENTER,
	/// so HasCenter() returns true for mono signals.
	/// \return True when center channel is present and configuration has more than 2 channels.
	 bool HasCenter() const
	{ 
		return channelConfig.HasCenter(); 
	}

	/// Get the number of bits per sample.
	/// \return The number of bits per sample
	 AkUInt32 GetBitsPerSample()	const						
	{ 
		return uBitsPerSample;
	}

	/// Get the block alignment.
	/// \return The block alignment
	 AkUInt32 GetBlockAlign() const
	{
		return uBlockAlign;
	}

	/// Get the data sample format (Float or Integer).
	/// \return The data sample format
	 AkUInt32 GetTypeID() const
	{
		return uTypeID;
	}

	/// Get the interleaved type.
	/// \return The interleaved type
	 AkUInt32 GetInterleaveID() const
	{
		return uInterleaveID;
	}

	/// Set all parameters of the audio format structure.
	/// Channels are specified by channel mask (standard configs).
	void SetAll(
		AkUInt32    in_uSampleRate,		///< Number of samples per second
		AkChannelConfig in_channelConfig,	///< Channel configuration
		AkUInt32    in_uBitsPerSample,	///< Number of bits per sample
		AkUInt32    in_uBlockAlign,		///< Number of bytes per sample frame. (For example a 5.1 PCM 16bit should have a uBlockAlign equal to 6(5.1 channels)*2(16 bits per sample) = 12.
		AkUInt32    in_uTypeID,			///< Data sample format (Float or Integer)
		AkUInt32    in_uInterleaveID	///< Interleaved type
		)
	{
		uSampleRate		= in_uSampleRate;
		channelConfig	= in_channelConfig;
		uBitsPerSample	= in_uBitsPerSample;
		uBlockAlign		= in_uBlockAlign;
		uTypeID			= in_uTypeID;
		uInterleaveID	= in_uInterleaveID;
	}

	 bool operator==(const AkAudioFormat & in_other) const
	{
		return uSampleRate == in_other.uSampleRate 
			&& channelConfig == in_other.channelConfig
			&& uBitsPerSample == in_other.uBitsPerSample
			&& uBlockAlign == in_other.uBlockAlign
			&& uTypeID == in_other.uTypeID
			&& uInterleaveID == in_other.uInterleaveID;
	}

	 bool operator!=(const AkAudioFormat & in_other) const
	{
		return uSampleRate != in_other.uSampleRate
			|| channelConfig != in_other.channelConfig
			|| uBitsPerSample != in_other.uBitsPerSample
			|| uBlockAlign != in_other.uBlockAlign
			|| uTypeID != in_other.uTypeID
			|| uInterleaveID != in_other.uInterleaveID;
	}
};

typedef AkUInt8(*AkChannelMappingFunc)(const AkChannelConfig &config, AkUInt8 idx);

#define AK_MAKE_CHANNELCONFIGOVERRIDE(_config,_order)	((AkInt64)_config.Serialize()|((AkInt64)_order<<32))
#define AK_GET_CHANNELCONFIGOVERRIDE_CONFIG(_over)		(_over&UINT_MAX)
#define AK_GET_CHANNELCONFIGOVERRIDE_ORDERING(_over)	((AK::AkChannelOrdering)(_over>>32))

// Build a 32 bit class identifier based on the Plug-in type,
// CompanyID and PluginID.
//
// Parameters:
//   - in_pluginType: A value from enum AkPluginType (4 bits)
//   - in_companyID: CompanyID as defined in the Plug-in's XML file (12 bits)
//			* 0-63: Reserved for Audiokinetic
//			* 64-255: Reserved for clients' in-house Plug-ins
//			* 256-4095: Assigned by Audiokinetic to third-party plug-in developers
//   - in_pluginID: PluginID as defined in the Plug-in's XML file (16 bits)
//			* 0-32767: Set freely by the Plug-in developer
#define AKMAKECLASSID( in_pluginType, in_companyID, in_pluginID ) \
	( (in_pluginType) + ( (in_companyID) << 4 ) + ( (in_pluginID) << ( 4 + 12 ) ) )

#define AKGETPLUGINTYPEFROMCLASSID( in_classID ) ( (in_classID) & AkPluginTypeMask )
#define AKGETCOMPANYIDFROMCLASSID( in_classID ) ( ( (in_classID) & 0x0000FFF0 ) >> 4 )
#define AKGETPLUGINIDFROMCLASSID( in_classID ) ( ( (in_classID) & 0xFFFF0000 ) >> ( 4 + 12 ) )

#define CODECID_FROM_PLUGINID AKGETPLUGINIDFROMCLASSID


namespace AK
{
	/// Struct containing metering information about a buffer. Depending on when this struct is generated, you may get metering data computed in the previous frame only. 
	struct AkMetering
	{
		/// Peak of each channel in this frame.
		/// Vector of linear peak levels, corresponding to each channel. NULL if AK_EnableBusMeter_Peak is not set (see IAkMixerPluginContext::SetMeteringFlags() or AK::SoundEngine::RegisterBusMeteringCallback()).
		AK::SpeakerVolumes::VectorPtr peak;

		/// True peak of each channel (as defined by ITU-R BS.1770) in this frame.
		/// Vector of linear true peak levels, corresponding to each channel. NULL if AK_EnableBusMeter_TruePeak is not set (see IAkMixerPluginContext::SetMeteringFlags() or AK::SoundEngine::RegisterBusMeteringCallback()).
		AK::SpeakerVolumes::VectorPtr truePeak;

		/// RMS value of each channel in this frame.
		/// Vector of linear rms levels, corresponding to each channel. NULL if AK_EnableBusMeter_RMS is not set (see IAkMixerPluginContext::SetMeteringFlags() or AK::SoundEngine::RegisterBusMeteringCallback()).
		AK::SpeakerVolumes::VectorPtr rms;

		/// Mean k-weighted power value in this frame, used to compute loudness (as defined by ITU-R BS.1770).
		/// Total linear k-weighted power of all channels. 0 if AK_EnableBusMeter_KPower is not set (see IAkMixerPluginContext::SetMeteringFlags() or AK::SoundEngine::RegisterBusMeteringCallback()).
		AkReal32 fMeanPowerK;
	};

	static inline bool IsBankCodecID(AkUInt32 in_codecID)
	{
		return in_codecID == AKCODECID_BANK ||
			in_codecID == AKCODECID_BANK_EVENT ||
			in_codecID == AKCODECID_BANK_BUS;
	}
}

// 3D Audio Object.
// ------------------------------------------------

/// Default listener transform.
#define AK_DEFAULT_LISTENER_POSITION_X (0.0f)				// at coordinate system's origin
#define AK_DEFAULT_LISTENER_POSITION_Y (0.0f)
#define AK_DEFAULT_LISTENER_POSITION_Z (0.0f)
#define AK_DEFAULT_LISTENER_FRONT_X    (0.0f)				// looking toward Z,
#define AK_DEFAULT_LISTENER_FRONT_Y    (0.0f)
#define AK_DEFAULT_LISTENER_FRONT_Z    (1.0f)
#define AK_DEFAULT_TOP_X               (0.0f)				// top towards Y
#define AK_DEFAULT_TOP_Y               (1.0f)
#define AK_DEFAULT_TOP_Z               (0.0f)


/// 3D data needed for 3D spatialization. 
/// Undergoes transformations based on emitter-listener placement.
struct Ak3dData
{
	Ak3dData()
		: spread(1.f)
		, focus(1.f)
		, uEmitterChannelMask(0xffffffff)
	{
		xform.Set(AK_DEFAULT_LISTENER_POSITION_X, AK_DEFAULT_LISTENER_POSITION_Y, AK_DEFAULT_LISTENER_POSITION_Z, AK_DEFAULT_LISTENER_FRONT_X, AK_DEFAULT_LISTENER_FRONT_Y, AK_DEFAULT_LISTENER_FRONT_Z, AK_DEFAULT_TOP_X, AK_DEFAULT_TOP_Y, AK_DEFAULT_TOP_Z);
	}

	AkTransform xform;								///< Object position / orientation.
	AkReal32 spread;								///< Spread [0,1]
	AkReal32 focus;									///< Focus [0,1]
	AkChannelMask uEmitterChannelMask;				///< Emitter channel mask. With 3D spatialization, zeroed channels should be dropped.
};

/// Positioning data inherited from sound structures and mix busses.
struct AkBehavioralPositioningData
{
	AkBehavioralPositioningData()
		: center(1.f)
		, panLR(0.f)
		, panBF(0.f)
		, panDU(0.f)
		, panSpatMix(1.f)
		, spatMode(AK_SpatializationMode_None)
		, panType(AK_DirectSpeakerAssignment)
		, enableHeightSpread(true)
	{}

	AkReal32 center;								///< Center percentage [0,1]
	AkReal32 panLR;									///< Pan left-right [-1,1]
	AkReal32 panBF;									///< Pan back-front [-1,1]
	AkReal32 panDU;									///< Pan down-up [-1,1]
	AkReal32 panSpatMix;							///< Panning vs 3D spatialization mix ([0,1], 1 being 100% spatialized).
	Ak3DSpatializationMode spatMode;				///< 3D spatialization mode.
	AkSpeakerPanningType panType;					///< Speaker panning type.
	bool enableHeightSpread;						///< When true, audio objects 3D spatialized onto a planar channel configuration will be given a minimum spread value based on their elevation angle, equal to sin(elevation)**2.
};

/// Positioning data of 3D audio objects.
struct AkPositioningData
{
	Ak3dData threeD;								///< 3D data used for 3D spatialization.
	AkBehavioralPositioningData behavioral;			///< Positioning data inherited from sound structures and mix busses.
};

/// Enum of the possible object destinations when reaching a 3D audio-capable sink
enum class AkAudioObjectDestination
{
	eDefault = 0,            // The destination will be chosen based on the audio object's metadata and channel configuration
	eMainMix = 1,            // The audio object will be mixed to the sink's main mix
	ePassthrough = 2,        // The audio object will be mixed to the sink's passthrough mix
	eSystemAudioObject = 3   // The audio object will not be mixed; it will be sent separately to the system's 3D audio endpoint
};

// Audio buffer.
// ------------------------------------------------

/// Native sample type. 
/// \remarks Sample values produced by insert effects must use this type.
/// \remarks Source plug-ins can produce samples of other types (specified through 
/// according fields of AkAudioFormat, at initial handshaking), but these will be 
/// format converted internally into the native format.
/// \sa
/// - \ref iaksourceeffect_init
/// - \ref iakmonadiceffect_init
typedef AkReal32 AkSampleType;	///< Audio sample data type (32 bit floating point)

/// Audio buffer structure including the address of an audio buffer, the number of valid frames inside, 
/// and the maximum number of frames the audio buffer can hold.
/// \sa
/// - \ref fx_audiobuffer_struct
class AkAudioBuffer
{
public:

	/// Constructor.
	AkAudioBuffer() 
	{ 
		Clear(); 
	}

	/// Clear data pointer.
	 void ClearData()
	{
		pData = NULL;
		uValidFrames = 0;
	}

	/// Clear members.
	 void Clear()
	{
		ClearData();
		uMaxFrames          = 0;
		eState              = AK_DataNeeded;
	}
	
	/// \name Channel queries.
	//@{
	/// Get the number of channels.
	 AkUInt32 NumChannels() const
	{
		return channelConfig.uNumChannels;
	}

	/// Returns true if there is an LFE channel present.
	 bool HasLFE() const
	{ 
		return channelConfig.HasLFE(); 
	}

	 AkChannelConfig GetChannelConfig() const { return channelConfig; }

	//@}

	/// \name Interleaved interface
	//@{
	/// Get address of data: to be used with interleaved buffers only.
	/// \remarks Only source plugins can output interleaved data. This is determined at 
	/// initial handshaking.
	/// \sa 
	/// - \ref fx_audiobuffer_struct
	 void * GetInterleavedData()
	{ 
		return pData; 
	}

	/// Attach interleaved data. Allocation is performed outside.
	inline void AttachInterleavedData( void * in_pData, AkUInt16 in_uMaxFrames, AkUInt16 in_uValidFrames )
	{ 
		pData = in_pData; 
		uMaxFrames = in_uMaxFrames; 
		uValidFrames = in_uValidFrames; 
		AKASSERT(channelConfig.IsValid());
	}
	/// Attach interleaved data with a new channel config. Allocation is performed outside.
	inline void AttachInterleavedData( void * in_pData, AkUInt16 in_uMaxFrames, AkUInt16 in_uValidFrames, AkChannelConfig in_channelConfig )
	{ 
		pData = in_pData; 
		uMaxFrames = in_uMaxFrames; 
		uValidFrames = in_uValidFrames; 
		channelConfig = in_channelConfig; 
	}
	//@}

	/// \name Deinterleaved interface
	//@{

	/// Check if buffer has samples attached to it.
	 bool HasData() const
	{
		return ( NULL != pData ); 
	}

	/// Convert a channel, identified by a single channel bit, to a buffer index used in GetChannel() below, for a given channel config.
	/// Standard indexing follows channel bit order (see AkSpeakerConfig.h). Pipeline/buffer indexing is the same but the LFE is moved to the end.
	static inline AkUInt32 StandardToPipelineIndex( 
		AkChannelConfig in_channelConfig,		///< Channel configuration.
		AkUInt32		in_uChannelIdx			///< Channel index in standard ordering to be converted to pipeline ordering.
		)
	{
		if ( in_channelConfig.HasLFE() )
		{
			AKASSERT( in_channelConfig.eConfigType == AK_ChannelConfigType_Standard );	// in_channelConfig.HasLFE() would not have returned true otherwise.
			AKASSERT( AK::GetNumNonZeroBits( in_channelConfig.uChannelMask ) );
			AkUInt32 uIdxLFE = AK::GetNumNonZeroBits( ( AK_SPEAKER_LOW_FREQUENCY - 1 ) & in_channelConfig.uChannelMask );
			if ( in_uChannelIdx == uIdxLFE )
				return in_channelConfig.uNumChannels - 1;
			else if ( in_uChannelIdx > uIdxLFE )
				return in_uChannelIdx - 1;
		}

		return in_uChannelIdx;
	}

	/// Get the buffer of the ith channel. 
	/// Access to channel data is most optimal through this method. Use whenever the
	/// speaker configuration is known, or when an operation must be made independently
	/// for each channel.
	/// \remarks When using a standard Wwise pipeline configuration, use ChannelBitToIndex() to convert channel bits to buffer indices.
	/// \return Address of the buffer of the ith channel.
	/// \sa
	/// - \ref fx_audiobuffer_struct
	/// - \ref fx_audiobuffer_struct_channels
	inline AkSampleType * GetChannel(
		AkUInt32 in_uIndex		///< Channel index [0,NumChannels()-1]
		)
	{
		AKASSERT( in_uIndex < NumChannels() );
		return (AkSampleType*)((AkUInt8*)(pData) + ( in_uIndex * sizeof(AkSampleType) * MaxFrames() ));
	}

	/// Get the buffer of the LFE.
	/// \return Address of the buffer of the LFE. Null if there is no LFE channel.
	/// \sa
	/// - \ref fx_audiobuffer_struct_channels
	inline AkSampleType * GetLFE()
	{
		if ( channelConfig.uChannelMask & AK_SPEAKER_LOW_FREQUENCY )
			return GetChannel( NumChannels()-1 );
		
		return (AkSampleType*)0;
	}

	/// Can be used to transform an incomplete into a complete buffer with valid data.
	/// The invalid frames are made valid (zeroed out) for all channels and the validFrames count will be made equal to uMaxFrames.
	void ZeroPadToMaxFrames()
	{
		AKASSERT(pData != nullptr || MaxFrames() == 0);
		// The following members MUST be copied locally due to multi-core calls to this function.
		const AkUInt32 uNumChannels = NumChannels();
		const AkUInt32 uNumCurrentFrames = AkMin(uValidFrames, MaxFrames());
		const AkUInt32 uNumZeroFrames = MaxFrames() - uNumCurrentFrames;
		if ( uNumZeroFrames )
		{
			AKASSERT(pData != nullptr);
			for ( AkUInt32 i = 0; i < uNumChannels; ++i )
			{
				memset( GetChannel(i) + uNumCurrentFrames, 0, uNumZeroFrames * sizeof(AkSampleType) );
			}
			uValidFrames = MaxFrames();
		}
	}

	/// Attach deinterleaved data where channels are contiguous in memory. Allocation is performed outside.
	 void AttachContiguousDeinterleavedData( void * in_pData, AkUInt16 in_uMaxFrames, AkUInt16 in_uValidFrames, AkChannelConfig in_channelConfig )
	{ 
		AttachInterleavedData( in_pData, in_uMaxFrames, in_uValidFrames, in_channelConfig );
	}
	/// Detach deinterleaved data where channels are contiguous in memory. The address of the buffer is returned and fields are cleared.
	 void * DetachContiguousDeinterleavedData()
	{
		uMaxFrames = 0; 
		uValidFrames = 0; 
		channelConfig.Clear();
		void * pDataOld = pData;
		pData = NULL;
		return pDataOld;
	}

	bool CheckValidSamples();	

	//@}

	void RelocateMedia( AkUInt8* in_pNewMedia,  AkUInt8* in_pOldMedia )
	{
		AkUIntPtr uMemoryOffset = (AkUIntPtr)in_pNewMedia - (AkUIntPtr)in_pOldMedia;
		pData = (void*) (((AkUIntPtr)pData) + uMemoryOffset);
	}

	/// Access to the number of sample frames the buffer can hold.
	/// \return Number of sample frames the buffer can hold.
	 AkUInt16 MaxFrames() const { return uMaxFrames; }

protected:
	void *			pData;				///< Start of the audio buffer.
	AkChannelConfig	channelConfig;		///< Channel config.

public:	
	AKRESULT		eState;				///< Execution status

protected:	
	AkUInt16		uMaxFrames;			///< Number of sample frames the buffer can hold. Access through AkAudioBuffer::MaxFrames().

public:
	AkUInt16        uValidFrames;      ///< Number of valid sample frames in the audio buffer
};

#endif // _AK_COMMON_DEFS_H_
/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Apache License Usage

Alternatively, this file may be used under the Apache License, Version 2.0 (the 
"Apache License"); you may not use this file except in compliance with the 
Apache License. You may obtain a copy of the Apache License at 
http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing, software distributed
under the Apache License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
OR CONDITIONS OF ANY KIND, either express or implied. See the Apache License for
the specific language governing permissions and limitations under the License.

  Copyright (c) 2026 Audiokinetic Inc.
*******************************************************************************/

#pragma once

#include <AK/SoundEngine/Common/AkTypes.h>
#include <AK/SoundEngine/Common/AkSoundEngineExport.h>
#include <AK/SoundEngine/Common/AkMidiTypes.h>
#include <AK/SoundEngine/Common/AkSpeakerConfig.h>

#ifdef __cplusplus

namespace AK
{
	class IAkGlobalPluginContext;
	class IAkMixerInputContext;
	class IAkMixerPluginContext;
	class IAkGlobalPluginContext;
	struct AkMetering;
}

class AkAudioBuffer;

typedef AK::IAkMixerInputContext* AkMixerInputContextPtr;
typedef AK::IAkMixerPluginContext* AkMixerPluginContextPtr;
typedef AK::IAkGlobalPluginContext* AkGlobalPluginContextPtr;

#else

typedef void* AkMixerInputContextPtr;
typedef void* AkMixerPluginContextPtr;
typedef void* AkGlobalPluginContextPtr;

#endif

/// Type of callback. Used as a bitfield in methods AK::SoundEngine::PostEvent() and AK::SoundEngine::DynamicSequence::Open().
enum AkCallbackType
{
	AK_EndOfEvent = 0x0001,                  ///<Callback triggered when reaching the end of an event. No additional callback information.
	AK_EndOfDynamicSequenceItem = 0x0002,    ///<Callback triggered when reaching the end of a dynamic sequence item. Callback info can be cast to AkDynamicSequenceItemCallbackInfo.
	AK_Marker = 0x0004,                      ///<Callback triggered when encountering a marker during playback. Callback info can be cast to AkMarkerCallbackInfo.
	AK_Duration = 0x0008,                    ///<Callback triggered when the duration of the sound is known by the sound engine. Callback info can be cast to AkDurationCallbackInfo.

	AK_SpeakerVolumeMatrix = 0x0010,         ///<Callback triggered at each frame, letting the client modify the speaker volume matrix. Callback info can be cast to AkSpeakerVolumeMatrixCallbackInfo.

	AK_Starvation = 0x0020,                  ///<Callback triggered when playback skips a frame due to stream starvation. No additional callback information.

	AK_MusicPlaylistSelect = 0x0040,         ///<Callback triggered when music playlist container must select the next item to play. Callback info can be cast to AkMusicPlaylistCallbackInfo.
	AK_MusicPlayStarted = 0x0080,            ///<Callback triggered when a "Play" or "Seek" command has been executed ("Seek" commands are issued from AK::SoundEngine::SeekOnEvent()). Applies to objects of the Containers hierarchy only. No additional callback information.

	AK_MusicSyncBeat = 0x0100,               ///<Enable notifications on Music Beat. Callback info can be cast to AkMusicSyncCallbackInfo.
	AK_MusicSyncBar = 0x0200,                ///<Enable notifications on Music Bar. Callback info can be cast to AkMusicSyncCallbackInfo.
	AK_MusicSyncEntry = 0x0400,              ///<Enable notifications on Music Entry Cue. Callback info can be cast to AkMusicSyncCallbackInfo.
	AK_MusicSyncExit = 0x0800,               ///<Enable notifications on Music Exit Cue. Callback info can be cast to AkMusicSyncCallbackInfo.
	AK_MusicSyncGrid = 0x1000,               ///<Enable notifications on Music Grid. Callback info can be cast to AkMusicSyncCallbackInfo.
	AK_MusicSyncUserCue = 0x2000,            ///<Enable notifications on Music Custom Cue. Callback info can be cast to AkMusicSyncCallbackInfo.
	AK_MusicSyncPoint = 0x4000,              ///<Enable notifications on Music switch transition synchronization point. Callback info can be cast to AkMusicSyncCallbackInfo.

	AK_MIDIEvent = 0x8000,                   ///<Enable notifications for MIDI events. Callback info can be cast to AkMIDIEventCallbackInfo.
	
	AK_DynamicSequenceSelect = 0x10000,      ///<Callback triggered when dynamic sequence must select the next item to play. Callback info can be cast to AkDynamicSequenceSelectCallbackInfo.

	AK_Callback_Last = 0x20000,              ///< Last callback unused bit, invalid value.

	AK_MusicSyncAll = 0x7f00,                ///<Use this flag if you want to receive all notifications concerning AK_MusicSync registration.    
	AK_CallbackBits = 0xfffff,               ///<Bitmask for all callback types.

	AK_EnableGetMusicPlayPosition = 0x200000,       ///<Enable play position information of music objects, queried via AK::MusicEngine::GetPlayingSegmentInfo().
	AK_EnableGetSourceStreamBuffering = 0x400000,   ///<Enable stream buffering information for use by AK::SoundEngine::GetSourceStreamBuffering(). 

	AK_SourceInfo_Last = 0x800000,           ///<Last source info enable bit, invalid value.
};
typedef AkUInt32 AkCallbackType_t;

enum AkAudioDeviceEvent
{
	AkAudioDeviceEvent_Initialization,              ///< Sent after an Audio Device has initialized.  Initialization might have failed, check the AKRESULT.
	AkAudioDeviceEvent_Removal,                     ///< Audio device was removed through explicit call (AK::SoundEngine::RemoveOutput or AK::SoundEngine::Term)
	AkAudioDeviceEvent_SystemRemoval,               ///< Audio device was removed because of a system event (disconnection), hardware or driver problem. Check the AKRESULT when called through AkDeviceStatusCallbackFunc, it may give more context.
	AkAudioDeviceEvent_Last                         ///< End of enum, invalid value.
};
typedef AkUInt8 AkAudioDeviceEvent_t;

/// Bit field of various locations in the audio processing loop where the game can be called back.
enum AkGlobalCallbackLocation
{
	AkGlobalCallbackLocation_Register = (1 << 0),     ///< Right after successful registration of callback/plugin. Typically used by plugins along with AkGlobalCallbackLocation_Term for allocating memory for the lifetime of the sound engine.

	AkGlobalCallbackLocation_Begin = (1 << 1),        ///< Start of audio processing. The number of frames about to be rendered depends on the sink/end-point and can be zero.

	AkGlobalCallbackLocation_PreProcessMessageQueueForRender = (1 << 2),  ///< Start of frame rendering, before having processed game messages.
	AkGlobalCallbackLocation_PostMessagesProcessed = (1 << 3),            ///< After one or more messages have been processed, but before updating game object and listener positions internally.
	AkGlobalCallbackLocation_BeginRender = (1 << 4),                      ///< Start of frame rendering, after having processed game messages.
	AkGlobalCallbackLocation_EndRender = (1 << 5),                        ///< End of frame rendering.

	AkGlobalCallbackLocation_End = (1 << 6),          ///< End of audio processing.

	AkGlobalCallbackLocation_Term = (1 << 7),         ///< Sound engine termination.

	AkGlobalCallbackLocation_Monitor = (1 << 8),      ///< Send monitor data
	AkGlobalCallbackLocation_MonitorRecap = (1 << 9), ///< Send monitor data connection to recap.

	AkGlobalCallbackLocation_Init = (1 << 10),        ///< Sound engine initialization.

	AkGlobalCallbackLocation_Suspend = (1 << 11),            ///< Sound engine suspension through \ref AK::SoundEngine::Suspend
	AkGlobalCallbackLocation_WakeupFromSuspend = (1 << 12),  ///< Sound engine awakening through \ref AK::SoundEngine::WakeupFromSuspend

	AkGlobalCallbackLocation_ProfilerConnect = (1 << 13),    ///< Wwise Profiler has connected to the game.
	AkGlobalCallbackLocation_ProfilerDisconnect = (1 << 14), ///< Wwise Profiler has disconnected from the game.


	// IMPORTANT: Keep in sync with number of locations.
	AkGlobalCallbackLocation_Num = 15				///< Total number of global callback locations.
};
typedef AkUInt8 AkGlobalCallbackLocation_t;

/// Structure used to query info on active playing segments.
struct AkSegmentInfo
{
	AkTimeMs iCurrentPosition;        ///< Current position of the segment, relative to the Entry Cue, in milliseconds. Range is [-iPreEntryDuration, iActiveDuration+iPostExitDuration].
	AkTimeMs iPreEntryDuration;       ///< Duration of the pre-entry region of the segment, in milliseconds.
	AkTimeMs iActiveDuration;         ///< Duration of the active region of the segment (between the Entry and Exit Cues), in milliseconds.
	AkTimeMs iPostExitDuration;       ///< Duration of the post-exit region of the segment, in milliseconds.
	AkTimeMs iRemainingLookAheadTime; ///< Number of milliseconds remaining in the "looking-ahead" state of the segment, when it is silent but streamed tracks are being prefetched.
	AkReal32 fBeatDuration;           ///< Beat Duration in seconds.
	AkReal32 fBarDuration;            ///< Bar Duration in seconds.
	AkReal32 fGridDuration;           ///< Grid duration in seconds.
	AkReal32 fGridOffset;             ///< Grid offset in seconds.
};

/// Callback information structure corresponding to \ref AK_EndOfEvent, \ref AK_MusicPlayStarted and \ref AK_Starvation.
/// \sa 
/// - AK::SoundEngine::PostEvent()
/// - \ref soundengine_events
struct AkEventCallbackInfo
{
	AkGameObjectID gameObjID;     ///< Game object ID
	AkPlayingID    playingID;     ///< Playing ID of Event, returned by PostEvent()
	AkUniqueID     eventID;       ///< Unique ID of Event, passed to PostEvent()
};

/// Callback information structure corresponding to \ref AkCallbackType::AK_MIDIEvent
/// \sa 
/// - AK::SoundEngine::PostEvent()
/// - \ref soundengine_events
struct AkMIDIEventCallbackInfo
{
	struct AkMIDIEvent midiEvent;          ///< MIDI event triggered by event.
};

/// Callback information structure corresponding to \ref AK_Marker.
/// \sa 
/// - AK::SoundEngine::PostEvent()
/// - \ref soundengine_events
/// - \ref soundengine_markers
struct AkMarkerCallbackInfo
{
	AkUInt32    uIdentifier;            ///< Cue point identifier
	AkUInt32    uPosition;              ///< Position in the cue point (unit: sample frames)
	const char* strLabel;               ///< Label of the marker (null-terminated)
	AkUInt32    uLabelSize;             ///< Size of the label string (including the terminating null character)
};

/// Callback information structure corresponding to \ref AK_Duration.
/// \sa 
/// - AK::SoundEngine::PostEvent()
/// - \ref soundengine_events
struct AkDurationCallbackInfo
{
	AkReal32    fDuration;              ///< Duration of the sound (unit: milliseconds)
	AkReal32    fEstimatedDuration;     ///< Estimated duration of the sound depending on source settings such as pitch. (unit: milliseconds)
	AkUniqueID  audioNodeID;            ///< Audio Node ID of playing item
	AkUniqueID  mediaID;                ///< Media ID of playing item. (corresponds to 'ID' attribute of 'File' element in SoundBank metadata file)
	bool        bStreaming;             ///< True if source is streaming, false otherwise.
};

/// Callback information structure corresponding to \ref AK_EndOfDynamicSequenceItem.
/// \sa 
/// - AK::SoundEngine::PostEvent()
/// - AK::SoundEngine::DynamicSequence::Open()
/// - \ref soundengine_events
struct AkDynamicSequenceItemCallbackInfo
{
	AkUniqueID audioNodeID;             ///< Audio Node ID of finished item
	void*      pCustomInfo;             ///< Custom info passed to the DynamicSequence::Open function
};

/// Callback information structure corresponding to \ref AK_SpeakerVolumeMatrix, and passed to callbacks registered in RegisterBusVolumeCallback()
/// or PostEvent() with AK_SpeakerVolumeMatrix. These callbacks are called at every audio frame for every connection from an input (voice
/// or bus) to an output bus (standard or auxiliary), at the point when an input signal is about to be mixed into a mixing bus, but just before 
/// having been scaled in accordance to volumes authored in Wwise. The volumes are passed via this structure as pointers because they can be modified
/// in the callbacks. They are factored into two linear values ([0..1]): a common base value (pfBaseVolume), that is channel-agnostic and represents 
/// the collapsed gain of all volume changes in Wwise (sliders, actions, RTPC, attenuations, ...), and a matrix of gains per input/output channel, 
/// which depends on spatialization. Use the methods of AK_SpeakerVolumes_Matrix, defined in AK/SoundEngine/Common/AkSpeakerVolumes.h, to perform operations on them. 
/// Access each input channel of the volumes matrix with AK_SpeakerVolumes_Matrix_GetChannel(), passing it the input and output channel configuration.
/// Then, you may access each element of the output vector using the standard bracket [] operator. See AK/SoundEngine/Common/AkSpeakerVolumes.h for more details.
/// It is crucial that the processing done in the callback be lightweight and non-blocking.
/// \sa 
/// - \ref goingfurther_speakermatrixcallback
/// - AK::SoundEngine::PostEvent()
/// - \ref soundengine_events
/// - AK::SoundEngine::RegisterBusVolumeCallback()
struct AkSpeakerVolumeMatrixCallbackInfo
{
	AkSpeakerVolumesMatrixPtr       pVolumes;                ///< Pointer to volume matrix describing the contribution of each source channel to destination channels. Use methods of AK::SpeakerVolumes::Matrix to interpret them. 
	struct AkChannelConfig          inputConfig;             ///< Channel configuration of the voice/bus.
	struct AkChannelConfig          outputConfig;            ///< Channel configuration of the output bus.
	AkReal32*                       pfBaseVolume;            ///< Base volume, common to all channels.
	AkReal32*                       pfEmitterListenerVolume; ///< Emitter-listener pair-specific gain. When there are multiple emitter-listener pairs, this volume is set to that of the loudest pair, and the relative gain of other pairs is applied directly on the channel volume matrix pVolumes.
	AkGameObjectID                  mixConnectionGameObjId;  ///< Game Object ID of the mix connection
	AkMixerInputContextPtr          pContext;                ///< Context of the current voice/bus about to be mixed into the output bus with specified base volume and volume matrix.
	AkMixerPluginContextPtr         pMixerContext;           ///< Output mixing bus context. Use it to access a few useful panning and mixing services, as well as the ID of the output bus. NULL if pContext is the Main Audio Bus.
};

/// Callback information structure corresponding to \ref AK_MusicPlaylistSelect.
/// Called when a music playlist container must select its next item to play.
/// The members uPlaylistSelection and uPlaylistItemDone are set by the sound
/// engine before the callback function call.  They are set to the next item
/// selected by the sound engine.  They are to be modified by the callback
/// function if the selection is to be changed.
/// \sa 
/// - \ref soundengine_events
/// - AK::SoundEngine::PostEvent()
/// - \ref soundengine_music_callbacks
struct AkMusicPlaylistCallbackInfo
{
	AkUniqueID playlistID;                ///< ID of playlist node
	AkUInt32   uNumPlaylistItems;         ///< Number of items in playlist node (may be segments or other playlists)
	AkUInt32   uPlaylistSelection;        ///< Selection: set by sound engine, modified by callback function (if not in range 0 <= uPlaylistSelection < uNumPlaylistItems then ignored).
	AkUInt32   uPlaylistItemDone;         ///< Playlist node done: set by sound engine, modified by callback function (if set to anything but 0 then the current playlist item is done, and uPlaylistSelection is ignored)
};

/// Callback information structure corresponding to \ref AK_MusicSyncEntry, \ref AK_MusicSyncBeat, \ref AK_MusicSyncBar, \ref AK_MusicSyncExit, \ref AK_MusicSyncGrid, \ref AK_MusicSyncPoint and \ref AK_MusicSyncUserCue.
/// If you need the Tempo, you can compute it using the fBeatDuration
/// Tempo (beats per minute) = 60/fBeatDuration
/// \sa 
/// - \ref soundengine_events
/// - AK::SoundEngine::PostEvent()
/// - \ref soundengine_music_callbacks
struct AkMusicSyncCallbackInfo
{
	struct AkSegmentInfo  segmentInfo;      ///< Segment information corresponding to the segment triggering this callback.
	enum AkCallbackType   musicSyncType;    ///< Would be either \ref AK_MusicSyncEntry, \ref AK_MusicSyncBeat, \ref AK_MusicSyncBar, \ref AK_MusicSyncExit, \ref AK_MusicSyncGrid, \ref AK_MusicSyncPoint or \ref AK_MusicSyncUserCue.
	const char*           pszUserCueName;   ///< Cue name (UTF-8 string). Set for notifications AK_MusicSyncUserCue. NULL if cue has no name.
};

#ifdef __cplusplus

struct AkCallbackInfo
{
	void*          pCookie;    ///<User data, passed to PostEvent()
	AkGameObjectID gameObjID;  ///<Game object ID
};

/// Callback information structure allowing to query signal metering on busses, at each frame, after having mixed all their inputs and processed their effects.
/// Register the callback using AK::SoundEngine::RegisterBusMeteringCallback.
struct AkBusMeteringCallbackInfo : public AkCallbackInfo
{
	AK::AkMetering* pMetering;       ///< Struct containing metering information.
	AkChannelConfig	channelConfig;   ///< Channel configuration of the bus.
	AkMeteringFlags eMeteringFlags;  ///< Metering flags that were asked for in RegisterBusMeteringCallback(). You may only access corresponding meter values from in_pMeteringInfo. Others will fail.
};

/// Callback information structure allowing to query signal metering on output devices, at each frame.
/// Register the callback using AK::SoundEngine::RegisterOutputDeviceMeteringCallback.
struct AkOutputDeviceMeteringCallbackInfo : public AkCallbackInfo
{
	AK::AkMetering*  pMainMixMetering;             ///< Metering information for the main mix
	AkChannelConfig	 mainMixConfig;                ///< Channel configuration of the main mix
	AK::AkMetering*  pPassthroughMetering;         ///< Metering information for the passthrough mix (if any; will be null otherwise)
	AkChannelConfig	 passthroughMixConfig;         ///< Channel configuration of the passthrough mix (if any; will be invalid otherwise)
	AkUInt32         uNumSystemAudioObjects;       ///< Number of System Audio Objects going out of the output device
	AK::AkMetering** ppSystemAudioObjectMetering;  ///< Metering information for each System Audio Object (number of elements is equal to uNumSystemAudioObjects)
	AkMeteringFlags  eMeteringFlags;               ///< Metering flags that were asked for in RegisterOutputDeviceMeteringCallback(). You may only access corresponding meter values from the metering objects. Others will fail.
};

#endif // __cplusplus

/// Resources data summary structure containing general information about the system
struct AkResourceMonitorDataSummary
{
	AkReal32 totalCPU;        ///< Pourcentage of the cpu time used for processing audio. Please note that the numbers may add up when using multiple threads.
	AkReal32 pluginCPU;       ///< Pourcentage of the cpu time used by plugin processing. Please note that the numbers may add up when using multiple threads.
	AkUInt32 physicalVoices;  ///< Number of active physical voices
	AkUInt32 virtualVoices;   ///< Number of active virtual voices
	AkUInt32 totalVoices;     ///< Number of active physical and virtual voices
	AkUInt32 nbActiveEvents;  ///< Number of events triggered at a certain time
};

/// Callback information structure corresponding to \ref AK_DynamicSequenceSelect.
/// Called when a dynamic sequence must select its next item to play.
/// The callee is expected to fill out the information contained in this structure.
/// If there is no item available to play, the callee must set \c audioNodeID to \c AK_INVALID_UNIQUE_ID.
/// This will cause the dynamic sequence to stop. Use \c AK::SoundEngine::DynamicSequence::Play to start playback again.
/// 
/// \akwarning 
/// When opening a dynamic sequence with the callback flag \c AK_DynamicSequenceSelect, the callback is the ONLY way to determine the next item to play.
/// \c AK::SoundEngine::DynamicSequence::LockPlaylist always returns \c NULL for dynamic sequences opened with \c AK_DynamicSequenceSelect.
/// \endakwarning
/// 
/// \sa 
/// - AK::SoundEngine::DynamicSequence::Open()
/// - \ref soundengine_dynamicdialogue
struct AkDynamicSequenceSelectCallbackInfo
{
	AkUniqueID audioNodeID;                          ///< Unique ID of Audio Node (can be resolved using AK::SoundEngine::DynamicDialogue API). Set to AK_INVALID_UNIQUE_ID to signal that no item is available to play.
	AkTimeMs msDelay;                                ///< Delay before playing this item, in milliseconds
	void* pCustomInfo;                               ///< Optional user data
	AkExternalSourceArray arExternalSources;         ///< Optional external sources. Use API described in AkExternalSourceArray.h to add required external sources to play the next item.
};

/// Function called on completion of an event, or when a marker is reached.
/// \param in_eType Type of callback.
/// \param in_pEventInfo Pointer to structure containing basic information about the event. This pointer is invalidated as soon as the callback function returns.
/// \param in_pCallbackInfo Pointer to structure containing callback information. This pointer is invalidated as soon as the callback function returns. Can be null if no callback information is available.
/// \remarks An event is considered completed once all of its actions have been executed and all the playbacks in this events are terminated.
/// \remarks This callback is executed from the audio processing thread. The processing time in the callback function should be minimal. Having too much processing time could result in slowing down the audio processing.
/// \remarks Before waiting on an \ref AK_EndOfEvent, make sure that the event is going to end. 
/// Some events can be continuously playing or infinitely looping, and the callback will not occur unless a specific stop event is sent to terminate the event.
/// \sa 
/// - AK::SoundEngine::PostEvent()
/// - AK::SoundEngine::DynamicSequence::Open()
/// - \ref soundengine_events
/// - \ref soundengine_markers
/// - \ref soundengine_music_callbacks
AK_CALLBACK(void, AkEventCallbackFunc)(enum AkCallbackType in_eType, struct AkEventCallbackInfo* in_pEventInfo, void* in_pCallbackInfo, void* in_pCookie);
typedef AkEventCallbackFunc AkCallbackFunc;

/// Function called on at every audio frame for the specified registered busses.
/// \sa 
/// - AkSpeakerVolumeMatrixCallbackInfo
/// - AK::SoundEngine::RegisterBusVolumeCallback()
/// - \ref goingfurther_speakermatrixcallback
AK_CALLBACK(void, AkBusCallbackFunc)(
	struct AkSpeakerVolumeMatrixCallbackInfo* in_pCallbackInfo, ///< Structure containing desired bus information.
	void* in_pCookie ///< Custom cookie that was passed to AK::SoundEngine::RegisterBusVolumeCallback
	);

/// Callback prototype used with asynchronous bank load/unload requests.
/// This function is called when the bank request has been processed 
/// and indicates if it was successfully executed or if an error occurred.
/// \param in_bankID Identifier of the bank that was explicitly loaded/unloaded. 
/// In the case of PrepareEvent() or PrepareGameSyncs(), this value contains 
/// the AkUniqueID of the event/game sync that was prepared/unprepared, if the array contained only
/// one element. Otherwise, in_bankID equals AK_INVALID_UNIQUE_ID.
/// \param in_pInMemoryBankPtr Value returned when the unloaded bank was loaded using an in memory location
/// \param in_eLoadResult Result of the requested action.
///	- AK_Success: Load or unload successful.
/// - AK_IDNotFound: At least one of the event/game sync identifiers passed to PrepareEvent() or PrepareGameSyncs() does not exist.
/// - AK_InsufficientMemory: Insufficient memory to store bank data.
/// - AK_BankReadError: I/O error.
/// - AK_WrongBankVersion: Invalid bank version: make sure the version of Wwise that 
/// you used to generate the SoundBanks matches that of the SDK you are currently using.
/// - AK_InvalidFile: File specified could not be opened.
/// - AK_InvalidParameter: Invalid parameter.
/// - AK_Fail: Load or unload failed for any other reason. (Most likely small allocation failure)
/// \param in_pCookie Optional cookie that was passed to the bank request.
/// \remarks This callback is executed from the bank thread. The processing time in the callback function should be minimal. Having too much processing time could slow down the bank loading process.
/// \sa 
/// - AK::SoundEngine::LoadBank()
/// - AK::SoundEngine::UnloadBank()
/// - AK::SoundEngine::PrepareEvent()
/// - AK::SoundEngine::PrepareGameSyncs()
/// - \ref soundengine_banks
AK_CALLBACK(void, AkBankCallbackFunc)(
	AkUInt32         in_bankID,
	const void*      in_pInMemoryBankPtr,
	enum AKRESULT    in_eLoadResult,
	void*            in_pCookie
	);

/// Callback prototype used for global callback registration.
/// This callback may be called from various locations within the audio processing loop. The exact location from which it is called is passed in in_eLocation, and corresponds to one of the values
/// that were passed to RegisterGlobalCallback(). See the possible values of AkGlobalCallbackLocation for more details about the available locations.
/// \remarks This callback is normally executed from the main audio thread. The processing time in the callback function should be minimal. Having too much processing time could cause voice starvation.
/// \sa
/// - AK::SoundEngine::RegisterGlobalCallback()
/// - AK::SoundEngine::UnregisterGlobalCallback()
AK_CALLBACK(void, AkGlobalCallbackFunc)(
	AkGlobalPluginContextPtr      in_pContext,       ///< Engine context.
	enum AkGlobalCallbackLocation in_eLocation,      ///< Location where this callback is fired.
	void*                         in_pCookie         ///< User cookie passed to AK::SoundEngine::RegisterGlobalCallback().
	);


AK_CALLBACK(void, AkResourceMonitorCallbackFunc)(
	const struct AkResourceMonitorDataSummary* in_pdataSummary  ///< Data summary passed to the function registered using AK::SoundEngine::RegisterResourceMonitorCallback().
	);

/// Callback for Audio Device status changes.
/// \sa AK::SoundEngine::AddOutput
AK_CALLBACK(void, AkDeviceStatusCallbackFunc)(
	AkGlobalPluginContextPtr in_pContext,                 ///< Engine context.
	AkUniqueID               in_idAudioDeviceShareset,    ///< The audio device shareset attached, as passed to AK::SoundEngine::AddOutput or AK::SoundEngine::Init
	AkUInt32                 in_idDeviceID,               ///< The audio device specific id, as passed to AK::SoundEngine::AddOutput or AK::SoundEngine::Init
	enum AkAudioDeviceEvent  in_idEvent,                  ///< The event for which this callback was called.  See AK::AkAudioDeviceEvent.  AKRESULT may provide more information.
	enum AKRESULT            in_AkResult                  ///< Result of the last operation.
	);

#ifdef __cplusplus

/// Function called on at every audio frame for the specified registered busses, just after metering has been computed.
/// \sa 
/// - AK::SoundEngine::RegisterBusMeteringCallback()
/// - AK::AkMetering
/// - AkBusMeteringCallbackInfo
/// - \ref goingfurther_speakermatrixcallback
AK_CALLBACK(void, AkBusMeteringCallbackFunc)(
	AkBusMeteringCallbackInfo* in_pCallbackInfo   ///< Structure containing desired bus information.
	);

/// Function called on at every audio frame for the specified registered output devices, just after metering has been computed.
/// \sa 
/// - AK::SoundEngine::RegisterOutputDeviceMeteringCallback()
/// - AK::AkMetering
/// - AkOutputDeviceMeteringCallbackInfo
/// - \ref goingfurther_speakermatrixcallback
AK_CALLBACK(void, AkOutputDeviceMeteringCallbackFunc)(
	AkOutputDeviceMeteringCallbackInfo* in_pCallbackInfo   ///< Structure containing desired output device information.
	);

/// Callback prototype used for audio capture callback registration.
/// This callback will be called at the end of each audio frame for each output device that has been registered.
/// \remarks This callback will be executed on the main audio thread during real-time rendering and will be executed on the thread that \ref AK::SoundEngine::RenderAudio is called from when offline rendering is enabled.
/// \remarks The processing time in the callback function should be minimal. Having too much processing time could cause voice starvation during real-time rendering.
/// \remarks Note that a callback registered with <tt>in_idOutput</tt> equal to <tt>AK_INVALID_OUTPUT_DEVICE_ID</tt>, will receive the <tt>AkOutputDeviceID</tt> associated with the main output device.
/// \sa
/// - AK::SoundEngine::RenderAudio()
/// - AK::SoundEngine::RegisterCaptureCallback()
/// - AK::SoundEngine::UnregisterCaptureCallback()
AK_CALLBACK(void, AkCaptureCallbackFunc)(
	AkAudioBuffer&   in_CaptureBuffer,    ///< Capture audio buffer. The data is always float interleaved.
	AkOutputDeviceID in_idOutput,         ///< The audio device specific id, as passed to AK::SoundEngine::AddOutput or AK::SoundEngine::Init
	void*            in_pCookie           ///< Callback cookie that will be sent to the callback function along with additional information
	);

namespace AK
{
	// \deprecated Use AkAudioDeviceEvent in the global scope.
	using ::AkAudioDeviceEvent;
	const ::AkAudioDeviceEvent AkAudioDeviceEvent_Initialization = ::AkAudioDeviceEvent_Initialization;
	const ::AkAudioDeviceEvent AkAudioDeviceEvent_Removal = ::AkAudioDeviceEvent_Removal;
	const ::AkAudioDeviceEvent AkAudioDeviceEvent_SystemRemoval = ::AkAudioDeviceEvent_SystemRemoval;
	const ::AkAudioDeviceEvent AkAudioDeviceEvent_Last = ::AkAudioDeviceEvent_Last;

	using ::AkDeviceStatusCallbackFunc;
}
#endif // __cplusplus
/***********************************************************************
  The content of this file includes source code for the sound engine
  portion of the AUDIOKINETIC Wwise Technology and constitutes "Level
  Two Source Code" as defined in the Source Code Addendum attached
  with this file.  Any use of the Level Two Source Code shall be
  subject to the terms and conditions outlined in the Source Code
  Addendum and the End User License Agreement for Wwise(R).

  Copyright (c) 2026 Audiokinetic Inc.
 ***********************************************************************/

#ifndef _AK_MIDI_WWISE_CMDS_H_
#define _AK_MIDI_WWISE_CMDS_H_

#define AK_MIDI_WWISE_CMD_PLAY          0
#define AK_MIDI_WWISE_CMD_STOP          1
#define AK_MIDI_WWISE_CMD_PAUSE         2
#define AK_MIDI_WWISE_CMD_RESUME        3
#define AK_MIDI_WWISE_CMD_SEEK_MS       4
#define AK_MIDI_WWISE_CMD_SEEK_SAMPLES  5

#endif
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

#include <AK/SoundEngine/Common/AkCallback.h>
#include <AK/Tools/Common/AkMonitorError.h>

#include "SwigExceptionSwitch.h"


PAUSE_SWIG_EXCEPTIONS
#ifdef SWIG
%feature("immutable");
#endif // SWIG

#if SWIG
%extend AkSerializedCallbackHeader
{
	AkSerializedCallbackHeader* pNext;
}
#else
struct AkSerializedCallbackHeader* AkSerializedCallbackHeader_pNext_get(struct AkSerializedCallbackHeader *info);
#endif // SWIG

// This is used in C# to distinguish between different callbacks. Event callbacks have a type in AkCallbackType,
// but we want to distinguish other types
enum AkCallbackCategory
{
	AK_CallbackCategory_None, // Unspecified callback
	AK_CallbackCategory_Event, // AkCallbackFunc
	AK_CallbackCategory_Monitoring, // AK::Monitor::LocalOutputFunc
	AK_CallbackCategory_Bank, // AkBankCallbackFunc
	AK_CallbackCategory_AudioInterruption, // AudioInterruptionCallbackFunc
	AK_CallbackCategory_AudioSourceChange // AkBackgroundMusicChangeCallbackFunc
};

struct AkSerializedCallbackHeader
{
	void* pPackage; //The C# CallbackPackage to return to C#
	AkSerializedCallbackHeader* pNext; //Pointer to the next callback
	AkCallbackCategory eCategory; //The type of callback function
	AkUInt32 eType; //The type of structure following, for now, an enumerate of AkCallbackType only used by event callbacks

	void* GetData() const { return (void*)(this + 1); }
};

struct AkSerializedCallbackInfo
{
	void* pCookie; ///< User data, passed to PostEvent()
	AkGameObjectID gameObjID; ///< Game object ID
};

struct AkSerializedEventCallbackInfo : AkSerializedCallbackInfo
{
	AkPlayingID		playingID;		///< Playing ID of Event, returned by PostEvent()
	AkUniqueID		eventID;		///< Unique ID of Event, passed to PostEvent()
};

enum AkMIDIEventTypes
{
	NOTE_OFF = 0x80,
	NOTE_ON = 0x90,
	NOTE_AFTERTOUCH = 0xa0,
	CONTROLLER = 0xb0,
	PROGRAM_CHANGE = 0xc0,
	CHANNEL_AFTERTOUCH = 0xd0,
	PITCH_BEND = 0xe0,
	SYSEX = 0xf0,
	ESCAPE = 0xf7,
	META = 0xff,
};

enum AkMIDICcTypes
{
	BANK_SELECT_COARSE = 0,
	MOD_WHEEL_COARSE = 1,
	BREATH_CTRL_COARSE = 2,
	CTRL_3_COARSE = 3,
	FOOT_PEDAL_COARSE = 4,
	PORTAMENTO_COARSE = 5,
	DATA_ENTRY_COARSE = 6,
	VOLUME_COARSE = 7,
	BALANCE_COARSE = 8,
	CTRL_9_COARSE = 9,
	PAN_POSITION_COARSE = 10,
	EXPRESSION_COARSE = 11,
	EFFECT_CTRL_1_COARSE = 12,
	EFFECT_CTRL_2_COARSE = 13,
	CTRL_14_COARSE = 14,
	CTRL_15_COARSE = 15,
	GEN_SLIDER_1 = 16,
	GEN_SLIDER_2 = 17,
	GEN_SLIDER_3 = 18,
	GEN_SLIDER_4 = 19,
	CTRL_20_COARSE = 20,
	CTRL_21_COARSE = 21,
	CTRL_22_COARSE = 22,
	CTRL_23_COARSE = 23,
	CTRL_24_COARSE = 24,
	CTRL_25_COARSE = 25,
	CTRL_26_COARSE = 26,
	CTRL_27_COARSE = 27,
	CTRL_28_COARSE = 28,
	CTRL_29_COARSE = 29,
	CTRL_30_COARSE = 30,
	CTRL_31_COARSE = 31,
	BANK_SELECT_FINE = 32,
	MOD_WHEEL_FINE = 33,
	BREATH_CTRL_FINE = 34,
	CTRL_3_FINE = 35,
	FOOT_PEDAL_FINE = 36,
	PORTAMENTO_FINE = 37,
	DATA_ENTRY_FINE = 38,
	VOLUME_FINE = 39,
	BALANCE_FINE = 40,
	CTRL_9_FINE = 41,
	PAN_POSITION_FINE = 42,
	EXPRESSION_FINE = 43,
	EFFECT_CTRL_1_FINE = 44,
	EFFECT_CTRL_2_FINE = 45,
	CTRL_14_FINE = 46,
	CTRL_15_FINE = 47,
	CTRL_20_FINE = 52,
	CTRL_21_FINE = 53,
	CTRL_22_FINE = 54,
	CTRL_23_FINE = 55,
	CTRL_24_FINE = 56,
	CTRL_25_FINE = 57,
	CTRL_26_FINE = 58,
	CTRL_27_FINE = 59,
	CTRL_28_FINE = 60,
	CTRL_29_FINE = 61,
	CTRL_30_FINE = 62,
	CTRL_31_FINE = 63,
	HOLD_PEDAL = 64,
	PORTAMENTO_ON_OFF = 65,
	SUSTENUTO_PEDAL = 66,
	SOFT_PEDAL = 67,
	LEGATO_PEDAL = 68,
	HOLD_PEDAL_2 = 69,
	SOUND_VARIATION = 70,
	SOUND_TIMBRE = 71,
	SOUND_RELEASE_TIME = 72,
	SOUND_ATTACK_TIME = 73,
	SOUND_BRIGHTNESS = 74,
	SOUND_CTRL_6 = 75,
	SOUND_CTRL_7 = 76,
	SOUND_CTRL_8 = 77,
	SOUND_CTRL_9 = 78,
	SOUND_CTRL_10 = 79,
	GENERAL_BUTTON_1 = 80,
	GENERAL_BUTTON_2 = 81,
	GENERAL_BUTTON_3 = 82,
	GENERAL_BUTTON_4 = 83,
	REVERB_LEVEL = 91,
	TREMOLO_LEVEL = 92,
	CHORUS_LEVEL = 93,
	CELESTE_LEVEL = 94,
	PHASER_LEVEL = 95,
	DATA_BUTTON_P1 = 96,
	DATA_BUTTON_M1 = 97,
	NON_REGISTER_COARSE = 98,
	NON_REGISTER_FINE = 99,
	ALL_SOUND_OFF = 120,
	ALL_CONTROLLERS_OFF = 121,
	LOCAL_KEYBOARD = 122,
	ALL_NOTES_OFF = 123,
	OMNI_MODE_OFF = 124,
	OMNI_MODE_ON = 125,
	OMNI_MONOPHONIC_ON = 126,
	OMNI_POLYPHONIC_ON = 127,
};

#if SWIG
%extend AkSerializedMIDIEventCallbackInfo
{
	AkMIDIEventTypes byType;

	AkMidiNoteNo	byOnOffNote;
	AkUInt8			byVelocity;

	AkMIDICcTypes		byCc;
	AkUInt8		byCcValue;

	AkUInt8		byValueLsb;
	AkUInt8		byValueMsb;

	AkUInt8		byAftertouchNote;
	AkUInt8		byNoteAftertouchValue;

	AkUInt8		byChanAftertouchValue;

	AkUInt8		byProgramNum;
}
#else
AkMIDIEventTypes AkSerializedMIDIEventCallbackInfo_byType_get(struct AkSerializedMIDIEventCallbackInfo *info);
AkMidiNoteNo AkSerializedMIDIEventCallbackInfo_byOnOffNote_get(struct AkSerializedMIDIEventCallbackInfo *info);
AkUInt8 AkSerializedMIDIEventCallbackInfo_byVelocity_get(struct AkSerializedMIDIEventCallbackInfo *info);
AkMIDICcTypes AkSerializedMIDIEventCallbackInfo_byCc_get(struct AkSerializedMIDIEventCallbackInfo *info);
AkUInt8 AkSerializedMIDIEventCallbackInfo_byCcValue_get(struct AkSerializedMIDIEventCallbackInfo *info);
AkUInt8 AkSerializedMIDIEventCallbackInfo_byValueLsb_get(struct AkSerializedMIDIEventCallbackInfo *info);
AkUInt8 AkSerializedMIDIEventCallbackInfo_byValueMsb_get(struct AkSerializedMIDIEventCallbackInfo *info);
AkUInt8 AkSerializedMIDIEventCallbackInfo_byAftertouchNote_get(struct AkSerializedMIDIEventCallbackInfo *info);
AkUInt8 AkSerializedMIDIEventCallbackInfo_byNoteAftertouchValue_get(struct AkSerializedMIDIEventCallbackInfo *info);
AkUInt8 AkSerializedMIDIEventCallbackInfo_byChanAftertouchValue_get(struct AkSerializedMIDIEventCallbackInfo *info);
AkUInt8 AkSerializedMIDIEventCallbackInfo_byProgramNum_get(struct AkSerializedMIDIEventCallbackInfo *info);
#endif // SWIG

struct AkSerializedMIDIEventCallbackInfo : AkSerializedEventCallbackInfo
{
	// AkMIDIEvent expanded to prevent packing issues
	// BEGIN_AKMIDIEVENT_EXPANSION

	AkUInt8 byType; // (Ak_MIDI_EVENT_TYPE_)
	AkMidiChannelNo byChan;

	AkUInt8		byParam1;
	AkUInt8		byParam2;

	// END_AKMIDIEVENT_EXPANSION
};

struct AkSerializedMarkerCallbackInfo : AkSerializedEventCallbackInfo
{
	AkUInt32	uIdentifier;		///< Cue point identifier
	AkUInt32	uPosition;			///< Position in the cue point (unit: sample frames)
	char		strLabel[1];		///< Label of the marker, read from the file
};

struct AkSerializedDurationCallbackInfo : AkSerializedEventCallbackInfo
{
	AkReal32	fDuration;				///< Duration of the sound (unit: milliseconds)
	AkReal32	fEstimatedDuration;		///< Estimated duration of the sound depending on source settings such as pitch. (unit: milliseconds)
	AkUniqueID	audioNodeID;			///< Audio Node ID of playing item
	AkUniqueID  mediaID;				///< Media ID of playing item. (corresponds to 'ID' attribute of 'File' element in SoundBank metadata file)
	bool bStreaming;				///< True if source is streaming, false otherwise.
};

struct AkSerializedDynamicSequenceItemCallbackInfo : AkSerializedCallbackInfo
{
	AkPlayingID playingID;			///< Playing ID of Dynamic Sequence, returned by AK::SoundEngine:DynamicSequence::Open()
	AkUniqueID	audioNodeID;		///< Audio Node ID of finished item
	void*		pCustomInfo;		///< Custom info passed to the DynamicSequence::Open function
};

struct AkSerializedMusicSyncCallbackInfo : AkSerializedCallbackInfo
{
	AkPlayingID playingID;			///< Playing ID of Event, returned by PostEvent()
	//AkSegmentInfo segmentInfo; ///< Segment information corresponding to the segment triggering this callback.

	// AkSegmentInfo expanded to prevent packing issues
	// BEGIN_AKSEGMENTINFO_EXPANSION
	AkTimeMs		segmentInfo_iCurrentPosition;		///< Current position of the segment, relative to the Entry Cue, in milliseconds. Range is [-iPreEntryDuration, iActiveDuration+iPostExitDuration].
	AkTimeMs		segmentInfo_iPreEntryDuration;		///< Duration of the pre-entry region of the segment, in milliseconds.
	AkTimeMs		segmentInfo_iActiveDuration;		///< Duration of the active region of the segment (between the Entry and Exit Cues), in milliseconds.
	AkTimeMs		segmentInfo_iPostExitDuration;		///< Duration of the post-exit region of the segment, in milliseconds.
	AkTimeMs		segmentInfo_iRemainingLookAheadTime;///< Number of milliseconds remaining in the "looking-ahead" state of the segment, when it is silent but streamed tracks are being prefetched.
	AkReal32		segmentInfo_fBeatDuration;			///< Beat Duration in seconds.
	AkReal32		segmentInfo_fBarDuration;			///< Bar Duration in seconds.
	AkReal32		segmentInfo_fGridDuration;			///< Grid duration in seconds.
	AkReal32		segmentInfo_fGridOffset;			///< Grid offset in seconds.
	// END_AKSEGMENTINFO_EXPANSION

	AkCallbackType musicSyncType;	///< Would be either AK_MusicSyncEntry, AK_MusicSyncBeat, AK_MusicSyncBar, AK_MusicSyncExit, AK_MusicSyncGrid, AK_MusicSyncPoint or AK_MusicSyncUserCue.
	char  userCueName[1];
};

struct AkSerializedMusicPlaylistCallbackInfo : public AkSerializedEventCallbackInfo
{
	AkUniqueID playlistID;			///< ID of playlist node
	AkUInt32 uNumPlaylistItems;		///< Number of items in playlist node (may be segments or other playlists)
	AkUInt32 uPlaylistSelection;	///< Selection: set by sound engine, modified by callback function (if not in range 0 <= uPlaylistSelection < uNumPlaylistItems then ignored).
	AkUInt32 uPlaylistItemDone;		///< Playlist node done: set by sound engine, modified by callback function (if set to anything but 0 then the current playlist item is done, and uPlaylistSelection is ignored)
};

struct AkSerializedBankCallbackInfo
{
	AkUInt32 bankID;
	void* inMemoryBankPtr; // changed from AkUIntPtr to 'void*'
	AKRESULT loadResult;
};

struct AkSerializedMonitoringCallbackInfo
{
	AK::Monitor::ErrorCode errorCode;
	AK::Monitor::ErrorLevel errorLevel;
	AkPlayingID playingID;
	AkGameObjectID gameObjID;
	AkOSChar message[1];
};

struct AkSerializedAudioInterruptionCallbackInfo
{
	bool bEnterInterruption;
};

struct AkSerializedAudioSourceChangeCallbackInfo
{
	bool bOtherAudioPlaying;
};

#ifdef SWIG
%feature("immutable", "");
#endif // SWIG
RESUME_SWIG_EXCEPTIONS

class AkWAAPIErrorMessageTranslator;
class AkXMLErrorMessageTranslator;
class AkUnityErrorMessageTranslator;

/// This class allows the Sound Engine callbacks to be processed in the user thread instead of the audio thread.
/// This is done by accumulating the callback generating events until the game calls CallbackSerializer::PostCallbacks().
/// All pending callbacks will be done at that point.  This removes the need for external synchronization for the callback
/// functions that the game would need otherwise.
class AkCallbackSerializer
{
public:
	static AKRESULT Init();

	PAUSE_SWIG_EXCEPTIONS
	static void Term();
	RESUME_SWIG_EXCEPTIONS

	static void* Lock();
	static void Unlock();

	static void SetLocalOutput(AkUInt32 in_uErrorLevel, char* in_ip, AkUInt32 in_port, char* in_xmlFilePath, AkUInt32 in_msXmlTranslatorTimeout, AkUInt32 in_msWaapiTranslatorTimeout);
	static void FreeXmlTranslatorHandle(char* in_xmlFilePath, AkUInt32 in_msXmlTranslatorTimeout);
	
	static void EventCallback(AkCallbackType in_eType, AkEventCallbackInfo* in_pEventInfo, void* in_pCallbackInfo, void* in_pCookie);
	static void BankCallback(AkUInt32 in_bankID, void* in_pInMemoryBankPtr, AKRESULT in_eLoadResult, void *in_pCookie);

	static void AudioInterruptionCallbackFunc(bool in_bEnterInterruption, void* in_pCookie);
	static AKRESULT AudioSourceChangeCallbackFunc(bool in_bOtherAudioPlaying, void* in_pCookie);
private:
#ifndef AK_OPTIMIZED
#if (defined AK_WIN) || defined AK_MAC
	static AkWAAPIErrorMessageTranslator m_waapiErrorMessageTranslator;
#endif
	static AkXMLErrorMessageTranslator m_xmlErrorMessageTranslator;
	static AkUnityErrorMessageTranslator m_unityErrorMessageTranslator;
#endif
};
/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Apache License Usage

Alternatively, this file may be used under the Apache License, Version 2.0 (the 
"Apache License"); you may not use this file except in compliance with the 
Apache License. You may obtain a copy of the Apache License at 
http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing, software distributed
under the Apache License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
OR CONDITIONS OF ANY KIND, either express or implied. See the Apache License for
the specific language governing permissions and limitations under the License.

  Copyright (c) 2026 Audiokinetic Inc.
*******************************************************************************/

#ifndef _AKMONITORERROR_H
#define _AKMONITORERROR_H

#include <AK/SoundEngine/Common/AkSoundEngineExport.h>
#include <AK/SoundEngine/Common/AkTypes.h>
#define ERROR_CODE_DEF( in_name, in_msg ){ MonitorErrorInfo(in_name, in_msg)  }

struct AkStreamMgrSettings;
struct AkDeviceSettings;
class AkErrorMessageTranslator;

namespace AK
{
    // Error monitoring.

	namespace Monitor
	{
		// This structure contains information related to the error message
		struct MsgContext
		{
			MsgContext(AkPlayingID pId = AK_INVALID_PLAYING_ID, AkGameObjectID goId = AK_INVALID_GAME_OBJECT, AkUniqueID nodeId = AK_INVALID_UNIQUE_ID, bool isBus = false)
				:	in_playingID{ pId },
					in_gameObjID{ goId },
					in_soundID{ nodeId },
					in_bIsBus{ isBus }
			{}

			AkPlayingID in_playingID;		///< Related Playing ID if applicable
			AkGameObjectID in_gameObjID;	///< Related Game Object ID if applicable, AK_INVALID_GAME_OBJECT otherwise
			AkUniqueID in_soundID;			///< Related Audio Node ID if applicable, AK_INVALID_UNIQUE_ID otherwise
			bool in_bIsBus;					///< true if in_audioNodeID is a bus
		};

		struct MonitorErrorInfo
		{
			MonitorErrorInfo(const AkOSChar* in_name = nullptr, const AkOSChar* in_message= nullptr)
				:	m_name{ in_name },
					m_message{in_message}

			{}
			const AkOSChar* m_name;
			const AkOSChar* m_message;
		};

		///  ErrorLevel
		enum ErrorLevel
		{
			ErrorLevel_Message	= (1<<0), // used as bitfield
			ErrorLevel_Error	= (1<<1),
			
			ErrorLevel_All = ErrorLevel_Message | ErrorLevel_Error
		};
		/// ErrorCode
		enum ErrorCode
		{
			ErrorCode_NoError = 0, // 0-based index into AK::Monitor::s_aszMonitorErrorInfos table 
			ErrorCode_FileNotFound, 
			ErrorCode_CannotOpenFile,
			ErrorCode_CannotStartStreamNoMemory,
			ErrorCode_IODeviceStr,
			ErrorCode_IncompatibleIOSettings,

			ErrorCode_PluginUnsupportedChannelConfiguration,
			ErrorCode_PluginMediaUnavailable,
			ErrorCode_PluginInitialisationFailed,
			ErrorCode_PluginProcessingFailed,
			ErrorCode_PluginExecutionInvalid,
			ErrorCode_PluginAllocationFailed,

			ErrorCode_VorbisSeekTableRecommended,

			ErrorCode_VorbisDecodeError,

			ErrorCode_ATRAC9DecodeFailed,
			ErrorCode_ATRAC9LoopSectionTooSmall,

			ErrorCode_InvalidAudioFileHeader,
			ErrorCode_AudioFileHeaderTooLarge,
			ErrorCode_LoopTooSmall,

			ErrorCode_TransitionNotAccurateChannel,
			ErrorCode_TransitionNotAccuratePluginMismatch,
			ErrorCode_TransitionNotAccurateRejectedByPlugin,
			ErrorCode_TransitionNotAccurateStarvation,
			ErrorCode_TransitionNotAccurateCodecError,
			ErrorCode_NothingToPlay, 
			ErrorCode_PlayFailed,

			ErrorCode_StingerCouldNotBeScheduled,
			ErrorCode_TooLongSegmentLookAhead,
			ErrorCode_CannotScheduleMusicSwitch,
			ErrorCode_TooManySimultaneousMusicSegments,
			ErrorCode_PlaylistStoppedForEditing,
			ErrorCode_MusicClipsRescheduledAfterTrackEdit,

			ErrorCode_CannotPlaySource_Create,
			ErrorCode_CannotPlaySource_VirtualOff,
			ErrorCode_CannotPlaySource_TimeSkip,
			ErrorCode_CannotPlaySource_InconsistentState,
			ErrorCode_MediaNotLoaded,
			ErrorCode_VoiceStarving,
			ErrorCode_StreamingSourceStarving,

			ErrorCode_PluginNotRegistered,
			ErrorCode_CodecNotRegistered,
			ErrorCode_PluginVersionMismatch,

			ErrorCode_EventIDNotFound,

			ErrorCode_InvalidGroupID,
			ErrorCode_SelectedNodeNotAvailable,
			ErrorCode_SelectedMediaNotAvailable,
			ErrorCode_NoValidSwitch,

			ErrorCode_BankLoadFailed,
			ErrorCode_ErrorWhileLoadingBank,
			ErrorCode_InsufficientSpaceToLoadBank,
			
			ErrorCode_LowerEngineCommandListFull,

			ErrorCode_SeekNoMarker,
			ErrorCode_CannotSeekContinuous,
			ErrorCode_SeekAfterEof,

			ErrorCode_UnknownGameObject,
			ErrorCode_GameObjectNeverRegistered,	//To be used by the Capture Log to replace ErrorCode_UnknownGameObject
			ErrorCode_DeadGameObject,				//To be used by the Capture Log to replace ErrorCode_UnknownGameObject
			ErrorCode_GameObjectIsNotEmitter,

			ErrorCode_ExternalSourceNotResolved,
			ErrorCode_FileFormatMismatch,

			ErrorCode_CommandQueueFull,
			ErrorCode_CommandTooLarge,

			ErrorCode_ModulatorScopeError_Inst,
			ErrorCode_ModulatorScopeError_Obj,

			ErrorCode_SeekAfterEndOfPlaylist,

			ErrorCode_OpusRequireSeekTable,
			ErrorCode_OpusDecodeError,
			
			ErrorCode_SourcePluginNotFound,

			ErrorCode_VirtualVoiceLimit,

			ErrorCode_NotEnoughMemoryToStart,
			ErrorCode_UnknownOpusError,			//Deprecated Opus error.

			ErrorCode_AudioDeviceInitFailure,
			ErrorCode_AudioDeviceRemoveFailure,
			ErrorCode_AudioDeviceNotFound,
			ErrorCode_AudioDeviceNotValid,

			ErrorCode_SpatialAudio_ListenerAutomationNotSupported,
			ErrorCode_MediaDuplicationLength,

			ErrorCode_HwVoicesSystemInitFailed, // When the hardware-accelerated subsystem fails to initialize
			ErrorCode_HwVoicesDecodeBatchFailed, // When a grouping of hardware-accelerated voices fail to decode collectively
			ErrorCode_HwVoiceLimitReached, // Cannot create any more hardware-accelerated voices
			ErrorCode_HwVoiceInitFailed, // A hardware-accelerated voice fails to be created, but not because the max number of voices was reached

			ErrorCode_OpusHWCommandFailed,
			
			ErrorCode_AddOutputListenerIdWithZeroListeners,

			ErrorCode_3DObjectLimitExceeded,

			ErrorCode_OpusHWFatalError,
			ErrorCode_OpusHWDecodeUnavailable,
			ErrorCode_OpusHWTimeout,

			ErrorCode_SystemAudioObjectsUnavailable,

			ErrorCode_AddOutputNoDistinctListener,

			ErrorCode_PluginCannotRunOnObjectConfig,
			ErrorCode_SpatialAudio_ReflectionBusError,

			ErrorCode_VorbisHWDecodeUnavailable,
			ErrorCode_ExternalSourceNoMemorySize,

			ErrorCode_MonitorQueueFull,
			ErrorCode_MonitorMsgTooLarge,

			ErrorCode_NonCompliantDeviceMemory,

			ErrorCode_JobWorkerFuncCallMismatch,
			ErrorCode_JobMgrOutOfMemory,

			ErrorCode_InvalidFileSize,
			ErrorCode_PluginMsg,

			
			ErrorCode_SinkOpenSL,
			ErrorCode_AudioOutOfRange,
			ErrorCode_AudioOutOfRangeOnBus,
			ErrorCode_AudioOutOfRangeOnBusFx,
			ErrorCode_AudioOutOfRangeRay,
			ErrorCode_UnknownDialogueEvent,
			ErrorCode_FailedPostingEvent,
			ErrorCode_OutputDeviceInitializationFailed,
			ErrorCode_UnloadBankFailed,

			ErrorCode_PluginFileNotFound,
			ErrorCode_PluginFileIncompatible,
			ErrorCode_PluginFileNotEnoughMemoryToStart,
			ErrorCode_PluginFileInvalid,
			ErrorCode_PluginFileRegisterFailed,

			ErrorCode_UnknownArgument,

			ErrorCode_DynamicSequenceAlreadyClosed,
			ErrorCode_PendingActionDestroyed,
			ErrorCode_CrossFadeTransitionIgnored,
			ErrorCode_MusicRendererSeekingFailed,

			//MONITOR_ERRORMSG
			ErrorCode_DynamicSequenceIdNotFound,
			ErrorCode_BusNotFoundByName,
			ErrorCode_AudioDeviceShareSetNotFound,
			ErrorCode_AudioDeviceShareSetNotFoundByName,

			ErrorCode_SoundEngineTooManyGameObjects,
			ErrorCode_SoundEngineTooManyPositions,
			ErrorCode_SoundEngineCantCallOnChildBus,
			ErrorCode_SoundEnginePlayingIdNotFound,
			ErrorCode_SoundEngineInvalidTransform,
			ErrorCode_SoundEngineTooManyEventPosts,

			ErrorCode_AudioSubsystemStoppedResponding,
			
			ErrorCode_NotEnoughMemInFunction,
			ErrorCode_FXNotFound,
			ErrorCode_AudioNodeNotFound,
			ErrorCode_SetBusConfigUnsupported,
			ErrorCode_BusNotFound,

			ErrorCode_MismatchingMediaSize,
			ErrorCode_IncompatibleBankVersion,
			ErrorCode_UnexpectedPrepareGameSyncsCall,
			ErrorCode_LoadingBankMismatch,

			ErrorCode_ProxyObjectMismatch,
			ErrorCode_ProxyObjectMemory,

			ErrorCode_MasterBusStructureNotLoaded,
			ErrorCode_TooManyChildren,
			ErrorCode_BankContainUneditableEffect,
			ErrorCode_MemoryAllocationFailed,
			ErrorCode_InvalidFloatPriority,
			ErrorCode_SoundLoadFailedInsufficientMemory,
			ErrorCode_NXDeviceRegistrationFailed,
			ErrorCode_MixPluginOnObjectBus,

			ErrorCode_MissingMusicNodeParent,
			ErrorCode_HardwareOpusDecoderError,
			ErrorCode_SetGeometryTooManyTriangleConnected,
			ErrorCode_SetGeometryTriangleTooLarge,
			ErrorCode_SetGeometryFailed,
			ErrorCode_RemovingGeometrySetFailed,
			ErrorCode_SetGeometryInstanceFailed,
			ErrorCode_RemovingGeometryInstanceFailed,

			ErrorCode_RevertingToDefaultAudioDevice,
			ErrorCode_RevertingToDummyAudioDevice,
			ErrorCode_AudioThreadSuspended,
			ErrorCode_AudioThreadResumed,
			ErrorCode_ResetPlaylistActionIgnoredGlobalScope,
			ErrorCode_ResetPlaylistActionIgnoredContinuous,
			ErrorCode_PlayingTriggerRateNotSupported,
			ErrorCode_SetGeometryTriangleIsSkipped,
			ErrorCode_SetGeometryInstanceInvalidTransform,

			//AkSpatialAudio:AkMonitorError_WithID
			ErrorCode_SetGameObjectRadiusSizeError,
			ErrorCode_SetPortalNonDistinctRoom,
			ErrorCode_SetPortalInvalidExtent,
			ErrorCode_SpatialAudio_PortalNotFound,

			//Invalid float
			ErrorCode_InvalidFloatInFunction,
			ErrorCode_FLTMAXNotSupported,


			ErrorCode_CannotInitializeAmbisonicChannelConfiguration,
			ErrorCode_CannotInitializePassthrough,
			ErrorCode_3DAudioUnsupportedSize,
			ErrorCode_AmbisonicNotAvailable,

			ErrorCode_NoAudioDevice,

			ErrorCode_Support,
			ErrorCode_ReplayMessage,
			ErrorCode_GameMessage,
			ErrorCode_TestMessage,
			ErrorCode_TranslatorStandardTagTest,
			ErrorCode_TranslatorWwiseTagTest,
			ErrorCode_TranslatorStringSizeTest,

			ErrorCode_InvalidParameter,

			ErrorCode_MaxAudioObjExceeded,
			ErrorCode_MMSNotEnabled,
			ErrorCode_NotEnoughSystemObj,
			ErrorCode_NotEnoughSystemObjWin,

			ErrorCode_TransitionNotAccurateSourceTooShort,

			ErrorCode_AlreadyInitialized,
			ErrorCode_WrongNumberOfArguments,
			ErrorCode_DataAlignement,
			ErrorCode_PluginMsgWithShareSet,
			ErrorCode_SoundEngineNotInit,
			ErrorCode_NoDefaultSwitch,
			ErrorCode_CantSetBoundSwitch,
			ErrorCode_IODeviceInitFailed,
			ErrorCode_SwitchListEmpty,
			ErrorCode_NoSwitchSelected,
			ErrorCode_FilePermissionError,

			ErrorCode_SetEffectOnRendered,
			ErrorCode_GeometryNotWatertight,
			
			ErrorCode_CannotInitialize3DAudio,
			ErrorCode_CannotInitializeInputCallbacks,
			ErrorCode_CannotConnectAVAudioEngineSource,
			
			ErrorCode_ChannelConfigRequestDenied,
			ErrorCode_MediaUpdatedFromWwise,
			ErrorCode_MediaErrorFromWwise,
			ErrorCode_OutputAlreadyExists,
			ErrorCode_UnknownStateGroup,
			ErrorCode_MediaErrorWwiseMRUFull,
			ErrorCode_AudioOut2ContextCreateError,
			ErrorCode_AudioOut2UserCreateError,

			ErrorCode_FeedbackOnAudioObjectsBus,
			
			ErrorCode_SpatialAudio_SiblingPortal,
			ErrorCode_ActivityPlayback_Warning,

			ErrorCode_CannotPlaySource_FileAccess,
			ErrorCode_MediaDiscrepancy,
			ErrorCode_WwiseIODisconnected,
			ErrorCode_WwiseIODisconnectedStr,
			ErrorCode_IODevice,

			ErrorCode_InvalidCommand,
			ErrorCode_PlayingIDAlreadyExists,
			ErrorCode_IOStreamLeak,

			ErrorCode_SetSidechainMixConfigInvalid,

			ErrorCode_NodeNotCompatibleWithMidi,

			// ALWAYS ADD NEW CODES AT THE END !!!!!!!
			// Otherwise it may break comm compatibility in a patch

			Num_ErrorCodes // THIS STAYS AT END OF ENUM
		};

		static_assert(Num_ErrorCodes == 227,
			"Please document your new ErrorCode "
			"in 'Documentation/Help/source_en/reference/common_errors_capture_log.xml', "
			"then you can increment this value."
		);

		/// Function prototype of local output function pointer.
		AK_CALLBACK( void, LocalOutputFunc )(
			ErrorCode in_eErrorCode,	///< Error code number value
			const AkOSChar* in_pszError,	///< Message or error string to be displayed
			ErrorLevel in_eErrorLevel,	///< Specifies whether it should be displayed as a message or an error
			AkPlayingID in_playingID,   ///< Related Playing ID if applicable, AK_INVALID_PLAYING_ID otherwise
			AkGameObjectID in_gameObjID ///< Related Game Object ID if applicable, AK_INVALID_GAME_OBJECT otherwise
			);

		extern const MonitorErrorInfo s_aszMonitorErrorInfos[ Num_ErrorCodes ];

		/// Post a monitoring message or error code. This will be displayed in the Wwise capture
		/// log. Since this function doesn't send variable arguments, be sure that the error code you're posting doesn't contain any tag.
		/// Otherwise, there'll be an undefined behavior
		/// \return AK_Success if successful, AK_Fail if there was a problem posting the message.
		///			In optimized mode, this function returns AK_NotCompatible.
		/// \remark This function is provided as a tracking tool only. It does nothing if it is 
		///			called in the optimized/release configuration and return AK_NotCompatible.
		extern   AKRESULT __cdecl  PostCode  ( 
			ErrorCode in_eError,		///< Message or error code to be displayed
			ErrorLevel in_eErrorLevel,	///< Specifies whether it should be displayed as a message or an error
			AkPlayingID in_playingID = AK_INVALID_PLAYING_ID,   ///< Related Playing ID if applicable
			AkGameObjectID in_gameObjID = AK_INVALID_GAME_OBJECT, ///< Related Game Object ID if applicable, AK_INVALID_GAME_OBJECT otherwise
			AkUniqueID in_audioNodeID = AK_INVALID_UNIQUE_ID, ///< Related Audio Node ID if applicable, AK_INVALID_UNIQUE_ID otherwise
			bool in_bIsBus = false		///< true if in_audioNodeID is a bus
			);

		extern  AKRESULT __cdecl  PostCodeVarArg (
			ErrorCode in_eError,		///< Error code to be displayed. This code corresponds to a predefined message, that may have parameters that can be passed in the variable arguments. Check the message format at the end of AkMonitorError.h.
			ErrorLevel in_eErrorLevel,	///< Specifies whether it should be displayed as a message or an error
			MsgContext msgContext,		///< The message context containing the following information : Related Playing ID if applicable, Related Game Object ID if applicable, AK_INVALID_GAME_OBJECT otherwise,  Related Audio Node ID if applicable, AK_INVALID_UNIQUE_ID otherwise and whether if in_audioNodeID is a bus
			...							///< The variable arguments, depending on the ErrorCode posted.
			);

		/// Post a monitoring message. This will be displayed in the Wwise capture log.
		/// \return AK_Success if successful, AK_Fail if there was a problem posting the message.
		///			In optimized mode, this function returns AK_NotCompatible.
		/// \remark This function is provided as a tracking tool only. It does nothing if it is 
		///			called in the optimized/release configuration and return AK_NotCompatible.
		extern  AKRESULT __cdecl  PostCodeVaList (
			ErrorCode in_eError,		///< Error code to be displayed. This code corresponds to a predefined message, that may have parameters that can be passed in the variable arguments. Check the message format at the end of AkMonitorError.h.
			ErrorLevel in_eErrorLevel,	///< Specifies whether it should be displayed as a message or an error
			MsgContext msgContext,		///< The message context containing the following information : Related Playing ID if applicable, Related Game Object ID if applicable, AK_INVALID_GAME_OBJECT otherwise,  Related Audio Node ID if applicable, AK_INVALID_UNIQUE_ID otherwise and whether if in_audioNodeID is a bus
			::va_list args				///< The variable arguments, depending on the ErrorCode posted.
			);

#ifdef AK_SUPPORT_WCHAR
		/// Post a unicode monitoring message or error string. This will be displayed in the Wwise capture
		/// log.
		/// \return AK_Success if successful, AK_Fail if there was a problem posting the message.
		///			In optimized mode, this function returns AK_NotCompatible.
		/// \remark This function is provided as a tracking tool only. It does nothing if it is 
		///			called in the optimized/release configuration and return AK_NotCompatible.
		extern   AKRESULT __cdecl  PostString  ( 
			const wchar_t* in_pszError,	///< Message or error string to be displayed
			ErrorLevel in_eErrorLevel,	///< Specifies whether it should be displayed as a message or an error
			AkPlayingID in_playingID = AK_INVALID_PLAYING_ID,   ///< Related Playing ID if applicable
			AkGameObjectID in_gameObjID = AK_INVALID_GAME_OBJECT, ///< Related Game Object ID if applicable, AK_INVALID_GAME_OBJECT otherwise
			AkUniqueID in_audioNodeID = AK_INVALID_UNIQUE_ID, ///< Related Audio Node ID if applicable, AK_INVALID_UNIQUE_ID otherwise
			bool in_bIsBus = false		///< true if in_audioNodeID is a bus
			);

#endif // #ifdef AK_SUPPORT_WCHAR

		/// Post a monitoring message or error string. This will be displayed in the Wwise capture
		/// log.
		/// \return AK_Success if successful, AK_Fail if there was a problem posting the message.
		///			In optimized mode, this function returns AK_NotCompatible.
		/// \remark This function is provided as a tracking tool only. It does nothing if it is 
		///			called in the optimized/release configuration and return AK_NotCompatible.
		extern   AKRESULT __cdecl  PostString  ( 
			const char* in_pszError,	///< Message or error string to be displayed
			ErrorLevel in_eErrorLevel,	///< Specifies whether it should be displayed as a message or an error
			AkPlayingID in_playingID = AK_INVALID_PLAYING_ID,   ///< Related Playing ID if applicable
			AkGameObjectID in_gameObjID = AK_INVALID_GAME_OBJECT, ///< Related Game Object ID if applicable, AK_INVALID_GAME_OBJECT otherwise
			AkUniqueID in_audioNodeID = AK_INVALID_UNIQUE_ID, ///< Related Audio Node ID if applicable, AK_INVALID_UNIQUE_ID otherwise
			bool in_bIsBus = false		///< true if in_audioNodeID is a bus
			);

		/// Enable/Disable local output of monitoring messages or errors. Pass 0 to disable,
		/// or any combination of ErrorLevel_Message and ErrorLevel_Error to enable. 
		/// \return AK_Success.
		///			In optimized/release configuration, this function returns AK_NotCompatible.
		extern   AKRESULT __cdecl  SetLocalOutput  (
			AkUInt32 in_uErrorLevel	= ErrorLevel_All, ///< ErrorLevel(s) to enable in output. Default parameters enable all.
			LocalOutputFunc in_pMonitorFunc = 0 	  ///< Handler for local output. If NULL, the standard platform debug output method is used.
			);

		/// Add a translator to the wwiseErrorHandler
		/// The additional translators increase the chance of a monitoring messages or errors
		/// to be successfully translated.
		/// \return AK_Success.
		///	In optimized/release configuration, this function returns AK_NotCompatible.
		extern   AKRESULT __cdecl  AddTranslator  (
			AkErrorMessageTranslator* translator,	///< The AkErrorMessageTranslator to add to the WwiseErrorHandler
			bool overridePreviousTranslators = false		///< Whether or not the newly added translator should override all the previous translators. 
															///< In both cases, the default translator will remain
			);

		/// Reset the wwiseErrorHandler to only using the default translator
		/// \return AK_Success.
		///	In optimized/release configuration, this function returns AK_NotCompatible.
		extern  AKRESULT __cdecl  ResetTranslator (
			);

		/// Get the time stamp shown in the capture log along with monitoring messages.
		/// \return Time stamp in milliseconds.
		///			In optimized/release configuration, this function returns 0.
		extern   AkTimeMs __cdecl  GetTimeStamp  ();

		/// Add the streaming manager settings to the profiler capture.
		extern  void __cdecl  MonitorStreamMgrInit (
			const AkStreamMgrSettings& in_streamMgrSettings
			);

		/// Add device settings to the list of active streaming devices.
		/// The list of streaming devices and their settings will be 
		/// sent to the profiler capture when remote connecting from Wwise.
		/// 
		/// \remark \c AK::Monitor::MonitorStreamMgrTerm must be called to
		///			clean-up memory	used to keep track of active streaming devices.
		extern  void __cdecl  MonitorStreamingDeviceInit (
			AkDeviceID in_deviceID,
			const AkDeviceSettings& in_deviceSettings 
			);

		/// Remove streaming device entry from the list of devices
		/// to send when remote connecting from Wwise.
		extern  void __cdecl  MonitorStreamingDeviceDestroyed (
			AkDeviceID in_deviceID
			);

		/// Monitor streaming manager destruction as part of the
		/// profiler capture.
		/// 
		/// \remark This function must be called to clean-up memory	used by
		///			\c AK::Monitor::MonitorStreamingDeviceInit and \c AK::Monitor::MonitorStreamingDeviceTerm
		/// 		to keep track of active streaming devices.
		extern  void __cdecl  MonitorStreamMgrTerm ();
	}
}

// Macros.
#ifndef AK_OPTIMIZED
	#define AK_MONITOR_ERROR( in_eErrorCode ) \
		AK::Monitor::PostCode( in_eErrorCode, AK::Monitor::ErrorLevel_Error )

	#define AK_MONITOR_STREAM_MGR_INIT( in_streamMgrSettings ) \
		AK::Monitor::MonitorStreamMgrInit( in_streamMgrSettings )

	#define AK_MONITOR_STREAMING_DEVICE_INIT( in_deviceID, in_deviceSettings ) \
		AK::Monitor::MonitorStreamingDeviceInit( in_deviceID, in_deviceSettings )

	#define AK_MONITOR_STREAMING_DEVICE_DESTROYED( in_deviceID ) \
		AK::Monitor::MonitorStreamingDeviceDestroyed( in_deviceID )

	#define AK_MONITOR_STREAM_MGR_TERM( ) \
		AK::Monitor::MonitorStreamMgrTerm()
#else
	#define AK_MONITOR_ERROR( in_eErrorCode )
	#define AK_MONITOR_STREAM_MGR_INIT( in_streamMgrSettings )
	#define AK_MONITOR_STREAMING_DEVICE_INIT( in_deviceID, in_deviceSettings )
	#define AK_MONITOR_STREAMING_DEVICE_DESTROYED( in_deviceID )
	#define AK_MONITOR_STREAM_MGR_TERM( )
#endif

#ifdef AK_MONITOR_IMPLEMENT_ERRORCODES
#include <AK/Tools/Common/AkMonitorErrorImpl.h>
#endif // AK_MONITOR_IMPLEMENT_ERRORCODES

#endif // _AKMONITORERROR_H
/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Apache License Usage

Alternatively, this file may be used under the Apache License, Version 2.0 (the 
"Apache License"); you may not use this file except in compliance with the 
Apache License. You may obtain a copy of the Apache License at 
http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing, software distributed
under the Apache License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
OR CONDITIONS OF ANY KIND, either express or implied. See the Apache License for
the specific language governing permissions and limitations under the License.

  Copyright (c) 2026 Audiokinetic Inc.
*******************************************************************************/

// AkPlatformFuncs.h

/// \file 
/// Platform-dependent functions definition.

#ifndef _AK_TOOLS_COMMON_AKPLATFORMFUNCS_H
#define _AK_TOOLS_COMMON_AKPLATFORMFUNCS_H

#include <AK/SoundEngine/Common/AkTypes.h>
#include <AK/SoundEngine/Common/AkAtomicTypes.h>

namespace AK
{
    class IAkPluginMemAlloc;
}

// Uncomment the following to enable built-in platform profiler markers in the sound engine
//#define AK_ENABLE_INSTRUMENT

#if defined(AK_NULL_PLATFORM)
// null platform has no funcs
struct AkThreadProperties {};
#elif defined(AK_WIN)
#include <AK/Tools/Win32/AkPlatformFuncs.h>

#elif defined (AK_XBOX)
#include <AK/Tools/XboxGC/AkPlatformFuncs.h>

#elif defined (AK_APPLE)
#include <AK/Tools/Mac/AkPlatformFuncs.h>
#include <AK/Tools/POSIX/AkPlatformFuncs.h>

#elif defined( AK_ANDROID ) || defined ( AK_LINUX_AOSP ) 
#include <AK/Tools/Android/AkPlatformFuncs.h>

#elif defined ( AK_HARMONY ) 
#include <AK/Tools/OpenHarmony/AkPlatformFuncs.h>

#elif defined (AK_PS4)
#include <AK/Tools/PS4/AkPlatformFuncs.h>

#elif defined (AK_PS5)
#include <AK/Tools/PS5/AkPlatformFuncs.h>

#elif defined (AK_EMSCRIPTEN)
#include <AK/Tools/Emscripten/AkPlatformFuncs.h>

#elif defined (AK_LINUX)
#include <AK/Tools/Linux/AkPlatformFuncs.h>
#include <AK/Tools/POSIX/AkPlatformFuncs.h>

#elif defined (AK_QNX)
#include <AK/Tools/QNX/AkPlatformFuncs.h>
#include <AK/Tools/POSIX/AkPlatformFuncs.h>

#elif defined (AK_NX)
#include <AK/Tools/NX/AkPlatformFuncs.h>

#elif defined (AK_OUNCE)
#include <AK/Tools/Ounce/AkPlatformFuncs.h>

#else
#error AkPlatformFuncs.h: Undefined platform
#endif

#ifndef AkPrefetchZero
#define AkPrefetchZero(___Dest, ___Size)
#endif

#ifndef AkPrefetchZeroAligned
#define AkPrefetchZeroAligned(___Dest, ___Size)
#endif

#ifndef AkZeroMemAligned
#define AkZeroMemAligned(___Dest, ___Size) AKPLATFORM::AkMemSet(___Dest, 0, ___Size);
#endif
#ifndef AkZeroMemLarge
#define AkZeroMemLarge(___Dest, ___Size) AKPLATFORM::AkMemSet(___Dest, 0, ___Size);
#endif
#ifndef AkZeroMemSmall
#define AkZeroMemSmall(___Dest, ___Size) AKPLATFORM::AkMemSet(___Dest, 0, ___Size);
#endif

#ifndef AK_THREAD_INIT_CODE
#define AK_THREAD_INIT_CODE(_threadProperties)
#endif

#ifndef AK_PLATFORM_MEMCPY
namespace AKPLATFORM
{
	/// Platform Independent Helper for memcpy/memmove/memset
	 void AkMemCpy(void* pDest, const void* pSrc, AkUInt32 uSize)
	{
		memcpy(pDest, pSrc, uSize);
	}

	 void AkMemMove(void* pDest, const void* pSrc, AkUInt32 uSize)
	{
		memmove(pDest, pSrc, uSize);
	}

	 void AkMemSet(void* pDest, AkInt32 iVal, AkUInt32 uSize)
	{
		memset(pDest, iVal, uSize);
	}
}
#endif // AK_PLATFORM_MEMCPY

#if !defined(AK_NULL_PLATFORM)
/// Platform-dependent helpers
namespace AKPLATFORM
{
	// Threads
	// ------------------------------------------------------------------

	// Returns true if in_pThread is in a valid state and executing
	bool AkIsValidThread(AkThread* in_pThread);

	// Destroys the internal thread object
	//  (set the memalloc interface to nullptr to use the soundengine's built-in memalloc systems)
	void AkCloseThread(AkThread* in_pThread, AK::IAkPluginMemAlloc* in_pMemAlloc);
	// Resets the AkThread to an invalid/null-thread state
	void AkClearThread(AkThread* in_pThread);

	// Initializes a thread 
	AKRESULT AkCreateThread(
		AkThreadRoutine pStartRoutine,                 // Thread routine.
		void* pParams,                                 // Routine params.
		const AkThreadProperties& in_threadProperties, // Properties. NULL for default.
		AkThread* out_pThread,                         // Returned thread handle.
		const char* in_szThreadName,                   // thread name.
		AK::IAkPluginMemAlloc* in_pMemAlloc            // Interface to memory allocator for thread creation (set to nullptr to use the soundengine's built-in memalloc systems)
		);
	
	// Forces the current thread to yield for at least the specified # of milliseconds.
	// Actual time that the thread is yielded for may be significantly higher that value specified on some platforms and operating environments
	void AkSleep(AkUInt32 in_ulMilliseconds);

	// Blocks execution until the provided thread has returned and completed execution
	void AkWaitForSingleThread(AkThread* in_pThread);
	
	// Returns a platform-specific threadId for the current thread
	AkThreadID CurrentThread();

	// Sets the values in out_threadProperties to the normal soundengine defaults. Defaults may vary across platforms
	void AkGetDefaultThreadProperties(AkThreadProperties& out_threadProperties);
	// Sets the values in out_threadProperties to the normal soundengine defaults, except for priority which will be "High"
	void AkGetDefaultHighPriorityThreadProperties(AkThreadProperties& out_threadProperties);
}
#endif

namespace AKPLATFORM
{
	// to be used to unequivocally cause a crash for scenarios that are critical failures that we cannot hope to recover from
	 void AkForceCrash()
	{
		// We don't want to simply abort() the program; we want a real SIGSEGV happening on *this* thread
		// This convoluted way of crashing is necessary to avoid compilers optimizing out invalid code
		// By calling PerformanceCounter, which is inherently non-deterministic, the compiler must generate code as-is
		AkInt64 one, two;
		PerformanceCounter(&one);
		PerformanceCounter(&two);
		AkUIntPtr ptr = (AkUIntPtr)two - (AkUIntPtr)one;
		((void(*)())ptr)();
	}
}

#ifndef AK_FORCE_CRASH
#define AK_FORCE_CRASH AKPLATFORM::AkForceCrash()
#endif

// on platforms that support it, this will invoke a breakpoint when a debugger is attached.
// Otherwise, forcefully crashes the process
#ifndef AK_DEBUG_BREAK
#define AK_DEBUG_BREAK AK_FORCE_CRASH
#endif

#endif // _AK_TOOLS_COMMON_AKPLATFORMFUNCS_H
/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Apache License Usage

Alternatively, this file may be used under the Apache License, Version 2.0 (the 
"Apache License"); you may not use this file except in compliance with the 
Apache License. You may obtain a copy of the Apache License at 
http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing, software distributed
under the Apache License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
OR CONDITIONS OF ANY KIND, either express or implied. See the Apache License for
the specific language governing permissions and limitations under the License.

  Copyright (c) 2026 Audiokinetic Inc.
*******************************************************************************/

#ifndef _AK_SOUNDENGINE_AKDYNAMICDIALOGUE_H
#define _AK_SOUNDENGINE_AKDYNAMICDIALOGUE_H

#include <AK/SoundEngine/Common/AkSoundEngine.h>

/// Callback prototype used with dialogue event resolution. This function is called
/// for every candidate in a ResolveDialogueEvent execution.
/// \return true to accept candidate, false to reject.
/// \sa 
/// - AK::SoundEngine::DynamicDialogue::ResolveDialogueEvent()
AK_CALLBACK( bool, AkCandidateCallbackFunc )(
	AkUniqueID in_idEvent,
	AkUniqueID in_idCandidate,
	void* in_cookie
	);

namespace AK
{
	namespace SoundEngine
	{
		/// Dynamic Dialogue namespace
		/// \remarks The functions in this namespace are thread-safe, unless stated otherwise.
		namespace DynamicDialogue
		{
			/// Resolve a dialogue event into an audio node ID based on the specified argument path.
	        /// \return Unique ID of audio node, or AK_INVALID_UNIQUE_ID if no audio node is defined for specified argument path
			extern   AkUniqueID __cdecl  ResolveDialogueEvent  (
					AkUniqueID			in_eventID,					///< Unique ID of dialogue event
					AkArgumentValueID*	in_aArgumentValues,			///< Argument path, as array of argument value IDs. AK_FALLBACK_ARGUMENTVALUE_ID indicates a fallback argument value
					AkUInt32			in_uNumArguments,			///< Number of argument value IDs in in_aArgumentValues
					AkPlayingID			in_idSequence = AK_INVALID_PLAYING_ID,	///< Optional sequence ID in which the token will be inserted (for profiling purposes)
					AkCandidateCallbackFunc in_candidateCallbackFunc = NULL, ///< Optional callback for candidate validation based on custom criteria
					void* in_pCookie = NULL							///< Callback cookie (reserved to user, passed to the callback function)
				);

#ifdef AK_SUPPORT_WCHAR
			/// Resolve a dialogue event into an audio node ID based on the specified argument path.
	        /// \return Unique ID of audio node, or AK_INVALID_UNIQUE_ID if no audio node is defined for specified argument path
			extern   AkUniqueID __cdecl  ResolveDialogueEvent  (
					const wchar_t*		in_pszEventName,			///< Name of dialogue event
					const wchar_t**		in_aArgumentValueNames,		///< Argument path, as array of argument value names. L"" indicates a fallback argument value
					AkUInt32			in_uNumArguments,			///< Number of argument value names in in_aArgumentValueNames
					AkPlayingID			in_idSequence = AK_INVALID_PLAYING_ID,	///< Optional sequence ID in which the token will be inserted (for profiling purposes)
					AkCandidateCallbackFunc in_candidateCallbackFunc = NULL, ///< Optional callback for candidate validation based on custom criteria
					void* in_pCookie = NULL							///< Callback cookie (reserved to user, passed to the callback function)
				);
#endif //AK_SUPPORT_WCHAR

			/// Resolve a dialogue event into an audio node ID based on the specified argument path.
	        /// \return Unique ID of audio node, or AK_INVALID_UNIQUE_ID if no audio node is defined for specified argument path
			extern   AkUniqueID __cdecl  ResolveDialogueEvent  (
					const char*			in_pszEventName,			///< Name of dialogue event
					const char**		in_aArgumentValueNames,		///< Argument path, as array of argument value names. "" indicates a fallback argument value
					AkUInt32			in_uNumArguments,			///< Number of argument value names in in_aArgumentValueNames
					AkPlayingID			in_idSequence = AK_INVALID_PLAYING_ID,	///< Optional sequence ID in which the token will be inserted (for profiling purposes)
					AkCandidateCallbackFunc in_candidateCallbackFunc = NULL, ///< Optional callback for candidate validation based on custom criteria
					void* in_pCookie = NULL							///< Callback cookie (reserved to user, passed to the callback function)
				);

			/// Get the value of a custom property of integer or boolean type.
			/// \return 
			/// - AK_Success if the value is found			
			/// - AK_PartialSuccess if the event was found but no matching custom property was found on this object. Note that it could mean this value is the default value.
			/// - AK_IDNotFound if the EventID is unknown (not loaded or typo in the id)
			extern  AKRESULT __cdecl  GetDialogueEventCustomPropertyValue (
				AkUniqueID in_eventID,			///< Unique ID of dialogue event
				AkUInt32 in_uPropID,			///< Property ID of your custom property found under the Custom Properties tab of the Wwise project settings.
				AkInt32& out_iValue				///< Property Value
				);

			/// Get the value of a custom property of real type.
			/// \return 
			/// - AK_Success if the value is found			
			/// - AK_PartialSuccess if the event was found but no matching custom property was found on this object. Note that it could mean this value is the default value.
			/// - AK_IDNotFound if the EventID is unknown (not loaded or typo in the id)
			extern  AKRESULT __cdecl  GetDialogueEventCustomPropertyValue (
				AkUniqueID in_eventID,			///< Unique ID of dialogue event
				AkUInt32 in_uPropID,			///< Property ID of your custom property found under the Custom Properties tab of the Wwise project settings.
				AkReal32& out_fValue			///< Property Value
				);
		}
	}
}

#endif // _AK_SOUNDENGINE_AKDYNAMICDIALOGUE_H
/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Apache License Usage

Alternatively, this file may be used under the Apache License, Version 2.0 (the 
"Apache License"); you may not use this file except in compliance with the 
Apache License. You may obtain a copy of the Apache License at 
http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing, software distributed
under the Apache License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
OR CONDITIONS OF ANY KIND, either express or implied. See the Apache License for
the specific language governing permissions and limitations under the License.

  Copyright (c) 2026 Audiokinetic Inc.
*******************************************************************************/

// AkQueryParameters.h

/// \file 
/// The sound engine parameter query interface.


#ifndef _AK_QUERYPARAMS_H_
#define _AK_QUERYPARAMS_H_

#include <AK/SoundEngine/Common/AkSoundEngineExport.h>
#include <AK/SoundEngine/Common/AkTypes.h>
#include <AK/SoundEngine/Common/AkCommonDefs.h>
#include <AK/Tools/Common/AkArray.h>

/// Positioning information obtained from an object
struct AkPositioningInfo
{
	AkReal32			fCenterPct;			///< Center % [0..1]
	AkSpeakerPanningType	pannerType;		///< Speaker panning type: type of panning logic when object is not 3D spatialized.
	Ak3DPositionType	e3dPositioningType;	///< 3D position type: defines what acts as the emitter position for computing spatialization against the listener. 
	bool				bHoldEmitterPosAndOrient;   ///< Hold emitter position and orientation values when starting playback.
	Ak3DSpatializationMode e3DSpatializationMode; ///< Spatialization mode
	bool				bEnableAttenuation;	///< Attenuation parameter set is active.

	bool				bUseConeAttenuation; ///< Use the cone attenuation
	AkReal32			fInnerAngle;		///< Inner angle
	AkReal32			fOuterAngle;		///< Outer angle
	AkReal32			fConeMaxAttenuation; ///< Cone max attenuation
	AkLPFType			LPFCone;			///< Cone low pass filter value
	AkLPFType			HPFCone;			///< Cone low pass filter value

	AkReal32			fMaxDistance;		///< Maximum distance
	AkReal32			fVolDryAtMaxDist;	///< Volume dry at maximum distance
	AkReal32			fVolAuxGameDefAtMaxDist;	///< Volume wet at maximum distance (if any) (based on the Game defined distance attenuation)
	AkReal32			fVolAuxUserDefAtMaxDist;	///< Volume wet at maximum distance (if any) (based on the User defined distance attenuation)
	AkLPFType			LPFValueAtMaxDist;  ///< Low pass filter value at max distance (if any)
	AkLPFType			HPFValueAtMaxDist;  ///< High pass filter value at max distance (if any)
};

/// Object information structure for QueryAudioObjectsIDs
struct AkObjectInfo
{
	AkUniqueID	objID;		///< Object ID
	AkUniqueID	parentID;	///< Object ID of the parent 
	AkInt32		iDepth;		///< Depth in tree
};

// Audiokinetic namespace
namespace AK
{
	// Audiokinetic sound engine namespace
	namespace SoundEngine
	{
		/// Query namespace
		/// \remarks The functions in this namespace are thread-safe, unless stated otherwise. We recommend that you use these functions in development builds only, because they can cause CPU spikes.
		///
		/// \akwarning
		/// The functions in this namespace might stall for several milliseconds before returning
		/// because they cannot execute while the main sound engine tick is running. 
		/// They should therefore not be called from any game-critical thread, such as the main game loop. 
		/// However, if the function definition states that it does not require the main audio lock, no delay should occur.
		///
		/// There might be a significant delay between a Sound Engine call, such as PostEvent, and
		/// the information being returned in a Query, such as GetIsGameObjectActive. 
		/// \endakwarning

		namespace Query
		{
			////////////////////////////////////////////////////////////////////////
			/// @name Game Objects
			//@{

			/// Get the position of a game object.
			/// \return AK_Success if succeeded, or AK_IDNotFound if the game object was not registered
			/// \sa 
			/// - \ref soundengine_3dpositions
			extern   AKRESULT __cdecl  GetPosition  ( 
				AkGameObjectID in_GameObjectID,				///< Game object identifier
				AkSoundPosition& out_rPosition				///< Position to get
				);

			//@}

			////////////////////////////////////////////////////////////////////////
			/// @name Listeners
			//@{

			/// Get a game object's listeners.  
			/// \return AK_Success if succeeded, or AK_IDNotFound if the game object was not registered
			/// \sa 
			/// - \ref soundengine_listeners
			extern   AKRESULT __cdecl  GetListeners  (
				AkGameObjectID in_GameObjectID,				///< Source game object identifier
				AkGameObjectID* out_ListenerObjectIDs,		///< Pointer to an array of AkGameObjectID's.  Will be populated with the IDs of the listeners of in_GameObjectID. Pass NULL to querry the size required.
				AkUInt32& oi_uNumListeners					///< Pass in the the available number of elements in the array 'out_ListenerObjectIDs'. After return, the number of valid elements filled in the array.
				);

			/// Get a listener's position.
			/// \return AK_Success if succeeded, or AK_InvalidParameter if the index is out of range
			/// \sa 
			/// - \ref soundengine_listeners_settingpos
			extern   AKRESULT __cdecl  GetListenerPosition  ( 
				AkGameObjectID in_uListenerID, 					///< Listener game object ID. 
				AkListenerPosition& out_rPosition			///< Position set
				);

			/// Get a listener's spatialization parameters. 
			/// \return AK_Success if succeeded, or AK_InvalidParameter if the index is out of range
			/// \sa 
			/// - AK::SoundEngine::SetListenerSpatialization().
			/// - \ref soundengine_listeners_spatial
			extern   AKRESULT __cdecl  GetListenerSpatialization  (
				AkGameObjectID in_uListenerID,				///< Listener game object ID. 
				bool& out_rbSpatialized,					///< Spatialization enabled
				AK::SpeakerVolumes::VectorPtr & out_pVolumeOffsets,	///< Per-speaker vector of volume offsets, in decibels. Use the functions of AK::SpeakerVolumes::Vector to interpret it.
				AkChannelConfig &out_channelConfig			///< Channel configuration associated with out_rpVolumeOffsets. 
				);
			
			//@}


			////////////////////////////////////////////////////////////////////////
			/// @name Game Syncs
			//@{

			/// Enum used to request a specific RTPC Value.
			/// Also used to inform the user of where the RTPC Value comes from.
			///
			/// For example, the user may request the GameObject specific value by specifying RTPCValue_GameObject
			/// and can receive the Global Value if there was no GameObject specific value, and even the 
			/// default value is there was no Global value either.
			/// \sa 
			/// - GetRTPCValue
			enum RTPCValue_type
			{
				RTPCValue_Default,		///< The value is the Default RTPC.
				RTPCValue_Global,		///< The value is the Global RTPC.
				RTPCValue_GameObject,	///< The value is the game object specific RTPC.
				RTPCValue_PlayingID,	///< The value is the playing ID specific RTPC.
				RTPCValue_Unavailable,	///< The value is not available for the RTPC specified.
				RTPCValue_Last,			///< End of enum, invalid value.
			};

			/// Get the value of a real-time parameter control (by ID)
			/// An RTPC can have a any combination of a global value, a unique value for each game object, or a unique value for each playing ID (Event).
			/// The value requested is determined by RTPCValue_type, in_gameObjectID and in_playingID.  
			/// If a value at the requested scope (determined by RTPCValue_type) is not found, the value that is available at the the next broadest scope will be returned, and io_rValueType will be changed to indicate this.
			/// \note
			///		When looking up RTPC values via playing ID (ie. io_rValueType is RTPC_PlayingID), in_gameObjectID can be set to a specific game object (if it is available to the caller) to use as a fall back value.
			///		If the game object is unknown or unavailable, AK_INVALID_GAME_OBJECT can be passed in in_gameObjectID, and the game object will be looked up via in_playingID.  
			///		However in this case, it is not possible to retrieve a game object value as a fall back value if the playing id does not exist.  It is best to pass in the game object if possible.
			/// \note
			///		Contrary to most AK::Query functions, GetRTPCValue does not block. This means that GetRTPCValue does not wait for all previous API to be completed (which may take several milliseconds), 
			///		and instead returns the current value. Therefore calling SetRTPCValue, RenderAudio and then GetRTPCValue might not immediately give the value set by SetRTPCValue.
			/// \return
			///	- AK_Success if succeeded
			/// - AK_IDNotFound if the RTPC does not exist
			/// - AK_InvalidID if the GameObject does not exist.
			/// \sa 
			/// - \ref soundengine_rtpc
			/// - RTPCValue_type
			extern  AKRESULT __cdecl  GetRTPCValue (
				AkRtpcID in_rtpcID, 				///< ID of the RTPC
				AkGameObjectID in_gameObjectID,		///< Associated game object ID, ignored if io_rValueType is RTPCValue_Global.
				AkPlayingID	in_playingID,			///< Associated playing ID, ignored if io_rValueType is not RTPC_PlayingID.
				AkRtpcValue& out_rValue, 			///< Value returned
				RTPCValue_type&	io_rValueType		///< In/Out value, the user must specify the requested type. The function will return in this variable the type of the returned value.
				);

#ifdef AK_SUPPORT_WCHAR

			/// Get the value of a real-time parameter control (by ID)
			/// An RTPC can have a any combination of a global value, a unique value for each game object, or a unique value for each playing ID.  
			/// The value requested is determined by RTPCValue_type, in_gameObjectID and in_playingID.  
			/// If a value at the requested scope (determined by RTPCValue_type) is not found, the value that is available at the the next broadest scope will be returned, and io_rValueType will be changed to indicate this.
			/// \note
			///		When looking up RTPC values via playing ID (ie. io_rValueType is RTPC_PlayingID), in_gameObjectID can be set to a specific game object (if it is available to the caller) to use as a fall back value.
			///		If the game object is unknown or unavailable, AK_INVALID_GAME_OBJECT can be passed in in_gameObjectID, and the game object will be looked up via in_playingID.  
			///		However in this case, it is not possible to retrieve a game object value as a fall back value if the playing id does not exist.  It is best to pass in the game object if possible.
			/// \note
			///		Contrary to most AK::Query functions, GetRTPCValue does not block. This means that GetRTPCValue does not wait for all previous API to be completed (which may take several milliseconds), 
			///		and instead returns the current value. Therefore calling SetRTPCValue, RenderAudio and then GetRTPCValue might not immediately give the value set by SetRTPCValue.
			/// \return
			///	- AK_Success if succeeded
			/// - AK_IDNotFound if the RTPC does not exist
			/// - AK_InvalidID if the GameObject does not exist.
			/// \sa 
			/// - \ref soundengine_rtpc
			/// - RTPCValue_type
			extern  AKRESULT __cdecl  GetRTPCValue (
				const wchar_t* in_pszRtpcName,		///< String name of the RTPC
				AkGameObjectID in_gameObjectID,		///< Associated game object ID, ignored if io_rValueType is RTPCValue_Global.
				AkPlayingID	in_playingID,			///< Associated playing ID, ignored if io_rValueType is not RTPC_PlayingID.
				AkRtpcValue& out_rValue, 			///< Value returned
				RTPCValue_type&	io_rValueType		///< In/Out value, the user must specify the requested type. The function will return in this variable the type of the returned value.				);
				);

#endif //AK_SUPPORT_WCHAR

			/// Get the value of a real-time parameter control (by ID)
			/// An RTPC can have a any combination of a global value, a unique value for each game object, or a unique value for each playing ID.  
			/// The value requested is determined by RTPCValue_type, in_gameObjectID and in_playingID.  
			/// If a value at the requested scope (determined by RTPCValue_type) is not found, the value that is available at the the next broadest scope will be returned, and io_rValueType will be changed to indicate this.
			/// \note
			///		When looking up RTPC values via playing ID (ie. io_rValueType is RTPC_PlayingID), in_gameObjectID can be set to a specific game object (if it is available to the caller) to use as a fall back value.
			///		If the game object is unknown or unavailable, AK_INVALID_GAME_OBJECT can be passed in in_gameObjectID, and the game object will be looked up via in_playingID.  
			///		However in this case, it is not possible to retrieve a game object value as a fall back value if the playing id does not exist.  It is best to pass in the game object if possible.
			/// \note
			///		Contrary to most AK::Query functions, GetRTPCValue does not block. This means that GetRTPCValue does not wait for all previous API to be completed (which may take several milliseconds), 
			///		and instead returns the current value. Therefore calling SetRTPCValue, RenderAudio and then GetRTPCValue might not immediately give the value set by SetRTPCValue.
			/// \return
			///	- AK_Success if succeeded
			/// - AK_IDNotFound if the RTPC does not exist
			/// - AK_InvalidID if the GameObject does not exist.
			/// \sa 
			/// - \ref soundengine_rtpc
			/// - RTPCValue_type
			extern  AKRESULT __cdecl  GetRTPCValue (
				const char* in_pszRtpcName,			///< String name of the RTPC
				AkGameObjectID in_gameObjectID,		///< Associated game object ID, ignored if io_rValueType is RTPCValue_Global.
				AkPlayingID	in_playingID,			///< Associated playing ID, ignored if io_rValueType is not RTPC_PlayingID.
				AkRtpcValue& out_rValue, 			///< Value returned
				RTPCValue_type&	io_rValueType		///< In/Out value, the user must specify the requested type. The function will return in this variable the type of the returned value.				);
				);

			/// Get the current Switch value of the specified Switch Group, for a game object 
			/// \note
			///		Contrary to most AK::Query functions, GetSwitch does not block. This means that GetSwitch does not wait for all previous API to be completed (which may take several milliseconds), 
			///		and instead returns the current value. Therefore calling SetSwitch, RenderAudio and then GetSwitch might not immediately give the value set by SetSwitch.
			/// \return
			///	- AK_Success if succeeded
			/// - AK_IDNotFound if the RTPC does not exist
			/// - AK_InvalidID if the GameObject does not exist.
			/// \return AK_Success if succeeded, or AK_IDNotFound if the game object was not registered or the Switch Group name cannot be found
			/// \sa 
			/// - \ref soundengine_switch
			/// - \ref AK::SoundEngine::SetSwitch
			extern   AKRESULT __cdecl  GetSwitch  ( 
				AkSwitchGroupID in_switchGroup, 			///< ID of the Switch Group
				AkGameObjectID  in_gameObjectID,			///< Associated game object ID
				AkSwitchStateID& out_rSwitchState 			///< ID of the Switch
				);

#ifdef AK_SUPPORT_WCHAR
			/// Get the current Switch value of the specified Switch Group, for a game object 
			/// \return AK_Success if succeeded, or AK_IDNotFound if the game object was not registered or the Switch Group name cannot be found
			/// \sa 
			/// - \ref soundengine_switch
			/// - \ref AK::SoundEngine::SetSwitch
			extern   AKRESULT __cdecl  GetSwitch  ( 
				const wchar_t* in_pstrSwitchGroupName,		///< String name of the Switch Group
				AkGameObjectID in_GameObj,					///< Associated game object ID
				AkSwitchStateID& out_rSwitchState			///< ID of the Switch
				);
#endif //AK_SUPPORT_WCHAR

			/// Get the current Switch value of the specified Switch Group, for a game object 
			/// \return AK_Success if succeeded, or AK_IDNotFound if the game object was not registered or the Switch Group name cannot be found
			/// \sa 
			/// - \ref soundengine_switch
			/// - \ref AK::SoundEngine::SetSwitch
			extern   AKRESULT __cdecl  GetSwitch  ( 
				const char* in_pstrSwitchGroupName,			///< String name of the Switch Group
				AkGameObjectID in_GameObj,					///< Associated game object ID
				AkSwitchStateID& out_rSwitchState			///< ID of the Switch
				);

			/// Get the current State value active in the specified State Group.
			/// \return AK_Success if succeeded
			/// \sa 
			/// - \ref soundengine_states
			/// - \ref AK::SoundEngine::SetState
			extern   AKRESULT __cdecl  GetState  ( 
				AkStateGroupID in_stateGroup, 				///< ID of the State Group
				AkStateID& out_rState 						///< ID of the state
				);

#ifdef AK_SUPPORT_WCHAR
			/// Get the current State value active in the specified State Group.
			/// \return AK_Success if succeeded
			/// \sa 
			/// - \ref soundengine_states
			/// - \ref AK::SoundEngine::SetState
			extern   AKRESULT __cdecl  GetState  ( 
				const wchar_t* in_pstrStateGroupName,		///< String name of the State Group
				AkStateID& out_rState						///< ID of the State
				);
#endif //AK_SUPPORT_WCHAR

			/// Get the current State value active in the specified State Group.
			/// \return AK_Success if succeeded
			/// \sa 
			/// - \ref soundengine_states
			/// - \ref AK::SoundEngine::SetState
			extern   AKRESULT __cdecl  GetState  ( 
				const char* in_pstrStateGroupName,			///< String name of the State Group
				AkStateID& out_rState						///< ID of the State
				);

			//@}

			////////////////////////////////////////////////////////////////////////
			/// @name Environments
			//@{

			/// Get the environmental ratios used by the specified game object.
			/// \sa 
			/// - \ref soundengine_environments
			/// - \ref soundengine_environments_dynamic_aux_bus_routing
			/// - \ref soundengine_environments_id_vs_string
			/// \return AK_Success if succeeded, or AK_InvalidParameter if io_ruNumSendValues is 0 or out_paEnvironmentValues is NULL, or AK_PartialSuccess if more environments exist than io_ruNumSendValues
			extern   AKRESULT __cdecl  GetGameObjectAuxSendValues  ( 
				AkGameObjectID		in_gameObjectID,		///< Associated game object ID
				AkAuxSendValue*		out_paAuxSendValues,	///< Variable-size array of AkAuxSendValue structures
																///< (it may be NULL if no aux send must be set)
				AkUInt32&			io_ruNumSendValues		///< The number of Auxiliary busses at the pointer's address
															///< (it must be 0 if no aux bus is set)
				);

			/// Get the environmental dry level to be used for the specified game object
			/// The control value is a number ranging from 0.0f to 1.0f.
			/// 0.0f stands for 0% dry, while 1.0f stands for 100% dry.
			/// \aknote Reducing the dry level does not mean increasing the wet level. \endaknote
			/// \sa 
			/// - \ref soundengine_environments
			/// - \ref soundengine_environments_setting_dry_environment
			/// - \ref soundengine_environments_id_vs_string
			/// \return AK_Success if succeeded, or AK_IDNotFound if the game object was not registered
			extern   AKRESULT __cdecl  GetGameObjectDryLevelValue  ( 
				AkGameObjectID		in_EmitterID,			///< Associated emitter game object ID
				AkGameObjectID		in_ListenerID,			///< Associated listener game object ID
				AkReal32&			out_rfControlValue		///< Dry level control value, ranging from 0.0f to 1.0f
															///< (0.0f stands for 0% dry, while 1.0f stands for 100% dry)
				);

			/// Get a game object's obstruction and occlusion levels.
			/// \sa 
			/// - \ref soundengine_obsocc
			/// - \ref soundengine_environments
			/// \return AK_Success if succeeded, AK_IDNotFound if the game object was not registered
			extern   AKRESULT __cdecl  GetObjectObstructionAndOcclusion  (  
				AkGameObjectID in_EmitterID,			///< Associated game object ID
				AkGameObjectID in_ListenerID,			///< Listener object ID
				AkReal32& out_rfObstructionLevel,		///< ObstructionLevel: [0.0f..1.0f]
				AkReal32& out_rfOcclusionLevel			///< OcclusionLevel: [0.0f..1.0f]
				);

			//@}

			/// Get the list of audio object IDs associated to an event.
			/// \aknote It is possible to call QueryAudioObjectIDs with io_ruNumItems = 0 to get the total size of the
			/// structure that should be allocated for out_aObjectInfos. \endaknote
			/// \return AK_Success if succeeded, AK_IDNotFound if the eventID cannot be found, AK_InvalidParameter if out_aObjectInfos is NULL while io_ruNumItems > 0
			/// or AK_PartialSuccess if io_ruNumItems was set to 0 to query the number of available items.
			extern   AKRESULT __cdecl  QueryAudioObjectIDs  (
				AkUniqueID in_eventID,				///< Event ID
				AkUInt32& io_ruNumItems,			///< Number of items in array provided / Number of items filled in array
				AkObjectInfo* out_aObjectInfos		///< Array of AkObjectInfo items to fill
				);

#ifdef AK_SUPPORT_WCHAR
			/// Get the list of audio object IDs associated to a event name.
			/// \aknote It is possible to call QueryAudioObjectIDs with io_ruNumItems = 0 to get the total size of the
			/// structure that should be allocated for out_aObjectInfos. \endaknote
			/// \return AK_Success if succeeded, AK_IDNotFound if the event name cannot be found, AK_InvalidParameter if out_aObjectInfos is NULL while io_ruNumItems > 0
			/// or AK_PartialSuccess if io_ruNumItems was set to 0 to query the number of available items.
			extern   AKRESULT __cdecl  QueryAudioObjectIDs  (
				const wchar_t* in_pszEventName,		///< Event name
				AkUInt32& io_ruNumItems,			///< Number of items in array provided / Number of items filled in array
				AkObjectInfo* out_aObjectInfos		///< Array of AkObjectInfo items to fill
				);
#endif //AK_SUPPORT_WCHAR

			/// Get the list of audio object IDs associated to an event name.
			/// \aknote It is possible to call QueryAudioObjectIDs with io_ruNumItems = 0 to get the total size of the
			/// structure that should be allocated for out_aObjectInfos. \endaknote
			/// \return AK_Success if succeeded, AK_IDNotFound if the event name cannot be found, AK_InvalidParameter if out_aObjectInfos is NULL while io_ruNumItems > 0
			/// or AK_PartialSuccess if io_ruNumItems was set to 0 to query the number of available items.
			extern   AKRESULT __cdecl  QueryAudioObjectIDs  (
				const char* in_pszEventName,		///< Event name
				AkUInt32& io_ruNumItems,			///< Number of items in array provided / Number of items filled in array
				AkObjectInfo* out_aObjectInfos		///< Array of AkObjectInfo items to fill
				);

			/// Get positioning information associated to an audio object.
			/// \return AK_Success if succeeded, AK_IDNotFound if the object ID cannot be found, AK_NotCompatible if the audio object cannot expose positioning
			extern   AKRESULT __cdecl  GetPositioningInfo  ( 
				AkUniqueID in_ObjectID,						///< Audio object ID
				AkPositioningInfo& out_rPositioningInfo		///< Positioning information structure to be filled
				);

			/// List passed to GetActiveGameObjects.
			/// After calling this function, the list will contain the list of all game objects that are currently active in the sound engine.
			/// Being active means that either a sound is playing or pending to be played using this game object.
			/// The caller is responsible for calling Term() on the list when the list is not required anymore
			/// \sa 
			/// - GetActiveGameObjects
			typedef AkArray<AkGameObjectID, AkGameObjectID> AkGameObjectsList;

			/// Fill the provided list with all the game object IDs that are currently active in the sound engine.
			/// The function may be used to avoid updating game objects positions that are not required at the moment.
			/// After calling this function, the list will contain the list of all game objects that are currently active in the sound engine.
			/// Being active means that either a sound is playing or pending to be played using this game object.
			/// \sa 
			/// - AkGameObjectsList
			extern   AKRESULT __cdecl  GetActiveGameObjects  ( 
				AkGameObjectsList& io_GameObjectList	///< returned list of active game objects.
				);

			/// Query if the specified game object is currently active.
			/// Being active means that either a sound is playing or pending to be played using this game object.
			extern   bool __cdecl  GetIsGameObjectActive  ( 
				AkGameObjectID in_GameObjId ///< Game object ID
				);

			/// Game object and max distance association.
			/// \sa 
			/// - \ref AkRadiusList
			struct GameObjDst
			{
				/// Default constructor
				GameObjDst()
					: m_gameObjID( AK_INVALID_GAME_OBJECT )
					, m_dst( -1.0f )
				{}

				/// Easy constructor
				GameObjDst( AkGameObjectID in_gameObjID, AkReal32 in_dst )
					: m_gameObjID( in_gameObjID )
					, m_dst( in_dst )
				{}

				AkGameObjectID	m_gameObjID;	///< Game object ID
				AkReal32		m_dst;			///< MaxDistance
			};

			/// List passed to GetMaxRadius.
			/// \sa 
			/// - \ref AK::SoundEngine::Query::GetMaxRadius
			typedef AkArray<GameObjDst, const GameObjDst&> AkRadiusList;

			/// Returns the maximum distance used in attenuations associated to all sounds currently playing.
			/// This may be used for example by the game to know if some processing need to be performed on the game side, that would not be required
			/// if the object is out of reach anyway.
			///
			/// Example usage:
			/// \code
			/// /*******************************************************/
			/// AkRadiusList RadLst; //creating the list( array ).
			/// // Do not reserve any size for the array, 
			/// // the system will reserve the correct size.
			///
			/// GetMaxRadius( RadLst );
			/// // Use the content of the list
			/// (...)
			///
			/// RadLst.Term();// the user is responsible to free the memory allocated
			/// /*******************************************************/
			/// \endcode
			///
			/// \aknote The returned value is NOT the distance from a listener to an object but
			/// the maximum attenuation distance of all sounds playing on this object. This is
			/// not related in any way to the current 3D position of the object. \endaknote
			///
			/// \return 
			/// - AK_Success if succeeded
			/// - AK_InsufficientMemory if there was not enough memory
			///
			/// \aknote 
			/// The Scaling factor (if one was specified on the game object) is included in the return value.
			/// The Scaling factor is not updated once a sound starts playing since it 
			/// is computed only when the playback starts with the initial scaling factor of this game object. Scaling factor will 
			/// be re-computed for every playback instance, always using the scaling factor available at this time.
			/// \endaknote
			///
			/// \sa 
			/// - AkRadiusList
			extern   AKRESULT __cdecl  GetMaxRadius  (
				AkRadiusList & io_RadiusList	///< List that will be filled with AK::SoundEngine::Query::GameObjDst objects.
				);

			/// Returns the maximum distance used in attenuations associated to sounds playing using the specified game object.
			/// This may be used for example by the game to know if some processing need to be performed on the game side, that would not be required
			/// if the object is out of reach anyway.
			/// 
			/// \aknote The returned value is NOT the distance from a listener to an object but the maximum attenuation distance of all sounds playing on this object. \endaknote
			///
			/// \return
			/// - A negative number if the game object specified is not playing.
			/// - 0, if the game object was only associated to sounds playing using no distance attenuation.
			/// - A positive number represents the maximum of all the distance attenuations playing on this game object.
			///
			/// \aknote 
			/// The Scaling factor (if one was specified on the game object) is included in the return value.
			/// The Scaling factor is not updated once a sound starts playing since it 
			/// is computed only when the playback starts with the initial scaling factor of this game object. Scaling factor will 
			/// be re-computed for every playback instance, always using the scaling factor available at this time.
			/// \endaknote
			///
			/// \sa 
			/// - \ref AK::SoundEngine::SetScalingFactor
			extern   AkReal32 __cdecl  GetMaxRadius  (
				AkGameObjectID in_GameObjId ///< Game object ID
				);

			/// Get the Event ID associated to the specified PlayingID.
			/// This function does not acquire the main audio lock.
			///
			/// \return AK_INVALID_UNIQUE_ID on failure.
			extern   AkUniqueID __cdecl  GetEventIDFromPlayingID  (
				AkPlayingID in_playingID ///< Associated PlayingID
				);

			/// Get the ObjectID associated to the specified PlayingID.
			/// This function does not acquire the main audio lock.
			///
			/// \return AK_INVALID_GAME_OBJECT on failure.
			extern   AkGameObjectID __cdecl  GetGameObjectFromPlayingID  (
				AkPlayingID in_playingID ///< Associated PlayingID
				);

			/// Get the list PlayingIDs associated with the given game object.
			/// This function does not acquire the main audio lock.
			///
			/// \aknote It is possible to call GetPlayingIDsFromGameObject with io_ruNumItems = 0 to get the total size of the
			/// structure that should be allocated for out_aPlayingIDs. \endaknote
			/// \return AK_Success if succeeded, AK_InvalidParameter if out_aPlayingIDs is NULL while io_ruNumItems > 0
			extern   AKRESULT __cdecl  GetPlayingIDsFromGameObject  (
				AkGameObjectID in_GameObjId,		///< Game object ID
				AkUInt32& io_ruNumIDs,				///< Number of items in array provided / Number of items filled in array
				AkPlayingID* out_aPlayingIDs		///< Array of AkPlayingID items to fill
				);

			/// Get the value of a custom property of integer or boolean type.
			/// \return AK_PartialSuccess if the object was found but no matching custom property was found on this object. Note that it could mean this value is the default value. 
			extern   AKRESULT __cdecl  GetCustomPropertyValue  (
				AkUniqueID in_ObjectID,			///< Object ID, this is the 32bit ShortID of the AudioFileSource or Sound object found in the .wwu XML file. At runtime it can only be retrieved by the AK_Duration callback when registered with PostEvent(), or by calling Query::QueryAudioObjectIDs() to get all the shortIDs associated with an event.
				AkUInt32 in_uPropID,			///< Property ID of your custom property found under the Custom Properties tab of the Wwise project settings.
				AkInt32& out_iValue				///< Property Value
				);

			/// Get the value of a custom property of real type.
			/// \return AK_PartialSuccess if the object was found but no matching custom property was found on this object. Note that it could mean this value is the default value.
			extern   AKRESULT __cdecl  GetCustomPropertyValue  (
				AkUniqueID in_ObjectID,			///< Object ID, this is the 32bit ShortID of the AudioFileSource or Sound object found in the .wwu XML file. At runtime it can only be retrieved by the AK_Duration callback when registered with PostEvent(), or by calling Query::QueryAudioObjectIDs() to get all the shortIDs associated with an event.
				AkUInt32 in_uPropID,			///< Property ID of your custom property found under the Custom Properties tab of the Wwise project settings.
				AkReal32& out_fValue			///< Property Value
				);

		} //namespace Query
	} //namespace SoundEngine
} //namespace AK

#endif // _AK_QUERYPARAMS_H_
/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Apache License Usage

Alternatively, this file may be used under the Apache License, Version 2.0 (the 
"Apache License"); you may not use this file except in compliance with the 
Apache License. You may obtain a copy of the Apache License at 
http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing, software distributed
under the Apache License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
OR CONDITIONS OF ANY KIND, either express or implied. See the Apache License for
the specific language governing permissions and limitations under the License.

  Copyright (c) 2026 Audiokinetic Inc.
*******************************************************************************/

#ifndef _AK_SPEAKERCONFIG_H_
#define _AK_SPEAKERCONFIG_H_

#include <AK/SoundEngine/Common/AkNumeralTypes.h>
#include <AK/SoundEngine/Common/AkTypedefs.h>
#include <AK/SoundEngine/Common/AkEnums.h>
#ifdef __cplusplus
#include <AK/Tools/Common/AkBitFuncs.h>
#endif

/// Standard speakers (channel mask):
#define AK_SPEAKER_FRONT_LEFT               0x1       ///< Front left speaker bit mask
#define AK_SPEAKER_FRONT_RIGHT              0x2       ///< Front right speaker bit mask
#define AK_SPEAKER_FRONT_CENTER             0x4       ///< Front center speaker bit mask
#define AK_SPEAKER_LOW_FREQUENCY            0x8       ///< Low-frequency speaker bit mask
#define AK_SPEAKER_BACK_LEFT                0x10      ///< Rear left speaker bit mask
#define AK_SPEAKER_BACK_RIGHT               0x20      ///< Rear right speaker bit mask
#define AK_SPEAKER_BACK_CENTER              0x100     ///< Rear center speaker ("surround speaker") bit mask
#define AK_SPEAKER_SIDE_LEFT                0x200     ///< Side left speaker bit mask
#define AK_SPEAKER_SIDE_RIGHT               0x400     ///< Side right speaker bit mask

/// "Height" speakers.
#define AK_SPEAKER_TOP                      0x800     ///< Top speaker bit mask
#define AK_SPEAKER_HEIGHT_FRONT_LEFT        0x1000    ///< Top front left speaker bit mask
#define AK_SPEAKER_HEIGHT_FRONT_CENTER      0x2000    ///< Top front center speaker bit mask
#define AK_SPEAKER_HEIGHT_FRONT_RIGHT       0x4000    ///< Top front right speaker bit mask
#define AK_SPEAKER_HEIGHT_BACK_LEFT         0x8000    ///< Top rear left speaker bit mask
#define AK_SPEAKER_HEIGHT_BACK_CENTER       0x10000   ///< Top rear center speaker bit mask
#define AK_SPEAKER_HEIGHT_BACK_RIGHT        0x20000   ///< Top rear right speaker bit mask
#define AK_SPEAKER_HEIGHT_SIDE_LEFT         0x40000   ///< Top side left speaker bit mask
#define AK_SPEAKER_HEIGHT_SIDE_RIGHT        0x80000   ///< Top side right speaker bit mask

//
// Supported speaker setups. Those are the ones that can be used in the Wwise Sound Engine audio pipeline.
//

#define AK_SPEAKER_SETUP_MONO           AK_SPEAKER_FRONT_CENTER                                                    ///< 1.0 setup channel mask
#define AK_SPEAKER_SETUP_0POINT1        AK_SPEAKER_LOW_FREQUENCY                                                   ///< 0.1 setup channel mask
#define AK_SPEAKER_SETUP_1POINT1        (AK_SPEAKER_FRONT_CENTER  | AK_SPEAKER_LOW_FREQUENCY)                      ///< 1.1 setup channel mask
#define AK_SPEAKER_SETUP_STEREO         (AK_SPEAKER_FRONT_LEFT    | AK_SPEAKER_FRONT_RIGHT)                        ///< 2.0 setup channel mask
#define AK_SPEAKER_SETUP_2POINT1        (AK_SPEAKER_SETUP_STEREO  | AK_SPEAKER_LOW_FREQUENCY)                      ///< 2.1 setup channel mask
#define AK_SPEAKER_SETUP_3STEREO        (AK_SPEAKER_SETUP_STEREO  | AK_SPEAKER_FRONT_CENTER)                       ///< 3.0 setup channel mask
#define AK_SPEAKER_SETUP_3POINT1        (AK_SPEAKER_SETUP_3STEREO | AK_SPEAKER_LOW_FREQUENCY)                      ///< 3.1 setup channel mask
#define AK_SPEAKER_SETUP_4              (AK_SPEAKER_SETUP_STEREO  | AK_SPEAKER_SIDE_LEFT | AK_SPEAKER_SIDE_RIGHT)  ///< 4.0 setup channel mask
#define AK_SPEAKER_SETUP_4POINT1        (AK_SPEAKER_SETUP_4       | AK_SPEAKER_LOW_FREQUENCY)                      ///< 4.1 setup channel mask
#define AK_SPEAKER_SETUP_5              (AK_SPEAKER_SETUP_4       | AK_SPEAKER_FRONT_CENTER)                       ///< 5.0 setup channel mask
#define AK_SPEAKER_SETUP_5POINT1        (AK_SPEAKER_SETUP_5       | AK_SPEAKER_LOW_FREQUENCY)                      ///< 5.1 setup channel mask
#define AK_SPEAKER_SETUP_6              (AK_SPEAKER_SETUP_4       | AK_SPEAKER_BACK_LEFT | AK_SPEAKER_BACK_RIGHT)  ///< 6.0 setup channel mask
#define AK_SPEAKER_SETUP_6POINT1        (AK_SPEAKER_SETUP_6       | AK_SPEAKER_LOW_FREQUENCY)                      ///< 6.1 setup channel mask
#define AK_SPEAKER_SETUP_7              (AK_SPEAKER_SETUP_6       | AK_SPEAKER_FRONT_CENTER)                       ///< 7.0 setup channel mask
#define AK_SPEAKER_SETUP_7POINT1        (AK_SPEAKER_SETUP_7       | AK_SPEAKER_LOW_FREQUENCY)                      ///< 7.1 setup channel mask
#define AK_SPEAKER_SETUP_SURROUND       (AK_SPEAKER_SETUP_STEREO  | AK_SPEAKER_BACK_CENTER)                        ///< Legacy surround setup channel mask

#define AK_SPEAKER_SETUP_HEIGHT_2       (AK_SPEAKER_HEIGHT_FRONT_LEFT | AK_SPEAKER_HEIGHT_FRONT_RIGHT)                                ///< 2 speaker height layer.
#define AK_SPEAKER_SETUP_HEIGHT_4       (AK_SPEAKER_SETUP_HEIGHT_2    | AK_SPEAKER_HEIGHT_BACK_LEFT | AK_SPEAKER_HEIGHT_BACK_RIGHT)   ///< 4 speaker height layer.
#define AK_SPEAKER_SETUP_HEIGHT_5       (AK_SPEAKER_SETUP_HEIGHT_4    | AK_SPEAKER_HEIGHT_FRONT_CENTER)                               ///< 5 speaker height layer.
#define AK_SPEAKER_SETUP_HEIGHT_ALL     (AK_SPEAKER_SETUP_HEIGHT_5    | AK_SPEAKER_HEIGHT_BACK_CENTER)                                ///< All height speaker layer.
#define AK_SPEAKER_SETUP_HEIGHT_4_TOP   (AK_SPEAKER_SETUP_HEIGHT_4    | AK_SPEAKER_TOP)                                               ///< 4 speaker height layer + top.
#define AK_SPEAKER_SETUP_HEIGHT_5_TOP   (AK_SPEAKER_SETUP_HEIGHT_5    | AK_SPEAKER_TOP)                                               ///< 5 speaker height layer + top.

// Auro speaker setups
#define AK_SPEAKER_SETUP_AURO_222           (AK_SPEAKER_SETUP_4            | AK_SPEAKER_HEIGHT_FRONT_LEFT | AK_SPEAKER_HEIGHT_FRONT_RIGHT)  ///< Auro-222 setup channel mask (4.0.2)
#define AK_SPEAKER_SETUP_AURO_8             (AK_SPEAKER_SETUP_AURO_222     | AK_SPEAKER_HEIGHT_BACK_LEFT  | AK_SPEAKER_HEIGHT_BACK_RIGHT)   ///< Auro-8 setup channel mask (4.0.4)
#define AK_SPEAKER_SETUP_AURO_9             (AK_SPEAKER_SETUP_AURO_8       | AK_SPEAKER_FRONT_CENTER)                                       ///< Auro-9.0 setup channel mask (5.0.4)
#define AK_SPEAKER_SETUP_AURO_9POINT1       (AK_SPEAKER_SETUP_AURO_9       | AK_SPEAKER_LOW_FREQUENCY)                                      ///< Auro-9.1 setup channel mask (5.1.4)
#define AK_SPEAKER_SETUP_AURO_10            (AK_SPEAKER_SETUP_AURO_9       | AK_SPEAKER_TOP)                                                ///< Auro-10.0 setup channel mask (5.0.4+top)
#define AK_SPEAKER_SETUP_AURO_10POINT1      (AK_SPEAKER_SETUP_AURO_10      | AK_SPEAKER_LOW_FREQUENCY)                                      ///< Auro-10.1 setup channel mask (5.1.4+top)
#define AK_SPEAKER_SETUP_AURO_11            (AK_SPEAKER_SETUP_AURO_10      | AK_SPEAKER_HEIGHT_FRONT_CENTER)                                ///< Auro-11.0 setup channel mask (5.0.5+top)
#define AK_SPEAKER_SETUP_AURO_11POINT1      (AK_SPEAKER_SETUP_AURO_11      | AK_SPEAKER_LOW_FREQUENCY)                                      ///< Auro-11.1 setup channel mask (5.1.5+top)
#define AK_SPEAKER_SETUP_AURO_11_740        (AK_SPEAKER_SETUP_7            | AK_SPEAKER_SETUP_HEIGHT_4)                                     ///< Auro-11.0 (7+4) setup channel mask (7.0.4)
#define AK_SPEAKER_SETUP_AURO_11POINT1_740  (AK_SPEAKER_SETUP_AURO_11_740  | AK_SPEAKER_LOW_FREQUENCY)                                      ///< Auro-11.1 (7+4) setup channel mask (7.1.4)
#define AK_SPEAKER_SETUP_AURO_13_751        (AK_SPEAKER_SETUP_7            | AK_SPEAKER_SETUP_HEIGHT_5 | AK_SPEAKER_TOP)                    ///< Auro-13.0 setup channel mask (7.0.5+top)
#define AK_SPEAKER_SETUP_AURO_13POINT1_751  (AK_SPEAKER_SETUP_AURO_13_751  | AK_SPEAKER_LOW_FREQUENCY)                                      ///< Auro-13.1 setup channel mask (7.1.5+top)

// Dolby speaker setups: in Dolby nomenclature, [#plane].[lfe].[#height]
#define AK_SPEAKER_SETUP_DOLBY_5_0_2        (AK_SPEAKER_SETUP_5            | AK_SPEAKER_HEIGHT_FRONT_LEFT | AK_SPEAKER_HEIGHT_FRONT_RIGHT )   ///< Dolby 5.0.2 setup channel mask
#define AK_SPEAKER_SETUP_DOLBY_5_1_2        (AK_SPEAKER_SETUP_DOLBY_5_0_2  | AK_SPEAKER_LOW_FREQUENCY )                                       ///< Dolby 5.1.2 setup channel mask
#define AK_SPEAKER_SETUP_DOLBY_5_0_4        (AK_SPEAKER_SETUP_DOLBY_5_0_2  | AK_SPEAKER_HEIGHT_BACK_LEFT  | AK_SPEAKER_HEIGHT_BACK_RIGHT )    ///< Dolby 5.0.4 setup channel mask
#define AK_SPEAKER_SETUP_DOLBY_5_1_4        (AK_SPEAKER_SETUP_DOLBY_5_0_4  | AK_SPEAKER_LOW_FREQUENCY )                                       ///< Dolby 5.1.4 setup channel mask
#define AK_SPEAKER_SETUP_DOLBY_6_0_2        (AK_SPEAKER_SETUP_6            | AK_SPEAKER_HEIGHT_FRONT_LEFT | AK_SPEAKER_HEIGHT_FRONT_RIGHT )   ///< Dolby 6.0.2 setup channel mask
#define AK_SPEAKER_SETUP_DOLBY_6_1_2        (AK_SPEAKER_SETUP_DOLBY_6_0_2  | AK_SPEAKER_LOW_FREQUENCY )                                       ///< Dolby 6.1.2 setup channel mask
#define AK_SPEAKER_SETUP_DOLBY_6_0_4        (AK_SPEAKER_SETUP_DOLBY_6_0_2  | AK_SPEAKER_HEIGHT_BACK_LEFT  | AK_SPEAKER_HEIGHT_BACK_RIGHT )    ///< Dolby 6.0.4 setup channel mask
#define AK_SPEAKER_SETUP_DOLBY_6_1_4        (AK_SPEAKER_SETUP_DOLBY_6_0_4  | AK_SPEAKER_LOW_FREQUENCY )                                       ///< Dolby 6.1.4 setup channel mask
#define AK_SPEAKER_SETUP_DOLBY_7_0_2        (AK_SPEAKER_SETUP_7            | AK_SPEAKER_HEIGHT_FRONT_LEFT | AK_SPEAKER_HEIGHT_FRONT_RIGHT )   ///< Dolby 7.0.2 setup channel mask
#define AK_SPEAKER_SETUP_DOLBY_7_1_2        (AK_SPEAKER_SETUP_DOLBY_7_0_2  | AK_SPEAKER_LOW_FREQUENCY )                                       ///< Dolby 7.1.2 setup channel mask
#define AK_SPEAKER_SETUP_DOLBY_7_0_4        (AK_SPEAKER_SETUP_DOLBY_7_0_2  | AK_SPEAKER_HEIGHT_BACK_LEFT  | AK_SPEAKER_HEIGHT_BACK_RIGHT )    ///< Dolby 7.0.4 setup channel mask
#define AK_SPEAKER_SETUP_DOLBY_7_1_4        (AK_SPEAKER_SETUP_DOLBY_7_0_4  | AK_SPEAKER_LOW_FREQUENCY )                                       ///< Dolby 7.1.4 setup channel mask

#define AK_SPEAKER_SETUP_ALL_SPEAKERS       (AK_SPEAKER_SETUP_7POINT1 | AK_SPEAKER_BACK_CENTER | AK_SPEAKER_SETUP_HEIGHT_ALL | AK_SPEAKER_TOP)  ///< All speakers.

// Channel indices.
// ------------------------------------------------

// Channel indices for standard setups on the plane.
#define AK_IDX_SETUP_FRONT_LEFT             (0)    ///< Index of front-left channel in all configurations.
#define AK_IDX_SETUP_FRONT_RIGHT            (1)    ///< Index of front-right channel in all configurations.
#define AK_IDX_SETUP_CENTER                 (2)    ///< Index of front-center channel in all configurations.

#define AK_IDX_SETUP_NOCENTER_BACK_LEFT     (2)    ///< Index of back-left channel in configurations with no front-center channel.
#define AK_IDX_SETUP_NOCENTER_BACK_RIGHT    (3)    ///< Index of back-right channel in configurations with no front-center channel.
#define AK_IDX_SETUP_NOCENTER_SIDE_LEFT     (4)    ///< Index of side-left channel in configurations with no front-center channel.
#define AK_IDX_SETUP_NOCENTER_SIDE_RIGHT    (5)    ///< Index of side-right channel in configurations with no front-center channel.

#define AK_IDX_SETUP_WITHCENTER_BACK_LEFT   (3)    ///< Index of back-left channel in configurations with a front-center channel.
#define AK_IDX_SETUP_WITHCENTER_BACK_RIGHT  (4)    ///< Index of back-right channel in configurations with a front-center channel.
#define AK_IDX_SETUP_WITHCENTER_SIDE_LEFT   (5)    ///< Index of side-left channel in configurations with a front-center channel.
#define AK_IDX_SETUP_WITHCENTER_SIDE_RIGHT  (6)    ///< Index of side-right channel in configurations with a front-center channel.

#define AK_IDX_SETUP_WITHCENTER_HEIGHT_FRONT_LEFT    (7)  ///< Index of height-front-left channel in configurations with a front-center channel.
#define AK_IDX_SETUP_WITHCENTER_HEIGHT_FRONT_RIGHT   (8)  ///< Index of height-front-right channel in configurations with a front-center channel.
#define AK_IDX_SETUP_WITHCENTER_HEIGHT_BACK_LEFT     (9)  ///< Index of height-back-left channel in configurations with a front-center channel.
#define AK_IDX_SETUP_WITHCENTER_HEIGHT_BACK_RIGHT    (10) ///< Index of height-back-right channel in configurations with a front-center channel.

// Channel indices for specific setups.
#define AK_IDX_SETUP_0_LFE          (0)    ///< Index of low-frequency channel in 0.1 setup (use with AkAudioBuffer::GetChannel())

#define AK_IDX_SETUP_1_CENTER       (0)    ///< Index of center channel in 1.x setups (use with AkAudioBuffer::GetChannel())
#define AK_IDX_SETUP_1_LFE          (1)    ///< Index of low-frequency channel in 1.1 setup (use with AkAudioBuffer::GetChannel())

#define AK_IDX_SETUP_2_LEFT         (0)    ///< Index of left channel in 2.x setups (use with AkAudioBuffer::GetChannel())
#define AK_IDX_SETUP_2_RIGHT        (1)    ///< Index of right channel in 2.x setups (use with AkAudioBuffer::GetChannel())
#define AK_IDX_SETUP_2_LFE          (2)    ///< Index of low-frequency channel in 2.1 setup (use with AkAudioBuffer::GetChannel())

#define AK_IDX_SETUP_3_LEFT         (0)    ///< Index of left channel in 3.x setups (use with AkAudioBuffer::GetChannel())
#define AK_IDX_SETUP_3_RIGHT        (1)    ///< Index of right channel in 3.x setups (use with AkAudioBuffer::GetChannel())
#define AK_IDX_SETUP_3_CENTER       (2)    ///< Index of center channel in 3.x setups (use with AkAudioBuffer::GetChannel())
#define AK_IDX_SETUP_3_LFE          (3)    ///< Index of low-frequency channel in 3.1 setup (use with AkAudioBuffer::GetChannel())

#define AK_IDX_SETUP_4_FRONTLEFT    (0)    ///< Index of front left channel in 4.x setups (use with AkAudioBuffer::GetChannel())
#define AK_IDX_SETUP_4_FRONTRIGHT   (1)    ///< Index of front right channel in 4.x setups (use with AkAudioBuffer::GetChannel())
#define AK_IDX_SETUP_4_REARLEFT     (2)    ///< Index of rear left channel in 4.x setups (use with AkAudioBuffer::GetChannel())
#define AK_IDX_SETUP_4_REARRIGHT    (3)    ///< Index of rear right channel in 4.x setups (use with AkAudioBuffer::GetChannel())
#define AK_IDX_SETUP_4_LFE          (4)    ///< Index of low-frequency channel in 4.1 setup (use with AkAudioBuffer::GetChannel())

#define AK_IDX_SETUP_5_FRONTLEFT    (0)    ///< Index of front left channel in 5.x setups (use with AkAudioBuffer::GetChannel())
#define AK_IDX_SETUP_5_FRONTRIGHT   (1)    ///< Index of front right channel in 5.x setups (use with AkAudioBuffer::GetChannel())
#define AK_IDX_SETUP_5_CENTER       (2)    ///< Index of center channel in 5.x setups (use with AkAudioBuffer::GetChannel())
#define AK_IDX_SETUP_5_REARLEFT     (3)    ///< Index of rear left channel in 5.x setups (use with AkAudioBuffer::GetChannel())
#define AK_IDX_SETUP_5_REARRIGHT    (4)    ///< Index of rear right channel in 5.x setups (use with AkAudioBuffer::GetChannel())
#define AK_IDX_SETUP_5_LFE          (5)    ///< Index of low-frequency channel in 5.1 setup (use with AkAudioBuffer::GetChannel())

#define AK_IDX_SETUP_6_FRONTLEFT    (0)    ///< Index of front left channel in 6.x setups (use with AkAudioBuffer::GetChannel())
#define AK_IDX_SETUP_6_FRONTRIGHT   (1)    ///< Index of front right channel in 6x setups (use with AkAudioBuffer::GetChannel())
#define AK_IDX_SETUP_6_REARLEFT     (2)    ///< Index of rear left channel in 6.x setups (use with AkAudioBuffer::GetChannel())
#define AK_IDX_SETUP_6_REARRIGHT    (3)    ///< Index of rear right channel in 6.x setups (use with AkAudioBuffer::GetChannel())
#define AK_IDX_SETUP_6_SIDELEFT     (4)    ///< Index of side left channel in 6.x setups (use with AkAudioBuffer::GetChannel())
#define AK_IDX_SETUP_6_SIDERIGHT    (5)    ///< Index of side right channel in 6.x setups (use with AkAudioBuffer::GetChannel())
#define AK_IDX_SETUP_6_LFE          (6)    ///< Index of low-frequency channel in 6.1 setup (use with AkAudioBuffer::GetChannel())

#define AK_IDX_SETUP_7_FRONTLEFT    (0)    ///< Index of front left channel in 7.x setups (use with AkAudioBuffer::GetChannel())
#define AK_IDX_SETUP_7_FRONTRIGHT   (1)    ///< Index of front right channel in 7.x setups (use with AkAudioBuffer::GetChannel())
#define AK_IDX_SETUP_7_CENTER       (2)    ///< Index of center channel in 7.x setups (use with AkAudioBuffer::GetChannel())
#define AK_IDX_SETUP_7_REARLEFT     (3)    ///< Index of rear left channel in 7.x setups (use with AkAudioBuffer::GetChannel())
#define AK_IDX_SETUP_7_REARRIGHT    (4)    ///< Index of rear right channel in 7.x setups (use with AkAudioBuffer::GetChannel())
#define AK_IDX_SETUP_7_SIDELEFT     (5)    ///< Index of side left channel in 7.x setups (use with AkAudioBuffer::GetChannel())
#define AK_IDX_SETUP_7_SIDERIGHT    (6)    ///< Index of side right channel in 7.x setups (use with AkAudioBuffer::GetChannel())
#define AK_IDX_SETUP_7_LFE          (7)    ///< Index of low-frequency channel in 7.1 setup (use with AkAudioBuffer::GetChannel())

//
// Extra speaker setups. This is a more exhaustive list of speaker setups, which might not all be supported
// by the Wwise Sound Engine audio pipeline.
//

#define AK_SPEAKER_SETUP_0_1        ( AK_SPEAKER_LOW_FREQUENCY )  //0.1

#define AK_SPEAKER_SETUP_1_0_CENTER ( AK_SPEAKER_FRONT_CENTER )                               //1.0 (C)
#define AK_SPEAKER_SETUP_1_1_CENTER ( AK_SPEAKER_FRONT_CENTER | AK_SPEAKER_LOW_FREQUENCY )    //1.1 (C)

#define AK_SPEAKER_SETUP_2_0        ( AK_SPEAKER_FRONT_LEFT | AK_SPEAKER_FRONT_RIGHT )                            //2.0
#define AK_SPEAKER_SETUP_2_1        ( AK_SPEAKER_FRONT_LEFT | AK_SPEAKER_FRONT_RIGHT | AK_SPEAKER_LOW_FREQUENCY ) //2.1

#define AK_SPEAKER_SETUP_3_0        ( AK_SPEAKER_FRONT_LEFT | AK_SPEAKER_FRONT_RIGHT | AK_SPEAKER_FRONT_CENTER )  //3.0
#define AK_SPEAKER_SETUP_3_1        ( AK_SPEAKER_SETUP_3_0  | AK_SPEAKER_LOW_FREQUENCY )                          //3.1

#define AK_SPEAKER_SETUP_FRONT      ( AK_SPEAKER_SETUP_3_0 )

#define AK_SPEAKER_SETUP_4_0        ( AK_SPEAKER_SETUP_4 )
#define AK_SPEAKER_SETUP_4_1        ( AK_SPEAKER_SETUP_4POINT1 )
#define AK_SPEAKER_SETUP_5_0        ( AK_SPEAKER_SETUP_5 )
#define AK_SPEAKER_SETUP_5_1        ( AK_SPEAKER_SETUP_5POINT1 )

#define AK_SPEAKER_SETUP_6_0        ( AK_SPEAKER_SETUP_6 )
#define AK_SPEAKER_SETUP_6_1        ( AK_SPEAKER_SETUP_6POINT1 )
#define AK_SPEAKER_SETUP_7_0        ( AK_SPEAKER_SETUP_7 )
#define AK_SPEAKER_SETUP_7_1        ( AK_SPEAKER_SETUP_7POINT1 )

// Standard/largest setup definitions.
#define AK_SPEAKER_SETUP_DEFAULT_PLANE          (AK_SPEAKER_SETUP_7POINT1)         ///< All speakers on the plane, supported on this platform.
#define AK_SUPPORTED_STANDARD_CHANNEL_MASK      (AK_SPEAKER_SETUP_ALL_SPEAKERS)    ///< Platform supports all standard channels.
#define AK_STANDARD_MAX_NUM_CHANNELS            (8)                                ///< Legacy: Platform supports at least 7.1

#define AK_MAX_AMBISONICS_ORDER    (5)

#define AK_DEFAULT_HEIGHT_ANGLE (30.0f)

#ifdef __cplusplus
// Helpers.
inline void AK_SPEAKER_SETUP_FIX_LEFT_TO_CENTER(AkUInt32& io_uChannelMask)
{
	if (!(io_uChannelMask & AK_SPEAKER_FRONT_CENTER)
		&& !(io_uChannelMask & AK_SPEAKER_FRONT_RIGHT)
		&& (io_uChannelMask & AK_SPEAKER_FRONT_LEFT))
	{
		io_uChannelMask &= ~AK_SPEAKER_FRONT_LEFT;		// remove left
		io_uChannelMask |= AK_SPEAKER_FRONT_CENTER;	// add center
	}
}

inline void AK_SPEAKER_SETUP_FIX_REAR_TO_SIDE(AkUInt32& io_uChannelMask)
{
	if (io_uChannelMask & (AK_SPEAKER_BACK_LEFT) && !(io_uChannelMask & AK_SPEAKER_SIDE_LEFT))
	{
		io_uChannelMask &= ~(AK_SPEAKER_BACK_LEFT | AK_SPEAKER_BACK_RIGHT);	// remove rears
		io_uChannelMask |= (AK_SPEAKER_SIDE_LEFT | AK_SPEAKER_SIDE_RIGHT);	// add sides
	}
}

inline void AK_SPEAKER_SETUP_CONVERT_TO_SUPPORTED(AkUInt32& io_uChannelMask)
{
	AK_SPEAKER_SETUP_FIX_LEFT_TO_CENTER(io_uChannelMask);
	AK_SPEAKER_SETUP_FIX_REAR_TO_SIDE(io_uChannelMask);
}

/// Ambisonics configurations (corresponding to AkChannelConfig::eConfigType == AK_ChannelConfigType_Ambisonic).
/// Convention: X points towards the front, and XYZ follow a right-hand rule, so Y is the side vector (pointing to the left).
/// Channel presence and ordering are predefined according to the number of channels. The ordering convention is ACN,
/// with the mapping of components to number of channels detailed below (source: https://en.wikipedia.org/wiki/Ambisonic_data_exchange_formats).
/// Normalization natively used in Wwise is SN3D.
///
/// <table cellspacing="0" cellpadding="1" border="1" width="800px">
/// <tr><td rowspan="2" align="center"><b>Number of channels</b></td> <td colspan="2" align="center"><b>Order</b></td><td rowspan="2" align="center"><b>Description</b></td><td rowspan="2" align="center"><b>Layout of components</b></td></tr>
/// <tr><td align="center">Horizontal</td><td align="center">Vertical</td></tr>
/// <tr><td align="right">1 &nbsp;&nbsp;&nbsp;</td> <td align="right">0 &nbsp;&nbsp;&nbsp;</td><td align="right">0 &nbsp;&nbsp;&nbsp;</td> <td>&nbsp;&nbsp;mono</td><td>&nbsp;</td></tr>
/// <tr><td align="right">4 &nbsp;&nbsp;&nbsp;</td> <td align="right">1 &nbsp;&nbsp;&nbsp;</td><td align="right">1 &nbsp;&nbsp;&nbsp;</td> <td>&nbsp;&nbsp;first-order full sphere</td><td>&nbsp;&nbsp;WYZX</td></tr>
/// <tr><td align="right">9 &nbsp;&nbsp;&nbsp;</td> <td align="right">2 &nbsp;&nbsp;&nbsp;</td><td align="right">2 &nbsp;&nbsp;&nbsp;</td> <td>&nbsp;&nbsp;second-order full sphere</td><td>&nbsp;&nbsp;WYZXVTRSU</td></tr>
/// <tr><td align="right">16 &nbsp;&nbsp;&nbsp;</td> <td align="right">3 &nbsp;&nbsp;&nbsp;</td><td align="right">3 &nbsp;&nbsp;&nbsp;</td> <td>&nbsp;&nbsp;third-order full sphere</td><td>&nbsp;&nbsp;WYZXVTRSUQOMKLNP</td></tr>
/// </table>
namespace AK
{

/// Returns the number of channels of a given channel configuration.
static inline AkUInt8 ChannelMaskToNumChannels( AkChannelMask in_uChannelMask )
{
	return (AkUInt8)AKPLATFORM::AkPopCount(in_uChannelMask);
}

/// Returns a 'best guess' channel configuration from a given number of channels.
/// Will return 0 if no guess can be made.
static inline AkChannelMask ChannelMaskFromNumChannels( unsigned int in_uNumChannels )
{
	AkChannelMask uChannelMask = 0;

	switch ( in_uNumChannels )
	{
	case 1:
		uChannelMask = AK_SPEAKER_SETUP_1_0_CENTER;
		break;
	case 2:
		uChannelMask = AK_SPEAKER_SETUP_2_0;
		break;
	case 3:
		uChannelMask = AK_SPEAKER_SETUP_2_1;
		break;
	case 4:
		uChannelMask = AK_SPEAKER_SETUP_4_0;
		break;
	case 5:
		uChannelMask = AK_SPEAKER_SETUP_5_0;
		break;
	case 6:
		uChannelMask = AK_SPEAKER_SETUP_5_1;
		break;
	case 7:
		uChannelMask = AK_SPEAKER_SETUP_7;
		break;
	case 8:
		uChannelMask = AK_SPEAKER_SETUP_7POINT1;
		break;
	}

	return uChannelMask;
}

/// Converts a channel bit to a channel index (in Wwise pipeline ordering - LFE at the end), given a channel mask in_uChannelMask.
/// \return Channel index.
static inline AkUInt8 ChannelBitToIndex(AkChannelMask in_uChannelBit, AkChannelMask in_uChannelMask)
{
#ifdef AKASSERT
	AKASSERT(ChannelMaskToNumChannels(in_uChannelBit) == 1);
#endif
	if (in_uChannelBit == AK_SPEAKER_LOW_FREQUENCY)
		return ChannelMaskToNumChannels(in_uChannelMask) - 1;
	return ChannelMaskToNumChannels(in_uChannelMask & ~AK_SPEAKER_LOW_FREQUENCY & (in_uChannelBit - 1)); // Count all channels prior this one except the LFE
}

/// Returns true when the LFE channel is present in a given channel configuration.
/// \return True if the LFE channel is present.
 bool HasLFE(AkChannelMask in_uChannelMask)
{
	return (in_uChannelMask & AK_SPEAKER_LOW_FREQUENCY) > 0;
}

/// Returns true when the center channel is present in a given channel configuration.
/// Note that mono configurations have one channel which is arbitrary set to AK_SPEAKER_FRONT_CENTER,
/// so HasCenter() returns true for mono signals.
/// \return True if the center channel is present.
 bool HasCenter(AkChannelMask in_uChannelMask)
{
	// All supported non-mono configurations have an AK_SPEAKER_FRONT_LEFT.
	return (in_uChannelMask & AK_SPEAKER_FRONT_CENTER) > 0;
}

/// Returns the number of angle values required to represent the given channel configuration.
/// Use this function with supported 2D standard channel configurations only.
/// \sa AK::SoundEngine::SetSpeakerAngles().
 AkUInt32 GetNumberOfAnglesForConfig(AkChannelMask in_uChannelMask)
{
#ifdef AKASSERT
	AKASSERT((in_uChannelMask & ~AK_SPEAKER_SETUP_DEFAULT_PLANE) == 0);
#endif

	// LFE is irrelevant.
	in_uChannelMask &= ~AK_SPEAKER_LOW_FREQUENCY;
	// Center speaker is always in the center and thus does not require an angle.
	in_uChannelMask &= ~AK_SPEAKER_FRONT_CENTER;
	// We should have complete pairs at this point, unless there is a speaker at 180 degrees, 
	// in which case we need one more angle to specify it.
#ifdef AKASSERT
	AKASSERT((in_uChannelMask & AK_SPEAKER_BACK_CENTER) || ((ChannelMaskToNumChannels(in_uChannelMask) % 2) == 0));
#endif
	return ChannelMaskToNumChannels(in_uChannelMask) >> 1;
}

/// Channel ordering type. 
enum AkChannelOrdering
{
	ChannelOrdering_Standard = 0, // L-R-C-LFE-RL-RR-RC-SL-SR-HL-HR-HC-HRL-HRR-HRC-T, or ACN ordering + SN3D norm
	ChannelOrdering_Film,         // L-C-R-SL-SR-RL-RR-LFE-HL-HR-HRL-HRR
	ChannelOrdering_FuMa,
	ChannelOrdering_RunTime,      // L-R-C-RL-RR-RC-SL-SR-HL-HR-HC-HRL-HRR-HRC-T-LFE

	ChannelOrdering_Last		// End of enum, invalid value.
};

/// Returns true if standard configuration represented by channel mask has surround
/// channels, either defined as side or back channels.
 bool HasSurroundChannels( AkChannelMask in_uChannelMask )
{
	return ( in_uChannelMask & AK_SPEAKER_BACK_LEFT || in_uChannelMask & AK_SPEAKER_SIDE_LEFT );
}

/// Returns true if standard configuration represented by channel mask has strictly one
/// pair of surround channels, either defined as side or back channels. 7.1 has two pairs
/// of surround channels and would thus return false.
 bool HasStrictlyOnePairOfSurroundChannels( AkChannelMask in_uChannelMask )
{
	return ( ( ( in_uChannelMask & AK_SPEAKER_BACK_LEFT ) != 0 ) ^ ( ( in_uChannelMask & AK_SPEAKER_SIDE_LEFT ) != 0 ) );
}

/// Returns true if standard configuration represented by channel mask has two
/// pair of surround channels, that is, side and back channels. 7.1 has two pairs
/// of surround channels and would thus return true, whereas 5.1 would return false.
 bool HasSideAndRearChannels( AkChannelMask in_uChannelMask )
{
	return ( in_uChannelMask & AK_SPEAKER_BACK_LEFT && in_uChannelMask & AK_SPEAKER_SIDE_LEFT );
}

/// Returns true if standard configuration represented by channel mask has at least one "height" channel (above the plane).
 bool HasHeightChannels(AkChannelMask in_uChannelMask)
{
	return (in_uChannelMask & ~AK_SPEAKER_SETUP_DEFAULT_PLANE) > 0;
}

/// Takes a channel mask and swap back channels with side channels if there is just
/// one pair of surround channels.
 AkChannelMask BackToSideChannels( AkChannelMask in_uChannelMask )
{
	if ( HasStrictlyOnePairOfSurroundChannels( in_uChannelMask ) )
	{
		in_uChannelMask &= ~( AK_SPEAKER_BACK_LEFT | AK_SPEAKER_BACK_RIGHT );	// remove rears
		in_uChannelMask |= ( AK_SPEAKER_SIDE_LEFT | AK_SPEAKER_SIDE_RIGHT );	// add sides
	}
	return in_uChannelMask;
}

} // namespace AK

#endif // __cplusplus

/// Defines a channel configuration.
/// Examples:
/// \code
/// AkChannelConfig cfg;
/// 
/// // Create a stereo configuration.
/// cfg.SetStandard(AK_SPEAKER_SETUP_STEREO);
///
/// // Create a 7.1.4 configuration (7.1 plus 4 height channels).
/// cfg.SetStandard(AK_SPEAKER_SETUP_AURO_11POINT1_740);
/// // or
/// cfg.SetStandard(AK_SPEAKER_SETUP_DOLBY_7_1_4);
///
/// // Create a 3rd order ambisonic configuration.
/// cfg.SetAmbisonic(16);	// pass in the number of spherical harmonics, (N+1)^2, where N is the ambisonics order.
///
/// // Invalidate (usually means "As Parent")
/// cfg.Clear();
/// \endcode
struct AkChannelConfig
{
	union {
		// Channel config: 
		// - uChannelMask is a bit field, whose channel identifiers depend on AkChannelConfigType (up to 20). Channel bits are defined in AkSpeakerConfig.h.
		// - eConfigType is a code that completes the identification of channels by uChannelMask.
		// - uNumChannels is the number of channels, identified (deduced from channel mask) or anonymous (set directly). 
		struct
		{
			AkUInt32 uNumChannels : 8;  ///< Number of channels.
			AkUInt32 eConfigType : 4;   ///< Channel config type (AkChannelConfigType).
			AkUInt32 uChannelMask : 20; ///< Channel mask (configuration). 
		};
		AkUInt32 uFullCfg;
	};
#ifdef __cplusplus

	/// Construct standard channel config from channel mask
	static  AkChannelConfig Standard(AkUInt32 in_uChannelMask)
	{
		return AkChannelConfig(AK::ChannelMaskToNumChannels(in_uChannelMask), in_uChannelMask);
	}

	// Construct anonymous channel config from number of channels
	static  AkChannelConfig Anonymous(AkUInt32 in_uNumChannels)
	{
		return AkChannelConfig(in_uNumChannels, 0);
	}

	/// Construct ambisonic channel config from number of channels (NOT order)
	static  AkChannelConfig Ambisonic(AkUInt32 in_uNumChannels)
	{
		AkChannelConfig cfg;
		cfg.SetAmbisonic(in_uNumChannels);
		return cfg;
	}

	// Construct object-based channel config
	static  AkChannelConfig Object()
	{
		AkChannelConfig cfg;
		cfg.SetObject();
		return cfg;
	}

	/// Constructor. Clears / sets the channel config in "invalid" state (IsValid() returns false).
	 AkChannelConfig()
	{
		uFullCfg = 0;
	}

	 AkChannelConfig(const AkChannelConfig& rCopy)
	{
		uFullCfg = rCopy.uFullCfg;
	}

	/// Constructor. Sets number of channels, and config type according to whether channel mask is defined or not. If defined, it must be consistent with the number of channels.
	 AkChannelConfig(AkUInt32 in_uNumChannels, AkUInt32 in_uChannelMask)
	{
		// Input arguments should be consistent.
		SetStandardOrAnonymous(in_uNumChannels, in_uChannelMask);
	}

	/// Operator != with a 32-bit word.
	 bool operator!=(AkUInt32 in_uBitField)
	{
		return (*((AkUInt32*)this) != in_uBitField);
	}

	/// Clear the channel config. Becomes "invalid" (IsValid() returns false).
	 void Clear()
	{
		uFullCfg = 0;
	}

	/// Set channel config as a standard configuration specified with given channel mask.
	 void SetStandard(AkUInt32 in_uChannelMask)
	{
		uNumChannels = AK::ChannelMaskToNumChannels(in_uChannelMask);
		eConfigType = AK_ChannelConfigType_Standard;
		uChannelMask = in_uChannelMask;
	}

	/// Set channel config as either a standard or an anonymous configuration, specified with both a given channel mask (0 if anonymous) and a number of channels (which must match the channel mask if standard).
	 void SetStandardOrAnonymous(AkUInt32 in_uNumChannels, AkUInt32 in_uChannelMask)
	{
#ifdef AKASSERT
		AKASSERT(in_uChannelMask == 0 || in_uNumChannels == AK::ChannelMaskToNumChannels(in_uChannelMask));
#endif
		uNumChannels = in_uNumChannels;
		eConfigType = (in_uChannelMask) ? AK_ChannelConfigType_Standard : AK_ChannelConfigType_Anonymous;
		uChannelMask = in_uChannelMask;
	}

	/// Set channel config as an anonymous configuration specified with given number of channels.
	 void SetAnonymous(AkUInt32 in_uNumChannels)
	{
		uNumChannels = in_uNumChannels;
		eConfigType = AK_ChannelConfigType_Anonymous;
		uChannelMask = 0;
	}

	/// Set channel config as an ambisonic configuration specified with given number of channels.
	 void SetAmbisonic(AkUInt32 in_uNumChannels)
	{
		uNumChannels = in_uNumChannels;
		eConfigType = AK_ChannelConfigType_Ambisonic;
		uChannelMask = 0;
	}

	/// Set channel config as an object-based configuration (implies dynamic number of objects).
	 void SetObject()
	{
		uNumChannels = 0;
		eConfigType = AK_ChannelConfigType_Objects;
		uChannelMask = 0;
	}

	/// Set channel config as the main mix channel configuration
	 void SetSameAsMainMix()
	{
		uNumChannels = 0;
		eConfigType = AK_ChannelConfigType_UseDeviceMain;
		uChannelMask = 0;
	}

	/// Set channel config as the passthrough mix channel configuration
	 void SetSameAsPassthrough()
	{
		uNumChannels = 0;
		eConfigType = AK_ChannelConfigType_UseDevicePassthrough;
		uChannelMask = 0;
	}

	/// Returns true if valid, false otherwise (as when it is constructed, or invalidated using Clear()).
	 bool IsValid() const
	{
		return eConfigType <= AK_ChannelConfigType_Objects && (uNumChannels != 0 || eConfigType == AK_ChannelConfigType_Objects);
	}

	/// Serialize channel config into a 32-bit word.
	 AkUInt32 Serialize() const
	{
		return uFullCfg;
	}

	/// Deserialize channel config from a 32-bit word.
	 void Deserialize(AkUInt32 in_uChannelConfig)
	{
		uFullCfg = in_uChannelConfig;
	}

	/// Returns a new config based on 'this' with no LFE.
	 AkChannelConfig RemoveLFE() const
	{
		AkChannelConfig newConfig = *this;
		AkUInt32 uNewChannelMask = newConfig.uChannelMask & ~AK_SPEAKER_LOW_FREQUENCY;
		AkUInt32 uNumLFEChannel = (newConfig.uChannelMask - uNewChannelMask) >> 3; // 0 or 1
#ifdef AKASSERT
		AKASSERT(uNumLFEChannel == 0 || uNumLFEChannel == 1);
#endif
		newConfig.uNumChannels -= uNumLFEChannel;
		newConfig.uChannelMask = uNewChannelMask;
		return newConfig;
	}

	/// Returns a new config based on 'this' with no Front Center channel.
	 AkChannelConfig RemoveCenter() const
	{
		AkChannelConfig newConfig = *this;
		AkUInt32 uNewChannelMask = newConfig.uChannelMask & ~AK_SPEAKER_FRONT_CENTER;
		AkUInt32 uNumCenterChannel = (newConfig.uChannelMask - uNewChannelMask) >> 2;	// 0 or 1.
#ifdef AKASSERT
		AKASSERT(uNumCenterChannel == 0 || uNumCenterChannel == 1);
#endif
		newConfig.uNumChannels -= uNumCenterChannel;
		newConfig.uChannelMask = uNewChannelMask;
		return newConfig;
	}

	/// Operator ==
	 bool operator==(const AkChannelConfig & in_other) const
	{
		return uFullCfg == in_other.uFullCfg;
	}

	/// Operator !=
	 bool operator!=(const AkChannelConfig & in_other) const
	{
		return uFullCfg != in_other.uFullCfg;
	}

	/// Query if LFE channel is present.
	/// \return True when LFE channel is present
	 bool HasLFE() const
	{
		return AK::HasLFE(uChannelMask);
	}

	/// Query if center channel is present.
	/// Note that mono configurations have one channel which is arbitrary set to AK_SPEAKER_FRONT_CENTER,
	/// so HasCenter() returns true for mono signals.
	/// \return True when center channel is present and configuration has more than 2 channels.
	 bool HasCenter() const
	{
		return AK::HasCenter(uChannelMask);
	}
#endif
};

#endif //_AK_SPEAKERCONFIG_H_
/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Apache License Usage

Alternatively, this file may be used under the Apache License, Version 2.0 (the
"Apache License"); you may not use this file except in compliance with the
Apache License. You may obtain a copy of the Apache License at
http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing, software distributed
under the Apache License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
OR CONDITIONS OF ANY KIND, either express or implied. See the Apache License for
the specific language governing permissions and limitations under the License.

  Copyright (c) 2026 Audiokinetic Inc.
*******************************************************************************/

#ifndef _AK_REFLECT_GAMEDATA_H_
#define _AK_REFLECT_GAMEDATA_H_

#include <AK/SoundEngine/Common/AkTypes.h>
#include <AK/SpatialAudio/Common/AkSpatialAudioTypes.h>

struct AkReflectImageSource
{
	AkReflectImageSource()
		: uID((AkImageSourceID)-1)
		, params()
		, texture()
		, name()
	{}

	AkReflectImageSource(AkImageSourceID in_uID, AkVector64 in_sourcePosition, AkReal32 in_fDistanceScalingFactor, AkReal32 in_fLevel)
		: uID(in_uID) 
		, params(in_sourcePosition, in_fDistanceScalingFactor, in_fLevel)
		, texture()
		, name()
	{
	}

	void SetName(const char * in_pName)
	{
		name.SetName(in_pName);
	}

	AkImageSourceID uID;						///< Image source ID (for matching delay lines across frames)
	AkImageSourceParams params;					///< Image source properties
	AkImageSourceTexture texture;				///< Image source's acoustic textures. Note that changing any of these textures across frames for a given image source, identified by uID, may result in a discontinuity in the audio signal.
	AkImageSourceName name;						///< Image source name, for profiling.
};

/// Data structure sent by the game to an instance of the Reflect plug-in.
struct AkReflectGameData
{
	AkGameObjectID listenerID;					///< ID of the listener used to compute spatialization and distance evaluation from within the targeted Reflect plug-in instance. It needs to be one of the listeners that are listening to the game object associated with the targeted plug-in instance. See AK::SoundEngine::SetListeners and AK::SoundEngine::SetGameObjectAuxSendValues.
	AkUInt32 uNumImageSources;					///< Number of image sources passed in the variable array, below.
	AkReflectImageSource arSources[1];			///< Variable array of image sources. You should allocate storage for the structure by calling AkReflectGameData::GetSize() with the desired number of sources.
	
	/// Default constructor.
	AkReflectGameData()
		: listenerID( AK_INVALID_GAME_OBJECT )
		, uNumImageSources(0)
	{}

	/// Helper function for computing the size required to allocate the AkReflectGameData structure.
	static AkUInt32 GetSize(AkUInt32 in_uNumSources)
	{
		return (in_uNumSources > 0) ? sizeof(AkReflectGameData) + (in_uNumSources - 1) * sizeof(AkReflectImageSource) : sizeof(AkReflectGameData);
	}
};
#endif // _AK_REFLECT_GAMEDATA_H_
/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Apache License Usage

Alternatively, this file may be used under the Apache License, Version 2.0 (the
"Apache License"); you may not use this file except in compliance with the
Apache License. You may obtain a copy of the Apache License at
http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing, software distributed
under the Apache License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
OR CONDITIONS OF ANY KIND, either express or implied. See the Apache License for
the specific language governing permissions and limitations under the License.

  Copyright (c) 2026 Audiokinetic Inc.
*******************************************************************************/

/// \file 
/// Spatial audio data type definitions.

#pragma once

#include <AK/SoundEngine/Common/AkTypes.h>
#ifdef __cplusplus
#include <AK/Tools/Common/AkString.h>
#endif

#define AK_MAX_NUM_TEXTURE 4
#define AK_MAX_REFLECT_ORDER 4
#define AK_MAX_REFLECTION_PATH_LENGTH (AK_MAX_REFLECT_ORDER + 4)
#define AK_STOCHASTIC_RESERVE_LENGTH AK_MAX_REFLECTION_PATH_LENGTH
#define AK_MAX_SOUND_PROPAGATION_DEPTH 8
#define AK_MAX_SOUND_PROPAGATION_WIDTH 32 // Can not exceed 32 total sound propagation paths.
#define AK_SA_EPSILON (0.001f)
#define AK_SA_DIFFRACTION_EPSILON (0.01f) // Radians (~0.57 degrees)
#define AK_SA_DIFFRACTION_DOT_EPSILON (0.00005f) // 1.f - cos(AK_SA_DIFFRACTION_EPSILON)
#define AK_SA_PLANE_THICKNESS (0.01f)
#define AK_SA_MIN_ENVIRONMENT_ABSORPTION (0.01f)
#define AK_SA_MIN_ENVIRONMENT_SURFACE_AREA (1.0f)

#define AK_INVALID_VERTEX ((AkVertIdx)(-1))
#define AK_INVALID_TRIANGLE ((AkTriIdx)(-1))
#define AK_INVALID_SURFACE ((AkSurfIdx)(-1))
#define AK_INVALID_SA_ID ((AkUInt64)-1)
#define AK_OUTDOORS_ROOM_ID AK_INVALID_SA_ID

/// Determines the operation used to calculate the transmission loss value for a transmission path.
/// A ray is cast from listener to emitter, and this operation is applied to combine the current surface with the previous.
/// A geometry instance can be marked as solid by using \c AkGeometryInstanceParams::IsSolid.
/// If a geometry instance is solid, transmission loss is applied for each pair of hit surfaces, representing an entrance and exit through the geometry.
/// The transmission loss from a pair of hits through a solid geometry instance will be the max of both hits.
enum AkTransmissionOperation
{
	AkTransmissionOperation_Add,				///< Transmission loss of each hit surface is summed until it reaches 100%.
	AkTransmissionOperation_Multiply,			///< The inverse of transmission loss (1 - TL) is multiplied in succession, and the result inverted. With each hit surface, the effect of additional transmission loss is reduced. The total loss will approach but never reach 100% unless a surface with 100% loss is found.
	AkTransmissionOperation_Max,				///< The highest transmission loss of all hit surfaces is used.
	AkTransmissionOperation_Default = AkTransmissionOperation_Max,
};
typedef AkUInt8 AkTransmissionOperation_t;

/// Determines how a room interacts with the distance calculation of other rooms that it overlaps or is nested within.
enum AkRoomDistanceBehavior
{
	/// This room is subtracted from the shape of parents and overlapping rooms of lower priority.
	/// Distance attenuation is applied to other rooms when the listener is in this room, because this room is a distinct space.
	AkRoomDistanceBehavior_Subtract,

	/// [\ref spatial_audio_experimental "Experimental"] This room does not participate in the distance calculation of other rooms.
	/// Distance attenuation does not apply to this room's parents, or overlapping sections of lower priority rooms,
	/// because this room does not form a distinct space and is not subtracted from the shape of other rooms.
	AkRoomDistanceBehavior_Exclude,

	AkRoomDistanceBehavior_Default = AkRoomDistanceBehavior_Subtract
};
typedef AkUInt8 AkRoomDistanceBehavior_t;

#ifdef __cplusplus

/// Base type for ID's used by Wwise spatial audio.  
struct AkSpatialAudioID
{
	/// Default constructor.  Creates an invalid ID.
	constexpr AkSpatialAudioID() : id(AK_INVALID_SA_ID) {}
	
	/// Construct from a 64-bit int.
	AkSpatialAudioID(AkUInt64 _id) : id(_id) {}

	/// Conversion from a pointer to a AkSpatialAudioID
	explicit AkSpatialAudioID(const void * ptr) : id(reinterpret_cast<AkUInt64>(ptr)) {}

	bool operator == (const AkSpatialAudioID& rhs) const { return id == rhs.id; }
	bool operator != (const AkSpatialAudioID& rhs) const { return id != rhs.id; }
	bool operator < (const AkSpatialAudioID& rhs) const { return id < rhs.id; }
	bool operator > (const AkSpatialAudioID& rhs) const { return id > rhs.id; }
	bool operator <= (const AkSpatialAudioID& rhs) const { return id <= rhs.id; }
	bool operator >= (const AkSpatialAudioID& rhs) const { return id >= rhs.id; }

	/// Determine if this ID is valid.
	bool IsValid() const { return id != (AkUInt64)-1; }

	/// Conversion function used internally to convert from a AkSpatialAudioID to a AkGameObjectID.
	AkGameObjectID AsGameObjectID() const { return (AkGameObjectID)id; }
	
	operator AkUInt64 () const { return id; }

	AkUInt64 id;
};

/// Spatial Audio Room ID type.  This ID type exists in the same ID-space as game object ID's.  The client is responsible for not choosing room ID's
/// that conflict with registered game objects' ID's.  Internally, the spatial audio rooms and portals API manages registration and un-registration of game objects that 
/// represent rooms using AkRoomID's provided by the client; AkRoomID's are converted to AkGameObjectID's by calling AsGameObjectID(). 
/// \sa 
///	- \ref AK::SpatialAudio::SetRoom
///	- \ref AK::SpatialAudio::RemoveRoom
struct AkRoomID : public AkSpatialAudioID
{
	/// Default constructor.  Creates an invalid ID.
	constexpr AkRoomID() : AkSpatialAudioID() {}

	/// Construct from a 64-bit int.
	AkRoomID(AkUInt64 _id) : AkSpatialAudioID(_id) {}

	/// Conversion from a pointer to a AkRoomID
	explicit AkRoomID(const void * ptr) : AkSpatialAudioID(ptr) {}

	/// Conversion function used to convert AkRoomID's to AkGameObjectIDs.
	AkGameObjectID AsGameObjectID() const { return IsValid() ? (AkGameObjectID)id : OutdoorsGameObjID; }
	
	/// Conversion function used to convert to AkGameObjectIDs to AkRoomID.
	static AkRoomID FromGameObjectID(AkGameObjectID in_fromGameObject)
	{
		AkRoomID id;
		if (in_fromGameObject != OutdoorsGameObjID)
			id.id = (AkUInt64)in_fromGameObject;
		return id;
	}

private:
	/// A game object ID that is in the reserved range, used for 'outdoor' rooms, i.e. when not in a room.
	/// \akwarning This AkGameObjectID is the underlying game object ID of the outdoor room, and should not be confused with the actual outdoor room's ID, AK::SpatialAudio::kOutdoorRoomID.\endakwarning
	static const AkGameObjectID OutdoorsGameObjID = (AkGameObjectID)-4;
};

namespace AK
{
	namespace SpatialAudio
	{
		/// The outdoor room ID. This room is created automatically and is typically used for outdoors, i.e. when not in a room. 
		constexpr AkRoomID kOutdoorRoomID;
	}
}

struct AkVertex : public AkVector
{
	AkVertex() : AkVector{ 0, 0, 0 } {}
	AkVertex(AkReal32 in_x, AkReal32 in_y, AkReal32 in_z)
		: AkVector{ in_x, in_y, in_z }
	{}
};

#else

typedef AkUInt64 AkSpatialAudioID;
typedef AkSpatialAudioID AkRoomID;

typedef struct AkVector AkVertex;

#endif


///< Unique ID for portals.  This ID type exists in the same ID-space as game object ID's.  The client is responsible for not choosing portal ID's
/// that conflict with registered game objects' ID's.  Internally, the spatial audio rooms and portals API manages registration and un-registration of game objects that 
/// represent portals using AkPortalID's provided by the client; AkPortalID's are convertied to AkGameObjectID's by calling AsGameObjectID(). 
/// \sa 
///	- \ref AK::SpatialAudio::SetPortal
///	- \ref AK::SpatialAudio::RemovePortal  
typedef AkSpatialAudioID AkPortalID;

///< Unique ID for identifying geometry sets.  Chosen by the client using any means desired.  
/// \sa 
///	- \ref AK::SpatialAudio::SetGeometry
///	- \ref AK::SpatialAudio::RemoveGeometry
typedef AkSpatialAudioID AkGeometrySetID;

///< Unique ID for identifying geometry set instances.  Chosen by the client using any means desired.  
/// \sa 
///	- \ref AK::SpatialAudio::SetGeometry
///	- \ref AK::SpatialAudio::RemoveGeometry
typedef AkSpatialAudioID AkGeometryInstanceID;

/// Data used to describe one image source in Reflect.
struct AkImageSourceName
{
#ifdef __cplusplus
	AkImageSourceName()
		: uNumChar(0)
		, pName(NULL)
	{
	}

	void SetName(const char* in_pName);
#endif

	AkUInt32 uNumChar;							///< Number of characters in image source name.
	const char* pName;							///< Optional image source name. Appears in Reflect's editor when profiling.
};


/// Initialization settings of the spatial audio module.
struct AkSpatialAudioInitSettings
{
#ifdef __cplusplus
	AkSpatialAudioInitSettings()
		: uMaxSoundPropagationDepth(AK_MAX_SOUND_PROPAGATION_DEPTH)
		, fMovementThreshold(0.25f)
		, uNumberOfPrimaryRays(35)
		, uMaxReflectionOrder(2)
		, uMaxDiffractionOrder(4)
		, uMaxDiffractionPaths(8)
		, uMaxGlobalReflectionPaths(0)
		, uMaxEmitterRoomAuxSends(3)
		, uDiffractionOnReflectionsOrder(2)
		, fMaxDiffractionAngleDegrees(180.f)
		, fMaxPathLength(1000.0f)
		, fCPULimitPercentage(0.0f)
		, uLoadBalancingSpread(1)
		, fSmoothingConstantMs(0.f)
		, fAdjacentRoomBleed(1.f)
		, bEnableGeometricDiffractionAndTransmission(true)
		, bCalcEmitterVirtualPosition(true)
		, eTransmissionOperation(AkTransmissionOperation_Default)
		, uClusteringMinPoints(0)
		, fClusteringMaxDistance(5.0)
		, fClusteringDeadZoneDistance(10.0f)
	{}
#endif

	AkUInt32 uMaxSoundPropagationDepth;				///< Maximum number of rooms that sound can propagate through; must be less than or equal to AK_MAX_SOUND_PROPAGATION_DEPTH.  Values below 2 will not propagate sound past the listener's own room.
	AkReal32 fMovementThreshold;					///< Amount that an emitter or listener has to move to trigger a validation of reflections/diffraction. Larger values can reduce the CPU load at the cost of reduced accuracy. Note that the ray tracing itself is not affected by this value. Rays are cast each time a Spatial Audio update is executed.
	AkUInt32 uNumberOfPrimaryRays;					///< The number of primary rays used in the ray tracing engine. A larger number of rays will increase the chances of finding reflection and diffraction paths, but will result in higher CPU usage.
	AkUInt32 uMaxReflectionOrder;					///< Maximum reflection order [1, 4] - the number of 'bounces' in a reflection path. A high reflection order renders more details at the expense of higher CPU usage.
	AkUInt32 uMaxDiffractionOrder;					///< Maximum diffraction order [1, 8] - the number of 'bends' in a diffraction path. A high diffraction order accommodates more complex geometry at the expense of higher CPU usage.
	///< Diffraction must be enabled on the geometry to find diffraction paths (see \c AkGeometryParams). Set to 0 to disable diffraction on all geometry.
	///< This parameter limits the recursion depth of diffraction rays cast from the listener to scan the environment, and also the depth of the diffraction search to find paths between emitter and listener.
	///< To optimize CPU usage, set it to the maximum number of edges you expect the obstructing geometry to traverse. 
	///< For example, if box-shaped geometry is used exclusively, and only a single box is expected between an emitter and then listener, limiting \c uMaxDiffractionOrder to 2 may be sufficient.
	///< A diffraction path search starts from the listener, so when the maximum diffraction order is exceeded, the remaining geometry between the end of the path and the emitter is ignored. 
	///< In such case, where the search is terminated before reaching the emitter, the diffraction coefficient will be underestimated. It is calculated from a partial path, ignoring any remaining geometry.	
	AkUInt32 uMaxDiffractionPaths;					///< Limit the maximum number of diffraction paths computed per emitter, excluding the direct/transmission path. The acoustics engine searches for up to uMaxDiffractionPaths paths and stops searching when this limit is reached. 
	///< Setting a low number for uMaxDiffractionPaths (1-4) uses fewer CPU resources, but is more likely to cause discontinuities in the resulting audio. This can occur, for example, when a more prominent path is discovered, displacing a less prominent one. 
	///< Conversely, a larger number (8 or more) produces higher quality output but requires more CPU resources. The recommended range is 2-8.
	AkUInt32 uMaxGlobalReflectionPaths;					///< [\ref spatial_audio_experimental "Experimental"] Set a global reflection path limit among all sound emitters with early reflections enabled. Potential reflection paths, discovered by raycasting, are first sorted according to a heuristic to determine which paths are the most prominent. 
	///< Afterwards, the full reflection path calculation is performed on only the uMaxGlobalReflectionPaths, most prominent paths. Limiting the total number of reflection path calculations can significantly reduce CPU usage. Recommended range: 10-50. 
	///< Set to 0 to disable the limit. In this case, the number of paths computed is unbounded and depends on how many are discovered by raycasting.
	AkUInt32 uMaxEmitterRoomAuxSends;				///< The maximum number of game-defined auxiliary sends that can originate from a single emitter. An emitter can send to its own room, and to all adjacent rooms if the emitter and listener are in the same room. If a limit is set, the most prominent sends are kept, based on spread to the adjacent portal from the emitters perspective.
	///< Set to 1 to only allow emitters to send directly to their current room, and to the room a listener is transitioning to if inside a portal. Set to 0 to disable the limit.
	AkUInt32 uDiffractionOnReflectionsOrder;		///< The maximum possible number of diffraction points at each end of a reflection path. Diffraction on reflection allows reflections to fade in and out smoothly as the listener or emitter moves in and out of the reflection's shadow zone.
	///< When greater than zero, diffraction rays are sent from the listener to search for reflections around one or more corners from the listener.
	///< Diffraction must be enabled on the geometry to find diffracted reflections (see \c AkGeometryParams). Set to 0 to disable diffraction on reflections.
	///< To allow reflections to propagate through portals without being cut off, set \c uDiffractionOnReflectionsOrder to 2 or greater.
	AkReal32 fMaxDiffractionAngleDegrees;			///< The largest possible diffraction value, in degrees, beyond which paths are not computed and are inaudible. Must be greater than zero. Default value: 180 degrees. 
	///< A large value (for example, 360 degrees) allows paths to propagate further around corners and obstacles, but takes more CPU time to compute. 
	///< A gain is applied to each diffraction path to taper the volume of the path to zero as the diffraction angle approaches fMaxDiffractionAngleDegrees,
	///< and appears in the Voice Inspector as "Diffraction Tapering". This tapering gain is applied in addition to the diffraction curves, and prevents paths from popping in or out suddenly when the maximum diffraction angle is exceeded. 
	///< In Wwise Authoring, the horizontal axis of a diffraction curve in the attenuation editor is defined over the range 0-100%, corresponding to angles 0-180 degrees.
	///< If fMaxDiffractionAngleDegrees is greater than 180 degrees, diffraction coefficients over 100% are clamped and the curve is evaluated at the rightmost point. 
	AkReal32 fMaxPathLength;						///< The total length of a path composed of a sequence of segments (or rays) cannot exceed the defined maximum path length. High values compute longer paths but increase the CPU cost.
	///< Each individual sound is also affected by its maximum attenuation distance, specified in the Authoring tool. Reflection or diffraction paths, calculated inside Spatial Audio, will never exceed a sound's maximum attenuation distance.
	///< Note, however, that attenuation is considered infinite if the furthest point is above the audibility threshold.
	AkReal32 fCPULimitPercentage;					///< Defines the targeted max computation time allocated for spatial audio. Defined as a percentage [0, 100] of the current audio frame. When the value is different from 0, Spatial Audio adapts dynamically the load balancing spread value between 1 and the specified AkSpatialAudioInitSettings::uLoadBalancingSpread according to current CPU usage and the specified CPU limit. Set to 0 to disable the dynamic load balancing spread computation.
	AkUInt32 uLoadBalancingSpread;					///< Spread the computation of paths on uLoadBalancingSpread frames [1..[. When uLoadBalancingSpread is set to 1, no load balancing is done. Values greater than 1 indicate the computation of paths will be spread on this number of frames. When CPU limit is active (see AkSpatialAudioInitSettings::fCPULimitPercentage), this setting represents the upper bound spread used by the dynamic load balancing instead of a fixed value.
	AkReal32 fSmoothingConstantMs;					///< [\ref spatial_audio_experimental "Experimental"]  Enable parameter smoothing on the diffraction paths output from the Acoustics Engine. Set fSmoothingConstantMs to a value greater than 0 to define the time constant (in milliseconds) for parameter smoothing. 
	///< The time constant of an exponential moving average is the amount of time for the smoothed response of a unit step function to reach 1 - 1/e ~= 63.2% of the original signal. 
	///< A large value (eg. 500-1000 ms) results in less variance but introduces lag, which is a good choice when using conservative values for uNumberOfPrimaryRays (eg. 5-10), uMaxDiffractionPaths (eg. 1-3) or fMovementThreshold ( > 1m ), in order to reduce overall CPU cost. 
	///< A small value (eg. 10-100 ms) results in greater accuracy and faster convergence of rendering parameters. Set to 0 to disable path smoothing.
	AkReal32 fAdjacentRoomBleed;					///< Set a global scaling factor that manipulates reverb send values. AkSpatialAudioInitSettings::fAdjacentRoomBleed affects the proportion of audio sent to adjacent rooms 
	///< and to the emitter's current room.
	///< This value is multiplied by AkPortalParams::AdjacentRoomBleed, which is used to scale reverb bleed for individual portals.
	///< When calculating reverb send amounts, each portal's aperture is multiplied by fAdjacentRoomBleed, which alters its perceived size:
	///<	- 1.0 (default): Maintains portals at their true geometric sizes (no scaling).
	///<	- > 1.0: Increases the perceived size of all portals, which increases bleed into adjacent rooms.
	///<	- < 1.0: Decreases the perceived size of all portals, which reduces bleed into adjacent rooms.
	///< Valid range: (0.0 - infinity)
	///< Note: Values close to 0 might cause abrupt portal transitions.
	bool bEnableGeometricDiffractionAndTransmission;///< Enable computation of geometric diffraction and transmission paths for all sources that have the <b>Enable Diffraction and Transmission</b> box checked in the Positioning tab of the Wwise Property Editor.
	///< This flag enables sound paths around (diffraction) and through (transmission) geometry (see \c AK::SpatialAudio::SetGeometry).
	///< Setting \c bEnableGeometricDiffractionAndTransmission to false implies that geometry is only to be used for reflection calculation.
	///< Diffraction edges must be enabled on geometry for diffraction calculation (see \c AkGeometryParams).
	///< If \c bEnableGeometricDiffractionAndTransmission is false but a sound has <b>Enable Diffraction and Transmission</b> selected in the Positioning tab of the authoring tool, the sound will diffract through portals but will pass through geometry as if it is not there.
	///< One would typically disable this setting in the case that the game intends to perform its own obstruction calculation, but geometry is still passed to spatial audio for reflection calculation.
	bool bCalcEmitterVirtualPosition;				///< An emitter that is diffracted through a portal or around geometry will have its apparent or virtual position calculated by Wwise Spatial Audio and passed on to the sound engine.
	enum AkTransmissionOperation eTransmissionOperation; ///< The operation used to determine transmission loss on direct paths.

	AkUInt32 uClusteringMinPoints;	///< [\ref spatial_audio_experimental "Experimental"] Minimum number of emitters in a cluster. Default value is 0. Values less than 2 disable the clustering.
	///< Note: Emitters with multi-positions are not clustered and are treated as independant emitters.
	///< Note: Changing an emitter from single to multi-positions with load balancing enabled (see \ref AkSpatialAudioInitSettings::uLoadBalancingSpread) might lead to unknown behaviors for a few frames.
	AkReal32 fClusteringMaxDistance; ///< Max distance between emitters to be considered as neighbors. This distance is specified for the reference distance defined by fClusteringDeadZoneDistance. Default value is 5.0.
	AkReal32 fClusteringDeadZoneDistance; ///< Defines a dead zone around the listener where no emitters are clusters. Default value is 10.0.
};

#pragma pack(push, 4)

struct AkImageSourceParams
{
#ifdef __cplusplus
	AkImageSourceParams()
		: fDistanceScalingFactor(1.f)
		, fLevel(1.f)
		, fDiffraction(0.f)
		, fOcclusion(0.f)
		, uDiffractionEmitterSide(0)
		, uDiffractionListenerSide(0)
	{
		sourcePosition.X = 0.f;
		sourcePosition.Y = 0.f;
		sourcePosition.Z = 0.f;
	}

	AkImageSourceParams(AkVector64 in_sourcePosition, AkReal32 in_fDistanceScalingFactor, AkReal32 in_fLevel)
		: sourcePosition(in_sourcePosition)
		, fDistanceScalingFactor(in_fDistanceScalingFactor)
		, fLevel(in_fLevel)
		, fDiffraction(0.f)
		, fOcclusion(0.f)
		, uDiffractionEmitterSide(0)
		, uDiffractionListenerSide(0)
	{
	}
#endif

	struct AkVector64 sourcePosition;           ///< Image source position, relative to the world.
	AkReal32 fDistanceScalingFactor;            ///< Image source distance scaling. This number effectively scales the sourcePosition vector with respect to the listener and, consequently, scales distance and preserves orientation.
	AkReal32 fLevel;                            ///< Game-controlled level for this source, linear.
	AkReal32 fDiffraction;                      ///< Diffraction amount, normalized to the range [0,1].
	AkReal32 fOcclusion;                        ///< Portal occlusion amount, in the range [0,1].
	AkUInt8 uDiffractionEmitterSide;            ///< If there is a shadow zone diffraction just after the emitter in the reflection path, indicates the number of diffraction edges, otherwise 0 if no diffraction.
	AkUInt8 uDiffractionListenerSide;           ///< If there is a shadow zone diffraction before reaching the listener in the reflection path, indicates the number of diffraction edges, otherwise 0 if no diffraction.
};

struct AkImageSourceTexture
{
#ifdef __cplusplus
	AkImageSourceTexture()
		: uNumTexture(1)
	{
		arTextureID[0] = AK_INVALID_UNIQUE_ID;
	}
#endif

	AkUInt32 uNumTexture;						///< Number of valid textures in the texture array.
	AkUniqueID arTextureID[AK_MAX_NUM_TEXTURE];	///< Unique IDs of the Acoustics Texture ShareSets used to filter this image source.
};

/// Settings for individual image sources.
struct AkImageSourceSettings
{
#ifdef __cplusplus
	/// Constructor
	AkImageSourceSettings() {}

	/// Constructor with parameters
	AkImageSourceSettings(AkVector64 in_sourcePosition, AkReal32 in_fDistanceScalingFactor, AkReal32 in_fLevel)
		: params(in_sourcePosition, in_fDistanceScalingFactor, in_fLevel)
		, texture()
	{
	}

	/// Helper function to set a single acoustic texture.
	void SetOneTexture(AkUniqueID in_texture)
	{
		texture.uNumTexture = 1;
		texture.arTextureID[0] = in_texture;
	}
#endif

	/// Image source parameters.
	struct AkImageSourceParams params;

	/// Acoustic texture that goes with this image source.
	/// \akwarning Note that changing acoustic textures across frames for a given image source, identified by in_srcID, may result in a discontinuity in the audio signal.
	/// \sa SetImageSource
	struct AkImageSourceTexture texture;
};

#pragma pack(pop)

///  AkExtent describes an extent with width, height and depth. halfWidth, halfHeight and halfDepth should form a vector from the centre of the volume to the positive corner.
///  Used in \c AkPortalParams, negative values in the extent will cause an error. For rooms, negative values can be used to opt out of room transmission.
struct AkExtent
{
#ifdef __cplusplus
	AkExtent(): halfWidth(0.0f), halfHeight(0.0f), halfDepth(0.0f) {}

	AkExtent(AkReal32 in_halfWidth, AkReal32 in_halfHeight, AkReal32 in_halfDepth)
		: halfWidth(in_halfWidth)
		, halfHeight(in_halfHeight)
		, halfDepth(in_halfDepth)
	{}
#endif

	AkReal32 halfWidth;
	AkReal32 halfHeight;
	AkReal32 halfDepth;
};

/// Triangle for a spatial audio mesh. 
struct AkTriangle
{
#ifdef __cplusplus
	/// Constructor
	AkTriangle() : point0(AK_INVALID_VERTEX)
		, point1(AK_INVALID_VERTEX)
		, point2(AK_INVALID_VERTEX)
		, surface(AK_INVALID_SURFACE)
	{}

	/// Constructor
	AkTriangle(AkVertIdx in_pt0, AkVertIdx in_pt1, AkVertIdx in_pt2, AkSurfIdx in_surfaceInfo)
		: point0(in_pt0)
		, point1(in_pt1)
		, point2(in_pt2)
		, surface(in_surfaceInfo)
	{}
#endif

	/// Index into the vertex table passed into \c AkGeometryParams that describes the first vertex of the triangle. Triangles are double-sided, so vertex order in not important.
	AkVertIdx point0;

	/// Index into the vertex table passed into \c AkGeometryParams that describes the second vertex of the triangle. Triangles are double-sided, so vertex order in not important.
	AkVertIdx point1;

	/// Index into the vertex table passed into \c AkGeometryParams that describes the third vertex of the triangle. Triangles are double-sided, so vertex order in not important.
	AkVertIdx point2;

	/// Index into the surface table passed into \c AkGeometryParams that describes the surface properties of the triangle.  
	/// If this field is left as \c AK_INVALID_SURFACE, then a default-constructed \c AkAcousticSurface is used.
	AkSurfIdx surface;
};

/// Describes the acoustic surface properties of one or more triangles.
/// An single acoustic surface may describe any number of triangles, depending on the granularity desired.  For example, if desired for debugging, one could create a unique 
/// \c AkAcousticSurface struct for each triangle, and define a unique name for each.  Alternatively, a single \c AkAcousticSurface could be used to describe all triangles.
/// In fact it is not necessary to define any acoustic surfaces at all.  If the \c AkTriangle::surface field is left as \c AK_INVALID_SURFACE, then a default-constructed \c AkAcousticSurface is used.
struct AkAcousticSurface
{
#ifdef __cplusplus
	/// Constructor
	AkAcousticSurface() : textureID(AK_INVALID_UNIQUE_ID)
		, transmissionLoss(1.0f)
		, strName(NULL)
	{}
#endif

	/// Acoustic texture ShareSet ID for the surface.  The acoustic texture is authored in Wwise, and the shareset ID may be obtained by calling \c AK::SoundEngine::GetIDFromString
	/// \sa <tt>\ref AK::SoundEngine::GetIDFromString()</tt>
	AkUInt32 textureID;

	/// Transmission loss value to apply when simulating sound transmission through this geometric surface. 
	/// Transmission is only simulated on a sound when the sound has <b>Enable Diffraction and Transmission</b> box
	/// enabled in Wwise Authoring. 
	/// 
	/// If more than one surface is intersected between the emitter and the listener, the result depends on \c AkSpatialAudioInitSettings::eTransmissionOperation. The default behavior is to use the maximum of all surfaces' transmission loss values.
	/// 
	/// Transmission loss is applied on the mix connection between the emitter and the listener for the dry path, and between the emitter and the room for the wet path.
	/// The transmission loss value is converted to volume attenuation, low-pass and/or high-pass filtering, using the transmission loss curves defined on the sound in Wwise Authoring.
	/// 
	/// A transmission loss value of 0 has special meaning in some contexts: 
	/// - Setting a transmission loss value of 0 effectively disables a surface for ray intersection. It is ignored for all diffraction and reflection calculations.
	/// - When geometry is used to define the shape of a Reverb Zone, surfaces with a transmission loss of 0 define the center of the transition between the Reverb Zone and its parent Room. 
	/// Only surfaces with transmission loss of 0 define the transition region, allowing the user to have transitions around parts of the geometry (certain walls, for example), and not others.
	/// Valid range: (0.f-1.f)
	/// - \ref AkRoomParams
	/// - \ref AK::SpatialAudio::SetReverbZone
	AkReal32 transmissionLoss;

	/// Name to describe this surface
	const char* strName;
};

/// Structure for retrieving information about the indirect paths of a sound that have been calculated via the geometric reflections API. Useful for debug draw applications.
struct AkReflectionPathInfo
{
	/// Apparent source of the reflected sound that follows this path.
	struct AkVector64 imageSource;

	/// Vertices of the indirect path.
	/// pathPoint[0] is closest to the emitter, pathPoint[numPathPoints-1] is closest to the listener.
	struct AkVector64 pathPoint[AK_MAX_REFLECTION_PATH_LENGTH];

	/// The texture that were hit in the path.
	/// textureIDs[0] is closest to the emitter, textureIDs[numPathPoints-1] is closest to the listener.
	AkUInt32 textureIDs[AK_MAX_REFLECTION_PATH_LENGTH];

	/// Number of valid elements in the \c pathPoint[], \c surfaces[], and \c diffraction[] arrays.
	AkUInt32 numPathPoints;

	/// Number of reflections in the \c pathPoint[] array. Shadow zone diffraction does not count as a reflection. If there is no shadow zone diffraction, \c numReflections is equal to \c numPathPoints.
	AkUInt32 numReflections;

	/// Diffraction amount, normalized to the range [0,1]
	AkReal32 diffraction[AK_MAX_REFLECTION_PATH_LENGTH];

	/// Linear gain applied to image source.
	AkReal32 level;

	/// Deprecated - always false. Occluded paths are not generated.
	bool isOccluded;
};

/// Structure for retrieving information about paths for a given emitter. 
/// The diffraction paths represent indirect sound paths from the emitter to the listener, whether they go through portals 
/// (via the rooms and portals API) or are diffracted around edges (via the geometric diffraction API).
/// The direct path is included here and can be identified by checking \c nodeCount == 0. The direct path may have a non-zero transmission loss 
/// if it passes through geometry or between rooms.
struct AkDiffractionPathInfo
{
#ifdef __cplusplus
	/// Defines the maximum number of nodes that a user can retrieve information about.  Longer paths will be truncated. 
	static const AkUInt32 kMaxNodes = AK_MAX_SOUND_PROPAGATION_DEPTH;
#endif

	/// Diffraction points along the path. nodes[0] is the point closest to the listener; nodes[numNodes-1] is the point closest to the emitter. 
	/// Neither the emitter position nor the listener position are represented in this array.
	struct AkVector64 nodes[AK_MAX_SOUND_PROPAGATION_DEPTH];

	/// Emitter position. This is the source position for an emitter. In all cases, except for radial emitters, it is the same position as the game object position.
	/// For radial emitters, it is the calculated position at the edge of the volume.
	struct AkVector64 emitterPos;

	/// Raw diffraction angles at each point, in radians.
	AkReal32 angles[AK_MAX_SOUND_PROPAGATION_DEPTH];

	/// ID of the portals that the path passes through.  For a given node at position i (in the nodes array), if the path diffracts on a geometric edge, then portals[i] will be an invalid portal ID (ie. portals[i].IsValid() will return false). 
	/// Otherwise, if the path diffracts through a portal at position i, then portals[i] will be the ID of that portal.
	/// portal[0] represents the node closest to the listener; portal[numNodes-1] represents the node closest to the emitter.
	AkPortalID portals[AK_MAX_SOUND_PROPAGATION_DEPTH];

	/// ID's of the rooms that the path passes through. For a given node at position i, room[i] is the room on the listener's side of the node. If node i diffracts through a portal, 
	/// then rooms[i] is on the listener's side of the portal, and rooms[i+1] is on the emitters side of the portal.
	/// There is always one extra slot for a room so that the emitters room is always returned in slot room[numNodes] (assuming the path has not been truncated).
	AkRoomID rooms[AK_MAX_SOUND_PROPAGATION_DEPTH + 1];

	/// Virtual emitter position. This is the position that is passed to the sound engine to render the audio using multi-positioning, for this particular path.
	struct AkWorldTransform virtualPos;

	/// Total number of nodes in the path.  Defines the number of valid entries in the \c nodes, \c angles, and \c portals arrays. The \c rooms array has one extra slot to fit the emitter's room.
	AkUInt32 nodeCount;

	/// Calculated total diffraction from this path, normalized to the range [0,1]
	/// The diffraction amount is calculated from the sum of the deviation angles from a straight line, of all angles at each nodePoint. 
	//	Can be thought of as how far into the 'shadow region' the sound has to 'bend' to reach the listener.
	/// This value is applied internally, by spatial audio, as the Diffraction value and built-in parameter of the emitter game object.
	/// \sa
	/// - \ref AkSpatialAudioInitSettings
	AkReal32 diffraction;

	/// Calculated total transmission loss from this path, normalized to the range [0,1]
	/// This field will be 0 for diffraction paths where \c nodeCount > 0. It may be non-zero for the direct path where \c nodeCount == 0.
	/// The path's transmission loss value might be geometric transmission loss, if geometry was intersected in the path, 
	/// or room transmission loss, if no geometry was available.
	/// The geometric transmission loss is calculated from the transmission loss values assigned to the geometry that this path transmits through.
	/// If a path transmits through multiple geometries with different transmission loss values, the largest value is taken.
	/// The room transmission loss is taken from the emitter and listener rooms' transmission loss values, and likewise, 
	/// if the listener's room and the emitter's room have different transmission loss values, the greater of the two is used.
	/// This value is applied internally, by spatial audio, as the Transmission Loss value and built-in parameter of the emitter game object.
	/// \sa
	/// - \ref AkSpatialAudioInitSettings
	/// - \ref AkRoomParams
	/// - \ref AkAcousticSurface
	AkReal32 transmissionLoss;

	/// Total path length
	/// Represents the sum of the length of the individual segments between nodes, with a correction factor applied for diffraction. 
	/// The correction factor simulates the phenomenon where by diffracted sound waves decay faster than incident sound waves and can be customized in the spatial audio init settings.
	/// \sa
	/// - \ref AkSpatialAudioInitSettings
	AkReal32 totLength;

	/// Obstruction value for this path 
	/// This value includes the accumulated portal obstruction for all portals along the path.
	AkReal32 obstructionValue;

	/// Occlusion value for this path 
	/// This value includes the accumulated portal occlusion for all portals along the path.
	AkReal32 occlusionValue;

	/// Propagation path gain.
	/// Includes volume tapering gain to ensure that diffraction paths do not cut in or out when the maximum diffraction angle is exceeded.
	AkReal32 gain;
};

#pragma pack(push, 4)

/// Parameters passed to \c SetPortal
struct AkPortalParams
{
#ifdef __cplusplus
	/// Constructor
	AkPortalParams()
		: bEnabled(false)
		, AdjacentRoomBleed(1.f)
	{}
#endif

	/// Portal's position and orientation in the 3D world. 
	/// Position vector is the center of the opening.
	/// OrientationFront vector must be unit-length and point along the normal of the portal, and must be orthogonal to Up. It defines the local positive-Z dimension (depth/transition axis) of the portal, used by Extent. 
	/// OrientationTop vector must be unit-length and point along the top of the portal (tangent to the wall), must be orthogonal to Front. It defines the local positive-Y direction (height) of the portal, used by Extent.
	struct AkWorldTransform Transform;

	/// Portal extent. Defines the dimensions of the portal relative to its center; all components must be positive numbers.
	/// The shape described by these extents is used to "cut out" the opening shape of room geometry. Geometry that overlaps the portal extent is effectively ignored.
	/// The depth dimension is used to perform transitions between connected rooms by manipulating game-defined auxiliary sends.
	/// The depth dimension is also used to place game objects into rooms while they are inside the portals.
	/// \sa
	/// - \ref AkExtent
	struct AkExtent Extent;

	/// Whether or not the portal is active/enabled. For example, this parameter may be used to simulate open/closed doors.
	/// Portal diffraction is simulated when at least one portal exists and is active between an emitter and the listener.
	/// To simulate a door that opens or closes gradually, use \c AK::SpatialAudio::SetPortalObstructionAndOcclusion to apply occlusion to a portal, according to the door's opening amount.
	/// \sa
	/// - \ref AK::SpatialAudio::SetPortalObstructionAndOcclusion
	bool bEnabled;

	/// ID of the room to which the portal connects, in the direction of the Front vector.  If a room with this ID has not been added via AK::SpatialAudio::SetRoom,
	/// a room will be created with this ID and with default AkRoomParams.  If you would later like to update the AkRoomParams, simply call AK::SpatialAudio::SetRoom again with this same ID.
	///	- \ref AK::SpatialAudio::SetRoom
	///	- \ref AK::SpatialAudio::RemoveRoom
	/// - \ref AkRoomParams
	AkRoomID FrontRoom;

	/// ID of the room to which the portal connects, in the direction opposite to the Front vector. If a room with this ID has not been added via AK::SpatialAudio::SetRoom,
	/// a room will be created with this ID and with default AkRoomParams.  If you would later like to update the AkRoomParams, simply call AK::SpatialAudio::SetRoom again with this same ID.
	///	- \ref AK::SpatialAudio::SetRoom
	///	- \ref AK::SpatialAudio::RemoveRoom
	/// - \ref AkRoomParams
	AkRoomID BackRoom;

	/// A scaling factor that manipulates reverb send values. AkPortalParams::AdjacentRoomBleed affects the proportion of audio sent to the adjacent room versus the proportion sent to the emitter's current room, for this particular portal.
	/// This value is multiplied by AkSpatialAudioInitSettings::fAdjacentRoomBleed, which is used to scale reverb bleed for all portals.
	/// When calculating reverb send amounts, the portal's aperture is multiplied by AdjacentRoomBleed, altering its perceived size:
	///	- 1.0 (default): Maintain the portal at its true geometric size (no scaling).
	///	- > 1.0: Increases the perceived portal size, allowing more bleed into adjacent rooms.
	///	- < 1.0: Decreases the perceived portal size, reducing bleed into adjacent rooms.
	/// Valid range: (0.0 - infinity)
	/// Note: Values approaching 0 may result in abrupt portal transitions.
	AkReal32 AdjacentRoomBleed;
};

/// Parameters passed to \c SetRoom
struct AkRoomParams
{
#ifdef __cplusplus
	/// Constructor
	AkRoomParams() : ReverbAuxBus(AK_INVALID_AUX_ID)
		, ReverbLevel(1.f)
		, TransmissionLoss(1.f)
		, RoomGameObj_AuxSendLevelToSelf(0.f)
		, RoomGameObj_KeepRegistered(false)
		, RoomPriority(100.f)
		, DistanceBehavior(AkRoomDistanceBehavior_Default)
	{
		// default invalid values
		Up.X = 0.f;
		Up.Y = 1.f;
		Up.Z = 0.f;
		Front.X = 0.f;
		Front.Y = 0.f;
		Front.Z = 1.f;
	}
#endif

	/// Room Orientation. Up and Front must be orthonormal.
	/// Room orientation has an effect when the associated aux bus (see ReverbAuxBus) is set with 3D Spatialization in Wwise, as 3D Spatialization implements relative rotation of the emitter (room) and listener.
	struct AkVector Front;

	/// Room Orientation. Up and Front must be orthonormal.
	/// Room orientation has an effect when the associated aux bus (see ReverbAuxBus) is set with 3D Spatialization in Wwise, as 3D Spatialization implements relative rotation of the emitter (room) and listener.
	struct AkVector Up;

	/// The reverb aux bus that is associated with this room.  
	/// When Spatial Audio is told that a game object is in a particular room via SetGameObjectInRoom, a send to this aux bus will be created to model the reverb of the room.
	/// Using a combination of Rooms and Portals, Spatial Audio manages which game object the aux bus is spawned on, and what control gain is sent to the bus.  
	/// When a game object is inside a connected portal, as defined by the portal's orientation and extent vectors, both this aux send and the aux send of the adjacent room are active.
	/// Spatial audio modulates the control value for each send based on the game object's position, in relation to the portal's z-azis and extent, to crossfade the reverb between the two rooms.
	/// If more advanced control of reverb is desired, SetGameObjectAuxSendValues can be used to add additional sends on to a game object.
	/// - \ref AK::SpatialAudio::SetGameObjectInRoom
	/// - \ref AK::SoundEngine::SetGameObjectAuxSendValues
	AkAuxBusID						ReverbAuxBus;

	/// The reverb control value for the send to ReverbAuxBus. Valid range: (0.f-1.f)
	/// Can be used to implement multiple rooms that share the same aux bus, but have different reverb levels.
	AkReal32						ReverbLevel;

	/// Characteristic transmission loss value for the Room, describing the average amount of sound energy dissipated by the walls of the room.
	/// 
	/// The Room transmission loss value is used in the following situations: 
	/// 1) If a room tone is playing on the Room Game Object, in a different room than that of the listener. 
	///    Transmission loss is only applied when the listener and the emitter are in different rooms.
	///    It is taken as the maximum between the emitter's room's transmission loss value and the listener's room's transmission loss value.
	/// 2) If, for both the emitter and listener's Rooms, geometry is not assigned with \c AkRoomParams::GeometryInstanceID, or the Geometry Instance that is assigned 
	///    has \c AkGeometryInstanceParams::UseForReflectionAndDiffraction set to false. In this case, room transmission loss is applied 
	///	   to the direct path when the emitter and listener are in different rooms.
	///	   When geometry is available, the transmission loss value is calculated instead by intersecting a ray with the geometry between the emitter and listener.
	///    Transmission loss on geometry overrides room transmission loss, allowing a room to have varying degrees of transmission loss (for example, some transparent walls and some opaque walls).
	/// 3) To spatialize the output of the Room Game Object. A low transmission loss value emphasizes panning the room 
	///    towards the room's center (the center of the room bounding box) and a higher value emphasizes panning towards the room's portals.
	/// 
	/// When room transmission is applied directly to either a room tone (situation 1) or a point source emitter (situation 2), it is applied on the mix connection between the sound and the room.
	/// The transmission loss value is converted to volume attenuation, low-pass and/or high-pass filtering, using the transmission loss curves defined on the sound in Wwise Authoring.
	/// 
	/// \aknote Even when relying on geometry for detailed transmission loss values (per triangle), it is still necessary to set \c AkSoundParams::TransmissionLoss to a representitive value so that
	/// the output of the Room Game Object is properly spatialized (situation 3).
	/// 
	/// Valid range: (0.f-1.f)
	/// - \ref AkAcousticSurface
	AkReal32						TransmissionLoss;

	/// Send level for sounds that are posted on the room game object; adds reverb to ambience and room tones. Valid range: (0.f-1.f).  Set to a value greater than 0 to have spatial audio create a send on the room game object, 
	/// where the room game object itself is specified as the listener and ReverbAuxBus is specified as the aux bus. A value of 0 disables the aux send. This should not be confused with ReverbLevel, which is the send level 
	/// for spatial audio emitters sending to the room game object.
	/// \aknote The room game object can be accessed though the ID that is passed to \c SetRoom() and the \c AkRoomID::AsGameObjectID() method.  Posting an event on the room game object leverages automatic room game object placement 
	///	by spatial audio so that when the listener is inside the room, the sound comes from all around the listener, and when the listener is outside the room, the sound comes from the portal(s). Typically, this would be used for
	/// surround ambiance beds or room tones. Point source sounds should use separate game objects that are registered as spatial audio emitters.
	/// \sa
	/// - \ref AkRoomParams::RoomGameObj_KeepRegistered
	/// - \ref AkRoomID
	AkReal32						RoomGameObj_AuxSendLevelToSelf;

	/// If set to true, the room game object is registered on calling \c SetRoom(), and not released until the room is deleted or removed with \c RemoveRoom(). If set to false, Spatial Audio registers
	/// the room object only when it is needed by the sound propagation system for the purposes of reverb, and unregisters the game object when all reverb tails are finished.
	/// We recommend that you set RoomGameObj_KeepRegistered to true if you use RTPCs on the room game object, if you call \c SetScalingFactor(), or call \c PostEvent() for the purpose of ambience or room tones.
	/// \aknote The room game object can be accessed through the ID that is passed to \c SetRoom() and the \c AkRoomID::AsGameObjectID() method. Posting an event on the room game object uses automatic room game object placement 
	///	by Spatial Audio so that when the listener is inside the room, the sound comes from all around the listener, and when the listener is outside the room, the sound comes from the portal(s). Typically, this would be used for
	/// surround ambience beds or room tones. For point source sounds, use separate game objects that are registered as Spatial Audio emitters.
	/// \sa
	/// - \ref AkRoomParams::RoomGameObj_AuxSendLevelToSelf
	/// - \ref AkRoomID
	bool							RoomGameObj_KeepRegistered;

	/// Assign a Geometry Instance \c GeometryInstanceID, which describes the size and shape of the Room. 
	/// Assigning a Geometry Instance to a Room serves several purposes: the shape and surface properties of the geometry are used to calculate the transmission of reverb and room tones through walls,
	/// it allows Spatial Audio to automatically determine which Game Objects are in which Room, and it allows for visualization of the room inside the Game Object 3D Viewer.
	/// To create Room geometry, a Geometry Set must be created using AK::SpatialAudio::SetGeometry, and then a Geometry Instance must be created using AK::SpatialAudio::SetGeometryInstance. 
	/// The Room itself, however, can be created either before or after the geometry is created.
	/// \aknote If the geometry instance is only used for room containment and not for reflection, diffraction, or transmission, then set \c AkGeometryInstanceParams::UseForReflectionAndDiffraction to false. 
	/// \sa
	/// - \ref spatial_audio_roomsportals_apiconfigroomgeometry
	/// - \ref AkGeometryInstanceParams
	/// - \ref AK::SpatialAudio::SetGameObjectInRoom
	AkGeometrySetID					GeometryInstanceID;

	/// Associate a priority with this room. Room priority is used by the room containment system to disambiguate cases where an object is inside several rooms at the same time. In this case, the room with the higher priority is selected.
	/// Default priority is 100.
	/// \aknote If several rooms have the same highest room priority, the inner one is selected.
	AkReal32						RoomPriority;

	/// Determines how a room interacts with the distance calculation of other rooms that it overlaps or is nested within.
	/// Default is AkRoomDistanceBehavior_Subtract.
	///  - \ref spatial_audio_roomsportals_distance
	enum AkRoomDistanceBehavior		DistanceBehavior;


};

/// Parameters passed to \c SetGeometry
struct AkGeometryParams
{
#ifdef __cplusplus
	/// Constructor
	AkGeometryParams() : Triangles(NULL), NumTriangles(0), Vertices(NULL), NumVertices(0), Surfaces(NULL), NumSurfaces(0), EnableDiffraction(false), EnableDiffractionOnBoundaryEdges(false) {}
#endif

	/// Pointer to an array of AkTriangle structures. 
	/// This array will be copied into spatial audio memory and will not be accessed after \c SetGeometry returns.
	///	- \ref AkTriangle
	///	- \ref AK::SpatialAudio::SetGeometry
	///	- \ref AK::SpatialAudio::RemoveGeometry
	AK_WPTR(struct AkTriangle*, Triangles);

	/// Number of triangles in Triangles.
	AkTriIdx NumTriangles;

	/// Pointer to an array of AkVertex structures. 
	/// This array will be copied into spatial audio memory and will not be accessed after \c SetGeometry returns.
	///	- \ref AkVertex
	///	- \ref AK::SpatialAudio::SetGeometry
	///	- \ref AK::SpatialAudio::RemoveGeometry
	AK_WPTR(AkVertex*, Vertices);

	/// Number of vertices in Vertices.
	AkVertIdx NumVertices;

	/// Pointer to an array of AkAcousticSurface structures.
	/// This array will be copied into spatial audio memory and will not be accessed after \c SetGeometry returns.
	///	- \ref AkVertex
	///	- \ref AK::SpatialAudio::SetGeometry
	///	- \ref AK::SpatialAudio::RemoveGeometry
	AK_WPTR(struct AkAcousticSurface*, Surfaces);

	/// Number of of AkTriangleInfo structures in in_pTriangleInfo and number of AkTriIdx's in in_infoMap.
	AkSurfIdx NumSurfaces;

	/// Switch to enable or disable geometric diffraction for this Geometry.
	bool EnableDiffraction;

	/// Switch to enable or disable geometric diffraction on boundary edges for this Geometry.  Boundary edges are edges that are connected to only one triangle.
	bool EnableDiffractionOnBoundaryEdges;
};

#define AK_DEFAULT_GEOMETRY_POSITION_X (0.0)
#define AK_DEFAULT_GEOMETRY_POSITION_Y (0.0)
#define AK_DEFAULT_GEOMETRY_POSITION_Z (0.0)
#define AK_DEFAULT_GEOMETRY_FRONT_X (0.0)
#define AK_DEFAULT_GEOMETRY_FRONT_Y (0.0)
#define AK_DEFAULT_GEOMETRY_FRONT_Z (1.0)
#define AK_DEFAULT_GEOMETRY_TOP_X (0.0)
#define AK_DEFAULT_GEOMETRY_TOP_Y (1.0)
#define AK_DEFAULT_GEOMETRY_TOP_Z (0.0)

/// Parameters passed to \c SetGeometryInstance
struct AkGeometryInstanceParams
{
#ifdef __cplusplus
	/// Constructor
	/// Creates an instance with an identity transform.
	/// \akwarning A default-constructed AkGeometryInstanceParams assumes the default floor plane is passed to AkInitSettings::eFloorPlane.
	AkGeometryInstanceParams()
		: Scale({ 1, 1, 1 })
		, UseForReflectionAndDiffraction(true)
		, BypassPortalSubtraction(false)
		, IsSolid(false)
	{
		PositionAndOrientation.Set(
			AK_DEFAULT_GEOMETRY_POSITION_X, AK_DEFAULT_GEOMETRY_POSITION_Y, AK_DEFAULT_GEOMETRY_POSITION_Z,
			AK_DEFAULT_GEOMETRY_FRONT_X, AK_DEFAULT_GEOMETRY_FRONT_Y, AK_DEFAULT_GEOMETRY_FRONT_Z,
			AK_DEFAULT_GEOMETRY_TOP_X, AK_DEFAULT_GEOMETRY_TOP_Y, AK_DEFAULT_GEOMETRY_TOP_Z);
	}
#endif

	/// Set the position and orientation of the geometry instance.
	/// AkWorldTransform uses one vector to define the position of the geometry instance, and two more to define the orientation; a forward vector and an up vector. 
	/// To ensure that a geometry instance has the correct rotation with respect to the game, AkInitSettings::eFloorPlane must be initialized with the correct value.
	///	\sa
	/// - \ref AkInitSettings::eFloorPlane
	/// - \ref AK::SpatialAudio::SetGeometryInstance
	///	- \ref AK::SpatialAudio::RemoveGeometryInstance
	struct AkWorldTransform PositionAndOrientation;

	/// Set the 3-dimensional scaling of the geometry instance.
	/// \sa
	/// - \ref AK::SpatialAudio::SetGeometryInstance
	///	- \ref AK::SpatialAudio::RemoveGeometryInstance
	struct AkVector Scale;

	/// Geometry set referenced by the instance
	/// \sa
	///	- \ref AK::SpatialAudio::SetGeometry
	///	- \ref AK::SpatialAudio::RemoveGeometry
	/// - \ref AK::SpatialAudio::SetGeometryInstance
	///	- \ref AK::SpatialAudio::RemoveGeometryInstance
	AkGeometrySetID GeometrySetID;

	/// When enabled, the geometry instance is indexed for ray computation and used to compute reflection, diffraction, and transmission.
	/// If the geometry instance is used only for room containment, this flag must be set to false.
	///	- \ref AK::SpatialAudio::SetRoom
	///	- \ref AkRoomParams
	///
	bool UseForReflectionAndDiffraction;

	/// [\ref spatial_audio_experimental "Experimental"]  When set to false (default), the intersection of the geometry instance with any portal bounding box is subtracted from the geometry. In effect, an opening is created at the portal location through which sound can pass.
	/// When set to true, portals cannot create openings in the geometry instance. Enable this to allow the geometry instance to be an obstacle to paths going into or through portal bounds.
	///	- \ref AK::SpatialAudio::SetPortal
	///
	bool BypassPortalSubtraction;

	/// A solid geometry instance applies transmission loss once for each time a transmission path enters and exits its volume, using the max transmission loss between each hit surface. A non-solid geometry instance is one where each surface is infinitely thin, applying transmission loss at each surface. This option has no effect if the Transmission Operation is set to Max.
	///
	bool IsSolid;
};

#pragma pack(pop)
/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Apache License Usage

Alternatively, this file may be used under the Apache License, Version 2.0 (the
"Apache License"); you may not use this file except in compliance with the
Apache License. You may obtain a copy of the Apache License at
http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing, software distributed
under the Apache License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
OR CONDITIONS OF ANY KIND, either express or implied. See the Apache License for
the specific language governing permissions and limitations under the License.

  Copyright (c) 2026 Audiokinetic Inc.
*******************************************************************************/

/// \file 
/// Spatial Audio interface.

#pragma once

#include <AK/SpatialAudio/Common/AkSpatialAudioTypes.h>
#include <AK/Plugin/AkReflectGameData.h>
#include <AK/SoundEngine/Common/AkSoundEngine.h>

/// Audiokinetic namespace
namespace AK
{
	/// Audiokinetic spatial audio namespace
	namespace SpatialAudio
	{
		////////////////////////////////////////////////////////////////////////
		/// @name Basic functions. 
		/// In order to use SpatialAudio, you need to initialize it using Init, and register the listeners that you plan on using with any of the services offered by SpatialAudio, using 
		/// RegisterListener respectively, _after_ having registered their corresponding game object to the sound engine.
		/// \akwarning At the moment, there can be only one Spatial Audio listener registered at any given time.
		//@{

		/// Initialize the SpatialAudio API.  
		extern  AKRESULT __cdecl  Init (const AkSpatialAudioInitSettings& in_initSettings);

		/// Assign a game object as the Spatial Audio listener.  There can be only one Spatial Audio listener registered at any given time; in_gameObjectID will replace any previously set Spatial Audio listener.
		/// The game object passed in must be registered by the client, at some point, for sound to be heard.  It is not necessary to be registered at the time of calling this function.
		/// If no listener is explicitly registered to spatial audio, then a default listener (set via \c AK::SoundEngine::SetDefaultListeners()) is selected.  If there are no default listeners, or there are more than one
		/// default listeners, then it is necessary to call RegisterListener() to specify which listener to use with Spatial Audio.
		extern  AKRESULT __cdecl  RegisterListener (
			AkGameObjectID in_gameObjectID				///< Game object ID
			);

		/// Unregister a game object as a listener in the SpatialAudio API; clean up Spatial Audio listener data associated with in_gameObjectID.  
		/// If in_gameObjectID is the current registered listener, calling this function will clear the Spatial Audio listener and
		/// Spatial Audio features will be disabled until another listener is registered.
		/// This function is optional - listener are automatically unregistered when their game object is deleted in the sound engine.
		/// \sa 
		/// - \ref AK::SpatialAudio::RegisterListener
		extern  AKRESULT __cdecl  UnregisterListener (
			AkGameObjectID in_gameObjectID				///< Game object ID
			);

		/// Define an inner and outer radius around each sound position for a specified game object.
		/// If the radii are set to 0, the game object is a point source. Non-zero radii create a Radial Emitter.
		/// The radii are used in spread and distance calculations that simulates sound emitting from a spherical volume of space.
		/// When applying attenuation curves, the distance between the listener and the inner sphere (defined by the sound position and \c in_innerRadius) is used. 
		/// The spread for each sound position is calculated as follows:
		/// - If the listener is outside the outer radius, the spread is defined by the area that the sphere occupies in the listener field of view. Specifically, this angle is calculated as 2.0*asinf( \c in_outerRadius / distance ), where distance is the distance between the listener and the sound position.
		///	- When the listener intersects the outer radius (the listener is exactly \c in_outerRadius units away from the sound position), the spread is exactly 50%.
		/// - When the listener is between the inner and outer radii, the spread interpolates linearly from 50% to 100% as the listener transitions from the outer radius towards the inner radius.
		/// - If the listener is inside the inner radius, the spread is 100%.
		/// \aknote Transmission and diffraction calculations in Spatial Audio always use the center of the sphere (the position(s) passed into \c AK::SoundEngine::SetPosition or \c AK::SoundEngine::SetMultiplePositions) for raycasting. 
		/// To obtain accurate diffraction and transmission calculations for radial sources, where different parts of the volume may take different paths through or around geometry,
		/// it is necessary to pass multiple sound positions into \c AK::SoundEngine::SetMultiplePositions to allow the engine to 'sample' the area at different points.
		/// - \ref AK::SoundEngine::SetPosition
		/// - \ref AK::SoundEngine::SetMultiplePositions
		extern  AKRESULT __cdecl  SetGameObjectRadius (
			AkGameObjectID in_gameObjectID,				///< Game object ID
			AkReal32 in_outerRadius,					///< Outer radius around each sound position, defining 50% spread. Must satisfy \c in_innerRadius <= \c in outerRadius.
			AkReal32 in_innerRadius						///< Inner radius around each sound position, defining 100% spread and 0 attenuation distance. Must satisfy \c in_innerRadius <= \c in outerRadius.
			);

		//@}

		////////////////////////////////////////////////////////////////////////
		/// @name Helper functions for passing game data to the Reflect plug-in. 
		/// Use this API for detailed placement of reflection image sources.
		/// \aknote These functions are low-level and useful when your game engine already implements a geometrical approach to sound propagation such as an image-source or a ray tracing algorithm.
		/// Functions of Geometry are preferred and easier to use with the Reflect plug-in. \endaknote
		//@{

		/// Add or update an individual image source for processing via the AkReflect plug-in.  Use this API for detailed placement of
		/// reflection image sources, whose positions have been determined by the client, such as from the results of a ray cast, computation or by manual placement.  One possible
		/// use case is generating reflections that originate far enough away that they can be modeled as a static point source, for example, off of a distant mountain.
		/// The SpatialAudio API manages image sources added via SetImageSource() and sends them to the AkReflect plug-in that is on the aux bus with ID \c in_AuxBusID. 
		/// The image source applies all game objects that have a reflections aux send defined in the authoring tool, or only to a specific game object if \c in_gameObjectID is used.
		/// \aknote The \c AkImageSourceSettings struct passed in \c in_info must contain a unique image source ID to be able to identify this image source across frames and when updating and/or removing it later.  
		/// Each instance of AkReflect has its own set of data, so you may reuse ID, if desired, as long as \c in_gameObjectID and \c in_AuxBusID are different.
		/// \aknote It is possible for the AkReflect plugin to process reflections from both \c SetImageSource and the geometric reflections API on the same aux bus and game object, but be aware that image source ID collisions are possible.
		/// The image source IDs used by the geometric reflections API are generated from hashed data that uniquely identifies the reflecting surfaces. If a collision occurs, one of the reflections will not be heard.
		/// While collision are rare, to ensure that it never occurs use an aux bus for \c SetImageSource that is unique from the aux bus(es) defined in the authoring tool, and from those passed to \c SetEarlyReflectionsAuxSend.
		/// \endaknote
		/// \aknote For proper operation with AkReflect and the SpatialAudio API, any aux bus using AkReflect should have 'Listener Relative Routing' checked and the 3D Spatialization set to None in the Wwise authoring tool. See \ref spatial_audio_wwiseprojectsetup_businstances for more details. \endaknote
		/// \sa 
		/// - \ref AK::SpatialAudio::RemoveImageSource
		///	- \ref AK::SpatialAudio::ClearImageSources
		/// - \ref AK::SpatialAudio::SetGameObjectInRoom
		/// - \ref AK::SpatialAudio::SetEarlyReflectionsAuxSend
		extern  AKRESULT __cdecl  SetImageSource (
			AkImageSourceID in_srcID,								///< The ID of the image source being added.
			const AkImageSourceSettings& in_info,					///< Image source information.
			const char* in_name,									///< Name given to image source, can be used to identify the image source in the AK Reflect plugin UI.
			AkUniqueID in_AuxBusID = AK_INVALID_AUX_ID,				///< Aux bus that has the AkReflect plug in for early reflection DSP. 
																	///< Pass AK_INVALID_AUX_ID to use the reflections aux bus defined in the authoring tool.
			AkGameObjectID in_gameObjectID = AK_INVALID_GAME_OBJECT	///< The ID of the emitter game object to which the image source applies. 
																	///< Pass AK_INVALID_GAME_OBJECT to apply to all game objects that have a reflections aux bus assigned in the authoring tool.
			);

		/// Remove an individual reflection image source that was previously added via \c SetImageSource.
		/// \sa 
		///	- \ref AK::SpatialAudio::SetImageSource
		///	- \ref AK::SpatialAudio::ClearImageSources
		extern  AKRESULT __cdecl  RemoveImageSource (
			AkImageSourceID in_srcID,									///< The ID of the image source to remove.
			AkUniqueID in_AuxBusID = AK_INVALID_AUX_ID,					///< Aux bus that was passed to SetImageSource.
			AkGameObjectID in_gameObjectID = AK_INVALID_GAME_OBJECT		///< Game object ID that was passed to SetImageSource.
			);

		/// Remove all image sources matching \c in_AuxBusID and \c in_gameObjectID that were previously added via \c SetImageSource.
		/// Both \c in_AuxBusID and \c in_gameObjectID can be treated as wild cards matching all aux buses and/or all game object, by passing \c AK_INVALID_AUX_ID and/or \c AK_INVALID_GAME_OBJECT, respectively.
		/// \sa 
		///	- \ref AK::SpatialAudio::SetImageSource
		/// - \ref AK::SpatialAudio::RemoveImageSource
		extern  AKRESULT __cdecl  ClearImageSources (
			AkUniqueID in_AuxBusID = AK_INVALID_AUX_ID,							///< Aux bus that was passed to SetImageSource, or AK_INVALID_AUX_ID to match all aux buses.
			AkGameObjectID in_gameObjectID = AK_INVALID_GAME_OBJECT				///< Game object ID that was passed to SetImageSource, or AK_INVALID_GAME_OBJECT to match all game objects.
			);

		//@}

		////////////////////////////////////////////////////////////////////////
		/// @name Geometry 
		/// Geometry API for early reflection processing using Reflect.
		//@{

		/// Add or update a set of geometry from the \c SpatialAudio module for geometric reflection and diffraction processing. A geometry set is a logical set of vertices, triangles, and acoustic surfaces,
		/// which are referenced by the same \c AkGeometrySetID. The ID (\c in_GeomSetID) must be unique and is also chosen by the client in a manner similar to \c AkGameObjectID's.
		/// It is necessary to create at least one geometry instance for each geometry set that is to be used for diffraction and reflection simulation.
		/// \sa 
		///	- \ref AkGeometryParams
		///	- \ref AK::SpatialAudio::SetGeometryInstance
		///	- \ref AK::SpatialAudio::RemoveGeometry
		extern  AKRESULT __cdecl  SetGeometry (
			AkGeometrySetID in_GeomSetID,		///< Unique geometry set ID, chosen by client.
			const AkGeometryParams& in_params	///< Geometry parameters to set.
			);

		/// Remove a set of geometry to the SpatialAudio API.
		/// Calling \c AK::SpatialAudio::RemoveGeometry will remove all instances of the geometry from the scene.
		/// \sa 
		///	- \ref AK::SpatialAudio::SetGeometry
		extern  AKRESULT __cdecl  RemoveGeometry (
			AkGeometrySetID in_SetID		///< ID of geometry set to be removed.
			);

		/// Add or update a geometry instance from the \c SpatialAudio module for geometric reflection and diffraction processing. 
		/// A geometry instance is a unique instance of a geometry set with a specified transform (position, rotation and scale). 
		/// It is necessary to create at least one geometry instance for each geometry set that is to be used for diffraction and reflection simulation.
		/// The ID (\c in_GeomSetInstanceID) must be unique amongst all geometry instances, including geometry instances referencing different geometry sets. The ID is chosen by the client in a manner similar to \c AkGameObjectID's.
		/// To update the transform of an existing geometry instance, call SetGeometryInstance again, passing the same \c AkGeometryInstanceID, with the updated transform. 
		/// \sa 
		///	- \ref AkGeometryInstanceParams
		///	- \ref AK::SpatialAudio::RemoveGeometryInstance
		extern  AKRESULT __cdecl  SetGeometryInstance (
			AkGeometryInstanceID in_GeometryInstanceID,	///< Unique geometry set instance ID, chosen by client.
			const AkGeometryInstanceParams& in_params	///< Geometry instance parameters to set.
			);

		/// Remove a geometry instance from the SpatialAudio API.
		/// \sa 
		///	- \ref AK::SpatialAudio::SetGeometryInstance
		extern  AKRESULT __cdecl  RemoveGeometryInstance (
			AkGeometryInstanceID in_GeometryInstanceID	///< ID of geometry set instance to be removed.
			);

		/// Query information about the reflection paths that have been calculated via geometric reflection processing in the SpatialAudio API. This function can be used for debugging purposes.
		/// This function must acquire the global sound engine lock and therefore, may block waiting for the lock.
		/// \sa
		/// - \ref AkReflectionPathInfo
		extern  AKRESULT __cdecl  QueryReflectionPaths (
			AkGameObjectID in_gameObjectID, ///< The ID of the game object that the client wishes to query.
			AkUInt32 in_positionIndex,		///< The index of the associated game object position.
			AkVector64& out_listenerPos,		///< Returns the position of the listener game object that is associated with the game object \c in_gameObjectID.
			AkVector64& out_emitterPos,		///< Returns the position of the emitter game object \c in_gameObjectID.
			AkReflectionPathInfo* out_aPaths,	///< Pointer to an array of \c AkReflectionPathInfo's which will be filled after returning.
			AkUInt32& io_uArraySize			///< The number of slots in \c out_aPaths, after returning the number of valid elements written.
			);

		//@}

		////////////////////////////////////////////////////////////////////////
		/// @name Rooms and Portals
		/// Sound Propagation API using rooms and portals.
		//@{

		/// Add or update a room. Rooms are used to connect portals and define an orientation for oriented reverbs. This function may be called multiple times with the same ID to update the parameters of the room.
		/// \akwarning The ID (\c in_RoomID) must be chosen in the same manner as \c AkGameObjectID's, as they are in the same ID-space. The spatial audio lib manages the 
		/// registration/unregistration of internal game objects for rooms that use these IDs and, therefore, must not collide. 
		/// Also, the room ID must not be in the reserved range (AkUInt64)(-32) to (AkUInt64)(-2) inclusively. You may, however, explicitly add the default room ID AK::SpatialAudio::kOutdoorRoomID (-1) 
		/// in order to customize its AkRoomParams, to provide a valid auxiliary bus, for example.\endakwarning
		/// \sa
		/// - \ref AkRoomID
		/// - \ref AkRoomParams
		/// - \ref AK::SpatialAudio::RemoveRoom
		extern  AKRESULT __cdecl  SetRoom (
			AkRoomID in_RoomID,				///< Unique room ID, chosen by the client.
			const AkRoomParams& in_Params,	///< Parameter for the room.
			const char* in_RoomName = nullptr   ///< Name used to identify room (optional)
			);

		/// Remove a room.
		/// \sa
		/// - \ref AkRoomID
		/// - \ref AK::SpatialAudio::SetRoom
		extern  AKRESULT __cdecl  RemoveRoom (
			AkRoomID in_RoomID	///< Room ID that was passed to \c SetRoom.
			);

		/// Add or update an acoustic portal. A portal is an opening that connects two or more rooms to simulate the transmission of reverberated (indirect) sound between the rooms. 
		/// This function may be called multiple times with the same ID to update the parameters of the portal. The ID (\c in_PortalID) must be chosen in the same manner as \c AkGameObjectID's, 
		/// as they are in the same ID-space. The spatial audio lib manages the registration/unregistration of internal game objects for portals that use these IDs, and therefore must not collide.
		/// \sa
		/// - \ref AkPortalID
		/// - \ref AkPortalParams
		/// - \ref AK::SpatialAudio::RemovePortal
		extern  AKRESULT __cdecl  SetPortal (
			AkPortalID in_PortalID,		///< Unique portal ID, chosen by the client.
			const AkPortalParams& in_Params,	///< Parameter for the portal.
			const char* in_PortalName = nullptr   ///< Name used to identify portal (optional)
			);

		/// Remove a portal.
		/// \sa
		/// - \ref AkPortalID
		/// - \ref AK::SpatialAudio::SetPortal
		extern  AKRESULT __cdecl  RemovePortal (
			AkPortalID in_PortalID		///< ID of portal to be removed, which was originally passed to SetPortal.
			);

		/// Use a Room as a Reverb Zone.
		/// AK::SpatialAudio::SetReverbZone establishes a parent-child relationship between two Rooms and allows for sound propagation between them
		/// as if they were the same Room, without the need for a connecting Portal. Setting a Room as a Reverb Zone 
		/// is useful in situations where two or more acoustic environments are not easily modeled as closed rooms connected by portals.
		/// Possible uses for Reverb Zones include: a covered area with no walls, a forested area within an outdoor space, or any situation 
		/// where multiple reverb effects are desired within a common space. Reverb Zones have many advantages compared to standard Game-Defined
		/// Auxiliary Sends. They are part of the wet path, and form reverb chains with other Rooms; they are spatialized according to their 3D extent;  
		/// they are also subject to other acoustic phenomena simulated in Wwise Spatial Audio, such as diffraction and transmission.
		/// A parent Room may have multiple Reverb Zones, but a Reverb Zone can only have a single Parent. If a Room is already assigned 
		/// to a parent Room, it will first be removed from the old parent (exactly as if AK::SpatialAudio::RemoveReverbZone were called) 
		/// before then being assigned to the new parent Room. A Room can not be its own parent.
		/// The Reverb Zone and its parent are both Rooms, and as such, must be specified using AK::SpatialAudio::SetRoom.
		/// If AK::SpatialAudio::SetReverbZone is called before AK::SpatialAudio::SetRoom, and either of the two rooms do not yet exist,
		/// placeholder Rooms with default parameters are created. They should be subsequently parameteized with AK::SpatialAudio::SetRoom.
		/// 
		/// To set which Reverb Zone a Game Object is in, use the AK::SpatialAudio::SetGameObjectInRoom API, and pass the Reverb Zone's Room ID. 
		/// In Wwise Spatial Audio, a Game Object can only ever be inside a single room, and Reverb Zones are no different in this regard.
		/// \aknote
		/// The automatically created 'outdoors' Room is commonly used as a parent Room for Reverb Zones, since they often model open spaces. 
		/// To attach a Reverb zone to outdoors, pass AK::SpatialAudio::kOutdoorRoomID as the \c in_ParentRoom argument. Like all Rooms, the 'outdoors' Room
		/// can be parameterized (for example, to assign a reverb bus) by passing AK::SpatialAudio::kOutdoorRoomID to AK::SpatialAudio::SetRoom.
		/// \sa 
		/// - \ref AkRoomID
		///	- \ref AK::SpatialAudio::SetRoom
		///	- \ref AK::SpatialAudio::RemoveRoom
		///	- \ref AK::SpatialAudio::RemoveReverbZone
		/// - \ref AK::SpatialAudio::kOutdoorRoomID
		extern  AKRESULT __cdecl  SetReverbZone (
			AkRoomID in_ReverbZone,  ///< ID of the Room which will be specified as a Reverb Zone. 
			AkRoomID in_ParentRoom, ///< ID of the parent Room.
			AkReal32 in_transitionRegionWidth ///< Width of the transition region between the Reverb Zone and its parent. The transition region is centered around the Reverb Zone geometry. It only applies where triangle transmission loss is set to 0.
			);

		/// Remove a Reverb Zone from its parent. 
		/// It will no longer be possible for sound to propagate between the two rooms, unless they are explicitly connected with a Portal.
		/// \sa 
		///	- \ref AK::SpatialAudio::SetReverbZone
		///	- \ref AK::SpatialAudio::RemoveRoom
		///	- \ref AK::SpatialAudio::RemoveReverbZone
		extern  AKRESULT __cdecl  RemoveReverbZone (
			AkRoomID in_ReverbZone ///< ID of the Room which has been specified as a Reverb Zone. 
			);

		/// Set the room that the game object is currently located in - usually the result of a containment test performed by the client. The room must have been registered with \c SetRoom.
		///	Setting the room for a game object provides the basis for the sound propagation service, and also sets which room's reverb aux bus to send to.  The sound propagation service traces the path
		/// of the sound from the emitter to the listener, and calculates the diffraction as the sound passes through each portal.  The portals are used to define the spatial location of the diffracted and reverberated
		/// audio.
		/// \sa 
		///	- \ref AK::SpatialAudio::SetRoom
		///	- \ref AK::SpatialAudio::RemoveRoom
		extern  AKRESULT __cdecl  SetGameObjectInRoom (
			AkGameObjectID in_gameObjectID, ///< Game object ID 
			AkRoomID in_CurrentRoomID		///< RoomID that was passed to \c AK::SpatialAudio::SetRoom
			);

		/// Unset the room that the game object is currently located in.
		///	When a game object has not been explicitly assigned to a room with \ref AK::SpatialAudio::SetGameObjectInRoom, the room is automatically computed.
		/// \sa 
		///	- \ref AK::SpatialAudio::SetRoom
		///	- \ref AK::SpatialAudio::RemoveRoom
		extern  AKRESULT __cdecl  UnsetGameObjectInRoom (
			AkGameObjectID in_gameObjectID ///< Game object ID
			);

		/// Set a global scaling factor that manipulates reverb send values. AK::SpatialAudio::SetAdjacentRoomBleed affects the proportion of audio sent to adjacent rooms 
		/// versus the proportion sent to the emitter's current room. It updates the initialization setting specified in AkSpatialAudioInitSettings::fAdjacentRoomBleed.
		/// This value is multiplied by AkPortalParams::AdjacentRoomBleed, which is used to scale reverb bleed for individual portals.
		/// When calculating reverb send amounts, each portal's aperture is multiplied by fAdjacentRoomBleed, altering its perceived size:
		///	- 1.0 (default): Maintain portals at its true geometric size (no scaling).
		///	- > 1.0: Increases the perceived size of all portals, allowing more bleed into adjacent rooms.
		///	- < 1.0: Decreases the perceived size of all portals, reducing bleed into adjacent rooms.
		/// Valid range: (0.0 - infinity)
		/// Note: Values approaching 0 may result in abrupt portal transitions.
		extern  AKRESULT __cdecl  SetAdjacentRoomBleed (
			AkReal32 in_fAdjacentRoomBleed
			);

		/// Set the early reflections order for reflection calculation. The reflections order indicates the number of times sound can bounce off of a surface. 
		/// A higher number requires more CPU resources but results in denser early reflections. Set to 0 to globally disable reflections processing.
		extern  AKRESULT __cdecl  SetReflectionsOrder (
			AkUInt32 in_uReflectionsOrder,	///< Number of reflections to calculate. Valid range [0,4]
			bool in_bUpdatePaths			///< Set to true to clear existing higher-order paths and to force the re-computation of new paths. If false, existing paths will remain and new paths will be computed when the emitter or listener moves.
			);

		/// Set the diffraction order for geometric path calculation. The diffraction order indicates the number of edges a sound can diffract around. 
		/// A higher number requires more CPU resources but results in paths found around more complex geometry. Set to 0 to globally disable geometric diffraction processing.
		/// \sa
		/// - \ref AkSpatialAudioInitSettings::uMaxDiffractionOrder
		extern  AKRESULT __cdecl  SetDiffractionOrder (
			AkUInt32 in_uDiffractionOrder,	///< Number of diffraction edges to consider in path calculations. Valid range [0,8]
			bool in_bUpdatePaths			///< Set to true to clear existing diffraction paths and to force the re-computation of new paths. If false, existing paths will remain and new paths will be computed when the emitter or listener moves.
			);

		/// Set the maximum number of computed reflection paths
		///
		extern  AKRESULT __cdecl  SetMaxGlobalReflectionPaths (
			AkUInt32 in_uMaxGlobalReflectionPaths	///< Maximum number of reflection paths. Valid range [0..[
			);

		/// Set the maximum number of computed diffraction paths. 
		/// Pass a valid Game Object ID to to \c in_gameObjectID to affect a specific game object and override the value set in AkSpatialAudioInitSettings::uMaxDiffractionPaths. 
		/// Pass \c AK_INVALID_GAME_OBJECT to apply the same limit to all Game Objects (that have not previously been passed to SetMaxDiffractionPaths), 
		/// updating the value set for AkSpatialAudioInitSettings::uMaxDiffractionPaths.
		///
		/// \sa
		/// - \ref AkSpatialAudioInitSettings::uMaxDiffractionPaths
		extern  AKRESULT __cdecl  SetMaxDiffractionPaths (
			AkUInt32 in_uMaxDiffractionPaths,						///< Maximum number of reflection paths. Valid range [0..32]
			AkGameObjectID in_gameObjectID = AK_INVALID_GAME_OBJECT	///< Game Object ID. Pass AK_INVALID_GAME_OBJECT to affect all Game Objects, effectivly updating AkSpatialAudioInitSettings::uMaxDiffractionPaths. Pass a valid Game Object ID to override the init setting for a specific Game Object.
			);

		/// Set the maximum number of game-defined auxiliary sends that can originate from a single emitter. 
		/// Set to 1 to only allow emitters to send directly to their current room. Set to 0 to disable the limit.
		/// \sa
		/// - \ref AkSpatialAudioInitSettings::uMaxEmitterRoomAuxSends
		extern  AKRESULT __cdecl  SetMaxEmitterRoomAuxSends (
			AkUInt32 in_uMaxEmitterRoomAuxSends	///< The maximum number of room aux send connections.
			);

		/// Set the number of rays cast from the listener by the stochastic ray casting engine.
		/// A higher number requires more CPU resources but provides more accurate results. Default value (35) should be good for most applications.
		///
		extern  AKRESULT __cdecl  SetNumberOfPrimaryRays (
			AkUInt32 in_uNbPrimaryRays		///< Number of rays cast from the listener
			);

		/// Set the number of frames on which the path validation phase will be spread. Value between [1..[
		/// High values delay the validation of paths. A value of 1 indicates no spread at all.
		///
		extern  AKRESULT __cdecl  SetLoadBalancingSpread (
			AkUInt32 in_uNbFrames		///< Number of spread frames
			);

		/// [\ref spatial_audio_experimental "Experimental"]  Enable parameter smoothing on the diffraction paths output from the Acoustics Engine, either globally or for a specific game object. Set fSmoothingConstantMs to a value greater than 0 to define the time constant (in milliseconds) for parameter smoothing. 
		/// \sa
		/// - \ref AkSpatialAudioInitSettings::fSmoothingConstantMs
		extern  AKRESULT __cdecl  SetSmoothingConstant (
			AkReal32 in_fSmoothingConstantMs,								///< Smoothing constant (ms)
			AkGameObjectID in_gameObjectID = AK_INVALID_GAME_OBJECT			///< Game Object ID. Pass AK_INVALID_GAME_OBJECT to set the global smoothing constant, affecting all Spatial Audio Emitters and Rooms.
			);

		/// Set an early reflections auxiliary bus for a particular game object. 
		/// Geometrical reflection calculation inside spatial audio is enabled for a game object if any sound playing on the game object has a valid early reflections aux bus specified in the authoring tool,
		/// or if an aux bus is specified via \c SetEarlyReflectionsAuxSend.
		/// The \c in_auxBusID parameter of SetEarlyReflectionsAuxSend applies to sounds playing on the game object \c in_gameObjectID which have not specified an early reflection bus in the authoring tool -
		/// the parameter specified on individual sounds' reflection bus takes priority over the value passed in to \c SetEarlyReflectionsAuxSend.
		/// \aknote 
		/// Users may apply this function to avoid duplicating sounds in the Containers hierarchy solely for the sake of specifying a unique early reflection bus, or in any situation where the same 
		/// sound should be played on different game objects with different early reflection aux buses (the early reflection bus must be left blank in the authoring tool if the user intends to specify it through the API). \endaknote
		extern  AKRESULT __cdecl  SetEarlyReflectionsAuxSend (
			AkGameObjectID in_gameObjectID, ///< Game object ID 
			AkAuxBusID in_auxBusID			///< Auxiliary bus ID. Applies only to sounds which have not specified an early reflection bus in the authoring tool. Pass \c AK_INVALID_AUX_ID to set only the send volume.
			);

		/// Set an early reflections send volume for a particular game object. 
		/// The \c in_fSendVolume parameter is used to control the volume of the early reflections send. It is combined with the early reflections volume specified in the authoring tool, and is applied to all sounds 
		/// playing on the game object.
		/// Setting \c in_fSendVolume to 0.f will disable all reflection processing for this game object.
		extern  AKRESULT __cdecl  SetEarlyReflectionsVolume (
			AkGameObjectID in_gameObjectID, ///< Game object ID 
			AkReal32 in_fSendVolume			///< Send volume (linear) for auxiliary send. Set 0.f to disable reflection processing. Valid range 0.f-1.f. 
			);

		/// Set the obstruction and occlusion value for a portal that has been registered with Spatial Audio.
		/// Portal obstruction simulates objects that block the direct sound path between the portal and the listener, but
		/// allows indirect sound to pass around the obstacle. For example, use portal obstruction 
		/// when a piece of furniture blocks the line of sight of the portal opening.
		/// Portal obstruction is applied to the connection between the emitter and the listener, and only affects the dry signal path.
		/// Portal occlusion simulates a complete blockage of both the direct and indirect sound through a portal. For example, use portal occlusion for 
		/// opening or closing a door or window.
		/// Portal occlusion is applied to the connection between the emitter and the first room in the chain, as well as the connection between the emitter and listener.
		/// Portal occlusion affects both the dry and wet (reverberant) signal paths.
		/// If you are using built-in game parameters to drive RTPCs, the obstruction and occlusion values set here 
		/// do not affect the RTPC values. This behavior is intentional and occurs because RTPCs only provide one 
		/// value per game object, but a single game object can have multiple paths through different Portals, 
		/// each with different obstruction and occlusion values.
		/// To apply detailed obstruction to specific sound paths but not others, use \c AK::SpatialAudio::SetGameObjectToPortalObstruction and \c AK::SpatialAudio::SetPortalToPortalObstruction.
		/// To apply occlusion and obstruction to the direct line of sight between the emitter and listener use \c AK::SoundEngine::SetObjectObstructionAndOcclusion.
		/// \sa
		/// - \ref AK::SpatialAudio::SetGameObjectToPortalObstruction
		/// - \ref AK::SpatialAudio::SetPortalToPortalObstruction
		/// - \ref AK::SoundEngine::SetObjectObstructionAndOcclusion
		extern  AKRESULT __cdecl  SetPortalObstructionAndOcclusion (
			AkPortalID in_PortalID,				///< Portal ID.
			AkReal32 in_fObstruction,			///< Obstruction value.  Valid range 0.f-1.f
			AkReal32 in_fOcclusion,				///< Occlusion value.  Valid range 0.f-1.f
			bool     in_bTransition = false		///< Transition obstruction and occlusion through portals.  Default false
			);

		/// Set the obstruction value of the path between a game object and a portal that has been created by Spatial Audio.
		/// The obstruction value is applied on one of the path segments of the sound between the emitter and the listener.
		/// Diffraction must be enabled on the sound for a diffraction path to be created.
		/// Also, there should not be any portals between the provided game object and portal ID parameters.
		/// The obstruction value is used to simulate objects between the portal and the game object that are obstructing the sound.
		/// Send an obstruction value of 0 to ensure the value is removed from the internal data structure.
		/// \sa
		/// - \ref AK::SpatialAudio::SetPortalObstructionAndOcclusion
		extern  AKRESULT __cdecl  SetGameObjectToPortalObstruction (
			AkGameObjectID in_gameObjectID,		///< Game object ID
			AkPortalID in_PortalID,				///< Portal ID
			AkReal32 in_fObstruction			///< Obstruction value.  Valid range 0.f-1.f
			);

		/// Set the obstruction value of the path between two portals that has been created by Spatial Audio.
		/// The obstruction value is applied on one of the path segments of the sound between the emitter and the listener.
		/// Diffraction must be enabled on the sound for a diffraction path to be created.
		/// Also, there should not be any portals between the two provided ID parameters.
		/// The obstruction value is used to simulate objects between the portals that are obstructing the sound.
		/// Send an obstruction value of 0 to ensure the value is removed from the internal data structure.
		/// \sa
		/// - \ref AK::SpatialAudio::SetPortalObstructionAndOcclusion
		extern  AKRESULT __cdecl  SetPortalToPortalObstruction (
			AkPortalID in_PortalID0,			///< Portal ID
			AkPortalID in_PortalID1,			///< Portal ID
			AkReal32 in_fObstruction			///< Obstruction value.  Valid range 0.f-1.f
			);

		/// Query information about the wet diffraction amount for the portal \c in_portal, returned as a normalized value \c out_wetDiffraction in the range [0,1].  
		/// The wet diffraction is calculated from how far into the 'shadow region' the listener is from the portal.  Unlike dry diffraction, the 
		/// wet diffraction does not depend on the incident angle, but only the normal of the portal.
		/// This value is applied by spatial audio, to the Diffraction value and built-in game parameter of the room game object that is
		/// on the other side of the portal (relative to the listener).
		/// This function must acquire the global sound engine lock and therefore, may block waiting for the lock.
		/// \sa
		/// - \ref AkSpatialAudioInitSettings
		extern  AKRESULT __cdecl  QueryWetDiffraction (
			AkPortalID in_portal,			///< The ID of the game object that the client wishes to query.
			AkReal32& out_wetDiffraction	///< The number of slots in \c out_aPaths, after returning the number of valid elements written.
			);

		/// Query information about the diffraction state for a particular listener and emitter, which has been calculated using the data provided via the spatial audio emitter API. This function can be used for debugging purposes.
		/// Returned in \c out_aPaths, this array contains the sound paths calculated from diffraction around a geometric edge and/or diffraction through portals connecting rooms.
		/// No paths will be returned in any of the following conditions: (1) the emitter game object has a direct line of sight to the listener game object, (2) the emitter and listener are in the same room, and the listener is completely outside the radius of the emitter, 
		/// or (3) The emitter and listener are in different rooms, but there are no paths found via portals between the emitter and the listener.
		/// A single path with zero diffraction nodes is returned when all of the following conditions are met: (1) the emitter and listener are in the same room, (2) there is no direct line of sight, and (3) either the Voice's Attenuation's curve max distance is exceeded or the accumulated diffraction coefficient exceeds 1.0.
		/// This function must acquire the global sound engine lock and, therefore, may block waiting for the lock.
		/// \sa
		/// - \ref AkDiffractionPathInfo
		extern  AKRESULT __cdecl  QueryDiffractionPaths (
			AkGameObjectID in_gameObjectID,		///< The ID of the game object that the client wishes to query.
			AkUInt32 in_positionIndex,			///< The index of the associated game object position.
			AkVector64& out_listenerPos,		///< Returns the position of the listener game object that is associated with the game object \c in_gameObjectID.
			AkVector64& out_emitterPos,			///< Returns the position of the emitter game object \c in_gameObjectID.
			AkDiffractionPathInfo* out_aPaths,	///< Pointer to an array of \c AkDiffractionPathInfo's which will be filled on return.
			AkUInt32& io_uArraySize				///< The number of slots in \c out_aPaths, after returning the number of valid elements written.
			);

		/// Set the operation used to calculate transmission loss on a direct path between emitter and listener.
		/// 
		extern  AKRESULT __cdecl  SetTransmissionOperation (
			AkTransmissionOperation in_eOperation		///< The operation to be used on all transmission paths.
			);

		/// Reset the stochastic engine state by re-initializing the random seeds.
		///
		extern  AKRESULT __cdecl  ResetStochasticEngine ();

		//@}
	}
};
/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Apache License Usage

Alternatively, this file may be used under the Apache License, Version 2.0 (the 
"Apache License"); you may not use this file except in compliance with the 
Apache License. You may obtain a copy of the Apache License at 
http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing, software distributed
under the Apache License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
OR CONDITIONS OF ANY KIND, either express or implied. See the Apache License for
the specific language governing permissions and limitations under the License.

  Copyright (c) 2026 Audiokinetic Inc.
*******************************************************************************/

// AkMacSoundEngine.h

/// \file 
/// Main Sound Engine interface, specific Mac.

#ifndef _AK_MAC_SOUND_ENGINE_H_
#define _AK_MAC_SOUND_ENGINE_H_

#include <AK/SoundEngine/Common/AkTypes.h>
#include <AK/Tools/Common/AkPlatformFuncs.h>

///< API used for audio output
///< Use with AkPlatformInitSettings to select the API used for audio output. The selected API may be ignored if the device does not support it.
///< Use AkAPI_Default, it will select the more appropriate API depending on the device's capabilities.  Other values should be used for testing purposes.
///< \sa AK::SoundEngine::Init
typedef enum AkAudioAPIMac
{
	AkAudioAPI_AVAudioEngine    = 1 << 0,                                 ///< Use AVFoundation framework (modern, has more capabilities, available only for macOS 10.15 or above)
	AkAudioAPI_AudioUnit        = 1 << 1,                                 ///< Use AudioUnit framework (basic functionality, compatible with all macOS devices)
	AkAudioAPI_Default = AkAudioAPI_AVAudioEngine | AkAudioAPI_AudioUnit, ///< Default value, will select the more appropriate API (AVAudioEngine for compatible devices, AudioUnit for others)
} AkAudioAPI;

/// Add this to the idDevice value in AkOutputSettings to start the Motion plug-in in "resident mode".
/// "Resident mode" will keep the Motion device active, even during periods of silence.
/// This will eliminate a delay when processing some effects, but will increase the power consumption of the device.
///< \sa AK::SoundEngine::AddOutput
#define AKMOTION_RESIDENT_MODE (0x80000000)

/// Platform specific initialization settings
/// \sa AK::SoundEngine::Init
/// \sa AK::SoundEngine::GetDefaultPlatformInitSettings
struct AkPlatformInitSettings
{
	// Threading model.
    AkThreadProperties  threadLEngine;			///< Lower engine threading properties
	AkThreadProperties  threadOutputMgr;		///< Ouput thread threading properties
	AkThreadProperties  threadBankManager;		///< Bank manager threading properties (its default priority is AK_THREAD_PRIORITY_NORMAL)
	AkThreadProperties  threadMonitor;			///< Monitor threading properties (its default priority is AK_THREAD_PRIORITY_ABOVENORMAL). This parameter is not used in Release build.
	
	AkUInt32			uSampleRate;			///< Sampling Rate. Default 48000 Hz
	// Voices.
	AkUInt16            uNumRefillsInVoice;		///< Number of refill buffers in voice buffer. 2 == double-buffered, defaults to 4.
	
	AkAudioAPI          eAudioAPI;  ///< Main audio API to use. Leave to AkAPI_Default for the default sink (default value).
									///< \ref AkAudioAPI
	
	AkUInt32            uNumSpatialAudioPointSources; ///< Number of Apple Spatial Audio point sources to allocate for 3D audio use (each point source is a system audio object). Default: 128
	
	bool                bVerboseSystemOutput;   ///< Print detailed system output information to the console log
};

namespace AK
{
	namespace SoundEngine
	{
		/// Get the motion device ID corresponding to a GCController player index. This device ID can be used to add/remove motion output for that gamepad.
		/// The player index is 0-based, and corresponds to a value of type GCControllerPlayerIndex in the Core.Haptics framework.
		/// \note The ID returned is unique to Wwise and does not correspond to any sensible value outside of Wwise.
		/// \return Unique device ID
		extern  AkDeviceID __cdecl  GetDeviceIDFromPlayerIndex  (int playerIndex);
	}
}

#endif //_AK_MAC_SOUND_ENGINE_H_
struct AkStreamMgrSettings
{
};

struct AkDeviceSettings
{
	void *				pIOMemory;					///< Pointer for I/O memory allocated by user.
													///< Pass NULL if you want memory to be allocated via AK::MemoryMgr::Malign().
													///< If specified, uIOMemorySize, uIOMemoryAlignment and ePoolAttributes are ignored.
	AkUInt32			uIOMemorySize;				///< Size of memory for I/O (for automatic streams). It is passed directly to AK::MemoryMgr::Malign(), after having been rounded down to a multiple of uGranularity.
	AkUInt32			uIOMemoryAlignment;			///< I/O memory alignment. It is passed directly to AK::MemoryMgr::Malign().
	AkUInt32			ePoolAttributes;			///< Attributes for I/O memory. Here, specify the allocation type (AkMemType_Device, and so on). It is passed directly to AK::MemoryMgr::Malign().
	AkUInt32			uGranularity;				///< I/O requests granularity (typical bytes/request).
    AkThreadProperties	threadProperties;			///< Scheduler thread properties.
	AkReal32			fTargetAutoStmBufferLength;	///< Targetted automatic stream buffer length (ms). When a stream reaches that buffering, it stops being scheduled for I/O except if the scheduler is idle.
	AkUInt32			uMaxConcurrentIO;			///< Maximum number of transfers that can be sent simultaneously to the Low-Level I/O.
	bool				bUseStreamCache;			///< If true, the device attempts to reuse I/O buffers that have already been streamed from disk. This is particularly useful when streaming small looping sounds. However, there is a small increase in CPU usage when allocating memory, and a slightly larger memory footprint in the StreamManager pool. 													
	AkUInt32			uMaxCachePinnedBytes;		///< Maximum number of bytes that can be "pinned" using AK::SoundEngine::PinEventInStreamCache() or AK::IAkStreamMgr::PinFileInCache()
};

struct AkThreadProperties
{
    int                 nPriority;		///< Thread priority
	size_t				uStackSize;		///< Thread stack size
	int					uSchedPolicy;	///< Thread scheduling policy
	AkUInt32			dwAffinityMask;	///< Affinity mask
};

// Platform-specific APIs
