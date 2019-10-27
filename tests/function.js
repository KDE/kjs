///////////////////////////////
// empty bodies

function empty1() { ; }
shouldBe("empty1()", "undefined");

function empty2() { }
shouldBe("empty2()", "undefined");

var g = 2;
var obj = new Object();
with (obj) {
  obj.func = function(a) { return g*l*a; };
}
obj.l = 3;

shouldBe("obj.func(11)", "66");

///////////////////////////////

function ctor(xx) {
  this.x = xx;
  this.dummy = new Array();  // once broke the returned completion
}

c = new ctor(11);
shouldBe("c.x", "11");

///////////////////////////////
// anonymous functions
///////////////////////////////

f = function (arg) {
  return arg;
};


shouldBe("f('bbbbb')", "'bbbbb'");

/////////////////////////////
// named function expressions
/////////////////////////////

var objectWithFunctionProperty = { f: function z(a) { return a*a; } }; 
shouldBe("objectWithFunctionProperty.f(3)", "9");

////////////////////////////////////

function f() {return 1;};

// just verify that this can be parsed
Math.round(function anon() { });

// verify that this is not broken (happened once)
function onceBroken() { return new String(); }

///////////////////////////////
// arguments object
///////////////////////////////

function foo() {
  var result = "|";
  for (i = 0; i < arguments.length; i++)
    result += arguments[i] + "|";
  return result;
}

shouldBe("foo()", "'|'");
shouldBe("foo('bar')", "'|bar|'");
shouldBe("foo('bar', 'x')", "'|bar|x|'");

function foo2(a) {
  var i = 0;
  for(a in arguments) // should NOT be enumerable
    i++;
  return i;
}

shouldBe("foo2(7)", "0");

// I have my doubts about the standard conformance of this
function foo3(i, j) {
  switch(i) {
    case 0:
       return foo3.arguments.length;
    case 1:
       return foo3.arguments;
    case 2:
       return foo3.j;
  }
}

shouldBe("foo3(0, 99)", "2");
shouldBe("foo3(1, 99).length", "2");
//shouldBe("foo3(2, 99)", "99"); // IE doesn't support this either

///////////////////////////////
// nested function declarations
///////////////////////////////

function nest0(a, b)
{
  function nest1(c) { return c*c; }
  return nest1(a*b);
}
shouldBe("nest0(2,3)", "36");

///////////////////////////////
// Function object
///////////////////////////////

shouldBe("(new Function('return 11;'))()", "11");
shouldBe("(new Function('', 'return 22;'))()", "22");
shouldBe("(new Function('a', 'b', 'return a*b;'))(11, 3)", "33");
shouldBe("(new Function(' a ', ' b ', 'return a*b;'))(11, 4)", "44");
shouldBe("(new Function(' a,b ', ' c ', 'return a*b;'))(11, 5)", "55");
shouldBe("(new Function(' a,b ', ' c3 ', 'return a*b;'))(11, 6)", "66");


///////////////////////////////
// length property
function funcp3(a, b, c) { }
shouldBe("funcp3.length", "3");
shouldBe("(function(a, b, c, d) { }).length", "4");
shouldBe("(new Function('a', 'b', '')).length", "2");

// recursive call
function f4(i)
{
   return i == 1 ? 1 : i*f4(i-1);
}

shouldBe("f4(4)", "24");

function f5(a) {
  return f6();
}

function f6() { 
  return f5.arguments[0];
}

shouldBe("f5(11)", "11");
shouldBe("f5.arguments", "null");

///////////////////////////////
// calling conventions
///////////////////////////////

function manipulate(x) {
   x[0] = 99;		// should operate on reference
   x = "nothing";	// should detach
}
var arr = new Array(1, 2, 3);
manipulate(arr);
shouldBe("arr[0]", "99");
arr = [1, 2, 3];
manipulate(arr);
shouldBe("arr[0]", "99");

///////////////////////////////
// toString() on functions
///////////////////////////////

function orgFunc(s) { return 'Hello ' + s; }
eval("var orgFuncClone = " + orgFunc);
shouldBe("typeof orgFuncClone", "'function'");
shouldBe("orgFuncClone('world')", "'Hello world'");

function groupFunc(a, b) { return (a+b)*3; } // check for () being preserved
eval("var groupClone = " + groupFunc);
shouldBe("groupClone(1, 2)", "9");

///////////////////////////////
// shadowed functions
///////////////////////////////

function shadow() { return 1; }
function shadow() { return 2; }
shouldBe("shadow()", "2");

/**** not portable *******

///////////////////////////////
// nested functions

var nest_property = "global nest";

function nested_outer() {

  function nested_inner() {
    return nest_property;
  }

}

function not_nested() {
  return nest_property;
}

nested_outer.nest_property = "outer nest";

var nested = nested_outer.nested_inner;
shouldBe("nested()","'outer nest'");

var not_nestedret = not_nested();
shouldBe("not_nestedret","'global nest'");

*** end of non-portable tests ***** */

///////////////////////////////
// other properties
///////////////////////////////
function sample() { }
shouldBeUndefined("sample.prototype.prototype");
shouldBeTrue("sample.prototype.constructor == sample");

var caught = false;
try {
  sample.apply(1, 1); // invalid argument
} catch(dummy) {
  caught = true;
}
shouldBeTrue("caught");


function functionWithVarOverride(a) {
  var a = 2;
  return a;
}

shouldBe("functionWithVarOverride(1)", "2");

// named function expressions
var fabs = function abs(x) { return x >= 0 ? x : abs(-x); }
shouldBe("fabs(-4)", "4");

function fh(x) {
  return function diff(arg) {
    if (arg == x)
      return 0;
    else
      return 1 + diff(arg + 1);
  };
};
shouldBe("fh(5)(2)", "3");

// test flags of function prototype's constructor property
// inspired by an Acid3 test
var funcToModify1 = function () { 1 };
funcToModify1.prototype.constructor = "altCtor1";
shouldBe("funcToModify1.prototype.constructor", "'altCtor1'");
delete funcToModify1.prototype.constructor;
shouldBe("funcToModify1.prototype.constructor", "Object.prototype.constructor");
function funcToModify2() { 1 };
funcToModify2.prototype.constructor = "altCtor2";
shouldBe("funcToModify2.prototype.constructor", "'altCtor2'");
delete funcToModify2.prototype.constructor;
shouldBe("funcToModify2.prototype.constructor", "Object.prototype.constructor");

debug("Done");

// .call

var testThis = {};

function callTest(a, b) {
	_saveThis = this;
	_saveA    = a;
	_saveB    = b;
	shouldBe("_saveThis", "testThis");
	shouldBe("_saveA", 1);
	shouldBe("_saveB", 42);
}

callTest.call(testThis, 1, 42);
