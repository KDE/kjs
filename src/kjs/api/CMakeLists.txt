set(kjsapi_LIB_SRCS
    kjsinterpreter.cpp
    kjscontext.cpp
    kjsobject.cpp
    kjsprototype.cpp
    kjsarguments.cpp
   )

add_library(KF5JSApi ${kjsapi_LIB_SRCS})
generate_export_header(KF5JSApi BASE_NAME KJSApi)
add_library(KF5::JSApi ALIAS KF5JSApi)

target_link_libraries(KF5JSApi
    PUBLIC Qt5::Core
    PRIVATE KF5::JS)

set_target_properties(KF5JSApi PROPERTIES VERSION ${KJS_VERSION_STRING}
                                          SOVERSION ${KJS_SOVERSION}
                                          EXPORT_NAME JSApi
)
install(TARGETS KF5JSApi EXPORT KF5JSTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/kjsapi_export.h
        kjsinterpreter.h
        kjscontext.h
        kjsobject.h
        kjsprototype.h
        kjsarguments.h
        DESTINATION  ${KDE_INSTALL_INCLUDEDIR_KF5}/kjs)

include(ECMGeneratePriFile)
ecm_generate_pri_file(BASE_NAME KJSApi LIB_NAME KF5JSApi DEPS "core" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/KJSApi)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
