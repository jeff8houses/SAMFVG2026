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
#ifdef SWIG
// Temporarily disable global exception handling until re-enabled
#define PAUSE_SWIG_EXCEPTIONS %exception {$action}

#define CANCEL_SWIG_EXCEPTIONS(FUNCTION) %exception FUNCTION {$action}

// Enable SWIG exception handling
#define RESUME_SWIG_EXCEPTIONS %exception \
{\
	if (AK::SoundEngine::IsInitialized()) {\
		$action \
	} else {\
		AKPLATFORM::OutputDebugMsg("Wwise warning in $decl: AkInitializer.cs Awake() was not executed yet. Set the Script Execution Order properly so the current call is executed after.");\
		return $null;\
	}\
}

#define SWIG_EXCEPTION(FUNCTION) %exception FUNCTION \
{\
	if (AK::SoundEngine::IsInitialized()) {\
		$action \
	} else {\
		AKPLATFORM::OutputDebugMsg("Wwise warning in $decl: AkInitializer.cs Awake() was not executed yet. Set the Script Execution Order properly so the current call is executed after.");\
		return $null;\
	}\
}

#else
	#define PAUSE_SWIG_EXCEPTIONS
	#define RESUME_SWIG_EXCEPTIONS
#endif // #ifdef SWIG
