/* Copyright (C) 2018 froglogic GmbH */

shouldBe("String.prototype.includes.length", "1");

// simple
shouldBeTrue("'abc'.includes('a')");
shouldBeTrue("'abc'.includes('b')");
shouldBeTrue("'abc'.includes('c')");
shouldBeTrue("'abc'.includes('bc')");
shouldBeTrue("'abc'.includes('abc')");
shouldBeFalse("'abc'.includes('d')");
shouldBeFalse("'abc'.includes('abcd')");

// with start position
shouldBeTrue("'abc'.includes('a', 0)");
shouldBeTrue("'abc'.includes('a', -1)");
shouldBeFalse("'abc'.includes('a', 1)");
shouldBeFalse("'abc'.includes('a', 4)");
shouldBeTrue("'abc'.includes('a', -Infinity)");
shouldBeFalse("'abc'.includes('a', +Infinity)");

// non-numerical position parameter
shouldBeTrue("'abc'.includes('a', undefined)");
shouldBeTrue("'abc'.includes('a', null)");
shouldBeTrue("'abc'.includes('a', NaN)");
shouldBeTrue("'abc'.includes('a', '0')");
shouldBeFalse("'abc'.includes('a', '1')");

// non-string objects
shouldBeTrue("'123'.includes(2)");
shouldBeFalse("'123'.includes(4)");
var arr = ['a', 'b'];
arr.includes = String.prototype.includes;
shouldBeTrue("arr.includes('a')");
var obj = { toString: function() { return "xyz"; } };
obj.includes = String.prototype.includes;
shouldBeTrue("obj.includes('xyz')");

// invalid
shouldThrow("'abc'.includes(/a/)");
