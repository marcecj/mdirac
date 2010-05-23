# TODO: Test Mac and Windows.

import os

# some options, help text says all
AddOption('--linux32', dest='linux32', action='store_true',
          help='Force 32 bit compilation ("-m32" GCC option) on Linux.')
AddOption('--make-msvc', dest='msvc', action='store_true',
          help='Create a MSVS solution file on Windows.')

matlab_is_32_bits = GetOption('linux32')
make_msvc         = GetOption('msvc')

# the mex tool automatically sets various environment variables
dirac    = Environment(tools = ['default', ('matlab', {'mex': True})])
platform = dirac['PLATFORM']

if platform == "win32":
    # Matlab doesn't follow the Windows standard and adds a 'lib' prefix anyway
    common_libs = ["libmex", "libmx"]
else:
    common_libs = ["mex", "mx", "m"]

# this tells SCons where to find mexversion.c
dirac.Repository(dirac["MATLAB"]["SRC"])

# OS dependent stuff, we assume GCC on Unix like platforms
if platform == "posix":
    # add "exceptions" option, without which any mex function that raises an
    # exception (e.g., mexErrMsgTxt()) causes Matlab to crash
    dirac.Append(LIBPATH="Linux",
                 CCFLAGS = "-O2 -fexceptions -pedantic -pthread -Wall -Wextra -Wpadded -dr",
                 LINKFLAGS="--as-needed")
    if matlab_is_32_bits:
        dirac.Append(CCFLAGS="-m32", LINKFLAGS="-m32")
    dirac_lib   = "Dirac"
elif platform == "win32":
    dirac.Append(LIBPATH="Win")
    dirac_lib   = "DiracLE"
elif platform == "darwin":
    dirac.Append(LIBPATH="Mac",
                 CCFLAGS="-O2 -fexceptions -pedantic -pthread -Wall -Wextra -Wpadded",
                 LINKFLAGS="--as-needed")
    dirac_lib   = "DiracLE"
else:
    exit("Oops, not a supported platform.")

# define operating system independent options and dependencies
dirac.Append(CPPPATH = "include",
             WINDOWS_INSERT_MANIFEST = True)

# clone environment from mDirac to mexversion
mexversion = dirac.Clone()

# look for libraries and corresponding headers and exit if they aren't found
# (autoconf-like behaviour)
if not GetOption('clean'):
    conf = dirac.Configure()
    if not conf.CheckLibWithHeader(dirac_lib, 'Dirac.h', 'cpp'):
        exit("You need to install Dirac!")
    dirac = conf.Finish()

dirac.Append(LIBS = common_libs)

# manually set the language standard after the configure checks (otherwise the
# Dirac check will not work because it is a C++ library)
if platform == 'posix':
    # vecLib framework is needed on Mac OS X
    dirac.Append(CFLAGS="--std=c99", FRAMEWORKS="vecLib")

# add compile targets
if platform != 'win32':
    mexversion_obj = mexversion.SharedObject("mexversion.c")
    dirac.SharedLibrary("mDirac", ["mDirac.c", mexversion_obj])
else:
    # optionally create MS VS project, otherwise just compile
    if make_msvc:
        dirac_vs = dirac.MSVSProject("mDirac"+dirac['MSVSPROJECTSUFFIX'],
                                     ["mDirac.c", "mDirac.def"])
        MSVSSolution(target="TimeStretchDirac", projects=[dirac_vs])
    else:
        dirac.SharedLibrary("mDirac", ["mDirac.c", "mDirac.def"])
