import os

# global TODO:
# - add search functions to look for matlab (maybe a combination of os.walk and
#   a glob pattern is enough, or use WhereIs()?)

# TODO: test, maybe extend MexBuilder
# env = Environment(tools=['default', 'mex'])
# mex_ext = env['MEX_EXT']
# env.MEX('mytabwrite'+'.'+mex_ext, 'mytabwrite.cc')

matlab_mex_version = "mexversion.c"

# Go ahead and define operating system independent options and dependencies
env_dirac = Environment(
        SHLIBPREFIX = "",
        LIBS        = ["m", "mex", "mx"],
        CPPDEFINES  = "MATLAB_MEX_FILE"
        )

# do OS dependent stuff, name of the Dirac library, Matlab path, etc.
# TODO: finish Mac/Windows support
if os.name == "posix":
    env_dirac.Replace(
            LIBPATH     = ["Linux", "/opt/matlab/bin/glnx86/"],
            SHLIBSUFFIX = ".mexglx",
            # enforce 32 bit compilation; add "exceptions" option, without which
            # any mex function that raises an exception (e.g. mexErrMsgTxt())
            # causes matlab to crash
            CCFLAGS     = "-m32 -fexceptions -std=c99 -pedantic -Wall -Wextra -Wpadded -dr",
            LINKFLAGS   = "-m32"
            )
    dirac               = "Dirac"
    matlab_include_path = "/opt/matlab/extern/include/"
    matlab_src_path     = "/opt/matlab/extern/src"
elif os.name == "nt":
    env_dirac.Replace(
            LIBPATH     = ["Win", "/opt/matlab/bin/glnx86/"],
            SHLIBSUFFIX = ".mexw32",
            CCFLAGS     = "",
            LINKFLAGS   = "",
            )
    dirac               = "DiracLE"
    matlab_include_path = ""
    matlab_src_path     = ""
elif os.name == "mac":
    env_dirac.Replace(
            LIBPATH     = ["Mac", "/opt/matlab/bin/glnx86/"],
            SHLIBSUFFIX = ".mexmaci",
            CCFLAGS     = "",
            LINKFLAGS   = "",
            )
    dirac               = "Dirac"
    matlab_include_path = ""
    matlab_src_path     = ""
else:
    exit("Oops, not a supported platform.")

# additionally append vecLib framework dependency on Macs
if os.name == "mac":
    env_dirac.Append(FRAMEWORKS="vecLib")

# TODO: fix mexversion.c inclusion
# VariantDir(".", matlab_src_path)
include_path = [".", matlab_include_path]

# add mDirac target
env_dirac.Append(LIBS = dirac, CPPPATH = include_path)
env_dirac.SharedLibrary("mDirac", ["mDirac.c", matlab_mex_version])
