# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

add_library(libsqlite3
        SHARED
        src/main/cpp/sqlite/sqlite3.c
        )

add_library( # Sets the name of the library.
        native-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        C:/Codes/Android/Server/app/src/main/cpp/buffer.c
        C:/Codes/Android/Server/app/src/main/cpp/connection.c
        C:/Codes/Android/Server/app/src/main/cpp/http_parser.c
        C:/Codes/Android/Server/app/src/main/cpp/log.c
        C:/Codes/Android/Server/app/src/main/cpp/lotos_epoll.c
        C:/Codes/Android/Server/app/src/main/cpp/mem_pool.c
        C:/Codes/Android/Server/app/src/main/cpp/misc.c
        C:/Codes/Android/Server/app/src/main/cpp/native-lib.c
        C:/Codes/Android/Server/app/src/main/cpp/request.c
        C:/Codes/Android/Server/app/src/main/cpp/response.c
        C:/Codes/Android/Server/app/src/main/cpp/server.c
        C:/Codes/Android/Server/app/src/main/cpp/ssstr.c
        )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

include_directories(native-lib PRIVATE)

find_library( # Sets the name of the path variable.
        log-lib
        libsqlite3
        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        native-lib

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})