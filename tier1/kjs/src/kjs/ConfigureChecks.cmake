# Configuration checks
include(FindThreads)
include(CheckLibraryExists)
include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckSymbolExists)
include(CheckCXXSymbolExists)
include(CheckFunctionExists)
include(CheckStructHasMember)

# The FindKDE4.cmake module sets _KDE4_PLATFORM_DEFINITIONS with
# definitions like _GNU_SOURCE that are needed on each platform.
set(CMAKE_REQUIRED_DEFINITIONS ${_KDE4_PLATFORM_DEFINITIONS})

check_library_exists(pthread pthread_attr_get_np "" HAVE_PTHREAD_ATTR_GET_NP)
check_library_exists(pthread pthread_getattr_np "" HAVE_PTHREAD_GETATTR_NP)

check_include_files("float.h"       HAVE_FLOAT_H)
check_include_files("sys/timeb.h"   HAVE_SYS_TIMEB_H)
check_include_files("ieeefp.h"      HAVE_IEEEFP_H)
check_include_files("pthread.h;pthread_np.h" HAVE_PTHREAD_NP_H)
check_include_files("valgrind/memcheck.h"   HAVE_MEMCHECK_H)
check_include_files("unistd.h"      HAVE_UNISTD_H)
check_include_files("stdint.h"      HAVE_STDINT_H)
check_include_files("sys/time.h"    HAVE_SYS_TIME_H)
check_include_files("errno.h"       HAVE_ERRNO_H)
check_include_files("strings.h"     HAVE_STRINGS_H)
check_include_files("crtdbg.h"      HAVE_CRTDBG_H)
check_struct_has_member("struct tm" tm_gmtoff time.h HAVE_TM_GMTOFF)

if(NOT WIN32)
    set(CMAKE_REQUIRED_LIBRARIES "-lm")
endif(NOT WIN32)

check_function_exists(_finite    HAVE_FUNC__FINITE)
check_function_exists(finite     HAVE_FUNC_FINITE)
check_function_exists(posix_memalign     HAVE_FUNC_POSIX_MEMALIGN)
check_function_exists(gettimeofday    HAVE_GETTIMEOFDAY)
check_symbol_exists(isnan   "math.h" HAVE_FUNC_ISNAN)
check_symbol_exists(isinf   "math.h" HAVE_FUNC_ISINF)
