class Generator:
    def __init__(self):
        self.buffer = ""
        self.ident = 0

    def push_ident(self):
        self.ident = self.ident + 1

    def pop_ident(self):
        self.ident = self.ident - 1

    def emit(self, *code):
        if (''.join(code) == ""):
            self.buffer += "\n"
        else:
            self.buffer += '    ' * self.ident + ''.join(code) + '\n'

    def emit_section(self, title):
        self.buffer += '    ' * self.ident + "/* --- " + title + " " + \
            "-" * (69 - len(title) - 4 * self.ident) + " */\n\n"  # nice

    def finalize(self):
        return self.buffer
