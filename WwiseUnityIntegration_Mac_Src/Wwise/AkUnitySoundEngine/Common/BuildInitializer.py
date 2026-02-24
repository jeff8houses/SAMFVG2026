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
# Copyright (c) 2025 Audiokinetic Inc.

import sys, os, os.path, argparse, subprocess, logging, shutil
from os import pardir, makedirs, removedirs
from os.path import dirname, abspath, join, exists
import BuildUtil

class BuildInitializer(object):
    '''For a specified platform. Create deployment folders. Cleanup old generated files.'''

    def __init__(self, pathMan):
        self.pathMan = pathMan
        self.logger = BuildUtil.CreateLogger(pathMan.Paths['Log'], __file__, self.__class__.__name__)

    def Initialize(self):
        self._RemoveDeployment()
        self._MakeDeploymentDirs()

    def _MakeDeploymentDirs(self):
        self._SafeMakeDirs(self.pathMan.Paths['Deploy_API_Generated_Common'])
        self._SafeMakeDirs(self.pathMan.Paths['Deploy_API_Generated_Platform'])
        self._SafeMakeDirs(self.pathMan.Paths['Deploy_Plugins'])

    def _SafeMakeDirs(self, p):
        if not os.path.isdir(p):
            try:
                makedirs(p)
            except Exception as e:
                self.logger.info('Failed to create folder {} with error: {}. Ignored.'.format(p, e))

    def _RemoveDeployment(self):
        self._RemoveGeneratedAPIs()

    def _RemoveGeneratedAPIs(self):
        self._SafeRemoveDirs(self.pathMan.Paths['Deploy_API_Generated_Common'])
        self._SafeRemoveDirs(self.pathMan.Paths['Deploy_API_Generated_Platform'])

    def _SafeRemoveDirs(self, p):
        if not exists(p):
            self.logger.info('Folder {} does not exist. Skipped.'.format(p))
            return
        try:
            shutil.rmtree(p)
        except Exception as e:
            self.logger.warning('Failed to remove folder {} with error {}. Ignored.'.format(p, e))

