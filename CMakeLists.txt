project(BWXML)
cmake_minimum_required( VERSION 2.8 )

#
# Compiler options
#
if(MSVC)
    set(Boost_USE_STATIC_LIBS ON)
else(MSVC)
    list(APPEND CMAKE_CXX_FLAGS "-std=c++0x")
endif(MSVC)

#
# Depends
#
find_package(Boost COMPONENTS filesystem program_options serialization system thread REQUIRED)

if (NOT Boost_FOUND)
    message(FATAL_ERROR "Fatal error: Boost required.\n")
endif (NOT Boost_FOUND)

#
# Build
#
aux_source_directory(BWXML SOURCES)
link_directories(${Boost_LIBRARY_DIRS})
include_directories(BWXML ${Boost_INCLUDE_DIRS})

file (GLOB headers BWXML/*.h*)
add_executable(bwxml ${SOURCES} ${headers})
target_link_libraries(bwxml ${Boost_LIBRARIES})

#
# Installation
#
if(WIN32)
    install(TARGETS bwxml RUNTIME DESTINATION .)
else(WIN32)
    install(TARGETS bwxml RUNTIME DESTINATION bin)
endif(WIN32)