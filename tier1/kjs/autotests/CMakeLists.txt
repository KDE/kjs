include(ECMMarkAsTest)

find_package(Qt5Test 5.2.0 REQUIRED NO_MODULE)
########### ecmatest ##############

add_executable(ecmatest
  ecmatest.cpp
)
ecm_mark_as_test(ecmatest)
target_link_libraries(ecmatest KF5::JS ${KDEWIN_LIBRARIES} Qt5::Test)

function(ADD_ECMATEST ECMA262_CHAPTER)
   if (IS_DIRECTORY "${ECMATEST_BASEDIR}/test/suite/${ECMA262_CHAPTER}")
      string(REPLACE "/15" "" ECMA262_TESTNAME "${ECMA262_CHAPTER}")
      add_test(ECMAscript262_${ECMA262_TESTNAME} ecmatest)
      if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ecmatest_broken_${ECMA262_TESTNAME}")
         set(_test262_broken_env ";ECMATEST_BROKEN=${CMAKE_CURRENT_SOURCE_DIR}/ecmatest_broken_${ECMA262_TESTNAME}")
      else ()
         set(_test262_broken_env)
      endif ()
      set_tests_properties(ECMAscript262_${ECMA262_TESTNAME} PROPERTIES ENVIRONMENT "ECMATEST_BASEDIR=${ECMATEST_BASEDIR};ECMATEST_CHAPTER=${ECMA262_CHAPTER}${_test262_broken_env}")
   endif ()
endfunction(ADD_ECMATEST)

if (BUILD_TESTING)
   set(ECMATEST_BASEDIR "" CACHE PATH "path to checkout of ECMAscript Tests262 checkout")
   if (ECMATEST_BASEDIR)
      file(GLOB _test262_chapters RELATIVE "${ECMATEST_BASEDIR}/test/suite/" "${ECMATEST_BASEDIR}/test/suite/*")
      list(SORT _test262_chapters)
      # The "Intl" object is not implemented (see bug 299962)
      list(REMOVE_ITEM _test262_chapters "intl402")
      # ch15 is way too big, split it into several subchapters
      list(REMOVE_ITEM _test262_chapters "ch15")
      foreach (_test262_chapter ${_test262_chapters})
         ADD_ECMATEST(${_test262_chapter})
      endforeach ()

      # now ch15
      file(GLOB _test262_15_chapters RELATIVE "${ECMATEST_BASEDIR}/test/suite/ch15" "${ECMATEST_BASEDIR}/test/suite/ch15/*")
      list(SORT _test262_15_chapters)
      foreach (_test262_15_chapter ${_test262_15_chapters})
         ADD_ECMATEST(ch15/${_test262_15_chapter})
      endforeach ()
   endif ()
endif ()

##################### kjsapitest ################

add_executable(kjsapitest kjsapitest.cpp)
ecm_mark_as_test(kjsapitest)
add_test(kjsapitest kjsapitest)
target_link_libraries(kjsapitest KF5::JSApi Qt5::Test)
