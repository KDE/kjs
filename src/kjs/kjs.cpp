/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 2006 Harri Porten (porten@kde.org)
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#include "JSLock.h"
#include "interpreter.h"
#include "object.h"
#include "package.h"
#include "function.h"

#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

#if PLATFORM(WIN_OS)
#  include <windows.h>
#  include <io.h>
#else
#  include <unistd.h>
#endif

#include <kjs_version.h>

enum ExitCode { ErrorNone,
                ErrorUnknownSwitch,
                ErrorMissingArg,
                ErrorReadFile,
                ErrorEval
              };

using std::strcmp;

using namespace KJS;

static void printUsage(const char *app)
{
    fprintf(stderr,
            "Usage: %s\n"
            "  [ -h | -help | --help ]\n"
            "  [ -e <statement> | <script> ]\n"
            "  [-v | -version | --version]\n",
            app);
}

static UString readFile(const char *fileName)
{
    int fd = open(fileName, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Error opening %s", fileName);
        return UString();
    }
    struct stat buf;
    if (fstat(fd, &buf) == -1) {
        fprintf(stderr, "Error stat'ing %s", fileName);
        close(fd);
        return UString();
    }
    int siz = buf.st_size;
    char *c = new char[siz + 1];
    int dataRead = read(fd, c, siz);
    if (dataRead == -1) {
        fprintf(stderr, "Error reading from %s", fileName);
        delete[] c;
        close(fd);
        return UString();
    }
    c[dataRead] = '\0';
    UString s = c;
    delete[] c;
    close(fd);
    return s;
}

static ExitCode evaluateFile(Interpreter *interp, const char *fileName);

class GlobalImp : public JSGlobalObject
{
public:
    virtual UString className() const
    {
        return "global";
    }
};

class TestFunctionImp : public JSObject
{
public:
    TestFunctionImp(int i, int length);
    virtual bool implementsCall() const
    {
        return true;
    }
    virtual JSValue *callAsFunction(ExecState *exec,
                                    JSObject *thisObj, const List &args);
    enum { Print, Quit, Load, GC };

private:
    int id;
};

TestFunctionImp::TestFunctionImp(int i, int length)
    : JSObject(), id(i)
{
    putDirect(Identifier("length"), length, DontDelete | ReadOnly | DontEnum);
}

JSValue *TestFunctionImp::callAsFunction(ExecState *exec,
        JSObject * /* thisObj */,
        const List &args)
{
    switch (id) {
    case Print:
        printf("%s\n", args[0]->toString(exec).UTF8String().c_str());
        return jsUndefined();
    case Quit:
        exit(0);
    case GC:
        while (Interpreter::collect()) {}
        break;
    case Load:
        evaluateFile(exec->dynamicInterpreter(), args[0]->toString(exec).UTF8String().c_str());
        break;
    default:
        abort();
    }
    return jsUndefined();
}

static ExitCode evaluateString(Interpreter *interp, const char *fileName,
                               const UString &code,
                               bool printResult = false)
{
    ExecState *exec = interp->globalExec();

    Completion res = interp->evaluate(fileName, 0, code);

    if (res.complType() == Throw) {
        CString msg = res.value()->toString(exec).UTF8String();
        JSObject *resObj = res.value()->toObject(exec);
        CString message = resObj->toString(exec).UTF8String();
        int line = resObj->toObject(exec)->get(exec, "line")->toUInt32(exec);

        if (fileName) {
            fprintf(stderr, "%s (line %d): ", fileName, line);
        }
        fprintf(stderr, "%s\n", msg.c_str());
        return ErrorEval;
    } else if (printResult) {
        if (res.isValueCompletion() && !res.value()->isUndefined()) {
            CString s8 = res.value()->toString(exec).UTF8String();
            if (s8.size() != 0) {
                fprintf(stdout, "%s\n", s8.c_str());
            }
        }
    }

    return ErrorNone;
}

static ExitCode evaluateFile(Interpreter *interp, const char *fileName)
{
    UString code = readFile(fileName);
    if (code.isNull()) {
        return ErrorReadFile;
    }

    return evaluateString(interp, fileName, code);
}

// primitive readline-like function
static char *readLine(const char *prompt)
{
    if (prompt) {
        fprintf(stdout, "%s", prompt);
    }

    const int bsize = 2 << 10;
    char *buffer = static_cast<char *>(malloc(bsize));
    char *s = fgets(buffer, bsize, stdin);
    if (s == nullptr) {
        // EOF
        fprintf(stdout, "\n");
        free(buffer);
    }
    return s;
}

static ExitCode evaluateInteractive(Interpreter *interp)
{
    char *line;
    while ((line = readLine("JS> ")) != nullptr) {
        UString code(line);
        free(line);
        evaluateString(interp, nullptr, code, true);
    }

    return ErrorNone;
}

static ExitCode parseArgs(int argc, char **argv)
{
    JSLock lock;

    GlobalImp *global = new GlobalImp();

    // create interpreter
    RefPtr<Interpreter> interp = new Interpreter(global);

    // add some global extension functions
    ExecState *gexec = interp->globalExec();
    global->put(gexec, "print",
                new TestFunctionImp(TestFunctionImp::Print, 1));
    global->put(gexec, "quit",
                new TestFunctionImp(TestFunctionImp::Quit, 0));
    global->put(gexec, "load",
                new TestFunctionImp(TestFunctionImp::Load, 1));
    global->put(gexec, "gc",
                new TestFunctionImp(TestFunctionImp::GC, 0));

    // enable package support
    StandardGlobalPackage package;
    interp->setGlobalPackage(&package);

    const char *script = nullptr, *command = nullptr;
    int ai = 1;
    bool ranOtherScript = false;
    for (ai = 1; ai < argc; ++ai) {
        const char *a = argv[ai];
        if (strcmp(a, "-v") == 0 || strcmp(a, "-version") == 0 ||
                strcmp(a, "--version") == 0) {
            printf("KDE: %s\n", KJS_VERSION_STRING);
            return ErrorNone;
        } else if (strcmp(a, "-h") == 0 || strcmp(a, "-help") == 0 ||
                   strcmp(a, "--help") == 0) {
            printUsage(argv[0]);
            return ErrorNone;
        } else if (strcmp(a, "-e") == 0) {
            ++ai;
            if (argc <= ai) {
                fprintf(stderr, "Missing -e argument.\n");
                return ErrorMissingArg;
            }
            command = argv[ai];
            ++ai;
            break;
        } else if (strcmp(a, "-f") == 0) { // Compatibility mode, for SunSpider
            ++ai;
            if (argc <= ai) {
                fprintf(stderr, "Missing -f argument.\n");
                return ErrorMissingArg;
            }
            ExitCode result = evaluateFile(interp.get(), argv[ai]);
            if (result != ErrorNone) {
                return result;
            }
            ranOtherScript = true;
        } else if (a[0] == '-') {
            fprintf(stderr, "Unknown switch %s.\n", a);
            return ErrorUnknownSwitch;
        } else {
            script = a;
            ++ai;
            break;
        }
    }

    // ###
    if (argc > ai) {
        fprintf(stderr, "Warning: ignoring extra arguments\n");
    }

    if (script) {
        return evaluateFile(interp.get(), script);
    } else if (command) {
        return evaluateString(interp.get(), "(eval)", command);
    } else if (!ranOtherScript) {
        return evaluateInteractive(interp.get());
    }
    return ErrorNone;
}

int main(int argc, char **argv)
{
    return int(parseArgs(argc, argv));
}
