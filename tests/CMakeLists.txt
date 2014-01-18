
include(ECMMarkAsTest)

########### testkjs_static ###############

set(testkjs_static_SRCS testkjs.cpp )

add_executable(testkjs_static ${testkjs_static_SRCS})
ecm_mark_as_test(testkjs_static)
ecm_mark_nongui_executable(testkjs_static)

target_link_libraries(testkjs_static KF5::JS Qt5::Core)

########### testkjs ###############

set(testkjs_SRCS testkjs.cpp )

add_executable(testkjs ${testkjs_SRCS})
ecm_mark_as_test(testkjs)
ecm_mark_nongui_executable(testkjs)

target_link_libraries(testkjs KF5::JS Qt5::Core)

if (WIN32)
   # for timeGetTime() we need winmm
   target_link_libraries(testkjs winmm)
   target_link_libraries(testkjs_static winmm)
endif()
