# TODO: Test Mac and Windows.

# some options, help text says all
AddOption('--with-32bits', dest='32bits', action='store_true',
          help='Force 32 bit compilation ("-m32" GCC option) on Unix.')

AddOption('--with-msvs', dest='msvs', action='store_true',
          help='Create a MSVS solution file under Windows.')

AddOption('--with-debug', dest='debug', action='store_true',
          help='Add debugging symbols')

matlab_is_32_bits = GetOption('32bits')
make_msvs         = GetOption('msvs')

# the mex_builder tool automatically sets various environment variables
dirac        = Environment(tools = ['default', 'packaging', 'matlab'])

# print dirac['CC']
# dirac.Replace(CC="clang")

platform     = dirac['PLATFORM']
msvs_variant = "Release"

# OS dependent stuff, we assume GCC on Unix like platforms
if platform == "posix":
    dirac.Append(LIBPATH="Linux",
                 CCFLAGS="-std=c99 -O2 -pedantic -Wall -Wextra -fdump-rtl-expand",
                 LIBS=["m"])
    if dirac['CC'] == 'gcc':
        dirac.Append(LINKFLAGS="-Wl,--as-needed")
    if matlab_is_32_bits:
        dirac.Append(CCFLAGS="-m32", LINKFLAGS="-m32")
    dirac_lib   = "Dirac"
elif platform == "win32":
    dirac.Append(LIBPATH="Win")
    dirac_lib   = "DiracLE"
elif platform == "darwin":
    dirac.Append(LIBPATH="Mac",
                 CCFLAGS="-std=c99 -O2 -pedantic -Wall -Wextra",
                 LIBS=["m"])
    dirac_lib   = "DiracLE"
else:
    exit("Oops, not a supported platform.")

# define operating system independent options and dependencies
dirac.Append(CPPPATH = "include")

# Dirac is bundled, so just add it
dirac.Append(LIBS=dirac_lib)

if platform == 'posix':
    # Dirac needs vecLib framework on Mac OS X
    dirac.Append(FRAMEWORKS="vecLib")

if GetOption('debug'):
    dirac.MergeFlags(["-g", "-O0"])
    msvs_variant = "Debug"

# add compile targets
mDirac = dirac.Mex("mDirac", ["mDirac.c"])
if platform == 'win32':
    # optionally create MS VS project, otherwise just compile
    if make_msvs:
        dirac_vs = dirac.MSVSProject("mDirac"+dirac['MSVSPROJECTSUFFIX'],
                                     ["mDirac.c", "mDirac.def"])
        MSVSSolution("TimeStretchDirac", [dirac_vs], msvs_variant)

# package the software
dirac.Package(
    source      = [mDirac, "TimeStretchDirac.m", "mDirac.m"],
    NAME        = "TimeStretchDirac",
    VERSION     = "1.0",
    PACKAGETYPE = "zip"
)
