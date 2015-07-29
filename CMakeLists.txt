cmake_minimum_required(VERSION 2.8)

project(hdf5)

find_package(HDF5 REQUIRED COMPONENTS C HL)
find_package(Boost REQUIRED COMPONENTS system)
find_package(MPI REQUIRED)
find_package(GTest REQUIRED) 

enable_testing()

set (CMAKE_CXX_FLAGS "-std=c++11")

include_directories(  
  ${GTEST_INCLUDE_DIRS}
  ${HDF5_INCLUDE_DIRS}
  ${MPI_INCLUDE_PATH}
  ${boost_INCLUDE_DIRS}
  )

include_directories(..)

add_executable(hdf5_tests test-hdf.cpp )
target_link_libraries(hdf5_tests
  ${Boost_LIBRARIES}
  ${GTEST_BOTH_LIBRARIES}
  ${HDF5_LIBRARIES}
  ${HDF5_LIBRARIES}
  ${MPI_EXTRA_LIBRARY}
  ${MPI_LIBRARY}
  )

