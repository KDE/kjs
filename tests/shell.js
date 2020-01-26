// Utility functions for JS test suite
// (c) 2001 Harri Porten <porten@kde.org>
// (c) 2020 froglogic GmbH <contact@froglogic.com>

function testPassed(msg)
{
  debug("PASS. " + msg);
}

function testFailed(msg)
{
  debug("FAIL. " + msg);
  if (this.regtest)
    regtest.reportResult(false, msg);  
}

function description(s)
{
    debug(s);
}

function evalError(arg, ex)
{
    //return "eval('" + arg + "') => " + ex.toString();
    return ex.toString() + " [eval('" + arg + "')]";
}

function shouldBe(_a, _b)
{
  if (typeof _a != "string" || typeof _b != "string")
    debug("WARN: shouldBe() expects string arguments");
  var _av, _bv;
  try {
    _av = eval(_a);
  } catch (e) {
    _av = evalError(_a, e);
  }
  try {
    _bv = eval(_b);
  } catch (e) {
    _bv = evalError(_b, e);
  }

  if (_av === _bv)
    testPassed(_a + " is " + _b);
  else
    testFailed(_a + " should be " + _bv + ". Was " + _av);
}

function shouldBeTrue(_a) { shouldBe(_a, "true"); }

function shouldBeFalse(_a) { shouldBe(_a, "false"); }

function shouldBeUndefined(_a)
{
  var _av;
  try {
    _av = eval(_a);
  } catch (e) {
    _av = evalError(_a, e);
  }

  if (typeof _av == "undefined")
    testPassed(_a + " is undefined.");
  else
    testFailed(_a + " should be undefined. Was " + _av);
}

function shouldThrow(_a, _e)
{
  var exception;
  var _av;
  try {
     _av = eval(_a);
  } catch (e) {
     exception = e;
  }

  var _ev;
  if (_e)
      _ev =  eval(_e);

  if (exception) {
    if (typeof _e == "undefined" || exception == _ev)
      testPassed(_a + " threw exception " + exception + ".");
    else
      testFailed(_a + " should throw exception " + _ev + ". Threw exception " + exception + ".");
  } else if (typeof _av == "undefined")
    testFailed(_a + " should throw exception " + _e + ". Was undefined.");
  else
    testFailed(_a + " should throw exception " + _e + ". Was " + _av + ".");
}

function shouldExcept(error, func, msg)
{
    var exception;
    try {
        func();
    } catch (e) {
        exception = e;
    }
    if (exception) {
        if (exception instanceof error) {
            test.pass(msg + ": got expected exception " + error.name);
        } else {
            test.fail(msg + ": got wrong exception. Name: " + error.name);
        }
    } else {
        test.fail(msg + ": function did not throw an exception");
    }
}
