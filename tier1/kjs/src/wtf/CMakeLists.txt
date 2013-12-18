
########### next target ###############
#add_definitions(-DBUILDING_KDE__)

#set(wtf_LIB_SRCS HashTable.cpp )

#add_library(wtf SHARED ${wtf_LIB_SRCS})

#set_target_properties(wtf PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION} )
#install(TARGETS wtf ${INSTALL_TARGETS_DEFAULT_ARGS} )

install(FILES
  AlwaysInline.h
  ASCIICType.h
  Assertions.h
  DisallowCType.h
  FastMalloc.h
  Forward.h
  GetPtr.h
  HashCountedSet.h
  HashFunctions.h
  HashIterators.h
  HashMap.h
  HashSet.h
  HashTable.h
  HashTraits.h
  ListRefPtr.h
  MathExtras.h
  Noncopyable.h
  OwnArrayPtr.h
  OwnPtr.h
  PassRefPtr.h
  Platform.h
  RefCounted.h
  RefPtr.h
  RefPtrHashMap.h
  SharedPtr.h
  UnusedParam.h
  Vector.h
  VectorTraits.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/wtf
  COMPONENT Devel
)
