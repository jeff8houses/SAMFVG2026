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
#include "AkUnityErrorMessageTranslator.h"
#include "AK/AkPlatforms.h"
#include <AK/Tools/Common/AkBankReadHelpers.h>

bool AkUnityErrorMessageTranslator::GetInfo(TagInformation* in_pTagList, AkUInt32 in_uCount, AkUInt32& out_uTranslated)
{
#ifndef AK_OPTIMIZED
	for (AkUInt32 i = 0; i < in_uCount && out_uTranslated != in_uCount; i++)
	{
		if (!in_pTagList[i].m_infoIsParsed && *in_pTagList[i].m_pTag == 'g')
		{
			AkUInt64 gId = AK::ReadUnaligned<AkUInt64>((AkUInt8*)in_pTagList[i].m_args);
			//This tag will be parsed in C#, in AkCallbackManager.PostCallbacks()
			in_pTagList[i].m_len = AK_OSPRINTF(in_pTagList[i].m_parsedInfo, AK_TRANSLATOR_MAX_NAME_SIZE, AKTEXT("$g%llu"), (unsigned long long int)gId); 
			in_pTagList[i].m_infoIsParsed = true;
			out_uTranslated++;
		}
	}
	return out_uTranslated == in_uCount;
#else
	return false;
#endif
}
