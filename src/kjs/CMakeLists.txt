if(NOT WIN32)
    set(CMAKE_REQUIRED_LIBRARIES "-lm")
endif()

set(HAVE_PCREPOSIX ${PCRE_FOUND})

# Configuration checks
include(ConfigureChecks.cmake)
# Generate global.h
check_cxx_compiler_flag(-fvisibility=hidden __KDE_HAVE_GCC_VISIBILITY)
configure_file(global.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/global.h )

include_directories(
  ${KDEWIN_INCLUDES}
)

# the check for pcre is in ../CMakeLists.txt
if(PCRE_FOUND AND NOT KJS_FORCE_DISABLE_PCRE)
   include_directories(${PCRE_INCLUDE_DIR})

   # tell check_symbol_exists to -I pcre dirs.
   include(CMakePushCheckState)
   cmake_push_check_state()
   set(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES} ${PCRE_INCLUDE_DIR})

   check_symbol_exists(PCRE_CONFIG_UTF8 "pcre.h" HAVE_PCRE_UTF8)
   check_symbol_exists(PCRE_CONFIG_STACKRECURSE "pcre.h" HAVE_PCRE_STACK)

   cmake_pop_check_state()

   # Even though we "support" non-PCRE builds, if we build PCRE, we want a version
   # recent enough, and we don't want to fallback to a completely crippled
   # POSIX code just like that.
   if (NOT HAVE_PCRE_UTF8  OR NOT  HAVE_PCRE_STACK)
      message(FATAL_ERROR "Your libPCRE is too old. KJS requires at least PCRE4.5")
   endif ()

else ()
   # if we're here, either PCRE support is disabled, or it's not found...
   # it better be disabled.
   if (NOT KJS_FORCE_DISABLE_PCRE)
        message(FATAL_ERROR "The PCRE regular expression library has not been found. KJS requires PCRE >= 4.5 to function properly. If you for some reason can not install it, you can force a build with POSIX regex.h by passing -DKJS_FORCE_DISABLE_PCRE=true to cmake. However, be advised that it'll result in many websites breaking")
   endif ()
   # if pcre is not installed or disabled, at least the posix regex.h has to be available
   if(APPLE)
      check_include_files("sys/types.h;regex.h" HAVE_REGEX_H)
   else()
      check_include_files(regex.h HAVE_REGEX_H)
   endif()
   if (NOT HAVE_REGEX_H)
      message(FATAL_ERROR "Neither the PCRE regular expression library nor the POSIX regex.h header have been found. Consider installing PCRE.")
   endif ()
endif()

# The crosscompiling parts are commented out on purpose. Alex
# if (CMAKE_CROSSCOMPILING)
#    set(IMPORT_ICEMAKER_EXECUTABLE "${KDE_HOST_TOOLS_PATH}/ImportIcemakerExecutable.cmake" CACHE FILEPATH "Point it to the export file of icemaker from a native build")
#    include(${IMPORT_ICEMAKER_EXECUTABLE})
#    set(ICEMAKER_EXECUTABLE icemaker)
# else ()

   ########### icemaker, generates some tables for kjs/frostbyte ###############
   set(icemaker_SRCS
       bytecode/generator/tablebuilder.cpp
       bytecode/generator/types.cpp
       bytecode/generator/codeprinter.cpp
       bytecode/generator/driver.cpp
       bytecode/generator/lexer.cpp
       bytecode/generator/parser.cpp
      )
   add_executable(icemaker ${icemaker_SRCS})
   target_include_directories(icemaker PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")

   # get the name of the generated wrapper script (which sets up LD_LIBRARY_PATH)
   #get_target_property(ICEMAKER_EXECUTABLE icemaker WRAPPER_SCRIPT)
   #get_target_property(ICEMAKER_EXECUTABLE_DIR icemaker RUNTIME_OUTPUT_DIRECTORY)
   set(ICEMAKER_EXECUTABLE icemaker)

#    export(TARGETS icemaker FILE ${CMAKE_BINARY_DIR}/ImportIcemakerExecutable.cmake)
# endif ()

# and the custom command
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/opcodes.h ${CMAKE_CURRENT_BINARY_DIR}/opcodes.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/machine.cpp
  COMMAND ${ICEMAKER_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bytecode
  DEPENDS icemaker ${CMAKE_CURRENT_SOURCE_DIR}/bytecode/codes.def
          ${CMAKE_CURRENT_SOURCE_DIR}/bytecode/opcodes.cpp.in
          ${CMAKE_CURRENT_SOURCE_DIR}/bytecode/opcodes.h.in
          ${CMAKE_CURRENT_SOURCE_DIR}/bytecode/machine.cpp.in
)

########### next target ###############

# We don't want -pedantic/--pedantic for KJS since we want to use GCC extension when available
string(REPLACE "--pedantic" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
string(REPLACE "-pedantic" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

add_definitions(-DBUILDING_KDE__)

set(CREATE_HASH_TABLE ${CMAKE_CURRENT_SOURCE_DIR}/create_hash_table )

macro(CREATE_LUT _srcs_LIST _in_FILE _out_FILE _dep_FILE)

   add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_out_FILE}
      COMMAND ${PERL_EXECUTABLE} ${CREATE_HASH_TABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${_in_FILE} -i > ${CMAKE_CURRENT_BINARY_DIR}/${_out_FILE}
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${_in_FILE} )
   set( ${_srcs_LIST}  ${${_srcs_LIST}} ${CMAKE_CURRENT_BINARY_DIR}/${_out_FILE})
endmacro(CREATE_LUT)

create_lut(kjs_LIB_SRCS date_object.cpp date_object.lut.h date_object.cpp)
create_lut(kjs_LIB_SRCS number_object.cpp number_object.lut.h number_object.cpp)
create_lut(kjs_LIB_SRCS string_object.cpp string_object.lut.h string_object.cpp)
create_lut(kjs_LIB_SRCS array_object.cpp array_object.lut.h array_object.cpp)
create_lut(kjs_LIB_SRCS math_object.cpp math_object.lut.h math_object.cpp)
create_lut(kjs_LIB_SRCS json_object.cpp json_object.lut.h json_object.cpp)
create_lut(kjs_LIB_SRCS regexp_object.cpp regexp_object.lut.h regexp_object.cpp)
create_lut(kjs_LIB_SRCS keywords.table lexer.lut.h lexer.cpp)

set(kjs_LIB_SRCS
   ${kjs_LIB_SRCS}
   ustring.cpp
   date_object.cpp
   collector.cpp
   nodes.cpp
   grammar.cpp
   lexer.cpp
   lookup.cpp
   operations.cpp
   regexp.cpp
   function_object.cpp
   string_object.cpp
   bool_object.cpp
   number_object.cpp
   internal.cpp
   ExecState.cpp
   Parser.cpp
   array_object.cpp
   array_instance.cpp
   math_object.cpp
   object_object.cpp
   regexp_object.cpp
   error_object.cpp
   function.cpp
   debugger.cpp
   value.cpp
   list.cpp
   object.cpp
   interpreter.cpp
   package.cpp
   property_map.cpp
   property_slot.cpp
   nodes2string.cpp
   identifier.cpp
   scope_chain.cpp
   dtoa.cpp
   fpconst.cpp
   JSLock.cpp
   JSImmediate.cpp
   PropertyNameArray.cpp
   JSWrapperObject.cpp
   CommonIdentifiers.cpp
   JSVariableObject.cpp
   ${CMAKE_CURRENT_BINARY_DIR}/opcodes.cpp
   ${CMAKE_CURRENT_BINARY_DIR}/machine.cpp
   nodes2bytecode.cpp
   CompileState.cpp
   jsonlexer.cpp
   json_object.cpp
   jsonstringify.cpp
   propertydescriptor.cpp
   )


add_library(KF5JS ${kjs_LIB_SRCS})
add_library(KF5::JS ALIAS KF5JS)

set(kjs_INCLUDES
  ${CMAKE_CURRENT_BINARY_DIR}/..
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  ${CMAKE_CURRENT_BINARY_DIR}/../..
  ${CMAKE_CURRENT_SOURCE_DIR}/../wtf
)

target_include_directories(KF5JS PUBLIC "$<BUILD_INTERFACE:${kjs_INCLUDES}>")

if(WIN32)
   target_link_libraries(KF5JS PRIVATE ${KDEWIN_LIBRARIES})
endif()

if(CMAKE_THREAD_LIBS_INIT)
   target_link_libraries(KF5JS PRIVATE ${CMAKE_THREAD_LIBS_INIT})
endif()

if(UNIX)
   target_link_libraries(KF5JS PRIVATE m)
endif()

if(PCRE_FOUND)
   target_link_libraries(KF5JS PRIVATE ${PCRE_LIBRARIES})
endif()

set_target_properties(KF5JS PROPERTIES VERSION ${KJS_VERSION_STRING}
                                       SOVERSION ${KJS_SOVERSION}
                                       EXPORT_NAME JS
)
install(TARGETS KF5JS EXPORT KF5JSTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

########### kjs - basic shell ###############

set(kjs_SRCS kjs.cpp)

#required by win32 see  kdelibs/cmake/modules/kde4Macros.cmake kde4_add_manifest
set (kjs_bin_OUTPUT_NAME kjs)

# 'kjs_bin' because cmake doesn't like having a lib and app with the same name
# This is a developer tool, not intended for normal user installs
add_executable(kjs_bin ${kjs_SRCS})

set_target_properties(kjs_bin PROPERTIES RUNTIME_OUTPUT_NAME ${kjs_bin_OUTPUT_NAME})

target_link_libraries(kjs_bin KF5JS)

install(TARGETS kjs_bin ${INSTALL_TARGETS_DEFAULT_ARGS})

########### KDE-specific API ##############

add_subdirectory(api)

########### install files ###############
# install( FILES
#     ExecState.h
#     JSImmediate.h
#     JSLock.h
#     JSType.h
#     PropertyNameArray.h
#     collector.h
#     completion.h
#     function.h
#     identifier.h
#     interpreter.h
#     list.h
#     lookup.h
#     object.h
#     operations.h
#     package.h
#     property_map.h
#     property_slot.h
#     protect.h
#     scope_chain.h
#     types.h
#     ustring.h
#     value.h
#     CommonIdentifiers.h
#
#     ${CMAKE_CURRENT_BINARY_DIR}/global.h
#
#     DESTINATION  ${INCLUDE_INSTALL_DIR}/kjs COMPONENT Devel )
install(FILES
  array_instance.h
  array_object.h
  bool_object.h
  collector.h
  CommonIdentifiers.h
  commonunicode.h
  CompileState.h
  completion.h
  context.h
  date_object.h
  debugger.h
  dtoa.h
  error_object.h
  ExecState.h
  function.h
  function_object.h
  grammar.h
  identifier.h
  internal.h
  interpreter.h
  JSImmediate.h
  JSLock.h
  jsonlexer.h
  json_object.h
  jsonstringify.h
  JSType.h
  JSVariableObject.h
  JSWrapperObject.h
  lexer.h
  list.h
  LocalStorage.h
  lookup.h
  makenodes.h
  math_object.h
  nodes2bytecode.h
  nodes.h
  number_object.h
  object.h
  object_object.h
  operations.h
  package.h
  Parser.h
  propertydescriptor.h
  property_map.h
  PropertyNameArray.h
  property_slot.h
  protect.h
  regexp.h
  regexp_object.h
  SavedBuiltins.h
  scope_chain.h
  scriptfunction.h
  string_object.h
  SymbolTable.h
  types.h
  ustring.h
  value.h
  operations.h
  ${CMAKE_CURRENT_BINARY_DIR}/global.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/kjs
  COMPONENT Devel
)

install(FILES create_hash_table DESTINATION ${DATA_INSTALL_DIR}/kjs/ )
