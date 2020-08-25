def abort(message: str):
    raise Exception("ERROR: " + message)


class Lexer:
    def __init__(self, code: str):
        self.code = code
        self.offset = 0

    def ended(self):
        return self.offset >= len(self.code)

    def forward(self, n=1):
        for i in range(n):
            if not self.ended():
                self.offset = self.offset + 1

    def peek(self, peek):
        offset = self.offset + peek

        if offset > len(self.code):
            return "\0"

        return self.code[offset]

    def current(self):
        return self.peek(0)

    def current_is(self, what):
        if self.ended():
            return False

        return self.code[self.offset] in what

    def current_is_word(self, what):
        for i in range(len(what)):
            if self.peek(i) != what[i]:
                return False

        return True

    def eat(self, what):
        while self.current_is(what) and not self.ended():
            self.forward()

    def skip_word(self, what: str) -> bool:
        if (self.current_is_word(what)):
            self.forward(len(what))
            return True

        return False

    def skip_and_expect(self, what: str):
        if not self.skip_word(what):
            abort("Expected " + what)

    def eat_whitespace(self):
        self.eat(" \t\n\r")
