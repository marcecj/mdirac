# TODO: add some options (mostly for Linux, like whether to enforce 32 bits or not)
cmake_minimum_required(VERSION 2.6)

# NOTE: various variables start with the project name, such as *_SOURCE_DIR
project(MDIRAC C)

# set(CMAKE_C_COMPILER "x86_64-pc-linux-gnu-gcc-4.1.2")
set(CMAKE_MODULE_PATH "${MDIRAC_SOURCE_DIR}")

# find matlab related stuff (thank god somebody already wrote this)
# NOTE: I had to modify FindMatlab.cmake, it's nowhere near all-encompassing.
# It'll probably have to be adapted further. I hope I'll find the time to send a
# patch upstream (though maybe CMake 2.8 has an updated version).
include(FindMatlab)

# NOTE: this doesn't really do what I want, which is bail on configure with a
# nice message when a header isn't found instead of at compile time or with a
# stupid error message.
#
# include(CheckIncludeFile)

# TODO: why must "lib" be prepended to library names?
if(WIN32)

    # FIXME: dlls are not found at runtime because they have to be in the same
    # folder as the executable, but they are not copied there
    message("Configuring for Windows")

    set(LIB_DIR Win)

    set(DIRAC_LIB DiracLE)
    add_library(libDiracLE SHARED IMPORTED)

    set(MEX_VERSION_C "${MATLAB_INCLUDE_DIR}/mexversion.rc")
    set(MEX_EXT "mexw32")

elseif(APPLE)

    # TODO: test under OS X!
    message("Configuring for OS X")

    # add exceptions option, see "UNIX" section for explanation
    set(CMAKE_C_FLAGS
        "${CMAKE_C_FLAGS} -fexceptions")
    message("Compiler flags:\t${CMAKE_C_FLAGS}")

    set(LIB_DIR Mac)

    set(DIRAC_LIB DiracLE)
    add_library(libDiracLE STATIC IMPORTED)

    # on OS X the Framework vecLib is required
    find_library(VECLIB_LIB vecLib)
    if(VECLIB_LIB MATCHES *NOTFOUND)
        message("Required Framework vecLib not found. This might lead to linker"
            "errors, or, according to the Dirac documentation, to falling back"
            "to scalar code.")
    else()
        message("Found vecLib at: ${VECLIB_LIB}")
        add_library(${VECLIB_LIB} IMPORTED)
    endif()

    set(MEX_VERSION_C "${MATLAB_INCLUDE_DIR}/../src/mexversion.c")
    set(MEX_EXT "mexmaci")

    # here we need to link to libm
    set(ADDITIONAL_LIBS "m")

elseif(UNIX)

    message("Configuring for Unix (Linux)")

    # enforce 32 bit compilation; add "exceptions" option, without which any mex
    # function that raises an exception (e.g. mexErrMsgTxt()) causes matlab to
    # crash
    set(CMAKE_C_FLAGS
        "${CMAKE_C_FLAGS} -m32 -fexceptions")
    message("Compiler flags:\t${CMAKE_C_FLAGS}")

    set(LIB_DIR Linux)

    set(DIRAC_LIB Dirac)
    add_library(libDirac STATIC IMPORTED)

    # abuse MATLAB_INCLUDE_DIR to get to the src directory
    set(MEX_VERSION_C "${MATLAB_INCLUDE_DIR}/../src/mexversion.c")
    set(MEX_EXT "mexglx")

    # additional CFLAGS; "-dr" dumps GCCs internal code representation (for a
    # call graph utility)
    set(EXTRA_CFLAGS "-std=c99 -Wall -Wextra -Wpadded -dr")
    # here we need to link to libm
    set(ADDITIONAL_LIBS "m")

else()

    message("Unsupported platform, exiting.")
    return()

endif()

# NOTE: link directories only apply to targets defined afterwards
include_directories(${MDIRAC_SOURCE_DIR} ${MATLAB_INCLUDE_DIR})
link_directories(${MDIRAC_SOURCE_DIR}/${LIB_DIR})

add_library(mDirac MODULE
    mDirac.c Dirac.h mDirac.def
    ${MEX_VERSION_C})

# set_property(SOURCE ${MEX_VERSION_C} dirac.c
#     PROPERTY COMPILE_FLAGS "-Wall -Wextra -O0 -fno-PIC")

set_target_properties(mDirac PROPERTIES
    COMPILE_FLAGS "${EXTRA_CFLAGS}"
    LINK_FLAGS ""
    DEFINE_SYMBOL "MATLAB_MEX_FILE"
    PREFIX ""
    SUFFIX ".${MEX_EXT}")

target_link_libraries(mDirac
    ${ADDITIONAL_LIBS} ${MATLAB_MX_LIBRARY} ${MATLAB_MEX_LIBRARY} ${DIRAC_LIB} ${VECLIB_LIB})
