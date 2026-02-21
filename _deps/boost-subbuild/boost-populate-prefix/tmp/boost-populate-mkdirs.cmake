# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/Users/morbo/CLionProjects/prologix_emulator/_deps/boost-src")
  file(MAKE_DIRECTORY "/Users/morbo/CLionProjects/prologix_emulator/_deps/boost-src")
endif()
file(MAKE_DIRECTORY
  "/Users/morbo/CLionProjects/prologix_emulator/_deps/boost-build"
  "/Users/morbo/CLionProjects/prologix_emulator/_deps/boost-subbuild/boost-populate-prefix"
  "/Users/morbo/CLionProjects/prologix_emulator/_deps/boost-subbuild/boost-populate-prefix/tmp"
  "/Users/morbo/CLionProjects/prologix_emulator/_deps/boost-subbuild/boost-populate-prefix/src/boost-populate-stamp"
  "/Users/morbo/CLionProjects/prologix_emulator/_deps/boost-subbuild/boost-populate-prefix/src"
  "/Users/morbo/CLionProjects/prologix_emulator/_deps/boost-subbuild/boost-populate-prefix/src/boost-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/morbo/CLionProjects/prologix_emulator/_deps/boost-subbuild/boost-populate-prefix/src/boost-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/morbo/CLionProjects/prologix_emulator/_deps/boost-subbuild/boost-populate-prefix/src/boost-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
