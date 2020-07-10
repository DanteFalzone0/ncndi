from token import Token, TokenType

class Lexer:
    def __init__(self, text):
        self.text = text
        self.pos = 0
        self.current_char = self.text[self.pos]

    def error(self, message):
        raise Exception("Error while lexing: " + message)

    def advance(self):
        self.pos += 1
        if self.pos > len(self.text) - 1:
            self.current_char = None
        else:
            self.current_char = self.text[self.pos]

    def skip_whitespace(self):
        while self.current_char is not None and self.current_char.isspace():
            self.advance()

    def parse_integer(self):
        result = ""
        while self.current_char is not None and self.current_char.isdigit() or self.current_char == '-':
            result += self.current_char
            self.advance()
        return int(result)

    def get_next_token(self):
        while self.current_char is not None:

            if self.current_char.isspace():
                self.skip_whitespace()
                continue

            if self.current_char.isdigit():
                return Token(TokenType.INTEGER, self.parse_integer())

            if self.current_char == '+':
                self.advance()
                return Token(TokenType.PLUS, '+')

            if self.current_char == '-':
                if self.pos == 0:
                    return Token(TokenType.INTEGER, self.parse_integer())
                elif self.text[self.pos - 1] in ('-', '+', '*', '/'):
                    return Token(TokenType.INTEGER, self.parse_integer())
                else:
                    self.advance()
                    return Token(TokenType.SUBTRACT, '-')

            if self.current_char == '*':
                self.advance()
                return Token(TokenType.MULT, '*')

            if self.current_char == '/':
                self.advance()
                return Token(TokenType.DIVIDE, '/')

            if self.current_char == '(':
                self.advance()
                return Token(TokenType.OPEN_PAREN, '(')

            if self.current_char == ')':
                self.advance()
                return Token(TokenType.CLOSE_PAREN, ')')

            self.error()

        return Token(TokenType.EOF, None)