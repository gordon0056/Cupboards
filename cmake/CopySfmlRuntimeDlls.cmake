# CopySfmlRuntimeDlls.cmake
# Copies SFML runtime DLLs (SFML shared libs + external deps like OpenAL)
# to a target directory so that test executables can run during
# catch_discover_tests / gtest_discover_tests (POST_BUILD steps).

cmake_minimum_required(VERSION 3.20)

if(NOT SFML_BINARY_DIR OR NOT TARGET_DIR)
    message(FATAL_ERROR "SFML_BINARY_DIR and TARGET_DIR are required")
endif()

if(NOT SFML_ARCH)
    set(SFML_ARCH "x64")
endif()

set(DLLS "")

# 1) Built SFML libraries (static build → no DLLs, but shared build → look in lib/bin)
file(GLOB_RECURSE BLD_DLLS
    "${SFML_BINARY_DIR}/lib/${SFML_ARCH}/*.dll"
    "${SFML_BINARY_DIR}/bin/${SFML_ARCH}/*.dll"
)
list(APPEND DLLS ${BLD_DLLS})

# 2) Prebuilt extlibs bundled with SFML source (OpenAL, FLAC, OGG, Vorbis)
#    Only search the correct architecture subdirectory to avoid bitness mismatch.
if(SFML_SOURCE_DIR)
    file(GLOB_RECURSE EXT_DLLS "${SFML_SOURCE_DIR}/extlibs/bin/${SFML_ARCH}/*.dll")
    list(APPEND DLLS ${EXT_DLLS})
endif()

list(REMOVE_DUPLICATES DLLS)

if(NOT DLLS)
    message(STATUS "No SFML runtime DLLs found — skipping copy (likely fully static build)")
    return()
endif()

foreach(DLL ${DLLS})
    file(COPY "${DLL}" DESTINATION "${TARGET_DIR}")
endforeach()

list(LENGTH DLLS COUNT)
message(STATUS "Copied ${COUNT} SFML runtime DLL(s) to ${TARGET_DIR}")
