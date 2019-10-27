var b = new Boolean();
b.x = 11;

with (b) {
  f = function(a) { return a*x; } // remember scope chain
}

shouldBe("f(2)", "22");

var OBJECT = new MyObject( "hello" );
function MyObject(value) {
    this.value = value;
    this.toString = new Function( "return this.value+''" );
    return this;
}
shouldBe("OBJECT.toString()", "'hello'");
var s;
with (OBJECT) {
    s = toString();
}
shouldBe("s", "'hello'");


// Make sure that for ... in reevaluates the scoping every time!
P = { foo : 1, bar : 2, baz : 3 }

function testForIn() {
   for (g in P) {
        eval("var g;") //Change the scope of g half-ways through the loop
   }
}

testForIn();
shouldBe("g", "'foo'"); //Before the eval, g was in outer scope, but not after!

// Check to make sure we cleanup the scope stack right
o = { x : 4 }
p = {};

x = 42;

label:with (o) {
    break label;
}

with(p) { // We need 'with' here to prevent bypassing of the scope stack by the optimizer
    y = x;
}

shouldBe('y', '42');

// And a nastier one, also with a catch scope
o = { x : 4 }
p = {};

x = 42;

label:try {
    throw "a";
} catch (x) {
    with (o) {
        break label;
    }
}

with(p) { // We need 'with' here to prevent bypassing of the scope stack by the optimizer
    y = x;
}

shouldBe('y', '42');

//# 108538 -- bind var assignments are proper assignments, not statically
function testVarInCatch()
{
   try {
      throw "any exception";
   } catch (x) { 
      var x = 2; // The catches scope is on of the chain, so
                 // this should assign there
      if (x != 2)
        return "FAIL";
   }

   // Now we should touch the normal local, which should be unchanged.
   if (x == undefined)
      return "PASS";
   else
      return "FAIL #2";
}

shouldBe('testVarInCatch()', '"PASS"');

function testVarInWith()
{
   o = { x: 42 };
   with (o) {
      if (x != 42)
        return "FAIL"; // Should lookup from 'o'.
      var x = 4; // Since o is in scope, should set there, not global.
      if (x != 4)
        return "FAIL #2";
      if (o.x != 4)
        return "FAIL #3";
   }
   if (x != undefined)
      return "FAIL #4";

   return "PASS";
}

shouldBe('testVarInWith()', '"PASS"');

// Local function injection with eval
function testFunInject()
{
    try {
	eval("function injected() { return 'PASS'; }");
	return injected();
    } catch (e) {
	return "Threw!";
    }
}

shouldBe('testFunInject()', '"PASS"');

debug("Done.");
