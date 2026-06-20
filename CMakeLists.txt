cmake_minimum_required(VERSION 3.16)
project(termacs_java CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# --- termacs-core dependency: SOURCE, never a published binary (spec §15.1) ---
# Dev (monorepo cockpit): build against the local sibling checkout — no tag
# round-trip for coupled edits. Release (R4): clear TERMACS_CORE_DIR so it
# clones the pinned GitHub tag instead.
include(FetchContent)
set(TERMACS_CORE_DIR "" CACHE PATH
    "Local termacs-core checkout (dev override). Empty => fetch the pinned tag.")
if(NOT TERMACS_CORE_DIR AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../termacs-core/CMakeLists.txt")
    set(TERMACS_CORE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../termacs-core")  # auto: monorepo sibling
endif()
# Remote source (used when no local override): pinned tag, overridable URL/tag
# so CI or a mirror can redirect without editing this file.
set(TERMACS_CORE_GIT "https://github.com/nidwe72/termacs-core.git" CACHE STRING "termacs-core git URL")
set(TERMACS_CORE_TAG "v0.2.0" CACHE STRING "termacs-core git tag")
if(TERMACS_CORE_DIR)
    message(STATUS "termacs-java: building against local termacs-core at ${TERMACS_CORE_DIR}")
    FetchContent_Declare(termacs_core SOURCE_DIR "${TERMACS_CORE_DIR}")
else()
    message(STATUS "termacs-java: fetching termacs-core ${TERMACS_CORE_TAG} from ${TERMACS_CORE_GIT}")
    FetchContent_Declare(termacs_core
        GIT_REPOSITORY "${TERMACS_CORE_GIT}"
        GIT_TAG "${TERMACS_CORE_TAG}")
endif()
FetchContent_MakeAvailable(termacs_core)

# --- JNI shim -> one self-contained libtermacsjni.so (JNI + C ABI + core + tvision) ---
find_package(JNI REQUIRED)
add_library(termacsjni SHARED native/termacs_jni.cpp)
target_include_directories(termacsjni PRIVATE ${JNI_INCLUDE_DIRS})
target_link_libraries(termacsjni PRIVATE termacs)  # static core: pulls the C ABI + tvision
set_target_properties(termacsjni PROPERTIES
    CXX_VISIBILITY_PRESET hidden VISIBILITY_INLINES_HIDDEN ON)
# Export only the JNI entry points; hide C ABI/core/tvision internals (§12.3).
target_link_options(termacsjni PRIVATE
    "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/native/jni.map")
