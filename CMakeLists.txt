project(BWXML)
cmake_minimum_required( VERSION 2.8 )

#
# Depends
#
find_package(Boost COMPONENTS system filesystem program_options thread REQUIRED)

if (NOT Boost_FOUND)
    message(FATAL_ERROR "Fatal error: Boost required.\n")
endif (NOT Boost_FOUND)

#
# Compiler options
#
if(NOT MSVC)
    list(APPEND CMAKE_CXX_FLAGS "-std=c++0x")
endif(NOT MSVC)

#
# Build
#
aux_source_directory(BWXML SOURCES)
link_directories(${Boost_LIBRARY_DIRS})
include_directories(BWXML ${Boost_INCLUDE_DIRS})

add_executable(bwxml ${SOURCES})
target_link_libraries(bwxml ${Boost_LIBRARIES})

#
# Installation
#
if(WIN32)
    install(TARGETS bwxml RUNTIME DESTINATION .)
else(WIN32)
    install(TARGETS bwxml RUNTIME DESTINATION bin)
endif(WIN32)