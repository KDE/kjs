function isNegativeZero(n)
{
  return n == 0 && 1 / n < 0;
}

// operator !
shouldBeTrue("!undefined");
shouldBeTrue("!null");
shouldBeTrue("!!true");
shouldBeTrue("!false");
shouldBeTrue("!!1");
shouldBeTrue("!0");
shouldBeTrue("!!'a'");
shouldBeTrue("!''");

// unary plus
shouldBe("+9", "9");
shouldBe("var i = 10; +i", "10");

// negation
shouldBe("-11", "-11");
shouldBe("var i = 12; -i", "-12");

// increment
shouldBe("var i = 0; ++i;", "1");
shouldBe("var i = 0; ++i; i", "1");
shouldBe("var i = 0; i++;", "0");
shouldBe("var i = 0; i++; i", "1");
shouldBe("var i = true; i++", "1");
shouldBe("var i = true; i++; i", "2");

// decrement
shouldBe("var i = 0; --i;", "-1");
shouldBe("var i = 0; --i; i", "-1");
shouldBe("var i = 0; i--;", "0");
shouldBe("var i = 0; i--; i", "-1");
shouldBe("var i = true; i--", "1");
shouldBe("var i = true; i--; i", "0");

// bitwise operators
shouldBe("~0", "-1");
shouldBe("~1", "-2");
shouldBe("~NaN", "-1");
shouldBe("~Infinity", "-1");
shouldBe("~Math.pow(2, 33)", "-1"); // 32 bit overflow
shouldBe("~(Math.pow(2, 32) + Math.pow(2, 31) + 2)",
         "2147483645"); // a signedness issue
shouldBe("~null", "-1");
shouldBe("3 & 1", "1");
shouldBe("2 | true", "3");
shouldBe("'3' ^ 1", "2");
shouldBe("3^4&5", "7");
shouldBe("2|4^5", "3");

shouldBe("1 << 2", "4");
shouldBe("8 >> 1", "4");
shouldBe("1 >> 2", "0");
shouldBe("-8 >> 24", "-1");
shouldBe("8 >>> 2", "2");
shouldBe("-8 >>> 24", "255");
shouldBe("(-2200000000 >> 1) << 1", "2094967296");
shouldBe("Infinity >> 1", "0");
shouldBe("Infinity << 1", "0");
shouldBe("Infinity >>> 1", "0");
shouldBe("NaN >> 1", "0");
shouldBe("NaN << 1", "0");
shouldBe("NaN >>> 1", "0");
shouldBe("8.1 >> 1", "4");
shouldBe("8.1 << 1", "16");
shouldBe("8.1 >>> 1", "4");
shouldBe("8.9 >> 1", "4");
shouldBe("8.9 << 1", "16");
shouldBe("8.9 >>> 1", "4");
shouldBe("Math.pow(2, 32) >> 1", "0");
shouldBe("Math.pow(2, 32) << 1", "0");
shouldBe("Math.pow(2, 32) >>> 1", "0");

// addition
shouldBe("1+2", "3");
shouldBe("'a'+'b'", "'ab'");
shouldBe("'a'+2", "'a2'");
shouldBe("'2'+'-1'", "'2-1'");
shouldBe("true+'a'", "'truea'");
shouldBe("'a' + null", "'anull'");
shouldBe("true+1", "2");
shouldBe("false+null", "0");

// substraction
shouldBe("1-3", "-2");
shouldBe("isNaN('a'-3)", "true");
shouldBe("'3'-'-1'", "4");
shouldBe("'4'-2", "2");
shouldBe("true-false", "1");
shouldBe("false-1", "-1");
shouldBe("null-true", "-1");

// multiplication
shouldBe("2 * 3", "6");
shouldBe("true * 3", "3");
shouldBe("2 * '3'", "6");
shouldBeTrue("isNaN(NaN * 2)");
shouldBeTrue("isNaN(2 * NaN)");
shouldBeTrue("isNaN(NaN * NaN)");
shouldBeTrue("isNaN(Infinity * 0)");
shouldBe("Infinity * Infinity", "Infinity");
shouldBe("Infinity * 1", "Infinity");
shouldBe("Infinity * -1", "-Infinity");

// division
shouldBe("6 / 4", "1.5");
shouldBe("true / false", "Infinity");
shouldBe("'6' / '2'", "3");
shouldBeTrue("isNaN('x' / 1)");
shouldBeTrue("isNaN(1 / NaN)");
shouldBeTrue("isNaN(Infinity / Infinity)");
shouldBe("Infinity / 0", "Infinity");
shouldBe("-Infinity / 0", "-Infinity");
shouldBe("Infinity / 1", "Infinity");
shouldBe("-Infinity / 1", "-Infinity");
shouldBeTrue("1 / Infinity == +0");
shouldBeTrue("isNegativeZero(1 / -Infinity)");
shouldBeTrue("isNaN(0/0)");
shouldBeTrue("0 / 1 === 0");
shouldBeTrue("isNegativeZero(0 / -1)");
shouldBe("1 / 0", "Infinity");
shouldBe("-1 / 0", "-Infinity");

// modulo
shouldBe("6 % 4", "2");
shouldBe("'-6' % 4", "-2");
shouldBe("6 % -4", "2");
shouldBe("6 % 1.5", "0");
shouldBe("6.5 % 4", "2.5");
shouldBeTrue("isNaN(6 % NaN)");
shouldBeTrue("isNaN(NaN % 2)");
shouldBeTrue("isNaN(NaN % NaN)");
shouldBeTrue("isNaN(Infinity % 2)");
shouldBeTrue("isNaN(6 % 0)");
shouldBeTrue("isNaN(Infinity % 0)");
shouldBe("6 % Infinity", "6");
shouldBe("0 % 1", "0");
shouldBeTrue("isNegativeZero(-0 % 1)");

shouldBe("2==2", "true");
shouldBe("1==2", "false");

shouldBe("1<2", "true");
shouldBe("1<=2", "true");
shouldBe("2<1", "false");
shouldBe("2<=1", "false");

shouldBe("2>1", "true");
shouldBe("2>=1", "true");
shouldBe("1>=2", "false");
shouldBe("1>2", "false");

shouldBe("'abc' == 'abc'", "true");
shouldBe("'abc' != 'xyz'", "true");
shouldBeTrue("true == true");
shouldBeTrue("false == false");
shouldBeTrue("true != false");
shouldBeTrue("'a' != null");
shouldBeTrue("'a' != undefined");
shouldBeTrue("null == null");
shouldBeTrue("null == undefined");
shouldBeTrue("undefined == undefined");
shouldBeTrue("NaN != NaN");
shouldBeTrue("true != undefined");
shouldBeTrue("true != null");
shouldBeTrue("false != undefined");
shouldBeTrue("false != null");
shouldBeTrue("'0' == 0");
shouldBeTrue("1 == '1'");
shouldBeTrue("NaN != NaN");
shouldBeTrue("NaN != 0");
shouldBeTrue("NaN != undefined");
shouldBeTrue("true == 1");
shouldBeTrue("true != 2");
shouldBeTrue("1 == true");
shouldBeTrue("false == 0");
shouldBeTrue("0 == false");

shouldBe("'abc' < 'abx'", "true");
shouldBe("'abc' < 'abcd'", "true");
shouldBe("'abc' < 'abc'", "false");
shouldBe("'abcd' < 'abcd'", "false");
shouldBe("'abx' < 'abc'", "false");

shouldBe("'abc' <= 'abc'", "true");
shouldBe("'abc' <= 'abx'", "true");
shouldBe("'abx' <= 'abc'", "false");
shouldBe("'abcd' <= 'abc'", "false");
shouldBe("'abc' <= 'abcd'", "true");

shouldBe("'abc' > 'abx'", "false");
shouldBe("'abc' > 'abc'", "false");
shouldBe("'abcd' > 'abc'", "true");
shouldBe("'abx' > 'abc'", "true");
shouldBe("'abc' > 'abcd'", "false");

shouldBe("'abc' >= 'abc'", "true");
shouldBe("'abcd' >= 'abc'", "true");
shouldBe("'abx' >= 'abc'", "true");
shouldBe("'abc' >= 'abx'", "false");
shouldBe("'abc' >= 'abx'", "false");
shouldBe("'abc' >= 'abcd'", "false");

// mixed strings and numbers - results validated in NS+moz+IE5
shouldBeFalse("'abc' <= 0"); // #35246
shouldBeTrue("'' <= 0");
shouldBeTrue("' ' <= 0");
shouldBeTrue("null <= 0");
shouldBeFalse("0 <= 'abc'");
shouldBeTrue("0 <= ''");
shouldBeTrue("0 <= null");
shouldBeTrue("null <= null");
shouldBeTrue("6 < '52'");
shouldBeTrue("6 < '72'"); // #36087
shouldBe("NaN < 0", "false");
shouldBe("NaN <= 0", "false");
shouldBe("NaN > 0", "false");
shouldBe("NaN >= 0", "false");

// strict comparison ===
shouldBe("0 === false", "false");
shouldBe("undefined === undefined", "true");
shouldBe("null === null", "true");
shouldBe("NaN === NaN", "false");
shouldBe("0.0 === 0", "true");
shouldBe("'abc' === 'abc'", "true");
shouldBe("'a' === 'x'", "false");
shouldBe("1 === '1'", "false");
shouldBe("'1' === 1", "false");
shouldBe("true === true", "true");
shouldBe("false === false", "true");
shouldBe("true === false", "false");
shouldBe("Math === Math", "true");
shouldBe("Math === Boolean", "false");
shouldBe("Infinity === Infinity", "true");

// !==
shouldBe("0 !== 0", "false");
shouldBe("0 !== 1", "true");

shouldBe("typeof undefined", "'undefined'");
shouldBe("typeof null", "'object'");
shouldBe("typeof true", "'boolean'");
shouldBe("typeof false", "'boolean'");
shouldBe("typeof 1", "'number'");
shouldBe("typeof 'a'", "'string'");
shouldBe("typeof shouldBe", "'function'");
shouldBe("typeof Number.NaN", "'number'");

shouldBe("11 && 22", "22");
shouldBe("null && true", "null");
shouldBe("11 || 22", "11");
shouldBe("null || 'a'", "'a'");

shouldBeUndefined("void 1");

shouldBeTrue("1 in [1, 2]");
shouldBeFalse("3 in [1, 2]");
shouldBeTrue("'a' in { a:1, b:2 }");

// instanceof
// Those 2 lines don't parse in Netscape...
shouldBe("(new Boolean()) instanceof Boolean", "true");
shouldBe("(new Boolean()) instanceof Number", "false");

// Regression test for a  ?: bug that showed up during FB work
function testCond() {
    // Need to be in function for var opt
    var i = 42;
    var j = (i != 42) ? i : 0;

    if (i == 42)
	testPassed("Test for ?: not messing up variable...");
    else
	testFailed("Test for ?: not messing up variable...");
}

testCond();

// Parens in some references..
shouldBe("var x = 0; ++(x); x", "1");
shouldBe("var x = 0; (x)++; x", "1");

// # 173202, and variants
shouldBe("null == 0", "false");
shouldBe("undefined == 0", "false");
shouldBe("false == 0", "true");
shouldBe("true == 0", "false");
shouldBe("true == 1", "true");

shouldBe("0 == null", "false");
shouldBe("0 == undefined", "false");
shouldBe("0 == false", "true");
shouldBe("0 == true", "false");
shouldBe("1 == true", "true");

shouldBe("var i = 1; i += 1", "2");
shouldBe("var i = 1; i -= 1", "0");
shouldBe("var i = 2; i *= 3", "6");
shouldBe("var i = 3; i /= 2", "1.5");
shouldBe("var i = 7; i %= 3", "1");
shouldBe("var i = 7; i <<= 1", "14");
shouldBe("var i = 8; i >>= 1", "4");
shouldBe("var i = -8; i >>>= 24", "255");
shouldBe("var i = 3; i &= 5", "1");
shouldBe("var i = 3; i |= 5", "7");
shouldBe("var i = 3; i ^= 5", "6");

debug("Done.");
