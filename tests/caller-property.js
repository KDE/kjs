// Created by Vladimir Olexa  <vladimir.olexa@gmail.com>

// This tests for caller property in functions. Only functions that
// are called from inside of other functions and have a parent should
// have this property set. Tests return true when caller is found and
// false when the caller is null.

function child()
{
    return child.caller;
}

function parent()
{
    return child();
}

var childHasCallerWhenCalledFromGlobalCode = (child.caller !== null);
var childHasCallerWhenCalledWithoutParent = (child() !== null);
var childHasCallerWhenCalledFromWithinParent = parent() == parent;

shouldBe('childHasCallerWhenCalledFromGlobalCode', 'false');
shouldBe('childHasCallerWhenCalledWithoutParent', 'false');
shouldBe('childHasCallerWhenCalledFromWithinParent', 'true')

var successfullyParsed = true;
