// Various evaluation order tests..

// Argument list should be evaluated before checking whether functions is callable, etc.
function test1() {
    function bad() {
        throw "PASS";
    }

    try {
        var f = 42;
        f(bad());
    } catch (e) {
        return e;
    }
}

shouldBe('test1()', '"PASS"');


// Make sure we do toObject properly
shouldThrow("var v = null; delete v.x;");

// Left hand-side of an assignment should be bound as a reference before evaluating RHS, test #1
x = 0;
function test2() {
    x = eval("var x; 42;");
}
test2();

shouldBe('x', '42');

// Same, but using with and not eval.
y = 0;

function test3() {
    var o = {};
    with (o) {
        y = (o.y = 42);
    }
}
test3();
shouldBe('y', '42');

// Now also throwing in functions in a mix. Also tests 'this' computation
z = "FAIL - wrong scope bound?";
o = {};

function test4() {
    var o = {fn: null /* so it goes there*/ };
    with (o) {
        fn = function() {
            this.z = "FAIL - wrong this?";
            return "PASS";
        }

        z = fn();
    }
}
test4();
shouldBe('z', '"PASS"');

// Here, we are making sure we call toObject on base of foo[bar] before calling toString on index
var b = undefined; // so toObject fails
index = { toString: function() { throw "FAIL"; } }

caught   = false;
theCatch = null;
try {
    b[index];
} catch (e) {
    caught  = true;
    theCatch = e;
}

shouldBeTrue('caught');
shouldBeTrue('theCatch != "FAIL"');

// Now make sure that toString on LHS gets called before evaluating RHS.
index = { toString: function() { throw "PASS"; } }
b     = {}
function rhs() {
    throw "FAIL";
}

caught   = false;
theCatch = null;
try {
    b[index] = rhs();
} catch (e) {
    caught  = true;
    theCatch = e;
}

shouldBeTrue('caught');
shouldBe('theCatch', "'PASS'");

// Make sure all the steps (other than the toObject, which we can't trace w/o failing) are in order for foo[bar]
out = "";
function getBase() {
    out += "1";
    return {};
}

function getIndex() {
    out += "2";
    o = { toString: function() { out += "3"; return "42"; } };
    return o;
}

function getRHS() {
    out += "4";
    return "answer";
}

getBase()[getIndex()] = getRHS();
shouldBe("out", "1234");


// Like assignment, 'var' binds the scope before doing evaluation
// So make sure it gets it right. eval can't inject a new variable here, so use catch/with capture.
function varScopeTest1() {
    var capturer = { captureInject: function(val) { this.local = val; return "ret:" + val; } }

    with (capturer) {
        // This should write to the activation's local.
        var local = captureInject(42);
    }

    if (local != "ret:42")
        return "FAIL";

    if (capturer.local != 42)
        return "FAIL #2";

    return "PASS";
}

shouldBe("varScopeTest1()", "'PASS'");

debug("Done.");
// kate: indent-width 4; replace-tabs on; tab-width 4; space-indent on;
