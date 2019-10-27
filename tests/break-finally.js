
// Easy case -- normal try-finally
out = "";
try {
    while(true) {
	out += "loop;";
	break;
    }
    out += "after;"
} catch(e) {
    out += "catch;"
} finally {
    out += "finally;"
}

shouldBe('out', '"loop;after;finally;"');

// A try nested inside the loop
out = "";
while (true) {
    out += "loop;";
    try {
	break;
    } catch(e) {
	out += "catch;"
    } finally {
	out += "finally;"
    }
    out += "after-try;";
}
out += "after-loop";

shouldBe('out', '"loop;finally;after-loop"');


// Throwing in a break shouldn't change things
out = "";
while (true) {
    out += "loop;";
    try {
	break;
    } catch(e) {
	out += "catch;"
    } finally {
	out += "finally;"
	break;
    }
    out += "after-try;";
}
out += "after-loop";

shouldBe('out', '"loop;finally;after-loop"');

// Continue & finally overriding the abrupt completion
out = "";
while (true) {
    out += "loop;";
    try {
	continue;
    } catch(e) {
	out += "catch;"
    } finally {
	out += "finally;"
	break;
    }
    out += "after-try;";
}
out += "after-loop";
shouldBe('out', '"loop;finally;after-loop"');

// Multi-level breaks. Fun.
out = "";
outer: while (true) {
    out += "out;"

    try {
	while (true) {
	    out += "in;"
	    break outer;
	}
    } finally {
	out += "finally;";
    }
    out += "after-2";
}
out += "after-1";

shouldBe('out', '"out;in;finally;after-1"');


// This testcase is by Oliver Hunt  (oliver at apple dt com)
msg="global";

out="";
function record(str) {
    out += "<" + str + ">";
}

function f() {
    L3: do {
        try {
            with ({msg:"outer3"}) {
                L2: do {
                    try {
                        with ({msg:"outer2"}) {
                            L1: do {
									try {
										with ({msg:"outer1"}) {
											record(msg);
											continue L1;
										}
									} finally {
										record("finally1")
										return;
									}
                            } while(0)
                            record(msg);
                            continue L2;
                        }
                    } finally {
                        record("finally2")
                        return;
                    }
                } while(0)
                record(msg);
                continue L3;
            }
        } finally {
            record("finally3")
            return;
        }
        record("while");
    } while(0);
}
f();
record(msg);
shouldBe('out', '"<outer1><finally1><finally2><finally3><global>"');

print("Done.");
