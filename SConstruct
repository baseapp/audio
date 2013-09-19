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

# include the core
env = SConscript('../../build_core/SConscript')

vars = Variables()
vars.Add('BINDINGS', 'Bindings to build (comma separated list): cpp, java', 'cpp,java')
vars.Add(EnumVariable('BUILD_SERVICES_SAMPLES',
                      'Build the services samples.',
                      'on',
                      allowed_values = ['on', 'off']))
vars.Add(PathVariable('ALLJOYN_DISTDIR',
                      'Directory containing a built AllJoyn Core dist directory.',
                      os.environ.get('ALLJOYN_DISTDIR')))
vars.Update(env)
Help(vars.GenerateHelpText(env))

if env.get('ALLJOYN_DISTDIR'):
    # normalize ALLJOYN_DISTDIR first
    env['ALLJOYN_DISTDIR'] = env.Dir('$ALLJOYN_DISTDIR')
    env.Append(CPPPATH = [ env.Dir('$ALLJOYN_DISTDIR/cpp/inc'),
                           env.Dir('$ALLJOYN_DISTDIR/about/inc') ])
    env.Append(LIBPATH = [ env.Dir('$ALLJOYN_DISTDIR/cpp/lib'),
                           env.Dir('$ALLJOYN_DISTDIR/about/lib') ])

env['bindings'] = set([ b.strip() for b in env['BINDINGS'].split(',') ])

env.SConscript('SConscript')

