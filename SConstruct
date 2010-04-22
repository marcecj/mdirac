# TODO: test Mac and Windows

import os

# Go ahead and define operating system independent options and dependencies; the
# 'matlab' tool automatically sets various environment variables
dirac_env = Environment(tools=['default', 'matlab'])
dirac_env.Append(
        CPPPATH = "include",
        WINDOWS_INSERT_MANIFEST = True,
        LIBS = ["m", "mex", "mx"])

Execute(Copy("mexversion.c", dirac_env["MATLAB"]["SRC"] + os.sep + "mexversion.c"))

# do OS dependent stuff, e.g., name of the Dirac library, Matlab path
# TODO: compare with env['MATLAB']['ARCH'] instead of os.name
if os.name == "posix":
    # enforce 32 bit compilation; add "exceptions" option, without which
    # any mex function that raises an exception (e.g. mexErrMsgTxt())
    # causes matlab to crash
    dirac_env.Append(
            LIBPATH     = "Linux",
            CCFLAGS     = "-m32 -fexceptions -std=c99 -pedantic -Wall -Wextra -Wpadded -dr",
            LINKFLAGS   = "-m32"
            )
    dirac = "Dirac"
elif os.name == "nt":
    dirac_env.Append(LIBPATH="Win")
    dirac = "DiracLE"
elif os.name == "mac":
    dirac_env.Append(LIBPATH="Mac")
    dirac = "Dirac"
else:
    exit("Oops, not a supported platform.")

# clone environment for mexversion from mDirac
mexversion_env = dirac_env.Clone()
mexversion     = mexversion_env.SharedObject("mexversion.c")

# append dependency on vecLib framework on Mac OS X
if os.name == "mac":
    dirac_env.Append(FRAMEWORKS="vecLib")

# add mDirac target
dirac_env.Append(LIBS = dirac)
dirac_env.SharedLibrary("mDirac", ["mDirac.c", mexversion])
