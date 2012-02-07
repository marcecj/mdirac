import os
import platform

Help(
"""This build system compiles the mDirac Mex file.  To compile, use one of the
following build targets:
    mDirac       -> compile mDirac (default)
    mDirac-dbg   -> compile mDirac with debugging information
    makezip      -> create a zip file
    install      -> install mDirac to directory "mDirac"
    all          -> runs both mDirac and makezip
"""
)

env_vars = Variables()
env_vars.Add('CC', 'The C compiler')

# the mex_builder tool automatically sets various environment variables
env = Environment(tools = ['default', 'packaging', 'matlab'],
                    variables = env_vars)

# define an AsciiDoc builder
asciidoc = env.Builder(action = ['asciidoc -o ${TARGET} ${SOURCE}'],
                       suffix = '.html',
                       single_source = True)
env['BUILDERS']['AsciiDoc'] = asciidoc

cur_platform = env['PLATFORM']

# OS dependent stuff, we assume GCC on Unix like platforms
if cur_platform in ("posix", "darwin"):

    env.Append(LIBPATH="Linux",
               CCFLAGS="-std=c99 -O2 -pedantic -Wall -Wextra",
               LIBS=["m"])

    if env['CC'] == 'gcc':
        env.Append(LINKFLAGS="-Wl,--as-needed")

    # if the system is 64 bit and Matlab is 32 bit, compile for 32 bit; since
    # Matlab currently only runs on x86 architectures, checking for x86_64
    # should suffice
    if platform.machine() == "x86_64" \
       and not env['MATLAB']['ARCH'].endswith('64'):
        env.Append(CCFLAGS="-m32", LINKFLAGS="-m32")

    dirac_lib = ("Dirac" if cur_platform == "posix" else "DiracLE")

elif cur_platform == "win32":

    env.Append(LIBPATH="Win")

    env.Replace(WINDOWS_INSERT_DEF = True)

    dirac_lib = "DiracLE"

else:

    exit("Oops, not a supported platform.")

# define operating system independent options and dependencies
env.Append(CPPPATH = "include",
           LIBS    = dirac_lib)

if cur_platform == 'darwin':
    # Dirac needs vecLib framework on Mac OS X
    env.Append(FRAMEWORKS="vecLib")

do_debug = False
mDirac, mfiles = env.SConscript(
    dirs        = 'src',
    variant_dir = os.sep.join(['build', 'release']),
    exports     = ['env', 'do_debug'],
    duplicate   = False
)

do_debug = True
mDirac_dbg = env.SConscript(
    dirs        = 'src',
    variant_dir = os.sep.join(['build', 'debug']),
    exports     = ['env', 'do_debug'],
    duplicate   = False
)

# optionally create MS VS project, otherwise just compile
if cur_platform == 'win32':
    mdirac_vs = env.MSVSProject(
        target      = "mDirac" + env['MSVSPROJECTSUFFIX'],
        buildtarget = ["mDirac", "mDirac-dbg"],
        runfile     = os.sep.join([env['MATLAB']['ROOT'], "bin", "matlab.exe"]),
        srcs        = Glob(os.sep.join(["src", "*.c"]), strings=True),
        localincs   = Glob(os.sep.join(["src", "*.h"]), strings=True),
        incs        = os.sep.join(["include", "*.h"]),
        variant     = ["Release", "Debug"]
    )

    Alias('vsproj', mDirac_vs + mDirac + mDirac_dbg)
    Help("    vsproj       -> create a visual studio project file")

# package the software
mDirac_inst = env.Install("mDirac", mDirac + mfiles)
mDirac_pkg = env.Package(
    NAME        = "mDirac",
    VERSION     = "1.0",
    PACKAGETYPE = "zip"
)

# create an alias for building the documentation, but only if the asciidoc
# binary could be found
if env.WhereIs('asciidoc') is not None:
    docs = env.AsciiDoc(['README', 'INSTALL', 'LICENSE'])

    Alias('doc', docs)

    Help("    doc          -> compiles documentation to HTML")
else:
    print "asciidoc not found! Cannot build documentation."

# define some useful aliases
Alias("makezip", mDirac_pkg)
Alias("install", mDirac_inst)
Alias("mDirac", mDirac)
Alias("mDirac-dbg", mDirac_dbg)
Alias("all", mDirac + mDirac_pkg)

Default(mDirac)

Help(
"""\n
The following environment variables can be overridden by passing them *after*
the call to scons, i.e. "scons CC=gcc":"""
+ env_vars.GenerateHelpText(env)
)
