# MD5 Hash Generator in C++

This TC assignment is a complete implementation of the MD5 hashing algorithm in C++. The program takes input either as a string or from a file, processes it according to the MD5 algorithm, and outputs the resulting hash in hexadecimal format. 

## Code Structure

### MD5Buffer Class

The `MD5Buffer` class maintains the 128-bit state of the MD5 hash, split into four 32-bit registers: A, B, C, and D. It provides:

- `initialize()`: Sets the initial state values as defined in the MD5 specification.
- `update(uint32_t a, uint32_t b, uint32_t c, uint32_t d)`: Updates the state after processing a block.
- `toHexString()`: Converts the final state into a hexadecimal string for output.

### Padding and Length Appending

- `paddingMessage(string &bitMessage)`: Pads the input so its length is congruent to 448 modulo 512, followed by a single '1' bit and the necessary '0' bits.
- `appendLength(string &bitMessage, uint64_t originalLength)`: Appends the original message length (in bits) as a 64-bit value in little-endian format.

### Block Splitting

- `splitIntoBlocks(const string &bitString)`: Divides the padded message into 512-bit blocks. Each block is further split into 16 words (32 bits each).

### MD5 Transformation

- `transformBlock(MD5Buffer &md5buffer, const vector<uint32_t> &block)`: Processes each 512-bit block through 64 transformation rounds. It uses non-linear functions, constants, and bitwise operations to update the MD5 state cumulatively.

### Auxiliary Functions

- Non-linear functions: `F`, `G`, `H`, and `I` as defined in the pdf Step 3.4.
- `leftrotate(uint32_t x, uint32_t n)`: Performs a left-rotate operation on a 32-bit integer.

### Main Function Workflow

The main function orchestrates the entire process:

1. **Input Handling**: Allows the user to choose between entering a string or reading from a file.
2. **Binary Conversion**: Converts the input to an 8-bit binary representation.
3. **Padding and Length Appending**: Prepares the input for processing by the MD5 algorithm.
4. **Block Processing**: Processes each 512-bit block through the transformation function.
5. **Final Output**: Displays the computed MD5 hash in hexadecimal format.

## How to Compile and Run

### Compilation

To compile the program, open a terminal in the directory containing the source file (e.g., `md5.cpp`) and run:
```bash
g++ -o md5 md5.cpp
```

### Execution

Run the compiled program:
```bash
./md5
```
You will be prompted to select the input method:
- Enter `1` to input a string directly.
- Enter `2` to specify a file from which the input will be read.

### Example Usage

#### Direct String Input
```plaintext
Enter 1 for string input or 2 for file input: 

> 1
Enter the string: Hello, World!
MD5 Hash: 65a8e27d8879283831b664bd8b7f0ad4
```

#### File Input
```plaintext
Enter 1 for string input or 2 for file input: 

> 2
Enter the filename: input.txt
MD5 Hash: 65a8e27d8879283831b664bd8b7f0ad4
```
