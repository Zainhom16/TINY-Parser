# TINY-Parser

## Description
This is a C++ project that implements a recursive descent parser for generating syntax trees from input tokens. The project validates syntax based on a predefined grammar, builds a syntax tree using child-sibling relationships, and represents it as an adjacency list for further processing in the GUI.

## Features
- Recursive descent parsing for the tiny language grammar.
- Syntax validation for tokenized input.
- Construction of syntax trees using child-sibling pointers.
- Representation of the syntax tree as an adjacency list (`map<TreeNode*, vector<TreeNode*>`).
- Modular design for future extensions or integrations.

## Technologies Used

- **Programming Language:** C++
- **Framework:** Qt (for GUI and Graphics)
- **Libraries:** Standard C++ Libraries (`<map>`, `<vector>`, `<string>`, `<sstream>`, `<QGraphicsScene>`, etc.)

## How It Works

1. **Load Source Code:** Use the "Open File" button to load source code from a text file.
2. **Tokenize:** Click the "Tokenize" button to convert the source code into tokens.
3. **Parse & Visualize:** Click the "Draw" button to generate and display the AST.

## Code Structure

### Key Classes and Functions

- **Node:** Represents a node in the AST.
- **Token:** Represents a token with its type and value.
- **Parser:** Implements a recursive-descent parser to build the AST.
- **Tokenize:** Breaks the input code into tokens using predefined patterns.
- **RemoveComments:** Removes comments from the source code.
- **MainWindow:** Manages the GUI and handles user interactions.

### Error Handling
- **Invalid Comments:** Detects unbalanced or misplaced comments.
- **Invalid Tokens:** Detects unrecognized tokens and identifiers with numbers.
- **Syntax Errors:** Detects unexpected tokens during parsing.

## Example Usage

1. Load a source file:
   ```text
   READ X;
   IF X < 10 THEN
      WRITE X;
   ELSE
      WRITE 10;
   END;
   ```
2. Click "Tokenize" to generate tokens.
3. Click "Draw" to visualize the AST.

## Acknowledgments

- Qt Documentation
- C++ Community Resources

 ## Project ScreenShot

 ![image](https://github.com/user-attachments/assets/efd90cb8-375f-40b2-b75f-457f14d1594f)


---

## Team

| Name     | ID               | 
|-------------|-----------------|
| Hazem Zainhom | 2000168 | 
| Mazen Saaed   | 2001080 | 
| Amged Shrief   | 2000121 | 
| Yossef Magdy    | 2001461 | 
| AbdElrhman Ahmed   | 2001067 | 
| Yousif Hazim   | 2001741 | 
