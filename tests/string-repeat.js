/* Copyright (C) 2019 froglogic GmbH */

// length property
shouldBe("String.prototype.repeat.length", "1");

// basic usage
shouldBe("''.repeat(1)", "''");
shouldBe("'ab'.repeat(0)", "''");
shouldBe("'ab'.repeat(1)", "'ab'");
shouldBe("'ab'.repeat(1.2)", "'ab'");
shouldBe("'ab'.repeat(1.9)", "'ab'");
shouldBe("'ab'.repeat(2)", "'abab'");
shouldBe("'ab'.repeat()", "''");
shouldBe("'ab'.repeat(NaN)", "''");

// generic applications
shouldBe("String.prototype.repeat.call(true, 2)", "'truetrue'");
shouldBe("String.prototype.repeat.call(1, 2)", "'11'");
shouldBe("String.prototype.repeat.call({toString: function() { return 'XY' }}, 2)", "'XYXY'");
shouldBe("String.prototype.repeat.call(/a/, 2)", "'/a//a/'");

// error cases
shouldExcept(RangeError, function() { 'a'.repeat(Infinity); }, "Infinite repeat");
shouldExcept(RangeError, function() { 'a'.repeat(-1); }, "-1 repeat");
shouldExcept(RangeError, function() { 'a'.repeat(-Infinity); }, "-Infinite repeat");
shouldExcept(TypeError, function() { String.prototype.repeat.call(undefined, 1) }, "Repeat undefined");
shouldExcept(TypeError, function() { String.prototype.repeat.call(null, 1) }, "Repeat null");
shouldExcept(TypeError, function() { String.prototype.repeat.call({toString: undefined}, 2) }, "Failed toObject()");
