var x = 0;
eval("\u0009 \u000B \u000C \u00A0x = 1;");
shouldBe("x", "1");

// integer
shouldBe("0", "0");
shouldBe("1", "1");
shouldBe("123", "123");
shouldBe("-123", "-123");
shouldBe("String(9007199254740992)", "'9007199254740992'");
shouldBe("String(144115188075855870)", "'144115188075855870'");

// scientific
shouldBe("12E02", "1200");
shouldBe("2E-02", "0.02");

// hex
shouldBe("0x0", "0");
shouldBe("0xF", "15");
shouldBe("0XF", "15");
shouldBe("0xFF", "255");
shouldBe("0x20000000000000", "9007199254740992");
shouldBe("0x200000000000000", "144115188075855870");
shouldThrow("0x");
shouldThrow("0xZ");
shouldThrow("0xFZ");

// binary
shouldBe("0b0", "0");
shouldBe("0B0", "0");
shouldBe("0b1", "1");
shouldBe("0b10", "2");
shouldBe("0b11111111", "255");
shouldThrow("0b");
shouldThrow("0b2");
shouldThrow("0b12");
shouldThrow("0b1in");

// octal
shouldBe("0o1", "1");
shouldBe("0o10", "8");
shouldBe("0O1", "1");
shouldBe("0O10", "8");
shouldBe("0o400000000000000000", "9007199254740992");
shouldBe("0o10000000000000000000", "144115188075855870");

shouldThrow("0o");
shouldThrow("0o9");
shouldThrow("0o19");
shouldThrow("0o1in");

// legacy octal (not available in strict mode)
shouldBe("01", "1");
shouldBe("010", "8");
shouldBe("09", "9");
shouldBe("019", "19")
shouldThrow("010in");
