function test() { return;}
function test() { while(0) break; }
function test() { while(0) continue; }

function test() { return lab;}
function test() { while(0) break lab; }
function test() { while(0) continue lab; }

function test() { return }
function test() { while(0) break }
function test() { while(0) continue }

function test() { return 0 }
function test() { while(0) break lab }
function test() { while(0) continue lab }

lab:

a = 1
b = 123 // comment
c = 2
c = 3 /* comment */
d = 4

// non-ascii identifier letters (not working copy of Mozilla?!)
var ident = "";
ident += "\u00E9"; // LATIN SMALL LETTER E WITH ACUTE
ident += "\u0100"; // LATIN CAPITAL LETTER A WITH MACRON
ident += "\u02af"; // LATIN SMALL LETTER TURNED H WITH FISHHOOK AND TAIL
ident += "\u0388"; // GREEK CAPITAL LETTER EPSILON WITH TONOS
ident += "\u18A8"; // MONGOLIAN LETTER MANCHU ALI GALI BHA
var code = "var " + ident + " = 11; " + ident + ";";
var res;
try {
  res = eval(code);
} catch( e ) {
  res = "exception. Missing Unicode support?";
}
shouldBe("res", "11");

var f\u006fo = 33;
shouldBe("foo", "33");
var \u0062ar = 44;
shouldBe("bar", "44");

// invalid identifier letters
shouldThrow("var f\xF7;");

// ASCII identifier characters as escape sequences
shouldBe("var \\u0061 = 102; a", "102");
shouldBe("var f\\u0030 = 103; f0", "103");

// non-ASCII identifier letters as escape sequences
shouldBe("var \\u00E9\\u0100\\u02AF\\u0388\\u18A8 = 104; \\u00E9\\u0100\\u02AF\\u0388\\u18A8;", "104");

// invalid identifier characters as escape sequences
shouldThrow("var f\\u00F7;");
shouldThrow("var \\u0030;");
shouldThrow("var test = { }; test.i= 0; test.i\\u002b= 1; test.i;");

shouldBe("var test = { }; test.i= 0; test.i\u002b= 1; test.i;", "1");

var stringWithNull = "'a" + String.fromCharCode(0) + "x'";
shouldBe("eval(stringWithNull)", stringWithNull);

// keywords can't be escaped
var caughtInvalidIf = false;
try {
  eval("i\\u0066 (1);"); // escaped 'if'
} catch (e) {
  caughtInvalidIf = true;
}
shouldBeTrue("caughtInvalidIf");

var i\u0066 = "escapedIf";
shouldBe("i\\u0066", "'escapedIf'");

debug("Done.");
