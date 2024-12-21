# Simple Interpreter and Lexer (Interpreter & Lexer)

This project is a **simple interpreter** and **lexer** system that tokenizes, parses, and executes commands from an input file (`code.sta`). It is designed for creating, running, and testing your own basic programming language.

## Features

- **Variable Declaration and Assignment:** Supports `int` and `text` types.
- **Loop Operations:** Allows simple loops with the `loop` command.
- **Input/Output Operations:**
  - `read`: Accepts input from the user.
  - `write`: Outputs data to the console.
- **Comment Handling:**
  - Ignores content between `/* ... */`.
- **Error Handling:**
  - Invalid tokens.
  - Missing string terminators.
  - Undefined variables and overly long variable names.

## Usage

### 1. Input File (`code.sta`)

You can write a program like the following in the input file:

```plaintext
int a, b.
text message is "Hello!".
loop 5 times {
    write message, a.
    a is a + 1.
}
```

### 2. Output File (`code.lex`)

The program analyzes the input file and generates token output as follows:

```plaintext
Keyword(int)
Identifier(a)
Comma
Identifier(b)
EndOfLine
Keyword(text)
Identifier(message)
Keyword(is)
String("Hello!")
EndOfLine
Keyword(loop)
IntConst(5)
Keyword(times)
LeftCurlyBracket
Keyword(write)
Identifier(message)
Comma
Identifier(a)
EndOfLine
Identifier(a)
Keyword(is)
Identifier(a)
Operator(+)
IntConst(1)
EndOfLine
RightCurlyBracket
```

### 3. Console Output

The console output will look like this:

```plaintext
Hello! 0
Hello! 1
Hello! 2
Hello! 3
Hello! 4
```

## Technical Details

- **Programming Language:** C
- **File Input/Output:**
  - `code.sta`: Input file.
  - `code.lex`: File where tokens are written.
- **Token Types:**
  - Keywords: `int`, `text`, `is`, `loop`, `times`, `read`, `write`, `newLine`
  - Integers: `IntConst`
  - Strings: `String`
  - Operators: `+`, `-`, `*`, `/`
  - Delimiters: `{`, `}`, `,`
  - Terminator: `.`

## Compilation and Execution

### 1. Compilation

Compile the project using the following command:

```bash
gcc -o interpreter interpreter.c
```

### 2. Execution

Run the program with the following command:

```bash
./interpreter
```

Make sure to create the `code.sta` file in the same directory as your project.

## Project Structure

- **`interpreter.c`**: Main program file.
- **`code.sta`**: Input file.
- **`code.lex`**: File where tokens are written.

## Contributing

If you would like to contribute to the development of this project, please submit a **Pull Request** or open an **Issue**.

## Contact

If you have any questions or suggestions, feel free to reach out:

- **Email:** amin.azizzade004@gmail.com
- **LinkedIn:** [[LinkedIn: Amin Azizzade]](https://www.linkedin.com/in/amin-azizzade-649486232/edit/forms/project/new/?profileFormEntryPoint=PROFILE_COMPLETION_HUB)

---

🚀 Feel free to explore this project and build your own programming language!
