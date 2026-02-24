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

#ifndef AK_SOUNDENGINE_STUBS_H_
#define AK_SOUNDENGINE_STUBS_H_

#if defined AK_WIN
#define AK_STDCALL __stdcall
#else
#define AK_STDCALL
#endif

typedef void (AK_STDCALL * AkErrorLogger)(const char*);
void SetErrorLogger(AkErrorLogger logger = NULL);

typedef void (AK_STDCALL * AkGetAudioFormat)(AkPlayingID playingID, AkAudioFormat* format);
typedef bool (AK_STDCALL * AkGetAudioSamples)(AkPlayingID playingID, AkSampleType* samples, AkUInt32 channelIndex, AkUInt32 frames);
void SetAudioInputCallbacks(AkGetAudioSamples getAudioSamples, AkGetAudioFormat getAudioFormat);

struct AkCommunicationSettings
{
	AkCommunicationSettings();

	AkUInt32 uPoolSize;
	AkUInt16 uDiscoveryBroadcastPort;
	AkUInt16 uCommandPort;

	/// Allows selecting the communication system used to connect remotely the Authoring tool on the device.
	enum AkCommSystem
	{
		AkCommSystem_Socket,	/// The recommended default communication system
		AkCommSystem_HTCS 		/// HTCS when available only, will default to AkCommSystem_Socket if the HTCS system is not available.
	};
	AkCommSystem commSystem;

	bool bInitSystemLib;

	char szAppNetworkName[64];
};

struct AkInitializationSettings
{
	AkInitializationSettings();
	~AkInitializationSettings();

	AkStreamMgrSettings streamMgrSettings;
	AkDeviceSettings deviceSettings;
	AkInitSettings initSettings;
	AkPlatformInitSettings platformSettings;

	//From AkMemSettings (not exposed as it is mostly function pointers)
    AkUInt32 uMemoryPrimarySbaInitSize;
    AkUInt32 uMemoryPrimaryTlsfInitSize;
    AkUInt32 uMemoryPrimaryTlsfSpanSize;
    AkUInt32 uMemoryPrimaryAllocSizeHuge;
    AkUInt32 uMemoryPrimaryReservedLimit;

    AkUInt32 uMemoryMediaTlsfInitSize;
    AkUInt32 uMemoryMediaTlsfSpanSize;
    AkUInt32 uMemoryMediaAllocSizeHuge;
    AkUInt32 uMemoryMediaReservedLimit;

	AkUInt32 uMemDebugLevel;
	
	// For the IO Hook
	bool bUseSubFoldersForGeneratedFiles; // This is a configuration parameter we need to expose to the Unity UI, but it's not part of the Device Settings
};

#if SWIG
%extend AkSerializedExternalSourceInfo
{
	AkUInt32 iExternalSrcCookie;
	AkCodecID idCodec;
	const char* szFile;
	void* pInMemory;
	AkUInt32 uiMemorySize;
	AkFileID idFile;
}
#else
AkUInt32 AkSerializedExternalSourceInfo_iExternalSrcCookie_get(struct AkSerializedExternalSourceInfo *info);
void AkSerializedExternalSourceInfo_iExternalSrcCookie_set(struct AkSerializedExternalSourceInfo *info, AkUInt32 value);

AkCodecID AkSerializedExternalSourceInfo_idCodec_get(struct AkSerializedExternalSourceInfo *info);
void AkSerializedExternalSourceInfo_idCodec_set(struct AkSerializedExternalSourceInfo *info, AkCodecID value);

const char* AkSerializedExternalSourceInfo_szFile_get(struct AkSerializedExternalSourceInfo *info);
void AkSerializedExternalSourceInfo_szFile_set(struct AkSerializedExternalSourceInfo *info, const char* value);

void* AkSerializedExternalSourceInfo_pInMemory_get(struct AkSerializedExternalSourceInfo *info);
void AkSerializedExternalSourceInfo_pInMemory_set(struct AkSerializedExternalSourceInfo *info, void* value);

AkUInt32 AkSerializedExternalSourceInfo_uiMemorySize_get(struct AkSerializedExternalSourceInfo *info);
void AkSerializedExternalSourceInfo_uiMemorySize_set(struct AkSerializedExternalSourceInfo *info, AkUInt32 value);

AkFileID AkSerializedExternalSourceInfo_idFile_get(struct AkSerializedExternalSourceInfo *info);
void AkSerializedExternalSourceInfo_idFile_set(struct AkSerializedExternalSourceInfo *info, AkFileID value);
#endif // SWIG

struct AkSerializedExternalSourceInfo : AkExternalSourceInfo
{
	AkSerializedExternalSourceInfo() {}
	~AkSerializedExternalSourceInfo();

	AkSerializedExternalSourceInfo(void* in_pInMemory, AkUInt32 in_uiMemorySize, AkUInt32 in_iExternalSrcCookie, AkCodecID in_idCodec)
		: AkExternalSourceInfo(in_pInMemory, in_uiMemorySize, in_iExternalSrcCookie, in_idCodec) {}

	AkSerializedExternalSourceInfo(const char* in_pszFileName, AkUInt32 in_iExternalSrcCookie, AkCodecID in_idCodec);

	AkSerializedExternalSourceInfo(AkFileID in_idFile, AkUInt32 in_iExternalSrcCookie, AkCodecID in_idCodec)
		: AkExternalSourceInfo(in_idFile, in_iExternalSrcCookie, in_idCodec) {}

	void Clear(); //< Called at construction of the array class to wipe the memory as if this object were default constructed
	void Clone(const AkSerializedExternalSourceInfo& other);

	static int GetSizeOf() { return sizeof(AkSerializedExternalSourceInfo); }
};

#if defined AK_ANDROID
	void SetAndroidActivity(void* activity);
#endif
	AKRESULT Init(AkInitializationSettings* settings);
	AKRESULT InitSpatialAudio(AkSpatialAudioInitSettings* settings);
	AKRESULT InitCommunication(AkCommunicationSettings* settings);

	void Term();

	AKRESULT RegisterGameObjInternal(AkGameObjectID in_GameObj);
	AKRESULT UnregisterGameObjInternal(AkGameObjectID in_GameObj);

	AKRESULT RegisterGameObjInternal_WithName(AkGameObjectID in_GameObj, const char* in_pszObjName);
	AKRESULT RegisterGameObjInternal_WithName(AkGameObjectID in_GameObj, const char* in_pszObjName);
	AKRESULT SetBasePath(const AkOSChar* in_pszBasePath);
	AKRESULT SetCurrentLanguage(const AkOSChar* in_pszAudioSrcPath);
	AKRESULT LoadFilePackage(const AkOSChar* in_pszFilePackageName, AkUInt32 & out_uPackageID);
	AKRESULT AddBasePath(const AkOSChar* in_pszBasePath);
	AKRESULT SetGameName(const AkOSChar* in_GameName);
	AKRESULT SetDecodedBankPath(const AkOSChar* in_DecodedPath);
	AKRESULT LoadAndDecodeBank(const AkOSChar* in_pszString, bool in_bSaveDecodedBank, AkBankID& out_bankID);
	AKRESULT LoadAndDecodeBankFromMemory(void* in_BankData, AkUInt32 in_BankDataSize, bool in_bSaveDecodedBank, const AkOSChar* in_DecodedBankName, bool in_bIsLanguageSpecific, AkBankID& out_bankID);

	const AkOSChar* GetCurrentLanguage();
	AKRESULT UnloadFilePackage(AkUInt32 in_uPackageID);
	AKRESULT UnloadAllFilePackages();

	//Override for SetPosition to avoid filling a AkSoundPosition in C# and marshal that. 
	AKRESULT SetObjectPosition(AkGameObjectID in_GameObjectID, AkVector Pos, AkVector Front, AkVector Top);

	AKRESULT GetSourceMultiplePlayPositions(
		AkPlayingID		in_PlayingID,				///< Playing ID returned by AK::SoundEngine::PostEvent()
		AkUniqueID *	out_audioNodeID,			///< Audio Node IDs of sources associated with the specified playing ID. Indexes in this array match indexes in out_msTime.
		AkUniqueID *	out_mediaID,				///< Media ID of playing item. (corresponds to 'ID' attribute of 'File' element in SoundBank metadata file)
		AkTimeMs *		out_msTime,					///< Audio positions of sources associated with the specified playing ID. Indexes in this array match indexes in out_audioNodeID.
		AkUInt32 *		io_pcPositions,				///< Number of entries in out_puPositions. Needs to be set to the size of the array: it is adjusted to the actual number of returned entries
		bool			in_bExtrapolate = true		///< Position is extrapolated based on time elapsed since last sound engine update
		);

	AKRESULT SetListeners(AkGameObjectID in_emitterGameObj, AkGameObjectID* in_pListenerGameObjs, AkUInt32 in_uNumListeners);

	AKRESULT SetDefaultListeners(AkGameObjectID* in_pListenerObjs, AkUInt32 in_uNumListeners);

	AkUInt32 GetNumOutputDevices(AkUniqueID in_AudioDeviceShareSetID);
	AKRESULT AddOutput(const AkOutputSettings & in_Settings, AkOutputDeviceID *out_pDeviceID = NULL, AkGameObjectID* in_pListenerIDs = NULL, AkUInt32 in_uNumListeners = 0);

	void GetDefaultStreamSettings(AkStreamMgrSettings & out_settings);
	void GetDefaultDeviceSettings(AkDeviceSettings & out_settings);
	void GetDefaultInitSettings(AkInitSettings & out_settings);
	void GetDefaultPlatformInitSettings(AkPlatformInitSettings &out_settings);

	AkUInt32 GetMajorMinorVersion();
	AkUInt32 GetSubminorBuildVersion();

	void StartResourceMonitoring();
	void StopResourceMonitoring();
	void GetResourceMonitorDataSummary(AkResourceMonitorDataSummary& resourceMonitorDataSummary);

	void StartDeviceCapture(AkOutputDeviceID in_idOutputDeviceID);
	void StopDeviceCapture(AkOutputDeviceID in_idOutputDeviceID);
	void ClearCaptureData();
	AkUInt32 UpdateCaptureSampleCount(AkOutputDeviceID in_idOutputDeviceID);
	AkUInt32 GetCaptureSamples(AkOutputDeviceID in_idOutputDeviceID, AkReal32* out_pSamples, AkUInt32 in_uBufferSize);

	// When AkRoomID arguments were placed after the bool, the AkRoomID values were wrong on Android.
	AKRESULT SetRoomPortal(AkPortalID in_PortalID, AkRoomID FrontRoom, AkRoomID BackRoom, const AkTransform& Transform, const AkExtent& Extent, bool bEnabled, const char* in_pName);

	AKRESULT SetRoom(AkRoomID in_RoomID, AkRoomParams& in_roomParams, AkGeometryInstanceID GeometryInstanceID, const char* in_pName);

	AKRESULT RegisterSpatialAudioListener(AkGameObjectID in_gameObjectID);

	AKRESULT UnregisterSpatialAudioListener(AkGameObjectID in_gameObjectID);

	AKRESULT SetGeometry(AkGeometrySetID in_GeomSetID,
		AkTriangle* Triangles,
		AkUInt32 NumTriangles,
		AkVertex* Vertices,
		AkUInt32 NumVertices,
		AkAcousticSurface* Surfaces,
		AkUInt32 NumSurfaces,
		bool EnableDiffraction,
		bool EnableDiffractionOnBoundaryEdges);

	AKRESULT SetGeometryInstance(
		AkGeometryInstanceID in_GeomInstanceID,
		const AkTransform& Transform,
		const AkVector& Scale,
		AkGeometrySetID GeometrySetID,
		bool UseForReflectionAndDiffraction,
		bool BypassPortalSubtraction,
		bool Solid);

	AKRESULT QueryReflectionPaths(
		AkGameObjectID in_gameObjectID,
		AkUInt32 in_positionIndex,
		AkVector& out_listenerPos,
		AkVector& out_emitterPos,
		AkReflectionPathInfo* out_aPaths,
		AkUInt32& io_uArraySize);

	AKRESULT QueryDiffractionPaths(
		AkGameObjectID in_gameObjectID,
		AkUInt32 in_positionIndex,
		AkVector& out_listenerPos,
		AkVector& out_emitterPos,
		AkDiffractionPathInfo* out_aPaths,
		AkUInt32& io_uArraySize);
	
	void PerformStreamMgrIO();

#if defined(AK_XBOX)
	AkUInt32 GetWwiseDeviceId(unsigned char* in_appLocalDeviceId);
#endif


#endif //AK_SOUNDENGINE_STUBS_H_
