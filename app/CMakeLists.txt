# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

# C:\Users\psycho\AppData\Local\Android\Sdk\cmake\3.6.4111459\bin\cmake.exe

cmake_minimum_required(VERSION 3.4.1)

file(GLOB SOURCES "src/main/cpp/*.c")

message("${SOURCES}")

add_library(libsqlite3
        SHARED
        src/main/cpp/sqlite/sqlite3.c
        )
add_library(libmongoose
        SHARED
        src/main/cpp/mongoose/mongoose.c
        )
add_library( # Sets the name of the library.
        native-lib

        # Sets the library as a shared library.
        SHARED

        ${SOURCES})

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

include_directories(native-lib PRIVATE)

find_library( # Sets the name of the path variable.
        log-lib
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        native-lib
        libsqlite3
        libmongoose
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})