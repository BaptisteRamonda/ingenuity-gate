cmake_minimum_required(VERSION 3.16)
project(Snapshot LANGUAGES C CXX)


macro(_find_dll_from_lib _LIB _DLL)
    # Get path and file name from lib file
    get_filename_component(_PATH_TO_FILE ${${_LIB}} DIRECTORY)
    get_filename_component(_FILE_NAME ${${_LIB}} NAME)
    string(REGEX REPLACE "\\.[^.]*$" "" _FILE_WITHOUT_EXT ${_FILE_NAME})

	# Check if DLL file exists next to the given LIB file
    find_file(${_FILE_WITHOUT_EXT}_DLL_FILE "${_FILE_WITHOUT_EXT}${CMAKE_SHARED_LIBRARY_SUFFIX}" PATHS ${_PATH_TO_FILE} NO_DEFAULT_PATH)
    if (${${_FILE_WITHOUT_EXT}_DLL_FILE} STREQUAL ${_FILE_WITHOUT_EXT}_DLL_FILE-NOTFOUND)
        message(DEBUG "File ${_PATH_TO_FILE}/${_FILE_WITHOUT_EXT}${CMAKE_SHARED_LIBRARY_SUFFIX} not found")

        # Check if libXXX.dll exists next to the given LIB file
        find_file(${_FILE_WITHOUT_EXT}_DLL_FILE "lib${_FILE_WITHOUT_EXT}${CMAKE_SHARED_LIBRARY_SUFFIX}" PATHS ${_PATH_TO_FILE} NO_DEFAULT_PATH)
        if (${${_FILE_WITHOUT_EXT}_DLL_FILE} STREQUAL ${_FILE_WITHOUT_EXT}_DLL_FILE-NOTFOUND)
            message(DEBUG "File ${_PATH_TO_FILE}/lib${_FILE_WITHOUT_EXT}${CMAKE_SHARED_LIBRARY_SUFFIX} not found")

            # Check if the DLL file exists in ../bin relatively to the given LIB file
            get_filename_component(_PATH_TO_FILE ${_PATH_TO_FILE} DIRECTORY)
            set(_PATH_TO_FILE ${_PATH_TO_FILE}/bin)
            find_file(${_FILE_WITHOUT_EXT}_DLL_FILE "${_FILE_WITHOUT_EXT}${CMAKE_SHARED_LIBRARY_SUFFIX}" PATHS ${_PATH_TO_FILE} NO_DEFAULT_PATH)
            if (${${_FILE_WITHOUT_EXT}_DLL_FILE} STREQUAL ${_FILE_WITHOUT_EXT}_DLL_FILE-NOTFOUND)
              message(DEBUG "File ${_PATH_TO_FILE}/${_FILE_WITHOUT_EXT}${CMAKE_SHARED_LIBRARY_SUFFIX} not found")

              # Check if libXXX.dll exists in ../bin relatively to the given LIB file
              find_file(${_FILE_WITHOUT_EXT}_DLL_FILE "lib${_FILE_WITHOUT_EXT}${CMAKE_SHARED_LIBRARY_SUFFIX}" PATHS ${_PATH_TO_FILE} NO_DEFAULT_PATH)
            endif()
        endif()
    endif()

    if (NOT ${${_FILE_WITHOUT_EXT}_DLL_FILE} STREQUAL ${_FILE_WITHOUT_EXT}_DLL_FILE-NOTFOUND)
        message("DLL file ${${_FILE_WITHOUT_EXT}_DLL_FILE} found from LIB file ${${_LIB}}")
        set(${_DLL} "${${_FILE_WITHOUT_EXT}_DLL_FILE}")
    else()
        message(WARNING "Could not find DLL file relative to ${${_LIB}}")
        set(${_DLL} "-NOTFOUND")
    endif()
endmacro()

set(CMAKE_OSX_DEPLOYMENT_TARGET "10.14")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
# By overriding CXX FLAGS with O2, we remove the ndebug flags, added by default
# ndebug flags remove asserts which can cause issues if you call a function into an assert
SET(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Check compiler flags availability before setting them
if (NOT MSVC)
    include(CheckCXXCompilerFlag)
    include(CheckCCompilerFlag)
    foreach(_OPT
            -fPIC
            -pipe
            -pedantic
            -Wall
            -Wextra
            -W
            -Wpointer-arith
            -Wwrite-strings
            -Wunused
            -Wshadow
            -Winline
            -Wnested-externs
            -Wno-long-long
            -Wfloat-equal
            -Wno-multichar
            -Wsign-compare
            -Wundef
            -Wno-format-nonliteral
            -Wendif-labels
            -Wstrict-prototypes
            -Wstrict-aliasing=3
            -Wcast-align
            -Wtype-limits
            -Wold-style-declaration
            -Wmissing-parameter-type
            -Wempty-body
            -Wclobbered
            -Wignored-qualifiers
            -Wconversion
            -Wno-sign-conversion
            -Wvla
            -Wdouble-promotion
            -Wno-system-headers
            -Wno-pedantic-ms-format)
        # CHECK_C_COMPILER_FLAG needs a new variable to store each new test result in.
        check_cxx_compiler_flag(${_OPT} OPT${_OPT}_CXX)
        if(OPT${_OPT}_CXX)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${_OPT}")
        endif()
        check_c_compiler_flag(${_OPT} OPT${_OPT}_C)
        if(OPT${_OPT}_C)
          set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${_OPT}")
        endif()
    endforeach()
else (NOT MSVC)
    add_definitions(-DNOMINMAX) # Do not define min/max macros
endif(NOT MSVC)

find_package(Qt6 COMPONENTS
    Concurrent
    Core
    Gui
    Quick
    QuickControls2Impl
    QuickLayouts
    QuickTemplates2
    Svg
    Test
    Widgets
    Xml
    REQUIRED
)

# Find ingescape dependency
find_package(sodium)
find_package(ZeroMQ 4.3.3 REQUIRED)
find_package(czmq 4.2.1 REQUIRED)
find_package(zyre 2.0.1 REQUIRED)
find_package(ingescape 3 REQUIRED)

if (${CI_PIPELINE_ID})
    math(EXPR pipeline_id "${CI_PIPELINE_ID}" OUTPUT_FORMAT DECIMAL)
else()
    set(pipeline_id "0")
endif()

if (APPLE)
    math(EXPR pipeline_id1 "${pipeline_id} / 1000" OUTPUT_FORMAT DECIMAL)
    math(EXPR pipeline_id2 "${pipeline_id} % 1000" OUTPUT_FORMAT DECIMAL)
    set(pipeline_id "${pipeline_id1}.${pipeline_id2}")
endif ()

# Macro to get project version from pro file
macro(get_app_version _MAJOR _MINOR _BUILD)
    file(READ ${CMAKE_CURRENT_SOURCE_DIR}/Snapshot.pro _DOT_PRO_CONTENT)
    string(REGEX MATCH "VERSION_MAJOR = ([0-9]*)" _ ${_DOT_PRO_CONTENT})
    set(${_MAJOR} ${CMAKE_MATCH_1})
    string(REGEX MATCH "VERSION_MINOR = ([0-9]*)" _ ${_DOT_PRO_CONTENT})
    set(${_MINOR} ${CMAKE_MATCH_1})
    set(${_BUILD} ${pipeline_id})
endmacro()

get_app_version(APP_VERSION_MAJOR APP_VERSION_MINOR APP_VERSION_BUILD)
set(APP_VERSION_STRING "${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}.${APP_VERSION_BUILD}")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../builds/cmake/modules")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../builds/cmake/modules/windows")


########################################################################
# sources list
########################################################################

# Note: sub directories will add also their own source list to this one
set( SOURCE_LIST
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/qmlinstrument.cpp
    ${CMAKE_CURRENT_LIST_DIR}/qml.qrc
)


########################################################################
# executable
########################################################################

qt_add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${SOURCE_LIST})

########################################################################
# sub directories
########################################################################

include(agents/agents.cmake)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../internal/I2Quick/builds/I2QuickHelper.cmake)
add_i2quick_to_target(${PROJECT_NAME})


target_compile_definitions(${PROJECT_NAME} PRIVATE VERSION="${APP_VERSION_STRING}")
target_compile_definitions(${PROJECT_NAME} PRIVATE APP_NAME="Snapshot")

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME}
  PRIVATE
  Qt6::Concurrent
  Qt6::Core
  Qt6::CorePrivate
  Qt6::Gui
  Qt6::Quick
  Qt6::QuickLayouts
  Qt6::QuickPrivate
  Qt6::QuickControls2Impl
  Qt6::QuickTemplates2
  Qt6::QuickTemplates2Private
  Qt6::QuickLayouts
  Qt6::Svg
  Qt6::Test
  Qt6::Widgets
  czmq
  ingescape
)

if (APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework Foundation -framework AppKit")
elseif (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE Wtsapi32 user32 Advapi32)
endif()

# Retrieve the absolute path to qmake and then use that path to find the binaries
get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)

if (APPLE)
    target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/icon.icns)
    set_source_files_properties("${CMAKE_CURRENT_LIST_DIR}/icon.icns" PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/macos-Info.plist.in)
    # Mac OS X bundle specific settings
    set(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")
    set(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} ${APP_VERSION_STRING}")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "${APP_VERSION_STRING}")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${APP_VERSION_MAJOR}")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${APP_VERSION_STRING}")
    set(MACOSX_BUNDLE_ICON_FILE "icon")
    set(MACOSX_BUNDLE_COPYRIGHT "Copyright © 2022-2023 Ingenuity i/o. All rights reserved.")

    # NB: macdeployqt only runs qmlimportscanner correctly when run from Qt bin directory
    find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${_qt_bin_dir}")
    add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND cd ${_qt_bin_dir} && "${MACDEPLOYQT_EXECUTABLE}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/../.." -qmldir="${CMAKE_CURRENT_LIST_DIR}" -no-strip
        COMMENT "Deploying Qt..."
    )

    set(CMAKE_SKIP_INSTALL_RPATH TRUE)
elseif (WIN32)
    if(MSVC) # Check if we are using the Visual Studio compiler
        # This line avoid the appearance of a console windows when runnin the GUI program
        set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
    endif(MSVC)

    configure_file(${CMAKE_CURRENT_LIST_DIR}/../../builds/cmake/modules/windows/Snapshot_version.rc.in ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.rc)
    target_sources(${PROJECT_NAME}
        PRIVATE
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.rc"
    )

    if(MSVC)
        file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/stable.cpp "#include <stable.h>")
        target_compile_options(${PROJECT_NAME} PRIVATE /Yu"stable.h" /FI"stable.h" /Fp${CMAKE_CURRENT_BINARY_DIR}/stable.pch)
        set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/stable.cpp
            PROPERTIES
                COMPILE_FLAGS "/Yc\"stable.h\" /Fp\"${CMAKE_CURRENT_BINARY_DIR}/stable.pch\""
                OBJECT_OUTPUTS "${CMAKE_CURRENT_BINARY_DIR}/stable.pch"
        )
        target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/stable.cpp)
    endif()

    # Copy ingescape and its dependencies
    message(STATUS "ZeroMQ DLL from CMake target: ${ZeroMQ_LIBRARY}")
    find_library(SODIUM_LIB sodium)
    _find_dll_from_lib(SODIUM_LIB SODIUM_DLL)
    find_library(CZMQ_LIB czmq)
    _find_dll_from_lib(CZMQ_LIB CZMQ_DLL)
    find_library(ZYRE_LIB zyre)
    _find_dll_from_lib(ZYRE_LIB ZYRE_DLL)
    find_library(IGS_LIB ingescape)
    _find_dll_from_lib(IGS_LIB IGS_DLL)

    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${ZeroMQ_LIBRARY}"
			"${SODIUM_DLL}"
			"${CZMQ_DLL}"
			"${ZYRE_DLL}"
			"${IGS_DLL}"
			$<TARGET_FILE_DIR:${PROJECT_NAME}>
    )

	install(FILES
			"${ZeroMQ_LIBRARY}"
			"${SODIUM_DLL}"
			"${CZMQ_DLL}"
			"${ZYRE_DLL}"
			"${IGS_DLL}"
			DESTINATION .)

    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        include(Windeploy)
        windeployqt(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR})
    endif ()
else ()
    #TODO Add unix target
endif ()

########################################################################
# installer
########################################################################
# Package installer just for release build
# NOTE: Windows packaging has been moved to the packaging directory
#       See .gitlab-ci.yml for commands to generate the Windows NSIS installer
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    install(TARGETS ${PROJECT_NAME} DESTINATION .)

    set(CPACK_MONOLITHIC_INSTALL ON)
    set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME}")
    set(CPACK_PACKAGE_VENDOR "Ingenuity IO")
    set(CPACK_PACKAGE_CONTACT "contact@ingenuity.io")
    set(CPACK_PACKAGE_VERSION ${APP_VERSION_STRING})
    set(CPACK_PACKAGE_VERSION_MAJOR "${APP_VERSION_MAJOR}")
    set(CPACK_PACKAGE_VERSION_MINOR "${APP_VERSION_MINOR}")
    set(CPACK_PACKAGE_VERSION_BUILD "${APP_VERSION_BUILD}")
    set(CPACK_PACKAGE_VERSION_PATCH "${APP_VERSION_PATCH}")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")

    if (APPLE)
        set(CPACK_GENERATOR "DragNDrop")
    elseif(WIN32)
        set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
        set(CPACK_NSIS_MUI_FINISHPAGE_RUN ${CPACK_PACKAGE_NAME}.exe)
        set(CPACK_GENERATOR "NSIS;ZIP")
    else ()
        #TODO Add unix target
    endif ()

    set(CPACK_PACKAGE_EXECUTABLES "${PROJECT_NAME}" "${PROJECT_NAME}")
    set(CPACK_CREATE_DESKTOP_LINKS "${PROJECT_NAME}" "${PROJECT_NAME}")

    include (CPack)
endif()

########################################################################
# summary
########################################################################
message ("")
message (STATUS "******************* Configuration Summary *******************")
message (STATUS "General:")
message (STATUS "  System            :   ${CMAKE_SYSTEM_NAME}")
message (STATUS "  CXX compiler      :   ${CMAKE_CXX_COMPILER}")
message (STATUS "")
message (STATUS "Dependencies:")
include(FeatureSummary)
feature_summary (WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

