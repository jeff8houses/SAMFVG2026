# The content of this file includes portions of the proprietary AUDIOKINETIC Wwise
# Technology released in source code form as part of the game integration package.
# The content of this file may not be used without valid licenses to the AUDIOKINETIC
# Wwise Technology.
# Note that the use of the game engine is subject to the Unity Terms of Service
# at https://unity3d.com/legal/terms-of-service.
#  
# License Usage
#  
# Licensees holding valid licenses to the AUDIOKINETIC Wwise Technology may use
# this file in accordance with the end user license agreement provided with the
# software or, alternatively, in accordance with the terms contained
# in a written agreement between you and Audiokinetic Inc.
# Copyright (c) <COPYRIGHTYEAR> Audiokinetic Inc.

import os, sys, platform, re, collections, subprocess
from os.path import join, exists
import BuildUtil
import PrepareSwigInput, PostprocessSwigOutput
from BuildInitializer import BuildInitializer

class SwigCommand(object):
	def __init__(self, pathMan):
		self.pathMan = pathMan

		self.platformDefines = []
		self.compilerDefines = []
		self.outputLanguage = ['-csharp']
		self.dllName = []
		self.Includes = []
		self.wordSize = 64 # Default to 64-bit. 32-bit platforms are expected to override this.

		self.logger = BuildUtil.CreateLogger(pathMan.Paths['Log'], __file__, self.__class__.__name__)

	def CreatePlatformCommand(self):
		return BuildUtil.BuildModules[self.pathMan.PlatformName].CreateSwigCommand(self.pathMan, self.pathMan.Arch)

	def GetSwigPathAbs(self):
		'''Find swig executable and return its absolute path'''
		if sys.platform in ('darwin', 'linux'):
			cmd_result = subprocess.run('which swig', capture_output=True, text=True, shell=True)
			if cmd_result.returncode != 0 or not cmd_result.stdout.strip():
				raise RuntimeError('SWIG not found, please install it and add it to PATH')
			return cmd_result.stdout.strip()
		elif sys.platform == 'win32':
			cmd_result = subprocess.run('where swig', capture_output=True, text=True, shell=True)
			if cmd_result.returncode == 0 and cmd_result.stdout.strip():
				return cmd_result.stdout.strip().split('\n')[0]
			elif exists('c:\\swig\\swig.exe'):
				return 'c:\\swig\\swig.exe'
			else:
				raise RuntimeError('SWIG not found, please install it and add it to PATH')
		else:
			raise RuntimeError('Unsupported platform: {}'.format(sys.platform))

	def Run(self):
		swig_exe = self.GetSwigPathAbs()

		# Get swig library path
		cmd_result = subprocess.run([swig_exe, '-swiglib'], capture_output=True, text=True)
		if cmd_result.returncode != 0:
			raise RuntimeError('SWIG not found, please install it and add it to PATH')
		swig_lib_path = cmd_result.stdout.strip()
		swig_csharp_module = os.path.join(swig_lib_path, 'csharp')
		swig_wchar_module = os.path.join(swig_csharp_module, 'wchar.i')

		exe = [swig_exe]

		output = ['-o', self.pathMan.Paths['SWIG_OutputCWrapper']]
		includes = ['-I{}'.format(self.pathMan.Paths['SWIG_WwiseSdkInclude']), '-I{}'.format(self.pathMan.Paths['SWIG_PlatformInclude']), '-I{}'.format(swig_csharp_module)] + self.Includes
		links = ['-l{}'.format(swig_wchar_module)]
		excludeExceptionHandling = ['-DSWIG_CSHARP_NO_EXCEPTION_HELPER', '-DSWIG_CSHARP_NO_WSTRING_HELPER', '-DSWIG_CSHARP_NO_STRING_HELPER', '-DSWIG_CSHARP_NO_WSTRING_EXCEPTION_HELPER']
		outdir = ['-outdir', self.pathMan.Paths['SWIG_OutputApiDir']]
		inputLanguage = ['-c++']
		interface = [self.pathMan.Paths['SWIG_Interface']]
		documentation = ['-doxygen']

		if not exists(self.pathMan.Paths['SWIG_OutputApiDir']):
			self.logger.info("Output C# API directory does not exist, creating {}".format(self.pathMan.Paths['SWIG_OutputApiDir']))
			try:
				os.makedirs(self.pathMan.Paths['SWIG_OutputApiDir'])
			except:
				pass

		if self.wordSize == 64:
			self.platformDefines += ["-DAK_POINTER_64"]

		cmd = exe + documentation + output + includes + links + self.platformDefines + self.compilerDefines + excludeExceptionHandling + self.dllName + outdir + inputLanguage + self.outputLanguage +  interface

		self.logger.debug(' '.join(cmd))

		try:
			(stdOut, stdErr, returnCode) = BuildUtil.RunCommand(cmd)
			self.logger.debug('stdout:\n{}'.format(stdOut))
			self.logger.debug('stderr:\n{}'.format(stdErr))
			isCmdFailed = returnCode != 0
			if isCmdFailed:
				msg = 'SWIG failed with return code: {}, Command: {}\nSWIG Command Error: {}'.format(returnCode, ' '.join(cmd), stdErr)
				raise RuntimeError(msg)
		except Exception as err:
			# Note: logger.exception() is buggy in py3.
			self.logger.error(err)
			raise RuntimeError(err)

class GccSwigCommand(SwigCommand):
	def __init__(self, pathMan):
		SwigCommand.__init__(self, pathMan)
		self.compilerDefines += ['-D__GNUC__=5', '-DGCC_HASCLASSVISIBILITY']

class AppleSwigCommand(GccSwigCommand):
	def __init__(self, pathMan):
		GccSwigCommand.__init__(self, pathMan)
		self.platformDefines = [
			'-D__APPLE__',
			'-DAK_APPLE',
			'-DAK_NEED_POSIX',
			'-DHAS_PRE_AK_TYPES_BINDING',
			'-DAKTYPESPATH=<AK/SoundEngine/Platforms/Mac/AkTypes.h>'
			]

		self.Includes += ["-I{}".format(os.path.join(pathMan.Paths['Src'], "Mac"))]

		# Sorted in subclasses. Descending versions.
		self.SdkCompatibilityNodes = collections.OrderedDict()

	def _InitPlatformSdkInfo(self):
		# For UnitTest that runs outside of Xcode on Mac OS X Mountain Lion
		SdkRoot = None

		# Parse versions
		pattern = re.compile(r'{}(?P<version>[0-9]+\.[0-9]+)\.sdk'.format(self.platformSdkName))

		isSdkRootEnvVarExist = 'SDKROOT' in os.environ and os.environ['SDKROOT'] != ''
		if isSdkRootEnvVarExist:
			SdkRoot = os.environ['SDKROOT']
			foundSdks = [os.path.split(SdkRoot)[1]]
			latestVersion = None
			for i, s in enumerate(foundSdks):
				match = pattern.search(s)
				if not match is None:
					latestVersion = match.groupdict()['version']
					break
		else: # search first valid Mac SDK
			self.logger.debug('Detected: Running in command line outside Xcode IDE environment. Proceed to find SDKROOT ourselves.')

			XcodeDeveloperDir = None
			cmd = ['/usr/bin/xcode-select', '--print-path']
			self.logger.debug(' '.join(cmd))
			try:
				(stdOut, stdErr, returnCode) = BuildUtil.RunCommand(cmd)
				outMsgs = stdOut.split(BuildUtil.SpecialChars['PosixLineEnd'])
				errMsgs = stdErr.split(BuildUtil.SpecialChars['PosixLineEnd'])
				for o in outMsgs:
					self.logger.debug('stdout: {}'.format(o))
				for e in errMsgs:
					self.logger.debug('stderr: {}'.format(e))
				isCmdFailed = returnCode != 0
				if isCmdFailed:
					msg = 'Command failed with return code: {}, Command: {}'.format(returnCode, ' '.join(cmd))
					raise RuntimeError(msg)
			except Exception as err:
				# Note: logger.exception() is buggy in py3.
				self.logger.error(err)
				raise RuntimeError(err)
			XcodeDeveloperDir = outMsgs[3]

			# Find latest SDK in SDKROOT folder.
			latestSdkRoot = None
			sdkRootParentDir = '{}/Platforms/{}.platform/Developer/SDKs'.format(XcodeDeveloperDir, self.platformSdkName)
			foundSdks = os.listdir(sdkRootParentDir)
			latestVersion = None
			for i, s in enumerate(foundSdks):
				match = pattern.search(s)
				if not match is None:
					latestVersion = match.groupdict()['version']
					break
			if latestVersion is None:
				msg = 'Failed to find platform SDK for: {}. Aborted.'.format(self.platformSdkName)
				self.logger.error(msg)
				raise RuntimeError(msg)
			latestSdkRoot = join(sdkRootParentDir, '{}{}.sdk'.format(self.platformSdkName, latestVersion))
			SdkRoot = latestSdkRoot

		# Compare latest version with nodes (descending versions).
		headerSwitch = None
		for verNode in self.SdkCompatibilityNodes.keys():
			isGreaterThanNodeVersion = float(latestVersion)+0.00001 > float(verNode)
			if isGreaterThanNodeVersion:
				headerSwitch = self.SdkCompatibilityNodes[verNode]
		if not headerSwitch is None:
			self.platformDefines.append(headerSwitch)

		self.Includes += ['-I{}'.format(join(SdkRoot, 'usr', 'include'))]

def main():
	pathMan = BuildUtil.Init()
	logger = BuildUtil.CreateLogger(pathMan.Paths['Log'], __file__, main.__name__)

	logger.info('Using WWISESDK: {}'.format(pathMan.Paths['Wwise_SDK']))
	logger.info('Generating SWIG binding [{}] [{}]'.format(pathMan.PlatformName, pathMan.Arch))
	logger.info('Remove existing API bindings to avoid conflicts.')
	initer = BuildInitializer(pathMan)
	initer.Initialize()

	logger.info('Prepare SDK header blob for generating API bindings, and platform source code for building plugin.')
	PrepareSwigInput.main(pathMan)

	logger.info('Generate API binding for Wwise SDK.')
	swigCmd = SwigCommand(pathMan).CreatePlatformCommand()
	swigCmd.Run()

	logger.info('Postprocess generated API bindings to make platform and architecture switchable and make iOS API work in Unity.')
	PostprocessSwigOutput.main(pathMan)

	logger.info('SUCCESS. API binding generated under {}.'.format(pathMan.Paths['Deploy_API_Generated']))

if __name__ == '__main__':
	main()
