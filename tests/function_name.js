// .name function property (Mozilla extension)
function foo()
{
   return foo.name;
}

shouldBe("foo.name", "'foo'");
shouldBe("foo()", "'foo'");
shouldBeFalse("delete foo.name");
foo.name = 'bar';
shouldBe("foo.name", "'foo'");
shouldBeFalse("delete foo.name");
