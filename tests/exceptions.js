function shouldBe(a, b, c)
{
  if ( a == b )
   debug(c+" .......... PASS");
  else
   debug(c+" .......... FAIL");
}

function testThrow()
{
  var caught = false;
  try {
    throw 99;
  } catch (e) {
    caught = true;
  }
  shouldBe(caught, true, "testing throw()");
}

// same as above but lacking a semicolon after throw
function testThrow2()
{
  var caught = false;
  try {
    throw 99
  } catch (e) {
    caught = true;
  }
  shouldBe(caught, true, "testing throw()");
}

function testReferenceError()
{
  var err = "noerror";
  var caught = false;
  try {
    var dummy = nonexistant; // throws reference error
  } catch (e) {
    caught = true;
    err = e.name;
  }
  // test err
  shouldBe(caught, true, "ReferenceError");
}

function testFunctionErrorHelper()
{
   var a = b;  // throws reference error
}

function testFunctionError()
{
  var caught = false;
  try {
    testFunctionErrorHelper();
  } catch (e) {
    caught = true;
  }
  shouldBe(caught, true, "error propagation in functions");
}

function testMathFunctionError()
{
  var caught = false;
  try {
    Math();
  } catch (e) {
    debug("catch");
    caught = true;
  } finally {
    debug("finally");
  }
  shouldBe(caught, true, "Math() error");
}

function testWhileAbortion()
{
  var caught = 0;
  try { 
    while (a=b, 1) { 	// "endless error" in condition
      ;
    }
  } catch (e) {
    caught++;
  }

  try { 
    while (1) {
      var a = b;	// error in body
    }
  } catch (e) {
    caught++;
  }
  shouldBe(caught, 2, "Abort while() on error");
}

function testCrashes()
{
    var caught = 0;
    // prefix node returned an invalid object in case of an error
    try {
	eval("!--D()");
    } catch(e) {
	++caught;
    }
    try {
	eval("--D() + ''");
    } catch(e) {
	++caught;
    }
    shouldBe(caught, 2, "no crashes");
}

function testVariable()
{
    try {
	throw 1;
    } catch (x) { // local to catch-block?
	var x = 2;
	var y = 3;
	z = 4;
	shouldBe(x, 2, "exception variable 1");
	shouldBe(y, 3, "exception variable 2");
	shouldBe(z, 4, "exception variable 3");
    }
    // IE and Firefox disagree on this one
    var pass = typeof x == "undefined" || x == 2;
    shouldBe(pass, true, "out-of-scope exception variable 1");
    shouldBe(y, 3, "out-of-scope exception variable 2");
    shouldBe(z, 4, "out-of-scope exception variable 3");
}

debug("Except a lot of errors. They should all be caught and lead to PASS");
testThrow();
testThrow2();
testReferenceError();
testFunctionError();
testMathFunctionError();
testWhileAbortion();
testCrashes();
testVariable();
