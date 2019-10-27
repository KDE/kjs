// Tests for making sure we properly handle negative index properties
// (#204143, #262398)

A  = {"-1" : true }

// As a value...
shouldBe("A['-1']", "true");
shouldBe("A[-1]", "true");

// As a reference...
B = {}
B['-1'] = true;
shouldBe("B[-1]", "true");
shouldBe("B['-1']", "true");

C = {}
C[-1] = true;
shouldBe("C[-1]", "true");
shouldBe("C['-1']", "true");

// And as one in delete context

D = {"-1" : true}
shouldBe("D['-1']", "true");
shouldBe("D[-1]", "true");
shouldBe("delete D['-1']", "true");
shouldBe("D['-1']", "undefined");
shouldBe("delete D['-1']", "true");

E = {"-1" : true}
shouldBe("E['-1']", "true");
shouldBe("E[-1]", "true");
shouldBe("delete E[-1]", "true");
shouldBe("E['-1']", "undefined");
shouldBe("delete E[-1]", "true");

debug("Done.");
