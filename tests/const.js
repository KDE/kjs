// constant definition
const c = 11;
shouldBe("c", "11");

// attempt to redefine should have no effect
c = 22;
shouldBe("c", "11");

const dummy = 0;
for (var v = 0;;) {
  ++v;
  shouldBe("v", "1");
  break;
}

// local vars & consts
function h ()
{
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

	var hvar = 1;
	const hconst = 1;
    shouldBe("hvar", "1");
    shouldBe("hconst", "1");

	hvar = 2;
	hconst = 2;
    shouldBe("hvar", "2");
    shouldBe("hconst", "1");

	++hvar;
	++hconst;
    shouldBe("hvar", "3");
    shouldBe("hconst", "1");

    shouldBe("(hvar = 4)", "4");
    shouldBe("(hconst = 4)", "4");

    shouldBe("hvar", "4");
    shouldBe("hconst", "1");
}

h();
h();

// ### check for forbidden redeclaration
