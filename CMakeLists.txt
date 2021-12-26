cmake_minimum_required(VERSION 3.14)
project(
    QRpc
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 -Wall -Wextra -Werror -Wno-unused-function")

if(NOT DEFINED QT_DEFAULT_MAJOR_VERSION)
    set(QT_DEFAULT_MAJOR_VERSION 5)
endif()

find_package(Qt5Core)
find_package(Qt5Network)
find_package(Qt5Sql)
find_package(Qt5WebSockets)

include_directories(${CMAKE_SOURCE_DIR}/../qstm/src)
include_directories(${CMAKE_SOURCE_DIR}/src/3rdparty/qtwebapp/httpserver)

if(MSVC)
    add_compile_options(/W3 /WX /Zc:lambda)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

#find_package(Qt5 COMPONENTS Core Network Sql WebSockets REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Network_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Sql_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5WebSockets_EXECUTABLE_COMPILE_FLAGS}")

include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Network_INCLUDE_DIRS})
include_directories(${Qt5Sql_INCLUDE_DIRS})
include_directories(${Qt5WebSockets_INCLUDE_DIRS})

add_subdirectory(src)

foreach(plugin ${Qt5Core_PLUGINS})
  get_target_property(_loc ${plugin} LOCATION)
  message("Plugin ${plugin} is at location ${_loc}")
endforeach()

foreach(plugin ${Qt5Network_PLUGINS})
  get_target_property(_loc ${plugin} LOCATION)
  message("Plugin ${plugin} is at location ${_loc}")
endforeach()

foreach(plugin ${Qt5Sql_PLUGINS})
  get_target_property(_loc ${plugin} LOCATION)
  message("Plugin ${plugin} is at location ${_loc}")
endforeach()

foreach(plugin ${Qt5WebSockets_PLUGINS})
  get_target_property(_loc ${plugin} LOCATION)
  message("Plugin ${plugin} is at location ${_loc}")
endforeach()

file(
    WRITE "${CMAKE_BINARY_DIR}/.clang-tidy"
    "
---
Checks: '-*,llvm-twine-local'
...
"
)

#feature_summary(
#    INCLUDE_QUIET_PACKAGES
#    WHAT ALL
#    FATAL_ON_MISSING_REQUIRED_PACKAGES
#)
