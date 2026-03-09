# Windows Wwise Unity Build module
import BuildUtil
import BuildWwiseUnityIntegration
import GenerateApiBinding
from os import path
from BuildWwiseUnityIntegration import MultiArchBuilder
from GenerateApiBinding import SwigCommand
from PrepareSwigInput import SwigPlatformHandler, SwigApiHeaderBlobber, PlatformStructProfile

class WindowsBuilder(MultiArchBuilder):
	def __init__(self, platformName, arches, configs, generateSwig, skipPremake, **kwargs):
		MultiArchBuilder.__init__(self, platformName, arches, configs, generateSwig, skipPremake, **kwargs)

		self._FindMSBuildPath("[17.0,18.0)")

		pathMan = BuildUtil.PathManager(self.platformName)
		self.solution = path.join(pathMan.Paths['Src_Platform'], '{}{}.sln'.format(pathMan.ProductName, self.platformName))

	def _CreateCommands(self, arch=None, config='Profile'):
		cmds = []

		for t in self.tasks:
			cmd = ['{}'.format(self.compiler), '{}'.format(self.solution), '/t:{}'.format(t), '/p:Configuration={}'.format(config), '/p:Platform={}'.format(arch)]
			cmds.append(cmd)

		return cmds

class Win32SwigCommand(SwigCommand):
	def __init__(self, pathMan):
		SwigCommand.__init__(self, pathMan)
		self.wordSize = 32

		self.platformDefines += [
			'-DWIN32',
			'-D_WIN32',
			'-DAK_WIN',
			'-DAK_MICROSOFT',
			'-DAKTYPESPATH=<AK/SoundEngine/Platforms/Windows/AkTypes.h>'
			]

class X64SwigCommand(SwigCommand):
	def __init__(self, pathMan):
		SwigCommand.__init__(self, pathMan)

		self.platformDefines = [
			'-DWIN64',
			'-D_WIN64',
			'-DAK_WIN',
			'-DAK_MICROSOFT',
			'-DAKTYPESPATH=<AK/SoundEngine/Platforms/Windows/AkTypes.h>'
			]

class SwigApiHeaderBlobberWindows(SwigApiHeaderBlobber):
	def __init__(self, pathMan):
		SwigApiHeaderBlobber.__init__(self, pathMan)

		self.inputHeaders.append(path.normpath(path.join(self.SdkIncludeDir, 'AK/SoundEngine/Platforms/Windows/AkWinSoundEngine.h')))

class SwigPlatformHandlerWindows (SwigPlatformHandler):
	def __init__(self, pathMan):
		SwigPlatformHandler.__init__(self, pathMan)

		ThreadPropertyHeader = 'AK/Tools/Win32/AkPlatformFuncs.h'
		self.PlatformStructProfiles += \
		[
			PlatformStructProfile(self.pathMan, ThreadPropertyHeader, SwigPlatformHandler.ThreadPropertiesRegEx)
		]

		self.ioFileSources = \
		[
			path.join(self.pathMan.Paths['Wwise_SDK_Samples'], path.normpath('SoundEngine/Win32/stdafx.cpp')),
			path.join(self.pathMan.Paths['Wwise_SDK_Samples'], path.normpath('SoundEngine/Win32/stdafx.h')),
			path.join(self.pathMan.Paths['Wwise_SDK_Samples'], path.normpath('SoundEngine/Win32/AkFileHelpers.h')),
			path.join(self.pathMan.Paths['Wwise_SDK_Samples'], path.normpath('SoundEngine/Win32/AkDefaultIOHookDeferred.cpp')),
			path.join(self.pathMan.Paths['Wwise_SDK_Samples'], path.normpath('SoundEngine/Win32/AkDefaultIOHookDeferred.h')),
		]

def Init(argv=None):
	BuildUtil.BankPlatforms['Windows'] = 'Windows'
	BuildUtil.SupportedArches['Windows'] = ['Win32', 'x64']
	BuildUtil.PremakeParameters['Windows'] = { 'os': 'windows', 'generator': 'vs2022' }
	BuildUtil.PlatformSwitches['Windows'] = '#if (UNITY_STANDALONE_WIN && !UNITY_EDITOR) || UNITY_EDITOR_WIN'
	BuildUtil.PlatformDependentFilenames.append('AkSoundQuality.cs')
	BuildUtil.PlatformDependentFilenames.append('AkSinkType.cs')
	BuildUtil.SupportedPlatforms['Windows'].append('Windows')

def CreatePlatformBuilder(platformName, arches, configs, generateSwig, skipPremake, isMultiSDK, **kwargs):
	return WindowsBuilder(platformName, arches, configs, generateSwig, skipPremake, **kwargs)

def CreateSwigCommand(pathMan, arch):
	if arch == 'Win32':
		return Win32SwigCommand(pathMan)
	elif arch == 'x64':
		return X64SwigCommand(pathMan)
	else:
		return None

def CreateSwigPlatformHandler(pathMan):
	return SwigPlatformHandlerWindows(pathMan)

def CreateSwigApiHeaderBlobber(pathMan):
	return SwigApiHeaderBlobberWindows(pathMan)

if __name__ == '__main__':
	pass
