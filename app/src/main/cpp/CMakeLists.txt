# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)
include_directories(
        include/config.h
)

# Enable C++11
set(CMAKE_CXX_STANDARD 20)
# Declares the project name. The project name can be accessed via ${ PROJECT_NAME},
# Since this is the top level CMakeLists.txt, the project name is also accessible
# with ${CMAKE_PROJECT_NAME} (both CMake variables are in-sync within the top level
# build script scope).
project("monitor")
enable_language(ASM)
link_libraries(z)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
#
# In this top level CMakeLists.txt, ${CMAKE_PROJECT_NAME} is used to define
# the target library name; in the sub-module's CMakeLists.txt, ${PROJECT_NAME}
# is preferred for the same purpose.
#
# In order to load a library into your app from Java/Kotlin, you must call
# System.loadLibrary() and pass the name of the library defined here;
# for GameActivity/NativeActivity derived applications, the same library name must be
# used in the AndroidManifest.xml file.
add_library(
        # so 文件的名字
        devicecheck
        # 共享库
        SHARED
        # List C/C++ source files with relative paths to this CMakeLists.txt.
        include/deviceDetection.h
        dectection/deviceDetection.cpp
        dectection/jni_device.cpp
)
add_library(
        hookcheck
        SHARED
        utils/local_dlfcn.h
        utils/local_dlfcn.cpp
        asm/bionic_asm.h
        asm/syscall.S
        include/elf_util.h
        include/hookDetection.h
        utils/elf_util.cpp
        dectection/hookDetection.cpp
        dectection/jni_hook.cpp
)
## Specifies libraries CMake should link to your target library. You
## can link libraries from various origins, such as libraries defined in this
## build script, prebuilt third-party libraries, or Android system libraries.
target_link_libraries(
        devicecheck
        # List libraries link to the target library
        android
        log
)
target_link_libraries(
        hookcheck
        # List libraries link to the target library
        android
        log
)