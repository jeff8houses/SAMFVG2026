-- Mac Unity premake module
local platform = {}

platform.akName = "Mac"

function platform.GetUnityPlatformName(platformName, cfgplatforms)
    return platformName, cfgplatforms
end

function platform.setupTargetAndLibDir(baseTargetDir)
    targetdir(baseTargetDir .. "Mac/%{cfg.buildcfg}")
    libdirs(wwiseSDKEnv .. "/Mac_Xcode1600/%{cfg.buildcfg}/lib")
end

function platform.platformSpecificConfiguration()
    links {
        "AudioToolbox.framework",
        "AudioUnit.framework",
        "AVFoundation.framework",
        "Foundation.framework",
        "CoreAudio.framework"
    }

    defines
    {
        "AK_PLATFORM_SPECIFIC_STUBS"
    }

    xcodebuildsettings {
		GENERATE_INFOPLIST_FILE = "YES",
		PRODUCT_BUNDLE_IDENTIFIER = "com.audiokinetic.wwiseunityintegration"
	}

end

return platform