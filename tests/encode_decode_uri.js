/*

var enc = encodeURI;
var dec = decodeURI;
//var enc = encodeURIComponent;
//var dec = decodeURIComponent;

function printstr(i,j)
{
  var str;
  if (j == undefined)
    str = "x"+String.fromCharCode(i)+"x";
  else
    str = "x"+String.fromCharCode(i)+String.fromCharCode(j)+"x";
  var encoded = "(exception)";
  var decoded = "(unknown)";
  try {
    encoded = enc(str);
    decoded = "(exception)";
    decoded = dec(encoded);
  }
  catch (e) {
  }

  var prefix = i;
  if (j != undefined)
    prefix += "/" + j;

  var match;
  if (encoded == "(exception)" || decoded == "(exception)")
    match = "";
  else if (str == decoded)
    match = " (match)";
  else
    match = " (nomatch)";

  print(prefix+": encoded "+encoded+" decoded "+decoded+" "+match);
}

for (var charcode = 0; charcode <= 0xFFFF; charcode++)
  printstr(charcode);

for (var charcode = 0xDC00; charcode <= 0xDFFF; charcode++)
  printstr(0xD800,charcode);

for (var charcode = 0xD800; charcode <= 0xDBFF; charcode++)
  printstr(charcode,0xD800);

*/

// --------------------------------------------------------------------------------

var resolution = 13; // set to 1 for 100% coverage

function checkEncodeException(encodeFunction,c1,c2)
{
  var str;
  if (c2 == undefined)
    str = "x"+String.fromCharCode(c1)+"x";
  else
    str = "x"+String.fromCharCode(c1)+String.fromCharCode(c2)+"x";

  try {
    encodeFunction(str);
//    print("checkEncodeException("+c1+","+c2+"): false");
    return false;
  }
  catch (e) {
//    print("checkEncodeException("+c1+","+c2+"): true");
    return true;
  }
}

function checkEncodeDecode(encodeFunction,decodeFunction,c1,c2)
{
  var str;
  if (c2 == undefined)
    str = "x"+String.fromCharCode(c1)+"x";
  else
    str = "x"+String.fromCharCode(c1)+String.fromCharCode(c2)+"x";

  try {
    var encoded = encodeFunction(str);
    var decoded = decodeFunction(encoded);

//    print("checkEncodeDecode("+c1+","+c2+"): "+(str == decoded));
    if (str != decoded) {
        debug("Encode/Decode mismatch with c1:" + c1 + " c2:" + c2);
        debug("input: "  + str);
        debug("encoded:" + encoded);
        debug("decoded:" + decoded);
        for (var i = 0; i < decoded.length; ++i)
            debug("at pos:" + i + " " + decoded.charCodeAt(i));
    }
    return (str == decoded);
  }
  catch (e) {
    debug("Exception with c1:" + c1 + " c2:" + c2);
//    print("checkEncodeDecode("+c1+","+c2+"): false (exception)");
    return false;
  }
}

/********* disabled because of conflicts ***********

function checkWithFunctions(encodeFunction,decodeFunction)
{
  var passes = 0;
  var failures = 0;
  var totalFailures = 0;

  // 0-0xD800 and 0xDC00-0xFFFF

  for (var charcode = 0; charcode < 0xD800; charcode += resolution) {
    if (checkEncodeDecode(encodeFunction,decodeFunction,charcode))
      passes++;
    else
      failures++;
  }

  debug("0x0000 - 0xD7FF range, passes:" + passes + ", failures:" + failures);

  totalFailures += failures;
  failures = 0;
  passes   = 0;

  for (var charcode = 0xE000; charcode <= 0xFFFF; charcode += resolution) {
    if (checkEncodeDecode(encodeFunction,decodeFunction,charcode))
      passes++;
    else
      failures++;
  }

  debug("0xE000 - 0xFFFF range, passes:" + passes + ", failures:" + failures);
  totalFailures += failures;
  failures = 0;
  passes   = 0;

  // 0xDC00-0xDFFF
  for (var charcode = 0xDC00; charcode <= 0xDFFF; charcode += resolution) {
    if (checkEncodeException(encodeFunction,charcode))
      passes++;
    else
      failures++;
  }

  debug("0xDC00 - 0xDFFF range (expected exception), passes:" + passes + ", failures:" + failures);
  totalFailures += failures;
  failures = 0;
  passes   = 0;


  // 0xD800-0xDBFF followed by 0xDC00-0xDFFF

  for (var charcode = 0xD800; charcode < 0xDC00; charcode += resolution) {
    if (checkEncodeDecode(encodeFunction,decodeFunction,charcode,0xDC00))
      passes++;
    else
      failures++;
  }

  for (var charcode = 0xDC00; charcode < 0xE000; charcode += resolution) {
    if (checkEncodeDecode(encodeFunction,decodeFunction,0xD800,charcode))
      passes++;
    else
      failures++;
  }

  debug("Surrogate pairs test, passes:" + passes + ", failures:" + failures);
  totalFailures += failures;
  failures = 0;
  passes   = 0;

  // 0xD800-0xDBFF _not_ followed by 0xDC00-0xDFFF

  for (var charcode = 0; charcode < 0xDC00; charcode += resolution) {
    if (checkEncodeException(encodeFunction,0xD800,charcode))
      passes++;
    else
      failures++;
  }

  for (var charcode = 0xE000; charcode <= 0xFFFF; charcode += resolution) {
    if (checkEncodeException(encodeFunction,0xD800,charcode))
      passes++;
    else
      failures++;
  }

  debug("Unfinished surrogate pairs test (expected exception), passes:" + passes + ", failures:" + failures);
  totalFailures += failures;
  failures = 0;
  passes   = 0;

//  print("passes = "+passes);
//  print("failures = "+failures);

  return (totalFailures == 0);
}


shouldBe("checkWithFunctions(encodeURI,decodeURI)","true");
shouldBe("checkWithFunctions(encodeURIComponent,decodeURIComponent)","true");

*********** end of disabled tests *********/

// reserved characters are not supposed to be decoded
shouldBe('decodeURI("%23")', '"%23"'); // #
shouldBe('decodeURI("%24")', '"%24"'); // $
shouldBe('decodeURI("%26")', '"%26"'); // &
shouldBe('decodeURI("%2b")', '"%2b"'); // +
shouldBe('decodeURI("%2c")', '"%2c"'); // ,
shouldBe('decodeURI("%2f")', '"%2f"'); // /
shouldBe('decodeURI("%3a")', '"%3a"'); // :
shouldBe('decodeURI("%3b")', '"%3b"'); // ;
shouldBe('decodeURI("%3d")', '"%3d"'); // =
shouldBe('decodeURI("%3f")', '"%3f"'); // ?
shouldBe('decodeURI("%40")', '"%40"'); // @

function checkInvalid(s)
{
    var d;
    try {
	d = decodeURI(s);
    } catch(e) {
	testPassed('decodeURI("' + s + '") threw an exception');
	return;
    }
    if (d.length == 1 && d.charCodeAt(0) == 65533 ||
	(d.length == 2 &&
	 d.charCodeAt(0) == 65533 && d.charCodeAt(1) == 65533)) {
	testPassed('decodeURI("' + s + '") resulted in replacement chars');
	return;
    }
    if (d == s) {
	testPassed('decodeURI("' + s + '") was not decoded');
	return;
    }

    testFailed('decodeURI("' + s + '") was decoded to "' + d + '"');
}

//
// based on http://www.cl.cam.ac.uk/~mgk25/ucs/examples/UTF-8-test.txt
//

// unexpected continuation bytes
checkInvalid("%80");
checkInvalid("%bf");
checkInvalid("%80%bf");
checkInvalid("%80%bf%80");
checkInvalid("%80%bf%80%bf");
checkInvalid("%80%bf%80%bf%80");
checkInvalid("%80%bf%80%bf%80%bf");
for (var cb = parseInt("80", 16); cb <= parseInt("BF", 16); ++cb)
    checkInvalid("%" + cb.toString(16));

// Lonely start characters
// All 32 first bytes of 2-byte sequences (0xc0-0xdf),
// each followed by a space character
for (var cb = parseInt("C0", 16); cb <= parseInt("DF", 16); ++cb)
    checkInvalid("%" + cb.toString(16) + "%20");

// All 16 first bytes of 3-byte sequences (0xe0-0xef),
// each followed by a space character
for (var cb = parseInt("E0", 16); cb <= parseInt("EF", 16); ++cb)
    checkInvalid("%" + cb.toString(16) + "%20");

// All 8 first bytes of 4-byte sequences (0xf0-0xf7),
// each followed by a space character
for (var cb = parseInt("E0", 16); cb <= parseInt("EF", 16); ++cb)
    checkInvalid("%" + cb.toString(16) + "%20");

// All 4 first bytes of 5-byte sequences (0xf8-0xfb),
// each followed by a space character
for (var cb = parseInt("F8", 16); cb <= parseInt("FB", 16); ++cb)
    checkInvalid("%" + cb.toString(16) + "%20");

// All 2 first bytes of 6-byte sequences (0xfc-0xfd),
// each followed by a space character
for (var cb = parseInt("FC", 16); cb <= parseInt("FD", 16); ++cb)
    checkInvalid("%" + cb.toString(16) + "%20");

// Sequences with last continuation byte missing
checkInvalid("%C0"); // 2-byte sequence with last byte missing (U+0000)
checkInvalid("%E0%80"); // 3-byte sequence with last byte missing (U+0000)
checkInvalid("%F0%80%80"); // 4-byte sequence with last byte missing (U+0000)
checkInvalid("%F8%80%80%80");
checkInvalid("%FC%80%80%80%80");
checkInvalid("%DF");
checkInvalid("%EF%BF");
checkInvalid("%F7%BF%BF");
checkInvalid("%FB%BF%BF%BF");
checkInvalid("%FD%BF%BF%BF%BF");

// impossible bytes
checkInvalid("%fe");
checkInvalid("%ff");
checkInvalid("%fe%fe%ff%ff");

// overlong ASCII character
checkInvalid("%C0%AF");
checkInvalid("%E0%80%AF");
checkInvalid("%F0%80%80%AF");
checkInvalid("%F8%80%80%80%AF");
checkInvalid("%FC%80%80%80%80%AF");

// maximum overlong sequences
checkInvalid("%c1%bf");
checkInvalid("%e0%9f%bf");
checkInvalid("%f0%8f%bf%bf");
checkInvalid("%f8%87%bf%bf%bf");
checkInvalid("%fc%83%bf%bf%bf%bf");

checkInvalid("%c0%80");
checkInvalid("%e0%80%80");
checkInvalid("%f0%80%80%80");
checkInvalid("%f8%80%80%80%80");
checkInvalid("%fc%80%80%80%80%80");

// invalid single UTF-16 surrogates
checkInvalid("%ed%a0%80"); // U+D800
checkInvalid("%ed%ad%bf"); // U+DB7F
checkInvalid("%ed%ae%80"); // U+DB80
checkInvalid("%ed%af%bf"); // U+DBFF
checkInvalid("%ed%b0%80"); // U+DC00 
checkInvalid("%ed%be%80"); // U+DF80
checkInvalid("%ed%bf%bf"); // U+DFFF

// invalid paired UTF-16 surrogates
checkInvalid("%ed%a0%80%ed%b0%80"); // U+D800 U+DC00
checkInvalid("%ed%a0%80%ed%bf%bf"); // U+D800 U+DFFF
checkInvalid("%ed%ad%bf%ed%b0%80"); // U+DB7F U+DC00
checkInvalid("%ed%ad%bf%ed%bf%bf"); // U+DB7F U+DFFF
checkInvalid("%ed%ae%80%ed%b0%80"); // U+DB80 U+DC00
checkInvalid("%ed%ae%80%ed%bf%bf"); // U+DB80 U+DFFF
checkInvalid("%ed%af%bf%ed%b0%80"); // U+DBFF U+DC00
checkInvalid("%ed%af%bf%ed%bf%bf"); // U+DBFF U+DFFF

// Other illegal code positions
checkInvalid("%EF%BF%BE"); // U+FFFE
checkInvalid("%EF%BF%BF"); // U+FFFF


// Regression test, #150381 -- make sure we don't
// reject OK things as invalid.
shouldBe('decodeURIComponent("%C3%A1")', '"\u00e1"');


debug("Done");
