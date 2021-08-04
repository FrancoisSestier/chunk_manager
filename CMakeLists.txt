cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(
  chunk_manager
  VERSION 0.1
  LANGUAGES CXX
)



if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

option(CKM_DONT_INSTALL_DEPS "intall external dep mls" OFF)

include(cmake/tools.cmake)
include(cmake/CPM.cmake)

CPMAddPackage("gh:TheLartians/PackageProject.cmake@1.6.0")

if (CKM_DONT_INSTALL_DEPS)
  find_package(mls REQUIRED)
  find_package(mls REQUIRED)

else()
  CPMAddPackage("gh:FrancoisSestier/matrix_layer_stack#fa02865b9c614f728a1c6bc7881ca8a046432975")
  CPMAddPackage(
  NAME fmt
  GITHUB_REPOSITORY  fmtlib/fmt
  GIT_TAG 8.0.1
  OPTIONS "FMT_HEADER_ONLY YES"  "FMT_INSTALL YES"
  )

endif()

#CPMAddPackage("gh:FrancoisSestier/glm#941395c1187409de06386852376180b42ddc7ca2") 
file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

add_library(chunk_manager_headers EXCLUDE_FROM_ALL ${headers})
set_target_properties(chunk_manager_headers PROPERTIES LINKER_LANGUAGE CXX)

add_library(chunk_manager INTERFACE)

target_link_libraries(chunk_manager INTERFACE mls)
target_link_libraries(chunk_manager INTERFACE fmt)

set_target_properties(chunk_manager PROPERTIES INTERFACE_COMPILE_FEATURES cxx_std_20)
target_compile_options(chunk_manager INTERFACE "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->")

target_include_directories(chunk_manager
INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/${PROJECT_NAME}-${PROJECT_VERSION}>
)

packageProject(
  NAME ${PROJECT_NAME}
  VERSION ${PROJECT_VERSION}
  BINARY_DIR ${PROJECT_BINARY_DIR}
  INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include
  INCLUDE_DESTINATION include/${PROJECT_NAME}-${PROJECT_VERSION}
)