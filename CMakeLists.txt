# /*
#     Copyright (C) 2016-2025 zafaco GmbH
#     Copyright (C) 2019 alladin-IT GmbH

#     This program is free software: you can redistribute it and/or modify
#     it under the terms of the GNU Affero General Public License version 3
#     as published by the Free Software Foundation.

#     This program is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU Affero General Public License for more details.

#     You should have received a copy of the GNU Affero General Public License
#     along with this program.  If not, see <http://www.gnu.org/licenses/>.
# */

cmake_minimum_required(VERSION 3.6)

set(CMAKE_BUILD_TYPE Release)

#---------------------------------------------------------------------------------

if (DEFINED ANDROID_ABI)
    message("building libtool for android")
    message("${CMAKE_CURRENT_LIST_DIR}")

    if (NOT DEFINED ANDROID_NDK)
        MESSAGE(FATAL_ERROR "ERROR: 'ANDROID_NDK' not exported; export the path to the android ndk")
    endif ()

    include(boringssl.cmake)
    include_directories(${boringssl_INCLUDE_DIR})
endif ()

IF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    include_directories(/opt/homebrew/opt/openssl@3/include)
    link_directories(/opt/homebrew/opt/openssl@3/lib)
endif ()

add_library(
        iastool SHARED
        tool.cpp
        connection.cpp
        configmanager.cpp
        basisthread.cpp
        trace.cpp
        http.cpp
        json11.cpp
        sha1.cpp
)

if (DEFINED ANDROID_ABI)
    add_dependencies(iastool boringssl)
    target_link_libraries(
            iastool
            ${boringssl_BUILD}/ssl/libssl.a
            ${boringssl_BUILD}/crypto/libcrypto.a
    )
else ()
    target_link_libraries(
            iastool
            ssl
            crypto
    )
endif ()