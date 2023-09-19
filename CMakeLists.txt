cmake_minimum_required(VERSION 3.19)

project(LayoutReader LANGUAGES C CXX)


enable_testing()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Initialization of common build parameters
set(BUILD_TYPE_LIST "Debug" "Release")
set(OUTPUT_DIR "${CMAKE_CURRENT_LIST_DIR}/Output")

set(OUTPUT_DEBUG_DIR "${CMAKE_CURRENT_LIST_DIR}/Debug")
set(OUTPUT_RELEASE_DIR "${CMAKE_CURRENT_LIST_DIR}/Release")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")
set(PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR})

set (CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "D:\\qt\\5.12.12\\mingw73_64\\")

find_package(Boost REQUIRED)
if(Boost_FOUND)
     include_directories(${Boost_INCLUDE_DIRS})
endif()



set(SHARED_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR})


function(REMOVE_TEST_FILES_FUNCTION PATH_LIST RETURN_LIST)
   set(FILTERED_LIST "")

   foreach(CURR_PATH IN LISTS PATH_LIST)
      if(NOT "${CURR_PATH}" MATCHES "^.*[Tt][Ee][Ss][Tt].*$")
         list(APPEND FILTERED_LIST "${CURR_PATH}")
      endif()
   endforeach()

   message("FILTERED_LIST inside function: ${FILTERED_LIST}")

   set(${RETURN_LIST} ${FILTERED_LIST} PARENT_SCOPE)
endfunction()
   


add_subdirectory(Lab1)