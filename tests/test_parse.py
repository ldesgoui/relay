import pytest

from relay import parse


def assert_match(pattern, *params, **mapped_params):
    results = (params, mapped_params)
    data = pattern.format(*params, **mapped_params)
    assert parse.match(pattern, data) == results

def test_arg():
    assert_match("Hello, {}!", "Jason")

def test_empty_arg():
    assert_match("Hello, {}!", "")

def test_kwargs():
    assert_match("{greet}, {person}!", greet="Bonjour", person="Thomas")

def test_arg_everythig():
    assert_match("{}", "i want everything!")

def test_kwarg_everything():
    assert_match("{everything}", everything="Good, good...")

def test_escaped():
    assert_match("sup {{}} just kidding {}", "haha")

def test_misformat():
    pattern = "sup{"
    data = "sup."
    with pytest.raises(ValueError) as excinfo:
        parse.match(pattern, data)
    assert "end of string while looking for pattern thingie" in str(excinfo.value)

def test_mismatch():
    pattern = "sup"
    data = "soupe"
    with pytest.raises(ValueError) as excinfo:
        print(parse.match(pattern, data))
    assert "mis-match in values" in str(excinfo.value)

def test_couldnt_finish_or_whatever():
    pattern = "test {}!"
    data = "test but this has no exclamation mark afterwards :("
    with pytest.raises(ValueError) as excinfo:
        print(parse.match(pattern, data))
    assert "end of string while matching" in str(excinfo.value)

def test_mismatch_empty():
    pattern = "test"
    data = "test but this is longer"
    with pytest.raises(ValueError) as excinfo:
        print(parse.match(pattern, data))
    assert "mis-match in values" in str(excinfo.value)
