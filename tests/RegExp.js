// invalid regexp
shouldThrow("/(/");
// should not crash
shouldThrow("''.match('(')");

shouldBe("(new RegExp()).source", "''");
shouldBe("Boolean(new RegExp())", "true");
shouldBeTrue("isNaN(Number(new RegExp()))");

// RegExp constructor called as a function
shouldBe("RegExp(/x/).source", "'x'");
//shouldBe("RegExp(/x/, 'g').source", "'/x/'"); // can't supply flags when constructing one RegExp from another, says mozilla
shouldBe("RegExp('x', 'g').global", "true");
shouldBe("RegExp('x').source", "'x'");

// RegExp constructor
shouldBe("new RegExp('x').source", "'x'");

var ri = /a/i;
var rm = /a/m;
var rg = /a/g;

shouldBeFalse("(/a/).global");
shouldBe("typeof (/a/).global", "'boolean'");
shouldBeTrue("rg.global");
shouldBeFalse("(/a/).ignoreCase");
shouldBeTrue("ri.ignoreCase");
shouldBeFalse("(/a/).multiline");
shouldBeTrue("rm.multiline");
shouldBe("rg.toString()", "'/a/g'");
shouldBe("ri.toString()", "'/a/i'");
shouldBe("rm.toString()", "'/a/m'");

// check properety attributes
rg.global = false;
shouldBeTrue("rg.global");
ri.ignoreCase = false;
shouldBeTrue("ri.ignoreCase");
rm.multiline = false;
shouldBeTrue("rm.multiline");

shouldBe("Boolean(/a/.test)", "true");
shouldBe("/(b)c/.exec('abcd').toString()", "\"bc,b\"");
shouldBe("/(b)c/.exec('abcd').length", "2");
shouldBe("/(b)c/.exec('abcd').index", "1");
shouldBe("/(b)c/.exec('abcd').input", "'abcd'");

var rs = /foo/;
rs.source = "bar";
shouldBe("rs.source", "'foo'");

shouldBe("var r = new RegExp(/x/); r.global=true; r.lastIndex = -1; typeof r.test('a')", "'boolean'");

shouldBe("'abcdefghi'.match(/(abc)def(ghi)/).toString()","'abcdefghi,abc,ghi'");
shouldBe("/(abc)def(ghi)/.exec('abcdefghi').toString()","'abcdefghi,abc,ghi'");
shouldBe("RegExp.$1","'abc'");
shouldBe("with(RegExp) { $1 }","'abc'");
shouldBe("RegExp.$2","'ghi'");
shouldBe("RegExp.$3","''");

shouldBe("'abcdefghi'.match(/(a(b(c(d(e)f)g)h)i)/).toString()", "'abcdefghi,abcdefghi,bcdefgh,cdefg,def,e'");
shouldBe("RegExp.$1","'abcdefghi'");
shouldBe("RegExp.$2","'bcdefgh'");
shouldBe("RegExp.$3","'cdefg'");
shouldBe("RegExp.$4","'def'");
shouldBe("RegExp.$5","'e'");
shouldBe("RegExp.$6","''");

shouldBe("'(100px 200px 150px 15px)'.match(/\\((\\d+)(px)* (\\d+)(px)* (\\d+)(px)* (\\d+)(px)*\\)/).toString()","'(100px 200px 150px 15px),100,px,200,px,150,px,15,px'");
shouldBe("RegExp.$1","'100'");
shouldBe("RegExp.$3","'200'");
shouldBe("RegExp.$5","'150'");
shouldBe("RegExp.$7","'15'");
shouldBe("''.match(/\((\\d+)(px)* (\\d+)(px)* (\\d+)(px)* (\\d+)(px)*\)/)","null");
shouldBe("RegExp.$1","''");
shouldBe("RegExp.$3","''");
shouldBe("RegExp.$5","''");
shouldBe("RegExp.$7","''");

var invalidChars = /[^@\.\w]/g; // #47092
shouldBe("'faure@kde.org'.match(invalidChars) == null", "true");
shouldBe("'faure-kde@kde.org'.match(invalidChars) == null", "false");

shouldBe("'test1test2'.replace('test','X')","'X1test2'");
shouldBe("'test1test2'.replace(/\\d/,'X')","'testXtest2'");
shouldBe("'1test2test3'.replace(/\\d/,'')","'test2test3'");
shouldBe("'test1test2'.replace(/test/g,'X')","'X1X2'");
shouldBe("'1test2test3'.replace(/\\d/g,'')","'testtest'");
shouldBe("'1test2test3'.replace(/x/g,'')","'1test2test3'");
shouldBe("'test1test2'.replace(/(te)(st)/g,'$2$1')","'stte1stte2'");
shouldBe("'foo+bar'.replace(/\\+/g,'%2B')", "'foo%2Bbar'");
var caught = false; try { new RegExp("+"); } catch (e) { caught = true; }
shouldBeTrue("caught"); // #40435
var caught = false; try { new RegExp("]"); } catch (e) { caught = true; }
shouldBeTrue("caught"); // test 89 of Acid3
var caught = false; try { new RegExp("\\]"); } catch (e) { caught = true; }
shouldBeFalse("caught");
shouldBe("'foo'.replace(/z?/g,'x')", "'xfxoxox'");
shouldBe("'test test'.replace(/\\s*/g,'')","'testtest'"); // #50985
shouldBe("'abc$%@'.replace(/[^0-9a-z]*/gi,'')","'abc'"); // #50848
shouldBe("'ab'.replace(/[^\\d\\.]*/gi,'')","''"); // #75292
shouldBe("'1ab'.replace(/[^\\d\\.]*/gi,'')","'1'"); // #75292

// Acid3 showed a difference to the Perl regex behavior
var x = /(\3)(\1)(a)/.exec('cat'); // the \3 matches the empty string, qv. ES3:15.10.2.9
shouldBeTrue("!!x"); // or, "/(\\3)(\\1)(a)/ failed to match 'cat'");
shouldBe("x.length", "4"); // or, "/(\\3)(\\1)(a)/ failed to return four components");
shouldBe("x[0]", "'a'"); // or, "/(\\3)(\\1)(a)/ failed to find 'a' in 'cat'");
shouldBe("x[1]", "''"); // or, "/(\\3)(\\1)(a)/ failed to find '' in 'cat' as first part");
shouldBe("x[2]", "''"); // or, "/(\\3)(\\1)(a)/ failed to find '' in 'cat' as second part");
shouldBe("x[3]", "'a'"); // or, "/(\\3)(\\1)(a)/ failed to find 'a' in 'cat' as third part");

// Empty character class matches nothing
var ok = false;
try { ok = (/[]/.exec('') == null); } catch (e) { }
shouldBeTrue("ok");

// [^] matches anything
ok = false;
try { ok = (/[^]/.exec('x') != null); } catch (e) { }
shouldBeTrue("ok");

shouldBe("'1test2test3blah'.split(/test/).toString()","'1,2,3blah'");
var reg = /(\d\d )/g;
var str = new String('98 76 blah');
shouldBe("reg.exec(str).toString()","'98 ,98 '");
shouldBe("reg.lastIndex","3");
shouldBe("RegExp.$1","'98 '");
shouldBe("RegExp.$2","''");

shouldBe("reg.exec(str).toString()","'76 ,76 '");
shouldBe("reg.lastIndex","6");
shouldBe("RegExp.$1","'76 '");
shouldBe("RegExp.$2","''");

shouldBe("reg.exec(str)","null");
shouldBe("reg.lastIndex","0");

// http://www.devguru.com/Technologies/ecmascript/quickref/regexp_lastindex.html
// Looks IE-only though
//shouldBe( "var re=/ships*\s/; re.exec('the hardships of traveling'); re.lastIndex", "14" );
//shouldBe( "var re=/ships*\s/; str='the hardships of traveling'; re.exec(str); re.exec(str); re.lastIndex", "0" );

// http://devedge.netscape.com/library/manuals/2000/javascript/1.5/guide/regexp.html
shouldBe( "myRe=/d(b+)d/g; myArray = myRe.exec('cdbbdbsbz'); myRe.lastIndex", "5" );

reg = /^u/i;
shouldBeTrue("reg.ignoreCase == true");
shouldBeTrue("reg.global === false");
shouldBeTrue("reg.multiline === false");
shouldBeTrue("reg.test('UGO')");

// regexp are writable ?
shouldBe("reg.x = 1; reg.x", "1");
// shared data ?
shouldBe("var r2 = reg; r2.x = 2; reg.x", "2");

var str = new String("For more information, see Chapter 3.4.5.1");
re = /(chapter \d+(\.\d)*)/i;
// This returns the array containing Chapter 3.4.5.1,Chapter 3.4.5.1,.1
// 'Chapter 3.4.5.1' is the first match and the first value remembered from (Chapter \d+(\.\d)*).
// '.1' is the second value remembered from (\.\d)
shouldBe("str.match(re).toString()","'Chapter 3.4.5.1,Chapter 3.4.5.1,.1'");

str = "abcDdcba";
// The returned array contains D, d.
shouldBe("str.match(/d/gi).toString()","'D,d'");

// unicode escape sequence
shouldBe("/\\u0061/.source", "'\\\\u0061'");
shouldBe("'abc'.match(/\\u0062/).toString()", "'b'");

shouldBe("Object.prototype.toString.apply(RegExp.prototype)",
	 "'[object RegExp]'");

// not sure what this should return. most importantly
// it doesn't throw an exception
shouldBe("typeof RegExp.prototype.toString()", "'string'");


// Tests for unicode handling inside strings and regexps..
str = "\u304f";
shouldBe("str.replace(/o/ig,'aaa')", "str");
shouldBe("'PASS'.replace(/[\\u0100-\\uFFFF]/g, '?')", "'PASS'");
shouldBe("'\uDBFF\uDFFD'.replace(/\uDBFF/, 'A').replace(/\uDFFD/, 'B')", "'AB'");
shouldBe("'\uDBFF\uDFFD'.replace(/\\uDBFF/, 'A').replace(/\\uDFFD/, 'B')", "'AB'");
shouldBe("'\uDBFF\uDFFD'.replace(/\\W/, '*')", "'*\uDFFD'");

myName   = "\u041C\u0430\u043A\u0441\u0438\u043C\u0020\u041E"; // Maksim O in Cyrillic

//If one get rids of the 'im', should get 'Maks O' back
nickName = myName.replace(/\u0438\u043C/, '');
shouldBe("nickName", "'\u041C\u0430\u043A\u0441\u0020\u041E'");

//If one gets rid of the ' O', should get Maks back
shouldBe("nickName.replace(/ \u041E/,'')", "'\u041C\u0430\u043A\u0441'");
 
//Some checks for index handling with unicode-encoded stuff...
multi = "\u0430\u0431\u0432\u0430\u0431\u0432\u0430\u0431\u0432"; //abv 3 times in Cyrillic
matches = multi.match(/\u0430\u0431\u0432/g); //match abv...
shouldBe("matches.length", "3");
shouldBe("matches[0]", "'\u0430\u0431\u0432'");
shouldBe("matches[1]", "'\u0430\u0431\u0432'");
shouldBe("matches[2]", "'\u0430\u0431\u0432'");

// null bytes handled safely
shouldBe("'a\\u0000b'.search(/\\u0000b/)", "1");

// escaped pattern characters
shouldBe("'abc'.search(/\\u005Ea/)", "-1"); // unlike /^a/
shouldBe("'abc'.search(/c\\u0024/)", "-1"); // unlike /c$/
shouldBe("'a\\\\bc'.search(/\\u005C/)", "1"); // Unlike /\/
shouldBe("'abc'.search(/b\\u002B/)", "-1"); // unlike /b+/
shouldBe("'abc'.search(/a\\u002Ec/)", "-1"); // unlike /a.c/
shouldBe("'abc'.search(/ax\\u002A/)", "-1"); // unlike /ax*/
shouldBe("'abc'.search(/ab\\u002B/)", "-1"); // unlike /ab+/
shouldBe("'abc'.search(/ax\\u003F/)", "-1"); // unlike /ax?/
shouldBe("'abc'.search(/b\\u007Cx/)", "-1"); // unlike /b|x/

// incomplete, non-standard \u sequences we model the IE behavior here
// which always treats e.g. \u9 as u9. Mozilla treats (\u9) as (u9)
// but just \u9 like \u0009.
shouldBe("'abc'.replace(/\\u0062/, 'x');", "'axc'");
shouldBe("'au062c'.replace(/\\u062/, 'x');", "'axc'");
shouldBe("'au62c'.replace(/\\u62/, 'x');", "'axc'");
shouldBe("'au2c'.replace(/\\u2/, 'x');", "'axc'");
shouldBe("'auc'.replace(/\\u/, 'x');", "'axc'");
shouldBe("'au62c'.replace(/[\\u62]+/, 'x');", "'axc'");
shouldBe("'au62wc'.search(/\\u62w/);", "1");
shouldBe("'au62c'.search(/(\\u62)/);", "1");
shouldBe("'au62c'.search(/(\\u62)/);", "1");
shouldBe("'au62bc'.search(/(\\u62|q)/);", "1");

var s = "*";
while (s.length < 100000)
    s = s + s;
var re = /^[^<]*(<(.|\s)+>)[^>]*$|^#(\w+)$/;

shouldThrow('("<scr" + s + "/scr>").match(re);'); // ... and not crash

// proper .lastIndex updating for .test
var re = /x/g;
var s = "x";
shouldBeFalse("re.test(s) && re.test(s)");

// Crash test for reentrancy
var re = 0;
function callback(str) {
    return crashme("lorem");
}
function crashme(txt) {
    if (!re) re = new RegExp('lorem (ipsum)',"gm");
    return txt.replace(re, callback);
}
crashme("lorem ipsum");

debug("Done.");
