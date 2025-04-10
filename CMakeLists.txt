cmake_minimum_required(VERSION 3.16)

# Define the project
project(FileWatson)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find wxWidgets
find_package(wxWidgets REQUIRED COMPONENTS core base)

# Find nlohmann json
find_package(nlohmann_json REQUIRED)

# wx-config required only on non-Windows platforms
if (NOT WIN32)
    # Find wx-config
    find_program(WX_CONFIG wx-config REQUIRED)

    if(NOT WX_CONFIG)
        message(FATAL_ERROR "wx-config not found. Install wxWidgets or set WX_CONFIG path.")
    endif()

    # Use wx-config to get compilation and linking flags
    execute_process(
        COMMAND ${WX_CONFIG} --cxxflags
        OUTPUT_VARIABLE WX_CXX_FLAGS
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    execute_process(
        COMMAND ${WX_CONFIG} --libs
        OUTPUT_VARIABLE WX_LIBS
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    # Set the flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WX_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${WX_LIBS}")
endif()

# Define the include directories
include_directories(include)
include_directories(resources)

# Include wxWidgets headers
include_directories(${wxWidgets_INCLUDE_DIRS})
include_directories(${NLOHMANN_JSON_INCLUDE_DIR})

# Link wxWidgets libraries
link_directories(${wxWidgets_LIBRARY_DIRS})

# Define the source files
file(GLOB_RECURSE SOURCES "src/*.cc")

# Add platform-specific icons
if(WIN32)
    list(APPEND SOURCES resources/icon/icon.rc)
elseif(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE "icon.icns")
    set_source_files_properties(resources/icon/icon.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    list(APPEND SOURCES resources/icon/icon.icns)
endif()

# Create the executable
add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${SOURCES})

# Link wxWidgets
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})

# Link dbus library on Linux
if (UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(DBUS REQUIRED dbus-1)

    include_directories(${DBUS_INCLUDE_DIRS})
    link_directories(${DBUS_LIBRARY_DIRS})
    target_link_libraries(${PROJECT_NAME} ${DBUS_LIBRARIES})
endif()

# Define the path to the resources/lang folder
set(LANG_DIR "${CMAKE_SOURCE_DIR}/resources/lang")

# Define the output directory for lang files
set(OUTDIR "${CMAKE_BINARY_DIR}/lang")

# Define the output directory for lang files
if (APPLE)
    set(LANG_OUTPUT_DIR "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.app/Contents/Resources/lang")
else()
    set(LANG_OUTPUT_DIR "${CMAKE_BINARY_DIR}/lang")
endif()

# Custom target to copy lang folder
add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${LANG_OUTPUT_DIR}"  # Remove existing folder
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${LANG_DIR}" "${LANG_OUTPUT_DIR}"  # Copy new folder
    COMMENT "Copying language files to ${LANG_OUTPUT_DIR}"
)

# ==========================
# INSTALL CONFIGURATION
# ==========================

if (UNIX AND NOT APPLE) 
# 1. Install the binary to /usr/local/bin/
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# 2. Install a symlink as `fw`
# This creates a symlink called `fw` pointing to FileWatson
install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} 
    -E create_symlink FileWatson \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/fw\")")

# 3. Install lang directory to /opt/fw/lang/
install(DIRECTORY ${LANG_DIR}/
    DESTINATION /opt/fw/lang
    FILES_MATCHING PATTERN "*.lang"
    PATTERN "*"
)
elseif (WIN32)
    # Get the Program Files path 
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)  # 64-bit system
        set(PROGRAM_FILES_DIR $ENV{ProgramW6432})
    else()  # 32-bit or 32-bit app on 64-bit system
        set(PROGRAM_FILES_DIR $ENV{ProgramFiles})
    endif()

    # Set the installation directory inside Program Files
    set(INSTALL_DIR "${PROGRAM_FILES_DIR}/FileWatson")

    # Ensure the FileWatson directory exists
    file(MAKE_DIRECTORY "${INSTALL_DIR}")

    # Install binary to the correct location in Program Files
    install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION "${INSTALL_DIR}"
    )

    # Copy lang directory from source to Program Files/FileWatson/lang
    install(DIRECTORY ${LANG_DIR}/
        DESTINATION "${INSTALL_DIR}/lang"
        FILES_MATCHING PATTERN "*.lang"
    )

    # Create a Start Menu shortcut using CPack
    set(CPACK_CREATE_DESKTOP_LINKS ${PROJECT_NAME})
    set(CPACK_NSIS_MENU_LINKS
        "FileWatson" "FileWatson"
    )
    set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
endif()