# TODO: add some options (mostly for Linux, like whether to enforce 32 bits or not)
cmake_minimum_required(VERSION 2.6)

# NOTE: various variables start with the project name, such as *_SOURCE_DIR
project(MDIRAC C)

set(CMAKE_MODULE_PATH "${MDIRAC_SOURCE_DIR}")

# find matlab related stuff (thank god somebody already wrote this stuff)
# NOTE: I had to modify FindMatlab.cmake, it's nowhere near all-encompassing.
# It'll probably have to be adapted further. I hope I'll find the time to send a
# patch upstream (though maybe CMake 2.8 has an updated version).
include(FindMatlab)

# NOTE: this doesn't really do what I want, which is bail on configure with a
# nice message when a header isn't found instead of at compile time or with a
# stupid error message.
#
# include(CheckIncludeFile)
# check_include_file(sndfile.h SNDFILE_H) 

# TODO: why must "lib" be prepended to library names?
if(WIN32)

    # FIXME: dlls are not found at runtime because they have to be in the same
    # folder as the executable, but they are not copied there
    message("Configuring for Windows")

    set(LIB_DIR Win)

    set(DIRAC_LIB DiracLE)
    add_library(libDiracLE SHARED IMPORTED)
    set(SNDFILE_LIB libsndfile-1)
    add_library(libsndfile SHARED IMPORTED)

    # Deal with stupid non-standard library/header locations under Windows .
    # Maybe more possibilities need to be added here.
    # (alternative: copy relevant files to project directory)
    set(SNDFILE_DIRS
        "C:/Programme/Mega-Nerd/libsndfile")
    find_path(SNDFILE_H_DIR sndfile.h ${SNDFILE_DIRS})
    find_path(SNDFILE_LIB_DIR sndfile ${SNDFILE_DIRS})

    # NOTE: for now we only care about 32bit Windows
    set(MEX_EXT "mexw32")

elseif(APPLE)

    # TODO: test under OS X!
    message("Configuring for OS X")

    set(LIB_DIR Mac)

    set(DIRAC_LIB DiracLE)
    add_library(libDiracLE STATIC IMPORTED)
    set(SNDFILE_LIB sndfile)
    add_library(libsndfile SHARED IMPORTED)

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

    # yeah, we only support intel macs
    set(MEX_EXT "mexmaci")

elseif(UNIX)

    # TODO: after changes to make this work under Windows, test on Linux again.
    message("Configuring for Unix (Linux)")

    # enforce 32 bit compilation
    set(CMAKE_C_FLAGS "-m32")
    set(CMAKE_C_FLAGS_RELEASE "-m32")
    message("Compiler flags:\t${CMAKE_C_FLAGS}")
    message("Compiler flags (release):\t${CMAKE_C_FLAGS_RELEASE}")

    set(LIB_DIR Linux)

    set(DIRAC_LIB Dirac)
    add_library(libDirac STATIC IMPORTED)
    set(SNDFILE_LIB sndfile)
    add_library(libsndfile SHARED IMPORTED)

    set(MEX_EXT "mexglx")

else()

    message("Unsupported platform, exiting.")
    return()

endif()

# NOTE: link directories only apply to targets defined afterwards
include_directories(${MATLAB_INCLUDE_DIR} ${SNDFILE_H_DIR})
link_directories(${MDIRAC_SOURCE_DIR}/${LIB_DIR} ${SNDFILE_LIB_DIR})

# TODO: Do I need to make dirac.def conditional for non-Windows platforms or
# does CMake do the right thing by itself?
add_library(dirac MODULE dirac.c Dirac.h dirac.def)

set_target_properties(dirac PROPERTIES
    COMPILE_FLAGS ""
    LINK_FLAGS ""
    PREFIX ""
    SUFFIX ".${MEX_EXT}")

if(WIN32)
    target_link_libraries(dirac ${SNDFILE_LIB} ${MATLAB_MEX_LIBRARY} ${DIRAC_LIB} ${VECLIB_LIB})
else()
    target_link_libraries(dirac ${SNDFILE_LIB} ${DIRAC_LIB} ${VECLIB_LIB})
endif()

