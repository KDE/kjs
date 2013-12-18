cmake_minimum_required(VERSION 2.8.12)

project(KJS)

# ECM setup
find_package(ECM 0.0.9 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(FeatureSummary)
include(CMakePackageConfigHelpers)
include(ECMSetupVersion)

ecm_setup_version(5.0.0 VARIABLE_PREFIX KJS
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kjs_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5JSConfigVersion.cmake")

# Dependencies
include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings)
include(KDECMakeSettings)

set(REQUIRED_QT_VERSION 5.2.0)
find_package(Qt5Core ${REQUIRED_QT_VERSION} REQUIRED NO_MODULE)

find_package(Perl)
set_package_properties(Perl PROPERTIES URL "http://www.perl.org" TYPE REQUIRED PURPOSE "Needed for building kdelibs")

#Do not make PCRE optional here. PCRE is a hard requirement for modern systems
#but we give old systems some slack... that's why we don't specify "REQUIRED".
find_package(PCRE)

set_package_properties(PCRE PROPERTIES
                       URL "http://www.pcre.org"
                       DESCRIPTION "Perl-compatible regular expressions in KJS"
                       TYPE OPTIONAL
                       PURPOSE "Without PCRE, KJS will have extremely poor regular expression support, breaking many webpages.")

option(KJS_FORCE_DISABLE_PCRE "Force building of KJS without PCRE. Doing this will result in many webpage working incorrectly, due to extremely poor regular expression support")

# TODO: Remove these
remove_definitions(-DQT_NO_CAST_FROM_ASCII)

add_subdirectory(src)
add_subdirectory(autotests)
add_subdirectory(tests)

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/KF5JS")

configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/KF5JSConfig.cmake.in"
                              "${CMAKE_CURRENT_BINARY_DIR}/KF5JSConfig.cmake"
                              INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
                              )

install(FILES  "${CMAKE_CURRENT_BINARY_DIR}/KF5JSConfig.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/KF5JSConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel )

install(EXPORT KF5JSTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KF5JSTargets.cmake NAMESPACE KF5:: )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kjs_version.h DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel )

if ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
    feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
endif()
