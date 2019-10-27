/* Copyright (C) 2018 froglogic GmbH */

///////////// String.prototype.startsWith() ////////////////

shouldBe("String.prototype.startsWith.length", "1");

// simple
shouldBeTrue("'abc'.startsWith('a')");
shouldBeTrue("'abc'.startsWith('ab')");
shouldBeFalse("'abc'.startsWith('b')");
shouldBeFalse("'abc'.startsWith('x')");
shouldBeFalse("'abc'.startsWith('abcd')");

// with start position
shouldBeTrue("'abc'.startsWith('a', 0)");
shouldBeTrue("'abc'.startsWith('a', -1)");
shouldBeTrue("'abc'.startsWith('b', 1)");
shouldBeFalse("'abc'.startsWith('c', 4)");
shouldBeTrue("'abc'.startsWith('a', -Infinity)");
shouldBeFalse("'abc'.startsWith('a', +Infinity)");

// non-numerical position parameter
shouldBeTrue("'abc'.startsWith('a', undefined)");
shouldBeTrue("'abc'.startsWith('a', null)");
shouldBeTrue("'abc'.startsWith('a', NaN)");
shouldBeTrue("'abc'.startsWith('a', '0')");
shouldBeFalse("'abc'.startsWith('a', '1')");

// non-string objects
shouldBeTrue("'123'.startsWith(1)");
shouldBeFalse("'123'.startsWith(4)");
var arr = ['a', 'b'];
arr.startsWith = String.prototype.startsWith;
shouldBeTrue("arr.startsWith('a')");
var obj = { toString: function() { return "xyz"; } };
obj.startsWith = String.prototype.startsWith;
shouldBeTrue("obj.startsWith('xyz')");

// invalid
shouldThrow("'abc'.startsWith(/a/)");

///////////// String.prototype.endsWith() ////////////////

shouldBe("String.prototype.endsWith.length", "1");

// simple
shouldBeTrue("'abc'.endsWith('c')");
shouldBeTrue("'abc'.endsWith('bc')");
shouldBeFalse("'abc'.endsWith('b')");
shouldBeFalse("'abc'.endsWith('x')");
shouldBeFalse("'abc'.endsWith('abcd')");

// with start position
shouldBeTrue("'abc'.endsWith('c', 3)");
shouldBeTrue("'abc'.endsWith('c', 4)");
shouldBeTrue("'abc'.endsWith('b', 2)");
shouldBeFalse("'abc'.endsWith('a', 0)");
shouldBeFalse("'abc'.endsWith('c', -1)");
shouldBeFalse("'abc'.endsWith('c', -Infinity)");
shouldBeTrue("'abc'.endsWith('c', +Infinity)");

// non-numerical position parameter
shouldBeTrue("'abc'.endsWith('c', undefined)");
shouldBeFalse("'abc'.endsWith('a', null)");
shouldBeFalse("'abc'.endsWith('c', null)");
shouldBeFalse("'abc'.endsWith('a', NaN)");
shouldBeFalse("'abc'.endsWith('c', NaN)");
shouldBeTrue("'abc'.endsWith('c', '4')");
shouldBeFalse("'abc'.endsWith('b', '1')");

// non-string objects
shouldBeTrue("'123'.endsWith(3)");
shouldBeFalse("'123'.endsWith(4)");
var arr = ['a', 'b'];
arr.endsWith = String.prototype.endsWith;
shouldBeTrue("arr.endsWith('b')");
var obj = { toString: function() { return "xyz"; } };
obj.endsWith = String.prototype.endsWith;
shouldBeTrue("obj.endsWith('xyz')");

// invalid
shouldThrow("'abc'.endsWith(/a/)");
