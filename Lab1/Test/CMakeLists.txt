set(TARGET_NAME Lab1)

file(GLOB_RECURSE TEST_SRCS ${CMAKE_CURRENT_LIST_DIR} *.cpp *.c)
file(GLOB_RECURSE TEST_HDRS ${CMAKE_CURRENT_LIST_DIR} *.hpp *.h)


message(STATUS "SRCS = ${SRCS} ${TEST_SRCS}")
message(STATUS "HDRS = ${HDRS} ${TEST_HDRS}")

add_executable(${TARGET_NAME} ${SRCS} ${TEST_SRCS}  ${HDRS} ${TEST_HDRS})
target_compile_definitions(${TARGET_NAME} PUBLIC "TEST_FILES_DIR=\"${CMAKE_CURRENT_LIST_DIR}/TestFiles\"")
message(STATUS "LAB1 include dirs : ${HEADERS_DIR} ${SHARED_INCLUDE_DIRS}")
target_include_directories(${TARGET_NAME} PRIVATE ${HEADERS_DIR} ${SHARED_INCLUDE_DIRS})
target_link_libraries(${TARGET_NAME} PUBLIC ${GTEST_LIBRARIES} )
