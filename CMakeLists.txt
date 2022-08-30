cmake_minimum_required(VERSION 3.5)

project(HMI_agent VERSION 0.1 LANGUAGES CXX)

# ==== add compiler setting flag ===================================#
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")     
    # TODO: compile static qt libraries then link with static
# ================================================================= #
# ==== add QT code generator =======================================#
set(CMAKE_AUTOUIC ON)   # .ui file (gui design file)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)   # .qrc file (resources files)
# ================================================================= #
# ==== find used QT package ======================================= #
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools SerialPort)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools SerialPort)
# find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS SerialComboBox)
qt_standard_project_setup()
# ================================================================= #
# ===== add translator ============================================ #
set(TS_FILES HMI_agent_zh_TW.ts HMI_agent_en_US.ts)
qt_add_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

# ================================================================= #
# ===== add executable ============================================ #
set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        serialcombobox.cpp
        serialcombobox.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(HMI_agent
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET HMI_agent APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
     qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
    if(ANDROID)
        add_library(HMI_agent SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(HMI_agent
            ${PROJECT_SOURCES}
        )
    endif()
endif()

qt_add_resources(HMI_agent "icons"
    PREFIX "/"
    BASE ${PROJECT_BINARY_DIR}
    FILES
    ${PROJECT_BINARY_DIR}/HMI_agent_en_US.qm
    ${PROJECT_BINARY_DIR}/HMI_agent_zh_TW.qm
)
# ================================================================= #
# ===== add QT/custom libraries ============================================ #
target_link_libraries(HMI_agent PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::SerialPort)
add_subdirectory(asaloader/asaloader/loader)
target_link_libraries(${PROJECT_NAME} PRIVATE asaloaderlib)
# ================================================================= #
set_target_properties(HMI_agent PROPERTIES
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(HMI_agent)
endif()
# ===== add install =====　The following script must only be executed at install time ======= #
set(deploy_script "${CMAKE_CURRENT_BINARY_DIR}/deploy_${PROJECT_NAME}.cmake")
set(executable_path "\${QT_DEPLOY_BIN_DIR}/$<TARGET_FILE_NAME:${PROJECT_NAME}>") 
file(GENERATE OUTPUT ${deploy_script} CONTENT "
# Including the file pointed to by QT_DEPLOY_SUPPORT ensures the generated
# deployment script has access to qt_deploy_runtime_dependencies()
include(\"${QT_DEPLOY_SUPPORT}\")
qt_deploy_runtime_dependencies(
    EXECUTABLE \"${executable_path}\"
    GENERATE_QT_CONF    # MUST NEED, when deploy in release, the path is different without it.
    VERBOSE
)")

# Omitting RUNTIME DESTINATION will install a non-bundle target to CMAKE_INSTALL_BINDIR,
# which coincides with the default value of QT_DEPLOY_BIN_DIR used above, './bin'.
# Installing macOS bundles always requires an explicit BUNDLE DESTINATION option.
install(TARGETS ${PROJECT_NAME} 
        BUNDLE  DESTINATION .    
)
install(SCRIPT ${deploy_script})    # Add its runtime dependencies
# ================================================================= #