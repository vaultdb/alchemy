cmake_minimum_required(VERSION 3.10)
project(Testing)
# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download)
if (result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif ()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download)
if (result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif ()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
include(CTest)
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
        ${CMAKE_BINARY_DIR}/googletest-build
        EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
endif ()
#TODO(madhavsuresh): this was moved out of the previous if as a hack because
# the build wasn't working without explicitly including the gtest headers.
include_directories("${gtest_SOURCE_DIR}/include")

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
IF (EXISTS "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json")
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
            ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
            )
ENDIF ()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -fno-omit-frame-pointer")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=[sanitizer_name] [additional_options] [-g] [-OX]")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -g")
#set(CMAKE_CXX_FLAGS "-fno-omit-frame-pointer -fsanitize=address -g -O0")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Ofast")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -g -fno-omit-frame-pointer -fsanitize=address")
#set (CMAKE_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS_DEBUG} -g -fno-omit-frame-pointer -fsanitize=address")


#find_library(GLOG glog)
link_directories(${CMAKE_SOURCE_DIR}/lib/lib/)
include_directories(${CMAKE_SOURCE_DIR}/lib/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/rpc)
include_directories(${CMAKE_SOURCE_DIR})
#/home/madhav/DB/linux-sgx/sgxsdk/include

add_subdirectory(rpc)
add_subdirectory(data)
add_subdirectory(operators)
add_subdirectory(querytable)
#EMP-TOOLKIT OPTIONS
include(${CMAKE_SOURCE_DIR}/lib/cmake/emp-tool-config.cmake)
include(${CMAKE_SOURCE_DIR}/lib/cmake/common.cmake)
include(${CMAKE_SOURCE_DIR}/lib/cmake/source_of_randomness.cmake)
include(${CMAKE_SOURCE_DIR}/lib/cmake/threading.cmake)
message("hello ${OPENSSL_LIBRARIES}")
message("wee ${EMP-OT_LIBRARIES}")
add_executable(main main.cpp operators/filter.cpp operators/filter.h common/macros.h common/defs.h)
target_link_libraries(main profiler gflags pthread emp-tool gmp ${EMP-OT_LIBRARIES} ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES}
        dataprovider secure_operators)



