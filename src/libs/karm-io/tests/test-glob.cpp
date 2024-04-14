#include <karm-io/glob.h>
#include <karm-test/macros.h>

namespace Karm::Io::Tests {

test$("glob-match") {
    expect$(matchGlob("", ""));
    expect$(matchGlob("a", "a"));
    expect$(not matchGlob("a", ""));
    expect$(not matchGlob("", "a"));
    expect$(matchGlob("abcABC123", "abcABC123"));
    expect$(not matchGlob("ABCabc123", "abcABC123"));
    expect$(matchGlob("?", "a"));
    expect$(matchGlob("?", "b"));
    expect$(matchGlob("?", "1"));
    expect$(matchGlob("?", "8"));
    expect$(not matchGlob("?", ""));
    expect$(matchGlob("abcABC*", "abcABC123"));
    expect$(matchGlob("abc*123", "abcABC123"));
    expect$(matchGlob("abc*123", "abc123"));
    expect$(matchGlob("*ABC123", "abcABC123"));
    expect$(not matchGlob("abcABC*", "ABCabc123"));
    expect$(not matchGlob("abc*123", "ABCabc123"));
    expect$(not matchGlob("abc*123", "abcABCXYZ"));
    expect$(not matchGlob("*ABC123", "ABCabc123"));
    expect$(matchGlob("[abc]", "a"));
    expect$(matchGlob("[abc]", "b"));
    expect$(matchGlob("[abc]", "c"));
    expect$(not matchGlob("[abc]", "1"));
    expect$(not matchGlob("[abc]", "2"));
    expect$(not matchGlob("[abc]", "3"));
    expect$(matchGlob("[a-z]", "a"));
    expect$(matchGlob("[a-z]", "z"));
    expect$(not matchGlob("[a-z]", "1"));
    expect$(not matchGlob("[a-z]", "9"));
    expect$(not matchGlob("[^a-z]", "a"));
    expect$(not matchGlob("[^a-z]", "z"));
    expect$(matchGlob("[^a-z]", "1"));
    expect$(matchGlob("[^a-z]", "9"));
    expect$(matchGlob("[a-z0-9]", "a"));
    expect$(matchGlob("[a-z0-9]", "k"));
    expect$(matchGlob("[a-z0-9]", "1"));
    expect$(matchGlob("[a-z0-9]", "7"));
    expect$(matchGlob("[a-z0-9]", "a"));
    expect$(not matchGlob("[a-z0-9]", "A"));
    expect$(not matchGlob("[a-z0-9]", "K"));

    return Ok();
}

} // namespace Karm::Io::Tests
