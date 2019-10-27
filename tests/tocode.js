// normalize a piece of source code
function normalize(s)
{
    return s.replace(/[ \u000A]+/g, " ").replace(/ +$/, "").replace(/^ +/, "");
}

// compare toString() conversion of code without
// taking newlines, indentation and other whitespace into account
function tolerantSourceCompare(s, s2)
{
    var expected = normalize(s2 ? s2 : s);
    var actual = normalize(eval("String(" + s + ")"));
    if (expected === actual)
    	testPassed("Got the expected '" + expected + "'");
    else
    	testFailed("Expected '" + expected + "', got '" + actual + "'");
}

tolerantSourceCompare('function () { bar("x"); }');
tolerantSourceCompare('function () { bar(/abc/i); }');
var cccc = String.fromCharCode(0xCCCC);
var ff = String.fromCharCode(0xFF);
tolerantSourceCompare('function () { bar("' + cccc + '"); }', 'function () { bar("\\uCCCC"); }');
tolerantSourceCompare('function () { var o = { "a": 1 }; }');
tolerantSourceCompare('function () { var o = { "' + cccc + '": 1 }; }',
		      'function () { var o = { "\\uCCCC": 1 }; }');
tolerantSourceCompare('function () { var a' + ff + '; }',
		      'function () { var a\\xFF; }');
tolerantSourceCompare('function f' + ff + '() { }',
		      'function f\\xFF() { }');

tolerantSourceCompare('function () { do 1; while (false); }',
                      'function () { do 1; while (false); }');
tolerantSourceCompare('function () { do 1; while (false) }',
                      'function () { do 1; while (false); }');


var sinStr = '\nfunction sin() {\n    [native code]\n}\n'
shouldBe("normalize(String(Math.sin))", "normalize(sinStr)");

tolerantSourceCompare('function () { [1]; }');
tolerantSourceCompare('function () { [1,2]; }');
tolerantSourceCompare('function () { [1,]; }');
tolerantSourceCompare('function () { [1,,]; }');
tolerantSourceCompare('function () { [1,,,]; }');

/**** non-portable JS extension
tolerantSourceCompare('function () { import a.b; }');
tolerantSourceCompare('function () { import a.b }',
                      'function () { import a.b; }');
tolerantSourceCompare('function () { import a.b.*; }');
tolerantSourceCompare('function () { import a.b.* }',
                      'function () { import a.b.*; }');
tolerantSourceCompare('function () { import x = a.b.c; }');
tolerantSourceCompare('function () { import x = a.b.c }',
                      'function () { import x = a.b.c; }');
*/
