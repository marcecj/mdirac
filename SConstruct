# TODO: Test Mac and Windows.
import platform

# some options, help text says all
AddOption('--with-debug', dest='debug', action='store_true',
          help='Add debugging symbols')

env_vars = Variables()
env_vars.Add('CC', 'The C compiler')

# the mex_builder tool automatically sets various environment variables
dirac = Environment(tools = ['default', 'packaging', 'matlab'],
                    variables = env_vars)

cur_platform = dirac['PLATFORM']

# OS dependent stuff, we assume GCC on Unix like platforms
if cur_platform == "posix":

    dirac.Append(LIBPATH="Linux",
                 CCFLAGS="-std=c99 -O2 -pedantic -Wall -Wextra -fdump-rtl-expand",
                 LIBS=["m"])

    if dirac['CC'] == 'gcc':
        dirac.Append(LINKFLAGS="-Wl,--as-needed")

    # if the system is 64 bit and Matlab is 32 bit, compile for 32 bit; since
    # Matlab currently only runs on x86 architectures, checking for x86_64
    # should suffice
    if platform.machine() == "x86_64" \
       and not dirac['MATLAB']['ARCH'].endswith('64'):
        dirac.Append(CCFLAGS="-m32", LINKFLAGS="-m32")

    dirac_lib   = "Dirac"

elif cur_platform == "win32":

    dirac.Append(LIBPATH="Win")
    dirac_lib = "DiracLE"

elif cur_platform == "darwin":

    dirac.Append(LIBPATH="Mac",
                 CCFLAGS="-std=c99 -O2 -pedantic -Wall -Wextra",
                 LIBS=["m"])
    dirac_lib = "DiracLE"

else:

    exit("Oops, not a supported platform.")

# define operating system independent options and dependencies
dirac.Append(CPPPATH="include")

# Dirac is bundled, so just add it
dirac.Append(LIBS=dirac_lib)

if cur_platform == 'darwin':
    # Dirac needs vecLib framework on Mac OS X
    dirac.Append(FRAMEWORKS="vecLib")

if GetOption('debug'):
    dirac.MergeFlags(["-g", "-O0"])
    msvs_variant = "Debug"

# add compile targets
mDirac = dirac.Mex("mDirac", ["mDirac.c"])

# optionally create MS VS project, otherwise just compile
if cur_platform == 'win32':
    dirac_vs = dirac.MSVSProject("mDirac"+dirac['MSVSPROJECTSUFFIX'],
                                 ["mDirac.c", "mDirac.def"])
    Alias('vsproj', dirac_vs)

# package the software
dirac.Package(
    source      = [mDirac, "TimeStretchDirac.m", "mDirac.m"],
    NAME        = "TimeStretchDirac",
    VERSION     = "1.0",
    PACKAGETYPE = "zip"
)
