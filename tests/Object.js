shouldBe("typeof Object()", "'object'");
shouldBe("var o = Object(); o.x = 11; o.x;", "11");
shouldBeTrue("Object(undefined).__proto__ === Object.prototype");
shouldBeTrue("Object(null).__proto__ === Object.prototype");
shouldBe("Object(1).valueOf()", "1");
shouldBe("Object(true).valueOf()", "true");
shouldBe("Object('s').valueOf()", "'s'");
shouldBe("Object({x: 12}).x", "12");

shouldBe("typeof (new Object())", "'object'");
shouldBeTrue("(new Object(undefined)).__proto__ === Object.prototype");
shouldBeTrue("(new Object(null)).__proto__ === Object.prototype");
shouldBeTrue("(new Object(1)).__proto__ === Number.prototype");
shouldBe("(new Object(1)).valueOf()", "1");
shouldBeTrue("(new Object(true)).__proto__ === Boolean.prototype");
shouldBe("(new Object(true)).valueOf()", "true");
shouldBeTrue("(new Object('s')).__proto__ === String.prototype");
shouldBe("(new Object('s')).valueOf()", "'s'");

shouldBe("String(Object())", "'[object Object]'");
shouldBe("Object().toString()", "'[object Object]'");
shouldBe("String(Object().valueOf())", "'[object Object]'");
