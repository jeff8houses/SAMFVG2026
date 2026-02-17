#pragma once
#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include "AkArrayProxy.h"
namespace AK
{
	namespace SoundEngine
	{
		namespace DynamicSequence
		{
				/// List of items to play in a Dynamic Sequence.
			/// \sa
			/// - AK::SoundEngine::DynamicSequence::LockPlaylist
			/// - AK::SoundEngine::DynamicSequence::UnlockPlaylist
			class Playlist
				: public AkArray<PlaylistItem, const PlaylistItem&>
			{
			public:
				/// Enqueue an Audio Node.
				/// \return AK_Success if successful, AK_Fail otherwise
				 AKRESULT Enqueue( 
					AkUniqueID in_audioNodeID,		///< Unique ID of Audio Node
					AkTimeMs in_msDelay = 0,		///< Delay before playing this item, in milliseconds
					void * in_pCustomInfo = NULL,	///< Optional user data
					AkUInt32 in_cExternals = 0,					///< Optional count of external source structures
					AkExternalSourceInfo *in_pExternalSources = NULL///< Optional array of external source resolution information
					)
				{
					PlaylistItem * pItem = AddLast();
					if ( !pItem )
						return AK_InsufficientMemory;

					pItem->audioNodeID = in_audioNodeID;
					pItem->msDelay = in_msDelay;
					pItem->pCustomInfo = in_pCustomInfo;
					return pItem->SetExternalSources(in_cExternals, in_pExternalSources);
				}
			};

			// \deprecated Use AkDynamicSequenceType in the global scope.
			using DynamicSequenceType = ::AkDynamicSequenceType;
			const AkDynamicSequenceType DynamicSequenceType_SampleAccurate = AkDynamicSequenceType_SampleAccurate;
			const AkDynamicSequenceType DynamicSequenceType_NormalTransition = AkDynamicSequenceType_NormalTransition;
			const AkDynamicSequenceType DynamicSequenceType_Last = AkDynamicSequenceType_Last;

			/// Open a new Dynamic Sequence.
	        /// \return Playing ID of the dynamic sequence, or AK_INVALID_PLAYING_ID in failure case and an error message in the debug console and Wwise Profiler
			/// 
			/// If \c AK_DynamicSequenceSelect bit is set in \c in_uFlags, the dynamic sequence uses the callback for determining the next item to play.
			/// If this bit is not set, then the dynamic sequence uses the Playlist to determine the next item to play.
			///
			/// \sa
			/// - AK::SoundEngine::DynamicSequence::DynamicSequenceType
			extern   AkPlayingID  Open  (
				AkGameObjectID		in_gameObjectID,			///< Associated game object ID
				AkUInt32			in_uFlags	   = 0,			///< Bitmask: see \ref AkCallbackType
				AkCallbackFunc		in_pfnCallback = NULL,		///< Callback function
				void* 				in_pCookie	   = NULL,		///< Callback cookie that will be sent to the callback function along with additional information;
				AkDynamicSequenceType in_eDynamicSequenceType = AkDynamicSequenceType_SampleAccurate ///< See : \ref AkDynamicSequenceType
				);
														
			/// Close specified Dynamic Sequence. The Dynamic Sequence will play until finished and then
			/// deallocate itself.
			/// \return 
			/// - AK_Success if the command was successfully queued
			/// - AK_PlayingIDNotFound if the playing ID does not match to any open Dynamic Sequence
			extern   AKRESULT  Close  (
				AkPlayingID in_playingID						///< AkPlayingID returned by DynamicSequence::Open
				);

			/// Play specified Dynamic Sequence.
			/// \return 
			/// - AK_Success if the command was successfully queued
			/// - AK_PlayingIDNotFound if the playing ID does not match to any open Dynamic Sequence
			extern   AKRESULT  Play  ( 
				AkPlayingID in_playingID,											///< AkPlayingID returned by DynamicSequence::Open
				AkTimeMs in_uTransitionDuration = 0,								///< Fade duration
				AkCurveInterpolation in_eFadeCurve = AkCurveInterpolation_Linear	///< Curve type to be used for the transition
				);

			/// Pause specified Dynamic Sequence. 
			/// To restart the sequence, call Resume.  The item paused will resume its playback, followed by the rest of the sequence.
			/// \return 
			/// - AK_Success if the command was successfully queued
			/// - AK_PlayingIDNotFound if the playing ID does not match to any open Dynamic Sequence
			extern   AKRESULT  Pause  ( 
				AkPlayingID in_playingID,											///< AkPlayingID returned by DynamicSequence::Open
				AkTimeMs in_uTransitionDuration = 0,								///< Fade duration
				AkCurveInterpolation in_eFadeCurve = AkCurveInterpolation_Linear	///< Curve type to be used for the transition
				);

			/// Resume specified Dynamic Sequence.
			/// \return 
			/// - AK_Success if the command was successfully queued
			/// - AK_PlayingIDNotFound if the playing ID does not match to any open Dynamic Sequence
			extern   AKRESULT  Resume  (
				AkPlayingID in_playingID,											///< AkPlayingID returned by DynamicSequence::Open
				AkTimeMs in_uTransitionDuration = 0,									///< Fade duration
				AkCurveInterpolation in_eFadeCurve = AkCurveInterpolation_Linear	///< Curve type to be used for the transition
				);

			/// Stop specified Dynamic Sequence immediately.  
			/// To restart the sequence, call Play. The sequence will restart with the item that was in the 
			/// playlist after the item that was stopped.
			/// \return 
			/// - AK_Success if the command was successfully queued
			/// - AK_PlayingIDNotFound if the playing ID does not match to any open Dynamic Sequence
			extern   AKRESULT  Stop  (
				AkPlayingID in_playingID,											///< AkPlayingID returned by DynamicSequence::Open
				AkTimeMs in_uTransitionDuration = 0,								///< Fade duration
				AkCurveInterpolation in_eFadeCurve = AkCurveInterpolation_Linear	///< Curve type to be used for the transition
				);

			/// Break specified Dynamic Sequence.  The sequence will stop after the current item.
			/// \return 
			/// - AK_Success if the command was successfully queued
			/// - AK_PlayingIDNotFound if the playing ID does not match to any open Dynamic Sequence
			extern   AKRESULT  Break  (
				AkPlayingID in_playingID						///< AkPlayingID returned by DynamicSequence::Open
				);

			/// Seek inside specified Dynamic Sequence.
			/// It is only possible to seek in the first item of the sequence.
			/// If you seek past the duration of the first item, it will be skipped and an error will reported in the Capture Log and debug output. 
			/// All the other items in the sequence will continue to play normally.
			/// \return 
			/// - AK_Success if the command was successfully queued
			/// - AK_PlayingIDNotFound if the playing ID does not match to any open Dynamic Sequence
			extern   AKRESULT  Seek  (
				AkPlayingID in_playingID,						///< AkPlayingID returned by DynamicSequence::Open
				AkTimeMs in_iPosition,							///< Position into the the sound, in milliseconds
				bool in_bSeekToNearestMarker					///< Snap to the marker nearest to the seek position.
				);

			/// Seek inside specified Dynamic Sequence.
			/// It is only possible to seek in the first item of the sequence.
			/// If you seek past the duration of the first item, it will be skipped and an error will reported in the Capture Log and debug output.
			/// All the other items in the sequence will continue to play normally.
			/// \return 
			/// - AK_Success if the command was successfully queued
			/// - AK_PlayingIDNotFound if the playing ID does not match to any open Dynamic Sequence
			extern  AKRESULT  Seek (
				AkPlayingID in_playingID,						///< AkPlayingID returned by DynamicSequence::Open
				AkReal32 in_fPercent,							///< Position into the the sound, in percentage of the whole duration.
				bool in_bSeekToNearestMarker					///< Snap to the marker nearest to the seek position.
				);

			/// Get pause times.
			/// \return 
			/// - AK_Success if successful
			/// - AK_PlayingIDNotFound if the playing ID does not match to any open Dynamic Sequence
			extern  AKRESULT  GetPauseTimes (
				AkPlayingID in_playingID,						///< AkPlayingID returned by DynamicSequence::Open
				AkUInt32 &out_uTime,							///< If sequence is currently paused, returns time when pause started, else 0.
				AkUInt32 &out_uDuration							///< Returns total pause duration since last call to GetPauseTimes, excluding the time elapsed in the current pause.
				);

			/// Get currently playing item. Note that this may be different from the currently heard item
			/// when sequence is in sample-accurate mode.
			/// \return 
			/// - AK_Success if successful
			/// - AK_PlayingIDNotFound if the playing ID does not match to any open Dynamic Sequence
			extern  AKRESULT  GetPlayingItem (
				AkPlayingID in_playingID,						///< AkPlayingID returned by DynamicSequence::Open
				AkUniqueID & out_audioNodeID, 					///< Returned audio node ID of playing item.
				void *& out_pCustomInfo							///< Returned user data of playing item.
				);

			/// Lock the Playlist for editing. Needs a corresponding UnlockPlaylist call.
			/// \return Pointer to locked Playlist if successful, NULL otherwise (in_playingID not found)
			/// 
			/// \akwarning 
			/// When opening a dynamic sequence with the callback flag \c AK_DynamicSequenceSelect, the callback is the ONLY way to determine the next item to play.
			/// \c AK::SoundEngine::DynamicSequence::LockPlaylist always returns \c NULL for dynamic sequences opened with \c AK_DynamicSequenceSelect.
			/// \endakwarning
			/// 
			/// \sa
			/// - AK::SoundEngine::DynamicSequence::UnlockPlaylist
			extern   Playlist *  LockPlaylist  (
				AkPlayingID in_playingID						///< AkPlayingID returned by DynamicSequence::Open
				);

			/// Unlock the playlist.
			/// \return 
			/// - AK_Success if successful
			/// - AK_PlayingIDNotFound if the playing ID does not match to any open Dynamic Sequence
			/// \sa
			/// - AK::SoundEngine::DynamicSequence::LockPlaylist
			extern   AKRESULT  UnlockPlaylist  (
				AkPlayingID in_playingID						///< AkPlayingID returned by DynamicSequence::Open
				);
		}
	}
}