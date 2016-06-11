# Configuration checks
include(FindThreads)
include(CheckLibraryExists)
include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckSymbolExists)
include(CheckCXXSourceCompiles)
include(CheckFunctionExists)
include(CheckStructHasMember)

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
check_include_files("sys/param.h"   HAVE_SYS_PARAM_H)
check_include_files("errno.h"       HAVE_ERRNO_H)
check_include_files("strings.h"     HAVE_STRINGS_H)
check_include_files("crtdbg.h"      HAVE_CRTDBG_H)
check_struct_has_member("struct tm" tm_gmtoff time.h HAVE_TM_GMTOFF)

if(NOT WIN32)
    set(CMAKE_REQUIRED_LIBRARIES "-lm")
endif(NOT WIN32)

check_function_exists(posix_memalign     HAVE_FUNC_POSIX_MEMALIGN)
check_function_exists(gettimeofday    HAVE_GETTIMEOFDAY)

macro (check_math_expr _expr _var)
    check_cxx_source_compiles("
#include <math.h>
#include <cmath>
int main(int argc, char ** argv)
{
    (void)${_expr};
    return 0;
}
" ${_var})
endmacro()

check_math_expr("copysign(1.0, 1.0)"     HAVE_FUNC_COPYSIGN)
check_math_expr("_copysign(1.0, 1.0)"    HAVE_FUNC__COPYSIGN)

check_math_expr("signbit(1.0)"      HAVE_FUNC_SIGNBIT)
check_math_expr("__signbit(1.0)"    HAVE_FUNC___SIGNBIT)
check_math_expr("std::signbit(1.0)" HAVE_FUNC_STD_SIGNBIT)

check_math_expr("_finite(1.0)"       HAVE_FUNC__FINITE)
check_math_expr("finite(1.0)"        HAVE_FUNC_FINITE)
check_math_expr("std::isfinite(1.0)" HAVE_FUNC_STD_ISFINITE)

check_math_expr("isnan(1.0)"       HAVE_FUNC_ISNAN)
check_math_expr("std::isnan(1.0)"  HAVE_FUNC_STD_ISNAN)
check_math_expr("isinf(1.0)"       HAVE_FUNC_ISINF)
check_math_expr("std::isinf(1.0)"  HAVE_FUNC_STD_ISINF)

check_math_expr("__builtin_clz(1)"  HAVE_FUNC_BUILTIN_CLZ)

check_function_exists(_fpclass HAVE_FUNC__FPCLASS)
