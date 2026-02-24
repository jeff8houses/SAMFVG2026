newoption
{
	trigger     = "wwiseroot",
	value       = "PATH",
	description = "Use this path as the root for Wwise folder. Will set up WwiseSDK and disallow using the environment variable."
}


if _OPTIONS["wwiseroot"] then
	wwiseRoot = _OPTIONS["wwiseroot"]
	wwiseSDK = path.getabsolute(wwiseRoot .. "/SDK")
	wwiseSDKEnv = wwiseSDK
	print("Hard-coding wwiseSDK path to: " .. wwiseSDK)
else
	wwiseSDK = path.getabsolute(os.getenv("WWISESDK"))
	wwiseSDKEnv = "$(WWISESDK)"
	print("Using default wwiseSDK environment variable: " .. wwiseSDK)
end


package.path = package.path .. ";" .. path.getabsolute(wwiseSDK .. "/source/Build") .. "/?.lua" .. ';'.. path.getabsolute(wwiseSDK .. "/../Scripts/Build") .. "/?.lua"

require "PremakePlatforms"
require "Premake5Helper"
require "Premake5PathHelpers"

_AK_ROOT_DIR = path.getabsolute(wwiseSDK .. "/source/Build") .. "/"
_AK_TARGETS = {}

local unityPlatforms = {}
for index, folder in ipairs(os.matchdirs("../*")) do
	local platformModule = path.getabsolute(path.join(folder, "platformPremake.lua"))
	if os.isfile(platformModule) then
		unityPlatform = require(path.replaceextension(platformModule, ""))
		unityPlatforms[path.getbasename(folder)] = unityPlatform
		if(path.getbasename(folder) ~= unityPlatform.akName) then
			unityPlatforms[unityPlatform.akName] = unityPlatform
		end

	end
end

local platform = AK.Platform

function SoundEngineInternals(suffix, communication, autobahn)
	local d = {
		"AkSpatialAudio"..suffix,
		"AkMemoryMgr"..suffix,
		"AkStreamMgr"..suffix,
	}

	if communication then
		d[#d+1] = "CommunicationCentral"..suffix	
		if autobahn then
			d[#d+1] = "AkAutobahn"..suffix
		end
	end

	d[#d+1] = "AkSoundEngine"..suffix
	d[#d+1] = "AkSilenceSource"..suffix
	d[#d+1] = "AkSineSource"..suffix
	d[#d+1] = "AkToneSource"..suffix
	d[#d+1] = "AkSynthOneSource"..suffix
	d[#d+1] = "AkVorbisDecoder"..suffix
	d[#d+1] = "AkOpusDecoder"..suffix
	d[#d+1] = "AkAudioInputSource"..suffix

	return d
end

function Dependencies(suffix, communication, autobahn)
	local d = SoundEngineInternals(suffix, communication, autobahn)

	d[#d+1] = "Ak3DAudioBedMixerFX"..suffix
	d[#d+1] = "AkCompressorFX"..suffix
	d[#d+1] = "AkDelayFX"..suffix
	d[#d+1] = "AkFlangerFX"..suffix
	d[#d+1] = "AkGainFX"..suffix
	d[#d+1] = "AkGuitarDistortionFX"..suffix
	d[#d+1] = "AkHarmonizerFX"..suffix
	d[#d+1] = "AkMatrixReverbFX"..suffix
	d[#d+1] = "AkParametricEQFX"..suffix
	d[#d+1] = "AkPeakLimiterFX"..suffix
	d[#d+1] = "AkPitchShifterFX"..suffix
	d[#d+1] = "AkRecorderFX"..suffix
	d[#d+1] = "AkReflectFX"..suffix
	d[#d+1] = "AkRoomVerbFX"..suffix
	d[#d+1] = "AkStereoDelayFX"..suffix
	d[#d+1] = "AkTimeStretchFX"..suffix
	d[#d+1] = "AkTremoloFX"..suffix

	return d
end

function SetupTargetAndLibDir(platformName)
	if platformName == "Mac" or platformName == "iOS" or platformName == "tvOS" or platformName == "visionOS" then
		xcodebuildsettings {
			WWISESDK = wwiseSDKEnv
		}
	end

	local baseTargetDir = "../../Integration/Assets/Wwise/API/Runtime/Plugins/"

	targetdir(baseTargetDir .. platformName .. "/%{cfg.architecture}/%{cfg.buildcfg}")
	print("SetupTargetAndLibDir " .. platformName)
	if unityPlatforms[platformName] ~= nil then
		unityPlatforms[platformName].setupTargetAndLibDir(baseTargetDir)
	else
		libdirs(wwiseSDKEnv .. "/%{cfg.platform}_%{cfg.architecture}/%{cfg.buildcfg}/lib")
	end
end

function GetUnityPlatformName(platformName, cfgplatforms)
	if unityPlatforms[platformName] ~= nil then
		return unityPlatforms[platformName].GetUnityPlatformName(platformName, cfgplatforms)
	else
		return platformName, cfgplatforms
	end
end

function FixupPropsheets(prj)
	-- Need to set the propsheets that way because the Wwise premake scripts insert a relative path.
	-- Since the Wwise SDK can be installed anywhere on a customer's machine, we need to use the
	-- environment variable instead. Simply using vs_propsheet() does not work, because it appends 
	-- to the internal array. We need to actually get to the internal array, and set it to what we
	-- need.
	for cfg in premake.project.eachconfig(prj) do
		for i, ps in ipairs(cfg.vs_propsheet) do
			cfg.vs_propsheet[i] = string.gsub(ps, wwiseSDK, "$(WWISESDK)")
		end
	end
end

function CreateCommonProject(platformName, suffix)
	-- A project defines one build target
	local pfFolder = "../"..platformName.."/";
	local libType = "SharedLib"
	if unityPlatforms[platformName] ~= nil and unityPlatforms[platformName].isStatic then
		libType = "StaticLib"
	end

	local prj = AK.Platform.CreateProject("AkUnitySoundEngine" .. platformName, "AkUnitySoundEngine" .. platformName, pfFolder, GetSuffixFromCurrentAction(), nil, libType)
		targetname "AkUnitySoundEngine"
		-- Files must be explicitly specified (no wildcards) because our build system calls premake before copying the sample files
		files {
			"../Common/AkCallbackSerializer.cpp",
			pfFolder .. "AkDefaultIOHookDeferred.cpp",
			"../Common/AkFileLocationBase.cpp",
			"../Common/AkMultipleFileLocation.cpp",
			"../Common/AkGeneratedSoundBanksResolver.cpp",
			"../Common/AkFilePackage.cpp",
			"../Common/AkFilePackageLUT.cpp",
			"../Common/AkUnitySoundEngineStubs.cpp",
			"../Common/AkUnityErrorMessageTranslator.cpp",
			pfFolder .. "SoundEngine_wrap.cxx",
			pfFolder .. "stdafx.cpp",
		}

		defines {
			"AUDIOKINETIC",
		}

		includedirs {
			"../"..platformName,
			wwiseSDKEnv .. "/include",
			wwiseSDKEnv .. "/tools",
			"."
		}

		if platformName == "Mac" then
			sharedlibtype "OSXBundle"
		end

		local bUseAutobahn = false
		if platformName == "Mac" or platformName == "Windows" then
			bUseAutobahn = true;
		end

		SetupTargetAndLibDir(platformName)
		
		filter "*Debug*"
			defines { "_DEBUG" }
			links(Dependencies(suffix, true, bUseAutobahn))

		filter "*Profile*"
			defines { "NDEBUG" }
			optimize "On"
			links(Dependencies(suffix, true, bUseAutobahn))

		filter "*Release*"
			defines { "NDEBUG", "AK_OPTIMIZED" }
			optimize "On"
			links(Dependencies(suffix, false, bUseAutobahn))

		filter {}
		
		if unityPlatforms[platformName] ~= nil then
			unityPlatforms[platformName].platformSpecificConfiguration()
		end

		if wwiseSDKEnv ~= wwiseSDK then
			FixupPropsheets(prj)
		end
	return prj
end

-- This is the main() of the script
if table.find(platform.validActions, _ACTION) then
	local unityPlatformName, cfgplatforms = GetUnityPlatformName(platform.name, AK.Platform.platforms)

	if _ACTION == "cmake" or _ACTION == "xcode4" then
		wwiseSDKEnv = "${WWISESDK}"
	end
	
	solution("AkUnitySoundEngine" .. unityPlatformName)
	if unityPlatformName == "Windows" then
		configurations { "Debug", "Profile", "Release", "Debug(StaticCRT)", "Profile(StaticCRT)", "Release(StaticCRT)" }
	else
		configurations { "Debug", "Profile", "Release" }
	end

	location("../" .. unityPlatformName)

	if type(cfgplatforms) == 'function' then
		cfgplatforms()
	else
		platforms(cfgplatforms)

		-- mlarouche: Workaround AK.Platforms not specifying platforms and architecture for Linux
		if unityPlatformName == "Linux" then
			platforms { "x64" }

			filter "platforms:x64"
				architecture "x86_64"
		end
	end
	
	CreateCommonProject(unityPlatformName, "")
else
	if _ACTION ~= nil then
		premake.error(string.format("No valid action specified for platform %s, valid actions are: %s", platform.name, table.concat(platform.validActions, ",")))
	end
end
