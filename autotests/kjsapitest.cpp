/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 2008 Harri Porten (porten@kde.org)
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

#include "kjsobject.h"
#include "kjsprototype.h"
#include "kjsarguments.h"
#include "kjsinterpreter.h"

#include "qtest.h"

class KJSApiTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void objectConstruction();
    void interpreterEvaluate();
    void interpreterNormalizeCode();
    void objectProperties();
    void prototypeConstants();
    void prototypeProperties();
    void prototypeFunctions();
    void globalObject();
};

void KJSApiTest::objectConstruction()
{
    KJSInterpreter ip;
    KJSContext *ctx = ip.globalContext();

    // Object
    QVERIFY2(KJSObject().isObject(), "Broken default object");

    // undefined
    QVERIFY2(KJSUndefined().isUndefined(),
             "Undefined object is not undefined");
    // null
    QVERIFY2(KJSNull().isNull(),
             "Null object is not null");

    // Boolean
    KJSBoolean boolObject(true);
    QVERIFY2(boolObject.isBoolean(), "Boolean object is not of boolean type");
    QVERIFY2(boolObject.toBoolean(ctx), "Boolean object has wrong value");
    QVERIFY2(!ctx->hasException(), "Boolean conversion threw exception");

    // Number
    KJSNumber numObject(42.0);
    QVERIFY2(numObject.isNumber(), "Number object is not of number type");
    QCOMPARE(numObject.toNumber(ctx), 42.0);
    QCOMPARE(numObject.toInt32(ctx), 42);
    QVERIFY2(!ctx->hasException(), "Number conversion threw exception");

    // String
    KJSString stringObject("Trunk");
    QVERIFY2(stringObject.isString(), "String object is not of string type");
    QCOMPARE(stringObject.toString(ctx), QLatin1String("Trunk"));
    QVERIFY2(!ctx->hasException(), "String conversion threw exception");

    // Array
    KJSArray arrayObject(ctx, 3);
    QVERIFY2(arrayObject.isObject(), "Array object is not of object type");
    QCOMPARE(arrayObject.property(ctx, "length").toNumber(ctx), 3.0);
    QCOMPARE(arrayObject.toString(ctx), QLatin1String(",,"));
    QVERIFY2(!ctx->hasException(), "Array conversion threw exception");

    // copying
    KJSObject copy(stringObject);
    QCOMPARE(copy.toString(ctx), QLatin1String("Trunk"));
    copy = numObject;
    QCOMPARE(copy.toNumber(ctx), 42.0);
}

void KJSApiTest::interpreterEvaluate()
{
    KJSInterpreter ip;
    KJSContext *ctx = ip.globalContext();
    KJSResult res;

    // syntax error
    res = ip.evaluate(")(");
    QVERIFY2(res.isException(), "Syntax error not caught");

    res = ip.evaluate("11+22");
    QVERIFY2(!res.isException(), "Evaluation returned non-number object");
    QCOMPARE(res.value().toNumber(ctx), 33.0);
}

void KJSApiTest::interpreterNormalizeCode()
{
    int errLine = -1;
    QString errMsg;
    QString norm;
    bool ok;

    // syntax error case
    ok = KJSInterpreter::normalizeCode(")(", &norm, &errLine, &errMsg);
    QVERIFY(!ok);
    QVERIFY(!errMsg.isEmpty());
    QVERIFY(errLine >= 0 && errLine <= 2); // ### imprecise

    // success case
    ok = KJSInterpreter::normalizeCode("foo(); bar();", &norm);
    QVERIFY(ok);
    QVERIFY(!norm.isEmpty());
    QStringList lines = norm.split('\n');
    QVERIFY(lines.size() >= 2); // ### imprecise
    int fooLine = lines.indexOf(QRegExp(" *foo\\(\\);"));
    int barLine = lines.indexOf(QRegExp(" *bar\\(\\);"));
    QVERIFY(fooLine >= 0);
    QVERIFY(barLine > fooLine);
}

void KJSApiTest::objectProperties()
{
    KJSInterpreter ip;
    KJSContext *ctx = ip.globalContext();

    KJSObject global = ip.globalObject();
    KJSObject v;

    // bool
    global.setProperty(ctx, "myprop", true);
    v = global.property(ctx, "myprop");
    QVERIFY(v.isBoolean());
    QCOMPARE(v.toBoolean(ctx), true);

    // double
    global.setProperty(ctx, "myprop", 21.0);
    v = global.property(ctx, "myprop");
    QVERIFY(v.isNumber());
    QCOMPARE(v.toNumber(ctx), 21.0);

    // int
    global.setProperty(ctx, "myprop", 22);
    v = global.property(ctx, "myprop");
    QVERIFY(v.isNumber());
    QCOMPARE(v.toNumber(ctx), 22.0);

    // string (8-bit)
    global.setProperty(ctx, "myprop", "myvalue8");
    v = global.property(ctx, "myprop");
    QVERIFY(v.isString());
    QCOMPARE(v.toString(ctx), QLatin1String("myvalue8"));

    // string (Unicode)
    global.setProperty(ctx, "myprop", QLatin1String("myvalue16"));
    v = global.property(ctx, "myprop");
    QVERIFY(v.isString());
    QCOMPARE(v.toString(ctx), QLatin1String("myvalue16"));
}

void KJSApiTest::prototypeConstants()
{
    KJSInterpreter ip;
    KJSContext *ctx = ip.globalContext();

    KJSPrototype proto;

    proto.defineConstant("d0", 44.4);
    proto.defineConstant("s0", QLatin1String("XYZ"));

    KJSObject obj = proto.constructObject(ctx, nullptr);

    QCOMPARE(obj.property(ctx, "d0").toNumber(ctx), 44.4);
    QCOMPARE(obj.property(ctx, "s0").toString(ctx), QLatin1String("XYZ"));
}

static struct O {
    int x;
} o0 = { 42 };

static KJSObject getX(KJSContext * /*context*/, void *object)
{
    O *o = reinterpret_cast<O *>(object);
    int x = o->x;
    return KJSNumber(x);
}

static void setX(KJSContext *context, void *object, KJSObject value)
{
    O *o = reinterpret_cast<O *>(object);
    double n = value.toNumber(context);
    o->x = n;
}

void KJSApiTest::prototypeProperties()
{
    KJSInterpreter ip;
    KJSContext *ctx = ip.globalContext();

    KJSPrototype proto;

    proto.defineProperty(ctx, "x", getX, setX);
    proto.defineProperty(ctx, "readOnlyX", getX);

    KJSObject obj = proto.constructObject(ctx, &o0);

    // read/write property
    QCOMPARE(obj.property(ctx, "x").toNumber(ctx), 42.0);
    obj.setProperty(ctx, "x", KJSNumber(43));
    QCOMPARE(obj.property(ctx, "x").toNumber(ctx), 43.0);

    QCOMPARE(obj.property(ctx, "readOnlyX").toNumber(ctx), 43.0);
    obj.setProperty(ctx, "readOnlyX", KJSNumber(44));
    QVERIFY2(ctx->hasException(), "Write access caused no exception");
    QCOMPARE(obj.property(ctx, "readOnlyX").toNumber(ctx), 43.0);
}

static KJSObject multiply(KJSContext *context, void *object,
                          const KJSArguments &arguments)
{
    double factor = *reinterpret_cast<double *>(object);

    // test number of arguments
    if (arguments.count() != 1) {
        return context->throwException("Missing argument");
    }

    KJSObject a0 = arguments.at(0);
    if (!a0.isNumber()) {
        return KJSNumber(-2);
    }

    double v0 = a0.toNumber(context);

    return KJSNumber(factor * v0);
}

void KJSApiTest::prototypeFunctions()
{
    KJSInterpreter ip;
    KJSContext *ctx = ip.globalContext();

    KJSPrototype proto;

    proto.defineFunction(ctx, "multiply", multiply);

    double factor = 3.0;
    KJSObject obj = proto.constructObject(ctx, &factor);
    ip.globalObject().setProperty(ctx, "obj", obj);

    KJSResult res = ip.evaluate("obj.multiply(4)");
    QCOMPARE(res.value().toNumber(ctx), 12.0);

    // expect exception
    res = ip.evaluate("obj.multiply()");
    QVERIFY2(res.isException(), "Exception did not work");
}

void KJSApiTest::globalObject()
{
    KJSPrototype proto;
    proto.defineConstant("g0", 55.5);

    KJSGlobalObject glob = proto.constructGlobalObject(nullptr);

    KJSInterpreter ip(glob);
    KJSResult res = ip.evaluate("2 * g0");
    QCOMPARE(res.value().toNumber(ip.globalContext()), 111.0);
}

QTEST_MAIN(KJSApiTest)

#include "kjsapitest.moc"
