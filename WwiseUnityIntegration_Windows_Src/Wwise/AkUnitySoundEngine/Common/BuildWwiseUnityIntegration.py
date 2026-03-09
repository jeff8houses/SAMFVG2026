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

import subprocess, sys, os, os.path, argparse, platform, copy, time, shlex, shutil
from os.path import dirname, abspath, join, exists, normpath
from argparse import RawTextHelpFormatter, BooleanOptionalAction
import BuildUtil
from BuildUtil import BuildModules

ScriptDir = abspath(dirname(__file__))

class PlatformBuilder(object):
	def __init__(self, platformName, arches, configs, generateSwig, skipPremake, **kwargs):
		self.platformName = platformName
		self.sdkVersion = "default SDK version"
		self.compiler = None
		self.solution = None
		self.arches = arches
		self.configs = configs
		self.isMultiSDK = False
		self.toolchainVers = None
		self.toolchainScript = None
		self.wwiseSdkVerEnvVar = None
		self.cleanExceptions = None
		self.tasks = ['Clean', 'Build']
		self.pathMan = BuildUtil.PathManager(self.platformName)
		self.logger = BuildUtil.CreateLogger(self.pathMan.Paths['Log'], __file__, self.__class__.__name__)
		self.generateSwig = generateSwig
		self.skipPremake = skipPremake


	def Build(self):
		'''Prototype of single-architecture build.'''
		results = []

		self._DoGeneratePremake()
		self._DoGenerateSWIG(arch=None)

		for c in self.configs:
			self.logger.info('Building: {} ({}) ...'.format(self.platformName, c))
			cmds = self._CreateCommands(arch=None, config=c)
			isSuccess = self._RunCommands(cmds)
			if isSuccess:
				resultCode = 'Success'
			else:
				resultCode = 'Fail'
			result = {'resultCode':resultCode, 'Message': '{}({}, {})'.format(self.platformName, c, self.sdkVersion)}
			results.append(result)
		return results

		# Call the GenerateAPIBinding script if required
	def _DoGenerateSWIG(self, arch):
		if self.generateSwig:
			self._GenerateSWIGOutput(self.platformName, arch)

	def _DoGeneratePremake(self):
		if self.skipPremake:
			return
		premakeScriptPath = self.pathMan.Paths['Premake_Scripts']
		premakeToolPath = self.pathMan.Paths['Premake_exe']
		premakeParameters = BuildUtil.PremakeParameters[self.platformName]
		premakeFile = self.pathMan.Paths['Premake5_lua_file']

		cmd = [premakeToolPath, '--scripts={}'.format(premakeScriptPath), '--os={}'.format(premakeParameters['os']), '--file={}'.format(premakeFile), premakeParameters['generator']]

		self.logger.critical('Premake Command [{}]: '.format(self.platformName) + str(cmd))

		res = subprocess.Popen(cmd).wait()
		if res != 0:
			sys.exit(res)

	def _ValidateEnvVar(self, varName, entityName):
		if not varName in os.environ:
			msg = 'Undefined {} environment variable: {}'.format(entityName, varName)
			self.logger.error(msg)
			raise RuntimeError(msg)
		elif not exists(os.environ[varName]):
			msg = 'Failed to find {} with environment variable {} = {}'.format(entityName, varName, os.environ[varName])
			self.logger.error(msg)
			raise RuntimeError(msg)

	def _CreateCommands(self, arch=None, config='Profile'):
		msg = BuildUtil.Messages['NotImplemented']
		self.logger.error(msg)
		raise NotImplementedError()

	def _RunCommands(self, cmds):
		'''The atomic build task to evaluate is a list of commands run in order.'''
		isSuccess = True
		for cmd in cmds:
			try:
				(stdOut, stdErr, returnCode) = BuildUtil.RunCommand(cmd)
				self.logger.debug('stdout:\n{}'.format(stdOut))
				self.logger.debug('stderr:\n{}'.format(stdErr))
				isCmdFailed = returnCode != 0
				if isCmdFailed:
					msg = 'Command failed with return code: {}'.format(returnCode)
					raise RuntimeError(msg)
			except Exception as err:
				isSuccess = False
				self.logger.error('Build task failed: command: {}, on-screen command: {}, error: {}. Skipped.'.format(cmd, ' '.join(cmd), err))
		return isSuccess

	def _FindLatestDotNetVersionDir(self):
		dotNetDir = '{}\\Microsoft.NET\\Framework'.format(os.environ['SYSTEMROOT'])
		contents = os.listdir(dotNetDir)
		versionDirs = []
		for d in contents:
			p = join(dotNetDir, d)
			isVersionDir = os.path.isdir(p) and d.startswith('v') and d[1].isdigit()
			if isVersionDir:
				versionDirs.append(d)
		versionDirs.sort()
		return join(dotNetDir, versionDirs[len(versionDirs)-1])

	def _GenerateSWIGOutput(self, platformName, arch):
		'''Generate the SWIG API binding for a given platform, arch and config'''
		pathMan = BuildUtil.PathManager(platformName)
		python_exe = 'py -3'
		if sys.platform == 'darwin' or sys.platform == 'linux':
				python_exe = 'python3'

		if(arch == None):
			self.logger.critical('Generating SWIG binding [{}]'.format(platformName))
			cmdToRunNow = '{} {} {}'.format(python_exe, pathMan.Paths['Src_Script_GenApi'] ,platformName)
		else:
			self.logger.critical('Generating SWIG binding [{}] [{}]'.format(platformName, arch))
			cmdToRunNow = '{} {} -a {} {}'.format(python_exe, pathMan.Paths['Src_Script_GenApi'], arch ,platformName)
		self.logger.critical('Command is ' + cmdToRunNow )
		os.system(cmdToRunNow)

	def _FindMSBuildPath(self, versionFilter="[16.0,17.0)"):
		vswherePath = os.path.expandvars("%ProgramFiles(x86)%\\Microsoft Visual Studio\\Installer\\vswhere.exe")
		findMsBuildStringCommand = "\"{}\" -latest -requires Microsoft.Component.MSBuild -find MSBuild/**/Bin/MSBuild.exe -version {}".format(vswherePath, versionFilter)
		findMsBuildCommand = shlex.split(findMsBuildStringCommand)
		proc = subprocess.Popen(findMsBuildCommand, stdout=subprocess.PIPE)
		result = proc.communicate()
		if (result[0] != None):
			self.compiler = result[0].strip().decode().replace("\\\\", "\\")

		if not self.compiler:
			msg = "Could not find an MSBuild path. You need it to compile plugins. Aborted."
			self.logger.error(msg)
			raise RuntimeError(msg)

	def ExtractCurrentSDKVersion(self):
		msg = BuildUtil.Messages['NotImplemented']
		self.logger.error(msg)
		raise NotImplementedError()
	
	def AddSwigCleanExceptions(self):
		swigExceptions = BuildUtil.BuildModules[self.platformName].CreateSwigPlatformHandler(self.pathMan).ioFileSources
		for exception in swigExceptions:
			self.cleanExceptions.append(os.path.basename(exception))

	def CleanBetweenMultiSDKIterations(self):
		if self.generateSwig is False:
			self.AddSwigCleanExceptions()

		for root, dirs, files in os.walk(self.pathMan.Paths['Src_Platform']):
			for file in files:
				if file not in self.cleanExceptions:
					os.unlink(os.path.join(root, file))
			for dir in dirs:
				shutil.rmtree(os.path.join(root, dir))

	def MultiSDKBuild(self, buildFunc):
		if self.wwiseSdkVerEnvVar is None:
			msg = 'Attempting to build platform {} as MultiSDK, but wwiseSdkVerEnvVar is not set.'.format(self.platformName)
			self.logger.error(msg)
			raise RuntimeError(msg)

		if self.isMultiSDK is False or self.toolchainVers is None or self.toolchainScript is None:
			sdkVer = self.ExtractCurrentSDKVersion()
			self.logger.info('No toolchain specified, using system environment variables to build against {} SDK {}'.format(self.platformName, sdkVer))
			os.environ[self.wwiseSdkVerEnvVar] = 'SDK' + sdkVer
			return buildFunc(self)

		self.logger.info('Building for {} for multiple SDK versions'.format(self.FixedArch))
		results = []
		objPath = os.path.join(self.pathMan.Paths['Src_Platform'], "obj")

		# fetch the list of toolchainvers, and add additional environments to set
		with open(self.toolchainVers, 'r') as toolchain_vers_file:
			toolchain_vers = toolchain_vers_file.readlines()

		for toolchain_ver in toolchain_vers:
			toolchain_ver = toolchain_ver.strip()
			self.sdkVersion = "{}_{}".format(self.platformName, toolchain_ver)

			try:
				toolchain_env = subprocess.check_output(sys.executable + " \"" + self.toolchainScript + "\" " + toolchain_ver).decode("utf-8")
				toolchain_env_ex = os.path.expandvars(toolchain_env)

				if toolchain_env_ex != "":
					for toolchain_env_kv in toolchain_env_ex.split(','):
						key, value = toolchain_env_kv.split('=', maxsplit=1)
						self.logger.info("Applying toolchain envvar for build command: " + key + "=" + value)
						os.environ[key] = value

			except Exception as err:
				self.logger.info("Could not get toolchain environment. GetToolchainEnv returned :" + toolchain_env)
				buildResult = {'resultCode': 'Fail', 'Message': '{}(all configs, all arches, {})'.format(self.platformName, self.sdkVersion)}
				results.append(buildResult)
				continue

			self.CleanBetweenMultiSDKIterations()

			buildResults = buildFunc(self)

			if toolchain_env_ex != "":
				for toolchain_env_kv in toolchain_env_ex.split(','):
					key, value = toolchain_env_kv.split('=', maxsplit=1)
					self.logger.info("Deleting toolchain envvar: " + key)
					del os.environ[key]

			for  buildResult in buildResults:
				results.append(buildResult)

		return results


class MultiArchBuilder(PlatformBuilder):
	def __init__(self, platformName, arches, configs, generateSwig, skipPremake, **kwargs):
		PlatformBuilder.__init__(self, platformName, arches, configs, generateSwig, skipPremake, **kwargs)

		isMultiArchPlatform = self.platformName in BuildUtil.SupportedArches.keys()
		if not isMultiArchPlatform:
			msg = 'Not a supported multi-architecture platform: {}. Instantiate from a single-arch prototype (PlatformBuilder) instead, or add {} to supported multi-arch platform list, available options: {}.'.format(self.platformName, self.platformName, ', '.join(BuildUtil.SupportedArches.keys()))
			self.logger.error(msg)
			raise RuntimeError(msg)

		isBuildAllArches = self.arches is None
		if isBuildAllArches:
			self.arches = BuildUtil.SupportedArches[self.platformName]

	def Build(self):
		'''Prototype of multi-architecture build.'''
		results = []

		self._DoGeneratePremake()

		for a in self.arches:
			self._DoGenerateSWIG(arch=a)
			for c in self.configs:
				self.logger.info('Building: {} ({}, {}, {}) ...'.format(self.platformName, a, c, self.sdkVersion))
				cmds = self._CreateCommands(a, c)
				isSuccess = self._RunCommands(cmds)
				if isSuccess:
					resultCode = 'Success'
				else:
					resultCode = 'Fail'
				result = {'resultCode': resultCode, 'Message': '{}({}, {}, {})'.format(self.platformName, a, c, self.sdkVersion)}

				results.append(result)
		return results

class VS2017Builder(PlatformBuilder):
	def __init__(self, platformName, arches, configs, generateSwig, skipPremake, **kwargs):
		PlatformBuilder.__init__(self, platformName, arches, configs, generateSwig, skipPremake, **kwargs)

		self._FindMSBuildPath("[15.0,16.0)")

		pathMan = BuildUtil.PathManager(self.platformName)
		self.solution = join(pathMan.Paths['Src_Platform'], '{}{}.sln'.format(pathMan.ProductName, self.platformName))

	def _CreateCommands(self, arch=None, config='Profile'):
		# Edit name back to Visual Studio arch name.
		cmds = []

		for t in self.tasks:
			cmd = ['{}'.format(self.compiler), '{}'.format(self.solution), '/t:{}'.format(t), '/p:Configuration={}'.format(config)]
			cmds.append(cmd)
		return cmds

class VS2019Builder(PlatformBuilder):
	def __init__(self, platformName, arches, configs, generateSwig, skipPremake, **kwargs):
		PlatformBuilder.__init__(self, platformName, arches, configs, generateSwig, skipPremake, **kwargs)

		self._FindMSBuildPath("[16.0,17.0)")

		pathMan = BuildUtil.PathManager(self.platformName)
		self.solution = join(pathMan.Paths['Src_Platform'], '{}{}.sln'.format(pathMan.ProductName, self.platformName))

	def _CreateCommands(self, arch=None, config='Profile'):
		# Edit name back to Visual Studio arch name.
		cmds = []

		for t in self.tasks:
			cmd = ['{}'.format(self.compiler), '{}'.format(self.solution), '/t:{}'.format(t), '/p:Configuration={}'.format(config)]
			cmds.append(cmd)
		return cmds

class VS2022Builder(PlatformBuilder):
	def __init__(self, platformName, arches, configs, generateSwig, skipPremake, **kwargs):
		PlatformBuilder.__init__(self, platformName, arches, configs, generateSwig, skipPremake, **kwargs)

		self._FindMSBuildPath("[17.0,18.0)")

		pathMan = BuildUtil.PathManager(self.platformName)
		self.solution = join(pathMan.Paths['Src_Platform'], '{}{}.sln'.format(pathMan.ProductName, self.platformName))

	def _CreateCommands(self, arch=None, config='Profile'):
		# Edit name back to Visual Studio arch name.
		cmds = []

		for t in self.tasks:
			cmd = ['{}'.format(self.compiler), '{}'.format(self.solution), '/t:{}'.format(t), '/p:Configuration={}'.format(config)]
			cmds.append(cmd)
		return cmds

class XCodeBuilder(PlatformBuilder):
	def __init__(self, platformName, arches, configs, generateSwig, skipPremake, **kwargs):
		PlatformBuilder.__init__(self, platformName, arches, configs, generateSwig, skipPremake, **kwargs)
		self.compiler = 'xcodebuild'
		self.toolchainScript = os.path.join(self.pathMan.Paths['Toolchain_setup'], platformName, 'GetToolchainEnv.py')
		self.toolchainVer = "Xcode1600" # Hard-code to latest
		pathMan = BuildUtil.PathManager(self.platformName)
		self.solution = join(pathMan.Paths['Src_Platform'], '{}{}.xcodeproj'.format(pathMan.ProductName, self.platformName))

		# Setup build environment for Xcode version
		try:
			toolchain_env = subprocess.check_output([sys.executable, self.toolchainScript, self.toolchainVer], text=True)
			toolchain_env_ex = os.path.expandvars(toolchain_env)

			if toolchain_env_ex != "":
				for toolchain_env_kv in toolchain_env_ex.split(','):
					key, value = toolchain_env_kv.split('=', maxsplit=1)
					self.logger.info("Applying toolchain envvar for build command: " + key + "=" + value)
					os.environ[key] = value

		except Exception as err:
			raise RuntimeError("Could not get toolchain environment for {} ({}). GetToolchainEnv returned : {}".format(self.platformName, self.toolchainVer, toolchain_env))


	def _CreateCommands(self, arch=None, config='Profile'):
		cmds = []

		cmd = [self.compiler, '-project', self.solution, '-configuration', config, 'WWISESDK={}'.format(os.environ['WWISESDK']), 'build', '-allowProvisioningUpdates']
		cmds.append(cmd)
		return cmds

class XCodeDeviceBuilder(XCodeBuilder):
	def __init__(self, platformName, arches, configs, generateSwig, skipPremake, **kwargs):
		XCodeBuilder.__init__(self, platformName, arches, configs, generateSwig, skipPremake, **kwargs)
		self.supportedSdks = []

	def _CreateCommands(self, arch=None, config='Profile'):
		cmds = []

		for supportedSdk in self.supportedSdks:
			cmd = [self.compiler, '-project', self.solution, '-configuration', config, '-sdk', supportedSdk, 'WWISESDK={}'.format(os.environ['WWISESDK']), 'build', '-allowProvisioningUpdates']
			cmds.append(cmd)

		return cmds

class WwiseUnityBuilder(object):
	'''Main console utility for building Wwise Unity Integration.'''

	def __init__(self, args):
		self.platforms = args.platforms
		self.arches = args.arches
		self.configs = args.configs
		self.wwiseSdkDir = args.wwiseSdkDir
		self.androidSdkDir = args.androidSdkDir
		self.androidNdkDir = args.androidNdkDir
		self.isUpdatePref = args.isUpdatePref
		self.isGeneratingSwigBinding = args.isGeneratingSwigBinding
		self.isSkippingPremake = args.isSkippingPremake
		self.isMultiSDK = args.isMultiSDK
		self.excludeSimulatorSdk = args.excludeSimulatorSdk
		self.toolchainVersFilename = args.toolchainVersFilename

		self.logger = BuildUtil.CreateLogger(BuildUtil.DefaultLogFile, __file__, self.__class__.__name__)

	def CreatePlatformBuilder(self, platformName, arches, configs, generateSwig, skipPremake, **kwargs):
		builder = BuildModules[platformName].CreatePlatformBuilder(platformName, arches, configs, generateSwig, skipPremake, self.isMultiSDK, **kwargs)
		if builder is None:
			# NOTE: Do not raise error here. Skip to allow batch build to continue with next platform.
			self.logger.error('Undefined platform: {}. Skipped.'.format(platformName))
		return builder

	def Build(self):
		self.logger.critical('Build started. {}'.format(BuildUtil.Messages['CheckLogs']))

		self._SetupEnvironment(name=BuildUtil.WwiseSdkEnvVar, value=self.wwiseSdkDir)
		if 'Android' in self.platforms:
			self._SetupEnvironment(name=BuildUtil.AndroidSdkEnvVar, value=self.androidSdkDir)
			self._SetupEnvironment(name=BuildUtil.AndroidNdkEnvVar, value=self.androidNdkDir)

		results = [] # Each platformResult is a pair of [code, msg]
		for p in self.platforms:
			kwargs = {}
			if p != 'Mac' and p in BuildUtil.SupportedPlatforms['Darwin']:
				kwargs['excludeSimulatorSdk'] = self.excludeSimulatorSdk

			kwargs['toolchainVersFilename'] = self.toolchainVersFilename
			builder = self.CreatePlatformBuilder(p, self.arches, self.configs, self.isGeneratingSwigBinding, self.isSkippingPremake, **kwargs)
			if builder is None:
				platformResults = self._GetUndefinedPlatformBuildResults()
			else:
				platformResults = builder.Build()
			results += platformResults

		return self._Report(results)

	def _SetupEnvironment(self, name, value):
		'''Update preference file and export to environment for the build process.'''
		if self.isUpdatePref:
			try:
				BuildUtil.WritePreferenceField(name, value)
			except Exception as e:
				msg = 'Failed to update preference field: {}, with error: {}. Aborted.'.format(name, e)
				self.logger.error(msg)
				raise RuntimeError(msg)

		# Export this env var because solution inclue/library paths depend on it.
		os.environ[name] = value
		self.logger.info('Environment variable updated: now {} = {}.'.format(name, value))

	def _GetUndefinedPlatformBuildResults(self):
		return ['Fail', 'UnsupportedPlatform']

	def _Report(self, results):
		'''Log build results. Result = ['Fail'/'Success', 'Platform(arch, config)']'''

		reports = { 'Fail': [], 'Success': []}
		for r in results:
			msg = '{}'.format(r['Message'])
			key = r['resultCode']
			reports[key].append(msg)

		msg = 'List of Failed Builds: {}'.format(', '.join(reports['Fail']))
		self.logger.critical(msg)
		msg = 'List of Succeeded Builds: {}'.format(', '.join(reports['Success']))
		self.logger.info(msg)
		isNoFails = len(reports['Fail']) == 0
		if isNoFails:
			self.logger.info('*BUILD SUCCEEDED*.{}'.format(BuildUtil.SpecialChars['PosixLineEnd']*2))
			return True
		else:
			msg = '***BUILD FAILED***. {}{}'.format(BuildUtil.Messages['CheckLogs'], BuildUtil.SpecialChars['PosixLineEnd']*3)
			self.logger.critical(msg)
			return False


def ParseAndValidateArgs(argv=None):
	ProgName = os.path.splitext(os.path.basename(__file__))[0]

	Epilog = '''
	Examples:
	# Build for Windows 32bit Debug.
	py -3 {} -p Windows -a Win32 -c Debug
	# Build for all configurations for Android armeabi-v7a architecture.
	py -3 {} -p Android -a armeabi-v7a
	# Build for Mac Profile with a specified Wwise SDK location and update the preference.
	python3 {} -p Mac -c Profile -w /Users/me/Wwise/wwise_v2013.2_build_4800/SDK -u
	# Build for all supported platforms by the current Unity Editor platform.
	py -3 {}
	'''.format(__file__, __file__, __file__, __file__)
	Epilog = '\n'.join(Epilog.splitlines(True))

	availableArchs = ""

	for k, v in BuildUtil.SupportedArches.items():
		if k != "placeholder":
			availableArchs += " {}: {}".format(k, v)

	parser = argparse.ArgumentParser(prog=ProgName, description='Main console utility for building Wwise Unity Integration.', add_help=True, epilog=Epilog, formatter_class=RawTextHelpFormatter)
	supportedPlatforms = BuildUtil.SupportedPlatforms[platform.system()]
	parser.add_argument('-v', '--version', action='version', version='%(prog)s {}'.format(BuildUtil.Version))
	parser.add_argument('-p', '--platforms', nargs='+', dest='platforms', default=supportedPlatforms, help='One or more target platforms to build, default to {} if the option or its arguments are not specified.'.format(', '.join(supportedPlatforms)))
	parser.add_argument('-a', '--arches', nargs='+', dest='arches', default=None, help='One or more target architectures to build for certain platforms, available options:{}, default to None if none given. Here None represents all supported architectures. When combined with -p, -p must receive only one platform and it must be a multi-architecture one.'.format(availableArchs))
	parser.add_argument('-c', '--configs', nargs='+', dest='configs', default=BuildUtil.SupportedConfigs, help='One or more target configurations to build, available options: {}, default to all configurations if none given.'.format(', '.join(BuildUtil.SupportedConfigs)))

	Undefined = 'Undefined'
	envWwiseSdkDir = Undefined
	if BuildUtil.WwiseSdkEnvVar in os.environ:
		envWwiseSdkDir = os.environ[BuildUtil.WwiseSdkEnvVar]
	parser.add_argument('-w', '--wwisesdkdir', nargs=1, dest='wwiseSdkDir', default=Undefined, help='The Wwise SDK folder to build the Integration against. Required if -u is used. Abort if the specified path is not found; if no path specified in arguments, fallback first to the preference file, then to the environment variable WWISESDK if the preference file is unavailable. Preference file location: {}, current WWISESDK = {}. For Android, no white spaces are allowed in the Wwise SDK folder path.'.format(BuildUtil.DefaultPrefFile, envWwiseSdkDir))
	parser.add_argument('-u', '--updatepref', action='store_true', dest='isUpdatePref', default=False, help='Flag to set whether or not to overwrite the relevant fields in the preference file with the user specified command options and arguments, default to unset (not to update).')
	parser.add_argument('-V', '--verbose', action='store_true', dest='isVerbose', default=False, help='Set the flag to show all log messages to console, default to unset (quiet).')
	parser.add_argument('-g', '--generateswigbinding', action='store_true', dest='isGeneratingSwigBinding', default=False, help='Generate SWIG binding before build process.')
	parser.add_argument('-m', '--generatepremake', action='store_true', dest='isGeneratingPremake', default=False, help='Generate Premake project before build process.')
	parser.add_argument('-M', '--multiSDK', action='store_true', dest='isMultiSDK', default=False, help='Build the Wwise Unity integration for multiple platform SDK versions.')
	parser.add_argument('-d', '--deleteLogs', action='store_true', dest='deleteLogs', default=False, help='Delete the previous log files before creating the ones for the current run.')
	parser.add_argument('-S', '--skipPremake', action='store_true', dest='isSkippingPremake', default=False, help='Skip the Premake generation of the project before build process.')

	# Apple platform(s) only
	parser.add_argument('-x', '--exclude-simulator-sdk', action=BooleanOptionalAction, dest='excludeSimulatorSdk', default=False, help='This option is use to exclude Apple simulator SDK targets from being built. Defaults to false. This means it will not exclude the simulator SDK targets.')

	# Multi SDK
	parser.add_argument('-t', '--toolchain-vers-filename', dest='toolchainVersFilename', default='ToolchainVers.txt', help='Filename of the toolchain vers file to use. The system will build for all SDKs version specified. Defaults to "ToolchainVers.txt".')

	# Android only
	envSdkDir = Undefined
	if BuildUtil.AndroidSdkEnvVar in os.environ:
		envSdkDir = os.environ[BuildUtil.AndroidSdkEnvVar]
	envNdkDir = Undefined
	if BuildUtil.AndroidNdkEnvVar in os.environ:
		envNdkDir = os.environ[BuildUtil.AndroidNdkEnvVar]
	parser.add_argument('-s', '--androidsdkdir', nargs=1, dest='androidSdkDir', default=Undefined, help='The Android SDK folder to build the Integration with. Required if Android is among the input platforms. Abort if the specified path is not found; if not specified in arguments, fallback first to the preference file, then to the environment variable {} if the preference file is unavailable. Preference file location: {}, current {} = {}'.format(BuildUtil.AndroidSdkEnvVar, BuildUtil.DefaultPrefFile, BuildUtil.AndroidSdkEnvVar, envSdkDir))
	parser.add_argument('-n', '--androidndkdir', nargs=1, dest='androidNdkDir', default=Undefined, help='The Android NDK folder to build the Integration with. Required if Android is among the input platforms. Abort if the specified path is not found; if not specified in arguments, fallback first to the preference file, then to the environment variable {} if the preference file is unavailable. Preference file location: {}, current {} = {}'.format(BuildUtil.AndroidNdkEnvVar, BuildUtil.DefaultPrefFile, BuildUtil.AndroidNdkEnvVar, envNdkDir))

	if argv is None:
		args = parser.parse_args()
	else: # for unittest
		args = parser.parse_args(argv[1:])

	if args.deleteLogs:
			shutil.rmtree(normpath(join(ScriptDir, "..", "..", 'Logs')), ignore_errors=True)

	logger = BuildUtil.CreateLogger(BuildUtil.DefaultLogFile, __file__, ParseAndValidateArgs.__name__)

	DefaultMsg = 'Use -h for help. Aborted'
	for p in args.platforms:
		if not p in BuildUtil.SupportedPlatforms[platform.system()]:
			logger.error('Found unsupported platform: {}. {}.'.format(p, DefaultMsg))
			return None

	# Arch only works for a single specified platform, and is ignored otherwise
	if not args.arches is None:
		if len(args.platforms) != 1:
			logger.error('Found {} platform(s) when using -a. Must use only one multi-architecture platform instead. {}.'.format(len(args.platforms), DefaultMsg))
			return None
		for a in args.arches:
			if not args.platforms[0] in BuildUtil.SupportedArches.keys():
				logger.error('Found single-architecture platform {} when using -a. Must not use any single-architecture platforms. Use only one multi-architecture platform when using -a. {}.'.format(args.platforms[0], DefaultMsg))
				return None
			if not a in BuildUtil.SupportedArches[args.platforms[0]]:
				logger.error('Found unsupported architecture {} for the platform {}. {}.'.format(a, args.platforms[0], DefaultMsg))
				return None

	for c in args.configs:
		if not c in BuildUtil.SupportedConfigs:
			logger.error('Found unsupported configuration {}. {}.'.format(c, DefaultMsg))
			return None

	# Policy: When calling as external process from Unity Editor (Mono), env var won't work.
	# So first check if Wwise SDK folder is specified in command;
	# if specified, check if the path exists and then if -u is on,
	# export to the preference file.
	# if not specified, then try to parse from the preference file;
	# if all above fail, check env var.
	# if -w is not specified, do not update pref at all.
	[args.wwiseSdkDir, isWwiseSdkSpecifiedInCmd] = ValidateLocationVar(varName=BuildUtil.WwiseSdkEnvVar, inVarValue=args.wwiseSdkDir, entityName='Wwise SDK folder', cliSwitch='-w')
	if args.wwiseSdkDir is None:
		return None
	if exists(args.wwiseSdkDir):
		if 'Android' in args.platforms and BuildUtil.SpecialChars['WhiteSpace'] in args.wwiseSdkDir:
			logger.error('Wwise SDK folder contains white spaces. Android build will fail. Consider removing all white spaces in Wwise SDK folder path: {}. {}.'.format(args.wwiseSdkDir, DefaultMsg))
			return None
	else:
		logger.error('Failed to find Wwise SDK folder: {}. {}.'.format(args.wwiseSdkDir, DefaultMsg))
		return None

	# Android only: See policy for -w
	isAndroidSdkSpecifiedInCmd = False
	isAndroidNdkSpecifiedInCmd = False
	if 'Android' in args.platforms:
		[args.androidSdkDir, isAndroidSdkSpecifiedInCmd] = ValidateLocationVar(varName=BuildUtil.AndroidSdkEnvVar, inVarValue=args.androidSdkDir, entityName='Android SDK folder', cliSwitch='-s')
		if args.androidSdkDir is None:
			return None
		[args.androidNdkDir, isAndroidNdkSpecifiedInCmd] = ValidateLocationVar(varName=BuildUtil.AndroidNdkEnvVar, inVarValue=args.androidNdkDir, entityName='Android NDK folder', cliSwitch='-n')
		if args.androidNdkDir is None:
			return None

	if args.isUpdatePref and [isWwiseSdkSpecifiedInCmd, isAndroidSdkSpecifiedInCmd, isAndroidNdkSpecifiedInCmd] == [False, False, False]:
		logger.error('Found -u without one of -w, -n, or -s. Cannot update preference without those options specified. {}.'.format(DefaultMsg))
		return None

	BuildUtil.IsVerbose = args.isVerbose

	return args

def ValidateLocationVar(varName, inVarValue, entityName, cliSwitch):
	'''Retrieve an environment variable with our 3-layer setup and also check if the variable is from CLI.'''
	logger = BuildUtil.CreateLogger(BuildUtil.DefaultLogFile, __file__, ValidateLocationVar.__name__)
	DefaultMsg = 'Use -h for help. Aborted'

	Undefined = 'Undefined'
	location = Undefined
	isLocationSpecifiedInCmd = inVarValue != Undefined
	if isLocationSpecifiedInCmd:
		# Assume input value is the argparse vector arg.
		location = inVarValue[0]
	else:
		prefLocation = BuildUtil.ReadPreferenceField(varName)
		if not prefLocation is None:
			logger.warn('No {} specified ({}). Fall back to use preference ({}): {}'.format(entityName, cliSwitch, BuildUtil.DefaultPrefFile, prefLocation))
			location = prefLocation
		else:
			if varName in os.environ:
				logger.warning('No {} specified ({}) and no preference found. Fall back to use environment variable: {} = {}'.format(entityName, cliSwitch, varName, os.environ[varName]))
				location = os.environ[varName]
			else:
				logger.error('Undefined environment variable: {}. {}.'.format(varName, DefaultMsg))
				return [None, isLocationSpecifiedInCmd]
	if not exists(location):
		logger.error('Failed to find {}: {}. {}.'.format(entityName, location, DefaultMsg))
		return [None, isLocationSpecifiedInCmd]

	return [location, isLocationSpecifiedInCmd]

def main(argv=None):
	BuildUtil.InitPlatforms()
	args = ParseAndValidateArgs(argv)
	if args is None:
		return 1

	builder = WwiseUnityBuilder(args)
	result = builder.Build()
	if result == True:
		return 0
	else:
		return 1

if __name__ == '__main__':
	sys.exit(main(sys.argv))
