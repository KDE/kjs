shouldBe("Object.prototype.__proto__ == Object.prototype","false");
shouldBe("Function.prototype.__proto__","Object.prototype");
shouldBe("Array.prototype.__proto__","Object.prototype");
shouldBe("String.prototype.__proto__","Object.prototype");
shouldBe("Boolean.prototype.__proto__","Object.prototype");
shouldBe("Number.prototype.__proto__","Object.prototype");
shouldBe("Date.prototype.__proto__","Object.prototype");
shouldBe("RegExp.prototype.__proto__","Object.prototype");
shouldBe("Error.prototype.__proto__","Object.prototype");
shouldBe("o = {}; v = 'FAIL'; try { o.__proto__ = o; } catch (e) { v = 'PA' }; o.foo = 'SS'; v = v + o.foo;","'PASS'");