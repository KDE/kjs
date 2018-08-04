cmake_minimum_required(VERSION 3.0)

set(KF5_VERSION "5.49.0") # handled by release scripts
project(KJS VERSION ${KF5_VERSION})

# ECM setup
include(FeatureSummary)
find_package(ECM 5.49.0  NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://projects.kde.org/projects/kdesupport/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)


include(GenerateExportHeader)
include(CMakePackageConfigHelpers)
include(ECMSetupVersion)
include(ECMMarkNonGuiExecutable)

ecm_setup_version(PROJECT VARIABLE_PREFIX KJS
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kjs_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5JSConfigVersion.cmake"
                        SOVERSION 5)

# Dependencies
include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)

# Most of KJS doesn't even require Qt, thus we can't use override 
string(REPLACE "-Wsuggest-override" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(REQUIRED_QT_VERSION 5.8.0)
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

find_package(KF5DocTools ${KF5_DEP_VERSION})

add_subdirectory(src)
if (BUILD_TESTING)
    add_subdirectory(autotests)
    add_subdirectory(tests)
endif()
if(KF5DocTools_FOUND)
    add_subdirectory(docs)
endif()

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5JS")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KF5JSConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KF5JSConfig.cmake"
    PATH_VARS KDE_INSTALL_DATADIR_KF5
    INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
)

install(FILES  "${CMAKE_CURRENT_BINARY_DIR}/KF5JSConfig.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/KF5JSConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel )

install(EXPORT KF5JSTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KF5JSTargets.cmake NAMESPACE KF5:: )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kjs_version.h DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5} COMPONENT Devel )

if(KF5DocTools_FOUND)
    kdoctools_install(po)
endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
