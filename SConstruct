# TODO: Test Mac and Windows.
# TODO: Add an option to do
#           env = Environment(platform = 'win32')
#       to create a MSVC project for Windows from Linux.

import os

# Add an option to enforce 32 bit compilation for students using 32 bit Matlab
# on 64 bit platforms.
AddOption('--linux32', dest='linux32', action='store_true',
        help='Force 32 bit compilation ("-m32" GCC option) on Linux.')

# the mex tool automatically sets various environment variables
dirac_env = Environment(tools=['default', ('matlab', {'mex': True})])

# this tells SCons where to find mexversion.c
Repository(dirac_env["MATLAB"]["SRC"])

# define operating system independent options and dependencies
dirac_env.Append(
        CPPPATH = "include",
        WINDOWS_INSERT_MANIFEST = True,
        LIBS = ["m", "mex", "mx"]
        )

# OS dependent stuff
if os.name == "posix":
    # add "exceptions" option, without which any mex function that raises an
    # exception (e.g., mexErrMsgTxt()) causes Matlab to crash
    dirac_env.Append(LIBPATH="Linux",
            CCFLAGS = "-fexceptions -std=c99 -pedantic -Wall -Wextra -Wpadded -dr")
    if GetOption('linux32'):
        dirac_env.Append(CCFLAGS="-m32", LINKFLAGS="-m32")
    dirac_lib = "Dirac"
elif os.name == "nt":
    dirac_env.Append(LIBPATH="Win")
    dirac_lib = "DiracLE"
elif os.name == "mac":
    dirac_env.Append(LIBPATH="Mac",
            CCFLAGS="-fexceptions -std=c99 -pedantic")
    dirac_lib = "DiracLE"
else:
    exit("Oops, not a supported platform.")

# clone environment from mDirac to mexversion
mexversion_env = dirac_env.Clone()

# do env dependent stuff
dirac_env.Append(LIBS = dirac_lib)

# append dependency on vecLib framework on Mac OS X
if os.name == "mac":
    dirac_env.Append(FRAMEWORKS="vecLib")

# add targets
mexversion = mexversion_env.SharedObject("mexversion.c")
dirac_env.SharedLibrary("mDirac", ["mDirac.c", mexversion])
