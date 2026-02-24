-- Windows Unity premake module
local platform = {}

platform.akName = "Windows"

function platform.GetUnityPlatformName(platformName, cfgplatforms)
    return platformName, cfgplatforms
end

function platform.setupTargetAndLibDir(baseTargetDir)
    -- Remap Debug, Profile, Release to StaticCRT variants
    configmap {
        ["Debug"] = "Debug(StaticCRT)",
        ["Profile"] = "Profile(StaticCRT)",
        ["Release"] = "Release(StaticCRT)"
    }
    libdirs("$(WWISESDK)/$(Platform)" .. GetSuffixFromCurrentAction() .. "/$(Configuration)/lib")
    -- Point back to non-StaticCRT folders for the final output
    filter {"Debug*"}
        targetdir(baseTargetDir .. "Windows/%{cfg.architecture}/Debug")
    filter {"Profile*"}
        targetdir(baseTargetDir .. "Windows/%{cfg.architecture}/Profile")
    filter {"Release*"}
        targetdir(baseTargetDir .. "Windows/%{cfg.architecture}/Release")
    filter {}

end

function platform.platformSpecificConfiguration()
    links {
        "dxguid"
    }

    filter "Debug* or Profile*"
        links { "ws2_32" }
end

return platform