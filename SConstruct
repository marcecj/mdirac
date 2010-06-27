# TODO: Test Mac and Windows.

# some options, help text says all
AddOption('--32bits', dest='32bits', action='store_true',
          help='Force 32 bit compilation ("-m32" GCC option) on Unix.')

AddOption('--make-msvs', dest='msvs', action='store_true',
          help='Create a MSVS solution file under Windows.')

AddOption('--debug-syms', dest='debug', action='store_true',
          help='Add debugging symbols')

matlab_is_32_bits = GetOption('32bits')
make_msvs         = GetOption('msvs')

# the mex tool automatically sets various environment variables
dirac        = Environment(tools = ['default', ('matlab', {'mex': True})])
platform     = dirac['PLATFORM']
msvs_variant = "Release"

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
                 CCFLAGS="-std=c99 -O2 -fexceptions -pedantic -pthread -Wall -Wextra -Wpadded -fdump-rtl-expand",
                 LINKFLAGS="--as-needed")
    if matlab_is_32_bits:
        dirac.Append(CCFLAGS="-m32", LINKFLAGS="-m32")
    dirac_lib   = "Dirac"
elif platform == "win32":
    dirac.Append(LIBPATH="Win")
    dirac_lib   = "DiracLE"
elif platform == "darwin":
    dirac.Append(LIBPATH="Mac",
                 CCFLAGS="-std=c99 -O2 -fexceptions -pedantic -pthread -Wall -Wextra -Wpadded",
                 LINKFLAGS="--as-needed")
    dirac_lib   = "DiracLE"
else:
    exit("Oops, not a supported platform.")

# define operating system independent options and dependencies
dirac.Append(CPPPATH = "include",
             WINDOWS_INSERT_MANIFEST = True)

# clone environment from mDirac to mexversion
mexversion = dirac.Clone()

# Dirac is bundled, so just add it
dirac.Append(LIBS = common_libs + dirac_lib)

if platform == 'posix':
    # Dirac needs vecLib framework on Mac OS X
    dirac.Append(FRAMEWORKS="vecLib")

if GetOption('debug'):
    dirac.MergeFlags(["-g", "-O0"])
    msvs_variant = "Debug"

# add compile targets
if platform != 'win32':
    if mexversion['MATLAB']['RELEASE'] < "R2009a":
        mexversion_obj = mexversion.SharedObject("mexversion.c")
        dirac.SharedLibrary("mDirac", ["mDirac.c", mexversion_obj])
    else:
        dirac.SharedLibrary("mDirac", ["mDirac.c"])
else:
    # optionally create MS VS project, otherwise just compile
    if make_msvs:
        dirac_vs = dirac.MSVSProject("mDirac"+dirac['MSVSPROJECTSUFFIX'],
                                     ["mDirac.c", "mDirac.def"])
        MSVSSolution("TimeStretchDirac", [dirac_vs], msvs_variant)
    else:
        dirac.SharedLibrary("mDirac", ["mDirac.c", "mDirac.def"])
