
include(ECMMarkAsTest)

########### testkjs_static ###############

set(testkjs_static_SRCS testkjs.cpp )

add_executable(testkjs_static ${testkjs_static_SRCS})
ecm_mark_as_test(testkjs_static)

target_link_libraries(testkjs_static KF5::JS ${KDEWIN_LIBRARIES} Qt5::Core)

########### testkjs ###############

set(testkjs_SRCS testkjs.cpp )

add_executable(testkjs ${testkjs_SRCS})
ecm_mark_as_test(testkjs)

target_link_libraries(testkjs KF5::JS ${KDEWIN_LIBRARIES} Qt5::Core)

