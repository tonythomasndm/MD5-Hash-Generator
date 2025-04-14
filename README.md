# MD5 Hash Generator in C++

This TC assignment is a complete implementation of the MD5 hashing algorithm in C++. The program takes input either as a string or from a file, processes it according to the MD5 algorithm, and outputs the resulting hash in hexadecimal format. 

## Implementation Details

### MD5Buffer Class

The `MD5Buffer` class manages the 128-bit state of the MD5 hash using four 32-bit registers: A, B, C, and D.

- **initialize()**  
  Sets the initial values for A, B, C, and D using predefined constants (`0x67452301`, `0xefcdab89`, `0x98badcfe`, `0x10325476`). These constants are chosen to promote confusion and diffusion within the hash computation.
  
- **update(uint32_t a, uint32_t b, uint32_t c, uint32_t d)**  
  Adds new block values to the current state. This cumulative update occurs after processing each 512-bit block.
  
- **toHexString()**  
  Converts the 128-bit state into a 32-character hexadecimal string in little-endian order. This involves extracting each byte from the 32-bit words and formatting them accordingly.

### Padding the Message

The MD5 algorithm requires the input to be padded so that its length (in bits) is congruent to 448 modulo 512. Two functions handle this:

- **paddingMessage(string &bitMessage)**  
  This function starts by appending a single '1' bit to the binary string. It then calculates the number of '0' bits needed so that the current length plus the padding will reach 448 modulo 512. A loop appends these '0' bits to the string.
  
- **appendLength(string &bitMessage, uint64_t originalLength)**  
  After padding, the original message length (in bits) is appended as a 64-bit value. The function converts the length into a binary string and appends it in little-endian order (i.e., the least significant byte is added first).

### Splitting into 512-bit Blocks

- **splitIntoBlocks(const string &bitString)**  
  This function divides the padded binary string into blocks of 512 bits. Each block is further split into 16 words (each word is 32 bits). The function ensures that the bit string length is an exact multiple of 512, otherwise it throws an exception. Each word is built by processing 8 bits at a time and arranging them in little-endian order.

### Auxiliary Functions

The MD5 algorithm uses four auxiliary non-linear functions that help in the transformation process:

- **F(X, Y, Z)**, **G(X, Y, Z)**, **H(X, Y, Z)**, **I(X, Y, Z)**  
  These functions mix the input bits in different ways to provide the necessary non-linearity.

- **leftrotate(uint32_t x, uint32_t n)**  
  A helper function that performs a left-rotate (circular shift) on a 32-bit integer by `n` bits. This operation is used in each of the 64 rounds of the transformation.

### Transforming a 512-bit Block

- **transformBlock(MD5Buffer &md5buffer, const vector<uint32_t> &block)**  
  This is the core of the MD5 algorithm. The function:
  1. Copies the current state from `md5buffer` into local variables.
  2. Processes the block through 64 rounds. In each round:
     - A non-linear function (F, G, H, or I) is chosen based on the round number.
     - A constant from the sine-derived table (`T[]`) is added.
     - A specific word from the block is selected.
     - The sum is rotated left by a pre-defined shift amount (`S[]`) and added to one of the state variables.
  3. After 64 rounds, the state is updated cumulatively using the `update()` method of `MD5Buffer`.

### Main Function Workflow

The `main()` function orchestrates the complete process:

1. **Input Handling:**  
   The user is prompted to choose between a direct string input or file input. The program reads the chosen input method accordingly.

2. **Binary Conversion:**  
   Each character of the input is converted into its 8-bit binary representation using the `bitset<8>` conversion.

3. **Message Preparation:**  
   The program stores the original length of the bit string, applies padding with `paddingMessage()`, and appends the original length using `appendLength()`.

4. **Block Processing:**  
   The padded message is split into 512-bit blocks using `splitIntoBlocks()`, and each block is processed using `transformBlock()`. 

5. **Output:**  
   Finally, the updated MD5 state is converted into a hexadecimal string with `toHexString()` and displayed as the MD5 digest.

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
