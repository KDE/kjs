// 12.6.1
var count = 0;
do {
  count++;
} while (count < 10);
shouldBe("count", "10");

count = 0;
for (var i = 0; i < 10; i++) {
  if (i == 5)
    break;
  count++;
}
shouldBe("count", "5");

// 12.6.3
count = 0;
for (i = 0; i < 10; i++) {
  count++;
}
shouldBe("count", "10");

// 12.6.4
obj = new Object();
obj.a = 11;
obj.b = 22;

properties = "";
for ( prop in obj )
  properties += (prop + "=" + obj[prop] + ";");

shouldBe("properties", "'a=11;b=22;'");

// now a test verifying the order. not standardized but common.
obj.y = 33;
obj.x = 44;
properties = "";
for ( prop in obj )
  properties += prop;
// shouldBe("properties", "'abyx'");

arr = new Array;
arr[0] = 100;
arr[1] = 101;
list = "";
for ( var j in arr ) {
  list += "[" + j + "]=" + arr[j] + ";";
}
shouldBe("list","'[0]=100;[1]=101;'");

list = "";
for (var a = [1,2,3], length = a.length, i = 0; i < length; i++) {
  list += a[i];
}
shouldBe("list", "'123'");

// completion values
var immediateWhileBreak = eval("while (true) { break; }");
shouldBeUndefined("immediateWhileBreak", "'foo'");
var laterWhileBreak = eval("while (true) { 'foo'; break; }");
shouldBe("laterWhileBreak", "'foo'");
var whileEnd = eval("var y = 0; while (y < 2) { ++y }");
shouldBe("whileEnd", "2");

var immediateDoWhileBreak = eval("do { break; } while(false); ");
shouldBeUndefined("immediateDoWhileBreak", "'foo'");
var laterDoWhileBreak = eval("do { 'bar'; break; } while(true);");
shouldBe("laterDoWhileBreak", "'bar'");
var doWhileEnd = eval("var y = 0; do { ++y } while (y < 3);");
shouldBe("doWhileEnd", "3");

var immediateForBreak = eval("for(;;) { break; }");
shouldBeUndefined("immediateForBreak", "'foo'");
var laterForBreak = eval("for(;;) { 'zilch'; break; }");
shouldBe("laterForBreak", "'zilch'");
var forEnd = eval("for(var y = 0; y < 4; ++y) { 'quark' }");
shouldBe("forEnd", "'quark'");

// Various stuff for control flow and error handling of break/continue
var iters;

// Test whether we handle multiple labels on the same statement right
// Note: this is in a function since konq 3.x just breaks out of the whole thing.
function testLabelNest() {
  iters = 0;
  foo: bar: while (true) {
    ++iters;
    if (iters < 10)
      continue foo;
    else
      break;
  }
}

testLabelNest();
shouldBe('iters', '10');

// Test to make sure doing continue outside a loop
// throws properly. (3.x didn't), and so does a label-less
// break outside of it.
shouldThrow('foo: continue foo; ');
shouldThrow('continue; ');
shouldThrow('break;');

// These ones should be fine OTOH
shouldBeUndefined('foo: break foo; ');
shouldBe('foo: { a = 1; break foo; a = 2; }', '1');

// Make sure we don't break out too far.
function testLocalBreak() {
  l: break l;
}

for (iters = 0; iters < 10; ++iters)
  testLocalBreak();

shouldBe('iters', '10');

// Regression test, bug during FB development.
var o = {a: 0, b: 1}
for (p in o) {
    if (p == "b")
        break;
    continue;
}

testPassed("Didn't crash on continue in for .. in");

/** Regression test for #165847: making sure that break
out of a for ... in cleans stuff up properly from the for ... in stack
*/
var list = [];
for (var i = 0; i < 10; ++i)
  list.push(i);

done = false;
for (i in list) {
  for (j in list) {
    if (i == 0 && j == 1) {
        done = true;
        break;
    }

    if (done)
      break;
  }
}



debug("Done.");
