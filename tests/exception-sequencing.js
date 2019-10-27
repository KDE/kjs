/*
Note: this test is from Apple

Test exception handling with various arithmetic and logic operators, it checks the following things:<br/>
<ul>
    <li>In assignment expressions the lefthand side is not modified if the right hand side throws</li>
    <li>If the left hand side of a binary operator throws then the right hand should not be executed</li>
    <li>If valueOf/toString throws in the left hand expression of a binop it does not prevent evaluation of the right hand expression, but does prevent evaluation of toString/valueOf on the rhs.</li>
</ul>
*/

function captureGlobal() {
    global = this;
}

captureGlobal();

if (global["debug"])
    log = debug;
else
    log = print;
    
fail = testFailed;
pass = function(msg) { ++passCount; testPassed(msg); }

var unaryOps = ['~', '+', '-'];
var binaryOps = ['-', '+', '*', '/', '%', '|', '&', '^', '<', '<=', '>=', '>', '==', '!=', '<<', '>>'];

var valueOfThrower = { valueOf: function() { throw "throw from valueOf"; } }
var toStringThrower = { toString: function() { throw "throw from toString"; } }
var testCount = 0;
var passCount = 0;
function createTest(expr) {
    // This creates a test case that ensures that a binary operand will execute the left hand expression first,
    // and will not execute the right hand side if the left hand side throws.
    var functionPrefix = "(function(){ executedRHS = false; var result = 'PASS'; try { result = ";
    var functionPostfix = "; fail('Did not throw exception with \"' + expr + '\"') } catch (e) { " +
                          " if (result != 'PASS' && executedRHS) { fail('\"'+expr+'\" threw exception, but modified assignment target and executed RHS'); " +
                          " } else if (result != 'PASS') { fail('\"'+expr+'\" threw exception, but modified assignment target'); " +
                          " } else if (executedRHS) { fail('\"'+expr+'\" threw exception, but executed right hand half of expression')" +
                          " } else { pass('Handled \"'+ expr +'\" correctly.') } } })";
    testCount++;
    try {
        return eval(functionPrefix + expr + functionPostfix);
    } catch(e) {
        throw new String(expr);
    }
}

function createTestWithRHSExec(expr) {
    // This tests that we evaluate the right hand side of a binary expression before we
    // do any type conversion with toString and/or valueOf which may throw.
    var functionPrefix = "(function(){ executedRHS = false; var result = 'PASS'; try { result = ";
    var functionPostfix = "; fail('Did not throw exception with \"' + expr + '\"') } catch (e) { " +
                          " if (result != 'PASS') { fail('\"'+expr+'\" threw exception, but modified assignment target'); " +
                          " } else if (!executedRHS) { fail('\"'+expr+'\" threw exception, and failed to execute RHS when expected')" +
                          " } else { pass('Handled \"'+ expr +'\" correctly.') } } })";
    testCount++;
    try {
        return eval(functionPrefix + expr + functionPostfix);
    } catch(e) {
        throw new String(expr);
    }
}

__defineGetter__('throwingProperty', function(){ throw "throwing resolve"; });

var throwingPropStr = 'throwingProperty';
var valueOfThrowerStr = 'valueOfThrower';
var toStringThrowerStr = 'toStringThrower';
var throwingObjGetter = '({get throwingProperty(){ throw "throwing property" }}).throwingProperty';

createTest(throwingPropStr)();
createTest(throwingObjGetter)();
createTest("!undefinedProperty")();

for (var i = 0; i < unaryOps.length; i++) {
    createTest(unaryOps[i]+valueOfThrowerStr)();
    createTest(unaryOps[i]+toStringThrowerStr)();
    createTest(unaryOps[i]+throwingPropStr)();
    createTest(unaryOps[i]+throwingObjGetter)();
}
rhsNonZeroNum = { valueOf: function(){ executedRHS = true; return 1; } };
rhsZeroNum = { valueOf: function(){ executedRHS = true; return 0; } };
rhsToStringThrower = { toString: function(){ executedRHS = true; return 'string'; }};
getterThrower = { get value() { throw "throwing in getter"; }};
var getterThrowerStr = "getterThrower.value";
rhsGetterTester = { get value() { executedRHS = true; return 'string'; }};
var rhsGetterTesterStr = "rhsGetterTester.value";

for (var i = 0; i < binaryOps.length; i++) {
    var numVal = binaryOps[i] == '||' ? '0' : '1';
    createTest(numVal + " " + binaryOps[i] + " " + valueOfThrowerStr )();
    createTest(numVal + " " + binaryOps[i] + " " + toStringThrowerStr)();
    createTest(numVal + " " + binaryOps[i] + " " + throwingPropStr   )();
    createTest(numVal + " " + binaryOps[i] + " " + throwingObjGetter )();
    createTest(numVal + " " + binaryOps[i] + " " + getterThrowerStr )();

    createTest("'string' " + binaryOps[i] + " " + valueOfThrowerStr )();
    createTest("'string' " + binaryOps[i] + " " + toStringThrowerStr)();
    createTest("'string' " + binaryOps[i] + " " + throwingPropStr   )();
    createTest("'string' " + binaryOps[i] + " " + throwingObjGetter )();
    createTest("'string' " + binaryOps[i] + " " + getterThrowerStr )();
    
    numVal = binaryOps[i] == '||' ? 'rhsZeroNum' : 'rhsNonZeroNum';
    createTest(valueOfThrowerStr  + " " + binaryOps[i] + " " + numVal)();
    createTest(toStringThrowerStr + " " + binaryOps[i] + " " + numVal)();
    createTest(throwingPropStr    + " " + binaryOps[i] + " " + numVal)();
    createTest(throwingObjGetter  + " " + binaryOps[i] + " " + numVal)();
    createTest(getterThrowerStr  + " " + binaryOps[i] + " " + numVal)();
    createTest(valueOfThrowerStr  + " " + binaryOps[i] + " rhsToStringThrower")();
    createTest(toStringThrowerStr + " " + binaryOps[i] + " rhsToStringThrower")();
    createTest(throwingPropStr    + " " + binaryOps[i] + " rhsToStringThrower")();
    createTest(throwingObjGetter  + " " + binaryOps[i] + " rhsToStringThrower")();
    createTest(getterThrowerStr  + " " + binaryOps[i] + " rhsToStringThrower")();
    createTestWithRHSExec(valueOfThrowerStr  + " " + binaryOps[i] + " " + rhsGetterTesterStr)();
    createTestWithRHSExec(toStringThrowerStr + " " + binaryOps[i] + " " + rhsGetterTesterStr)();

}

var executionOrder = "";
__defineGetter__("nonThrowingIndexBase", function(){ 
    executionOrder += "nonThrowingIndexBase, "; 
    return {
        get nonThrowingTestIndex(){ executionOrder += "get nonThrowingTestIndex, "; return undefined; },
        get throwingTestIndex(){ executionOrder += "get nonThrowingTestIndex, "; throw {}; return undefined; },
        set nonThrowingTestIndex(){ executionOrder += "set nonThrowingTestIndex, "; return undefined; },
        set throwingTestIndex(){ executionOrder += "set nonThrowingTestIndex, "; throw {}; return undefined; }
    }
  });
__defineGetter__("throwingIndexBase", function(){ 
    executionOrder += "throwingIndexBase, "; 
    throw {};
  });

__defineGetter__("nonThrowingIndexNoThrowProperty", function(){
    return {
        toString: function() {executionOrder += "nonThrowingIndexNoThrowProperty, "; return "nonThrowingTestIndex"; }
    }
});

__defineGetter__("nonThrowingIndexThrowProperty", function(){ 
    return {
        toString: function() {executionOrder += "nonThrowingIndexThrowProperty, "; return "throwingTestIndex"; }
    }
});

__defineGetter__("throwingIndex", function(){
    return {
        toString: function() {executionOrder += "throwingIndex, "; throw {};}
    }
});

__defineGetter__("valueForAssignment", function(){ executionOrder += "valueForAssignment, "; return 1; })

function createTestWithExecOrderTest(expr, expected) {
    // This tests that we evaluate the right hand side of a binary expression before we
    // do any type conversion with toString and/or valueOf which may throw.
    var functionPrefix = "(function(){ executionOrder = ''; var result = 'PASS'; try { result = ";
    var functionPostfix = "; } catch (e) { executionOrder +=  '***throw***'; } if (executionOrder == expected) pass(expr); else " +
                          "fail(expr + ' executed as: <br/>' + executionOrder + '<br/>expected: '+executionOrder); })";
    testCount++;
    try {
        return eval(functionPrefix + expr + functionPostfix);
    } catch(e) {
        throw new String(expr);
    }
}

createTestWithExecOrderTest("nonThrowingIndexBase[nonThrowingIndexNoThrowProperty]", "nonThrowingIndexBase, nonThrowingIndexNoThrowProperty, get nonThrowingTestIndex, ")();

createTestWithExecOrderTest("nonThrowingIndexBase[nonThrowingIndexThrowProperty]", "nonThrowingIndexBase, nonThrowingIndexThrowProperty, get nonThrowingTestIndex, ***throw***")();

createTestWithExecOrderTest("nonThrowingIndexBase[throwingIndex]", "nonThrowingIndexBase, throwingIndex, ***throw***")();

createTestWithExecOrderTest("throwingIndexBase[nonThrowingIndexNoThrowProperty]", "throwingIndexBase, ***throw***")();

createTestWithExecOrderTest("throwingIndexBase[nonThrowingIndexThrowProperty]", "throwingIndexBase, ***throw***")();

createTestWithExecOrderTest("throwingIndexBase[throwingIndex]", "throwingIndexBase, ***throw***")();

createTestWithExecOrderTest("nonThrowingIndexBase[nonThrowingIndexNoThrowProperty] = valueForAssignment", "nonThrowingIndexBase, valueForAssignment, nonThrowingIndexNoThrowProperty, set nonThrowingTestIndex, ")();

createTestWithExecOrderTest("nonThrowingIndexBase[nonThrowingIndexThrowProperty] = valueForAssignment", "nonThrowingIndexBase, valueForAssignment, nonThrowingIndexThrowProperty, set nonThrowingTestIndex, ***throw***")();

createTestWithExecOrderTest("nonThrowingIndexBase[throwingIndex] = valueForAssignment", "nonThrowingIndexBase, valueForAssignment, throwingIndex, ***throw***")();

createTestWithExecOrderTest("throwingIndexBase[nonThrowingIndexNoThrowProperty] = valueForAssignment", "throwingIndexBase, ***throw***")();

createTestWithExecOrderTest("throwingIndexBase[nonThrowingIndexThrowProperty] = valueForAssignment", "throwingIndexBase, ***throw***")();

createTestWithExecOrderTest("throwingIndexBase[throwingIndex] = valueForAssignment", "throwingIndexBase, ***throw***")();


log("Passed " + passCount + " of " + testCount + " tests.");

//</script>