cmake_minimum_required(VERSION 2.6)

# NOTE: various variables start with the project name, such as *_SOURCE_DIR
project(MDIRAC C)

set(CMAKE_MODULE_PATH "${MDIRAC_SOURCE_DIR}")

# find matlab related stuff (thank god somebody already wrote this stuff)
# NOTE: I had to modify FindMatlab.cmake, it's nowhere near all-encompassing.
# It'll probably have to be adapted further. I hope I'll find the time to send a
# patch upstream (though maybe CMake 2.8 has an updated version).
include(FindMatlab)
include_directories(${MATLAB_INCLUDE_DIR})
message("${MATLAB_INCLUDE_DIR}")

# NOTE: this doesn't really do what I want, which is bail on configure when a
# header isn't found instead of at compile time.
# check_include_file(sndfile.h SNDFILE_H)
# if(!SNDFILE_H)
    # message("sndfile.h not found!")
# endif()

# TODO: why must "lib" be prepended to library names?
if(WIN32)

    # FIXME: dlls are not found at runtime because they have to be in the same
    # folder as the executable, but they are not copied there
    message("Configuring for Windows")
    set(LIB_DIR Win)
    set(DIRAC_LIB DiracLE)
    add_library(libDiracLE SHARED IMPORTED)
    set(MEX_EXT "mexglx")

elseif(APPLE)

    # TODO: test under OS X!
    message("Configuring for OS X")
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

elseif(UNIX)

    message("Configuring for Unix (Linux)")

    # enforce 32 bit compilation
    set(CMAKE_C_FLAGS "-m32")
    set(CMAKE_C_FLAGS_RELEASE "-m32")
    message("Compiler flags:\t${CMAKE_C_FLAGS}")
    message("Compiler flags (release):\t${CMAKE_C_FLAGS_RELEASE}")

    set(LIB_DIR Linux)
    set(DIRAC_LIB Dirac)
    set(MEX_EXT "mexglx")
    add_library(libDirac STATIC IMPORTED)
    add_library(libsndfile SHARED IMPORTED)
else()

    message("Unsupported platform, exiting.")
    return()

endif()

# NOTE: link directories only apply to targets defined afterwards
link_directories(${MDIRAC_SOURCE_DIR}/${LIB_DIR})
add_library(dirac MODULE dirac.c Dirac.h)

set_target_properties(dirac PROPERTIES
    COMPILE_FLAGS ""
    LINK_FLAGS ""
    PREFIX ""
    SUFFIX ".${MEX_EXT}")

target_link_libraries(dirac sndfile ${DIRAC_LIB} ${VECLIB_LIB})
