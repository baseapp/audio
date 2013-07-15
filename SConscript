# Copyright 2013, Qualcomm Innovation Center, Inc.
#
#    All rights reserved.
#    This file is licensed under the 3-clause BSD license in the NOTICE.txt
#    file for this project. A copy of the 3-clause BSD license is found at:
#
#        http://opensource.org/licenses/BSD-3-Clause. 
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the license is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the license for the specific language governing permissions and
#    limitations under the license.

import os
import subprocess
import sys
import time

Import('env')

vars = Variables()
vars.Add(PathVariable('ABOUT_BASE', 'Base directory of About service', os.environ.get('ABOUT_BASE')))
vars.Add(PathVariable('ALAC_BASE', 'Base directory of ALAC', os.environ.get('ALAC_BASE')))
vars.Add(EnumVariable('JAVAVERSION', '''The version of Java pointed to by the JAVA_HOME
    environment variable. This is not used to select one version of
    the Java comiler vs. another.''', '1.6', allowed_values=('1.5', '1.6')))
vars.Update(env)

Help(vars.GenerateHelpText(env))

sys.path.append('../../build_core/tools/scons')
from configurejni import ConfigureJNI

# Dependent Projects
if not env.has_key('_ALLJOYNCORE_'):
    env.SConscript('../../alljoyn_core/SConscript')

if(not(env.has_key('BULLSEYE_BIN'))):
    print('BULLSEYE_BIN not specified')
else:
    env.PrependENVPath('PATH', env.get('BULLSEYE_BIN'))

# The return value is the collection of files installed in the build destination.
returnValue = []

# Make audio dist a sub-directory of the alljoyn dist.  This avoids any conflicts with alljoyn dist targets.
env['AUDIO_DISTDIR'] = env['DISTDIR'] + '/audio'
env['AUDIO_TESTDIR'] = env['TESTDIR'] + '/audio'
env['AUDIO_CPP_DISTDIR'] = env['AUDIO_DISTDIR'] + '/cpp'
env['AUDIO_CPP_TESTDIR'] = env['AUDIO_TESTDIR'] + '/cpp'
env['AUDIO_JAVA_DISTDIR'] = env['AUDIO_DISTDIR'] + '/java'

# Add support for multiple build targets in the same workset
env.VariantDir('$OBJDIR', 'src', duplicate = 0)
env.VariantDir('$OBJDIR/jni', 'jni', duplicate = 0)
env.VariantDir('$OBJDIR/samples', 'samples', duplicate = 0)
env.VariantDir('$OBJDIR/test', 'test', duplicate = 0)

# Audio install
audio_headers = env.Install('$AUDIO_CPP_DISTDIR/inc/alljoyn', 'inc/alljoyn/audio')
returnValue += audio_headers

# Header file includes
env.Append(CPPPATH = [env.Dir('inc')])

# Audio streaming libraries
libs = env.SConscript('$OBJDIR/SConscript')
audiolib = env.Install('$AUDIO_CPP_DISTDIR/lib', libs)
returnValue += audiolib

env.Append(LIBPATH = [env.Dir('$AUDIO_CPP_DISTDIR/lib')])

# Set the alljoyn_audio library 
env.Prepend(LIBS = audiolib)

# Unit tests
env.SConscript('unit_test/SConscript', variant_dir='$OBJDIR/unittest', duplicate=0)

# Sample programs
returnValue += env.SConscript('$OBJDIR/samples/SConscript')

# Release notes and misc. legals
if env['OS_CONF'] == 'linux':
    returnValue += env.InstallAs('$AUDIO_DISTDIR/README.txt', 'docs/README.linux.txt')
    #returnValue += env.Install('$AUDIO_DISTDIR', 'docs/ReleaseNotes.txt')
    returnValue += env.Install('$AUDIO_DISTDIR', 'README.md')
    returnValue += env.Install('$AUDIO_DISTDIR', 'NOTICE.txt')
elif env['OS_CONF'] == 'android':
    returnValue += env.InstallAs('$AUDIO_DISTDIR/README.txt', 'docs/README.android.txt')
    #returnValue += env.Install('$AUDIO_DISTDIR', 'docs/ReleaseNotes.txt')
    returnValue += env.Install('$AUDIO_DISTDIR', 'README.md')
    returnValue += env.Install('$AUDIO_DISTDIR', 'NOTICE.txt')

# Build docs
installDocs = env.SConscript('docs/SConscript')
env.Depends(installDocs, audio_headers);
returnValue += installDocs

# AllJoyn Audio Java
if env['OS_CONF'] == 'android':
    if not ConfigureJNI(env):
        if not GetOption('help'):
            Exit()

    if not os.environ.get('CLASSPATH'):
        print "CLASSPATH not set"
        if not GetOption('help'):
            Exit()

    # Tell dependent dirs where class and jar files are located
    env.Append(CLASSDIR = '$OBJDIR/classes')
    env.Append(JARDIR = '$AUDIO_JAVA_DISTDIR/jar')

    # Sample programs
    returnValue += env.Install('$AUDIO_JAVA_DISTDIR/samples/Source', 'samples/android/Source/Application');
    returnValue += env.Install('$AUDIO_JAVA_DISTDIR/samples/Source', 'samples/android/Source/Library');
    returnValue += env.Install('$AUDIO_JAVA_DISTDIR/samples', 'samples/android/Sink');
    env.SConscript('$OBJDIR/samples/android/SConscript');

    # AllJoyn Audio Java binding docs
    env['PROJECT_SHORT_NAME'] = 'AllJoyn Audio Java API<br/>Reference Manual'
    env['PROJECT_LONG_NAME'] = 'AllJoyn Audio Java API Reference Manual'
    env['PROJECT_NUMBER'] = 'Version 1.0.0'
    env['PROJECT_COPYRIGHT'] = "Copyright &copy; 2013 Qualcomm Innovation Center, Inc.<br/>This document is licensed under a Creative Commons Attribution-Share Alike 3.0 Unported License; provided,<br/>that (i) any source code incorporated in this document is licensed as detailed in the file NOTICE.txt<br/>and (ii) <b>THIS DOCUMENT AND ALL INFORMATION CONTAIN HEREIN ARE PROVIDED ON AN \"AS-IS\" BASIS WITHOUT WARRANTY OF ANY KIND</b>.<br/><a href='http://creativecommons.org/licenses/by-sa/3.0'>Creative Commons Attribution-Share Alike 3.0 Unported License</a><br/><b>MAY CONTAIN U.S. AND INTERNATIONAL EXPORT CONTROLLED INFORMATION</b>"
    env.JavaDoc('$AUDIO_JAVA_DISTDIR/docs/html', 'samples/android/Source/Library/src', JAVACLASSPATH = env.subst('$JAVACLASSPATH'))

Return('returnValue')
