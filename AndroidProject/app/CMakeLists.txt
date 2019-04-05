# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

if(NOT DEFINED ${ENABLE_DEBUG})
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        set(ENABLE_DEBUG "TRUE")
    else()
        set(ENABLE_DEBUG "FALSE")
    endif()
endif()

if(${ENABLE_DEBUG} MATCHES "TRUE")
    add_compile_options(-O0)
    add_definitions(-DDEBUG -D_DEBUG)
else()
    add_compile_options(-Ofast -fno-rtti)
    add_definitions(-DNDEBUG -D_NDEBUG)
endif()

add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-std=c++1z> $<$<COMPILE_LANGUAGE:C>:-std=c99>)
add_compile_options(-Wno-missing-braces -Wno-inconsistent-missing-override -Wpessimizing-move -Wlarge-by-value-copy -Winstantiation-after-specialization -Wself-assign)
add_compile_options(-fno-stack-protector -D_FORTIFY_SOURCE=0 -fno-sanitize=memory -fno-exceptions -fvisibility=hidden -fvisibility-inlines-hidden)

set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld")
set (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -fuse-ld=lld")
set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=lld")

add_definitions(-DPLATFORM_ANDROID)

file(GLOB CORE_FILES "../../Core/*.cpp")
file(GLOB PLATFORMABSTRACTION_FILES "../../PlatformAbstraction/*.cpp")
file(GLOB MISCELLANEOUS_FILES "../../Miscellaneous/*.cpp")
file(GLOB UNITTEST_FILES "../../UnitTests/*.cpp")

foreach(item ${PLATFORMABSTRACTION_FILES})
  if(${item} MATCHES "Win_.*cpp")
    list(REMOVE_ITEM PLATFORMABSTRACTION_FILES ${item})
  endif()
endforeach()

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library(
    # Sets the name of the library.
    native-lib

    # Sets the library as a shared library.
    SHARED

    # Provides a relative path to your source file(s).
    src/main/cpp/native-lib.cpp
    ${CORE_FILES}
    ${PLATFORMABSTRACTION_FILES}
    ${MISCELLANEOUS_FILES}
    ${UNITTEST_FILES}
)

include_directories(
    ../../Core/
    ../../PlatformAbstraction/
    ../../Miscellaneous/
    ../../UnitTests/
)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       native-lib

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )