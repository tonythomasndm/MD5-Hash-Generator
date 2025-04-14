#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <bitset>
#include <cstdint>
#include <cmath>
#include <stdexcept>
#include <cstdio>

using namespace std;

// MD5Buffer class: This class holds the 128-bit (4x32-bit) MD5 state and provides
// 3 methods to intilaize the buffer, to update the buffer and
// to output the digest as a little-endian hexadecimal string.
// Why a class? Because again it is easy to maintain the state and do processing on it.
class MD5Buffer {
    // Made public so that the values/ variabled and methods are usable outside the function
public:

    // The four word(32 bit) A,B,C,D of the MD5 Buffer
    uint32_t A, B, C, D;

    // Intialization of the md5 buffer, these are already predefined values of md5 algorithm 
    // They are believed to be very helpful for confusion and diffusion    
    void initialize() {
        A = 0x67452301;
        B = 0xefcdab89;
        C = 0x98badcfe;
        D = 0x10325476;
    }

    // Updation the MD5 buffer state by adding in the new block values.
    // This is done for each message block value accoridngly
    // This is done so that the function can be used directly for many blockwise updations directly
    // Instead of defining it again and again
    // moreover, they are used to update the values accordingly
    void update(uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
        A += a;
        B += b;
        C += c;
        D += d;
    }

    // Conversion of the MD5 buffer state to a hexadecimal string in little-endian order.
    // This function is used to display the digest of the input message at the end for the user
    // This is merely a printing function for the display purpose
    // The message will be displayed in the low endian format accoridngly
    string toHexString() const {
        // digest defined to store the values and 32th position for the '\0'
        char digest[33];

        // For each 32-bit register, output its bytes in little-endian order.
        // We have used format specifiers for hexadecimal display
        sprintf(digest, "%02x%02x%02x%02x"
                        "%02x%02x%02x%02x"
                        "%02x%02x%02x%02x"
                        "%02x%02x%02x%02x",
                (A >> 0) & 0xff, (A >> 8) & 0xff, (A >> 16) & 0xff, (A >> 24) & 0xff,
                (B >> 0) & 0xff, (B >> 8) & 0xff, (B >> 16) & 0xff, (B >> 24) & 0xff,
                (C >> 0) & 0xff, (C >> 8) & 0xff, (C >> 16) & 0xff, (C >> 24) & 0xff,
                (D >> 0) & 0xff, (D >> 8) & 0xff, (D >> 16) & 0xff, (D >> 24) & 0xff);
        
        return string(digest);
    }
};

// Step - 1 Padding the message function
// Pads the bit-string so that its length (in bits) is congruent to 448 modulo 512. 
// The padding is done on the binary string (which contains only '0' and '1' characters).
// At least 1 bit is padded and at most 512 bits will be padded.
// All the work will be done on bit strings here.
// Input is string -> bitMessage of '0' and '1' and the ouput is null, inplace changes are made to the input string.
void paddingMessage(string &bitMessage) {
    // Here we append a "1" bit(which is a must)
    bitMessage.push_back('1');

    // then, we find the current length of the bitMessage string after padding the '1' bit.
    __uint128_t currentLength = bitMessage.size();

    // After that we determine how many bits are to be pad so that (currentLength + pad) ≡ 448 mod 512.
    // To acheive this, we are calculating first the no of 512 bit blocks required(including the appendLength thing also)

    __uint128_t noOfBlocksRequired = ceil((currentLength + 64) / (double)512);
    // Then we multiply it with 512 and remove the current length and 64 bits to find the final padding length required.
    __uint128_t paddingZeroLength = noOfBlocksRequired * 512 - currentLength - 64;
    // A for loop is used for padding the zero bits accordingly to the bit Message.
    // Since this is inplace, we dont need to worry about returning anything, as the changes are made to the same variable.
    for(int i=0;i<paddingZeroLength;i++) bitMessage+='0';
    // Since the function is void - no return statement is used.
}

// Step 2. Append Length
// This function is used to append the original message length (in bits) as a 64-bit little-endian value.
// Therfore it will help us in completing the last 64 bits which will make the bitMessage an exact multiple of 512 bits
// The use of uint64_t helps us in this, in a very unlikely scenario, if the length of the message is greater than 
// 2^64 then the overflow will be adjusted and only the low endian 64 bits will remain.
// We have used the unsigned version here so it caters to 0 as the lowest possible end value for the overflow
// Input - will be a bit message and the original length of the message. Output is void -> inplace changes to string
void appendLength(string &bitMessage, uint64_t originalLength) {
    // Conversion of originalLength to a 64-bit binary string using the bitset, defined to be 64 bit and 
    // to_string to convert it into string format.
    string lenBinary = bitset<64>(originalLength).to_string();
    // Now we append the 64-bit length in little-endian order (8 bytes, each 8 bits).
    // This means that we have appendded the 8-bit byte/word in reverse order, hence the low endian order
    // Reverse for loop for extraction in low endian order
    for (int i = 7; i >= 0; i--) {
        // Substring extraction from the string 
        string byteStr = lenBinary.substr(i * 8, 8);
        // adding it inplace to the message string in the low endian format, as we get the byte -> we add it
        bitMessage += byteStr;
    }
    // Void function - hence no return statement
}

// Step 3.1 : Conversion of bit string into message blocks(512 bit each)
// Each message block consists of 16 words and each word is 32 bit long only.
// This function splits the padded bit string into 512-bit blocks. Each block is then
// subdivided into 16 words (32 bits each) interpreted in little-endian order.
// Input is the bitString or bitMessage which is a string consisting of '0' and '1's
// Output is a 2D vector. Outer vector represents number of blocks of multiples of 512
// Inner vector represents a block of 16 words each(each word is 32 bit)
vector<vector<uint32_t>> splitIntoBlocks(const string &bitString) {
    const size_t blockSize = 512;      // bits per block
    const size_t wordSize = 32;        // bits per word
    const size_t bytesPerWord = wordSize / 8; // 4 bytes per 32-bit word
    const size_t wordsPerBlock = blockSize / wordSize; // 16 words

    // Error argument if the block size is not an exact mutiple of 512 
    // This throws/ raises an exception
    // Indicating some problem in the the padding or appendingLength functions above
    if (bitString.size() % blockSize != 0) {
        throw invalid_argument("Bit string length is not a multiple of 512.");
    }


    // Finding out the total number of message blocks(i.e how many mutiples of 512 bits exist in the bitString)
    size_t totalBlocks = bitString.size() / blockSize;

    // Intialization of the number of message blocks and the vector which is to be returned.
    vector<vector<uint32_t>> blocks(totalBlocks);

    // For loop for each message block(i.e 512 bit multiple)
    for (size_t i = 0; i < totalBlocks; i++) {
        // Intialization of a block for storing the 16 words(where each word is 32 bit)
        vector<uint32_t> block(wordsPerBlock, 0);
        // block start - gives the start index for storing the blocks, the indexes are with referncve to the bit string
        size_t blockStart = i * blockSize;
        // For loop for each word i.e 16 words
        for (size_t w = 0; w < wordsPerBlock; w++) {
            // Word intilization for each word for those 16 words
            uint32_t word = 0;
            // For loop for Each word is composed of 4 bytes.
            for (size_t b = 0; b < bytesPerWord; b++) {

                // Main Extraction is being performed here for extracting the 32 bit word from the string accoridngly
                // Calculating the starting position of the current byte within the bit string.
                size_t byteStart = blockStart + ((w * bytesPerWord) + b) * 8;
                uint8_t byteVal = 0;
                // For loop here for processing 8 bits for this byte.
                for (size_t bit = 0; bit < 8; bit++) {
                    char c = bitString[byteStart + bit];
                    if (c != '0' && c != '1') {
                        // Throws/Raises an exception when we get an invalid character except 0 and 1
                        throw invalid_argument("Invalid character in bit string.");
                    }
                     // Shift the current byte left by 1 bit and append the new bit (0 or 1).
                     byteVal = (byteVal << 1) | (c - '0');
                    }
                // Convert the byte into its position in the 32-bit word.
                // Little-endian: shift the byte by 8 * b to its appropriate position.
                word |= static_cast<uint32_t>(byteVal) << (8 * b);
            }
            // for string the word i.e 32 bit word
            block[w] = word;
        }
        // for storing the 16 word(512 bit block)
        blocks[i] = block;
    }
    // returning the message blocks
    return blocks;
}


// --- MD5 Auxiliary Functions ---
// These functions are defined accoridngly as stated in Step 3.4
uint32_t F(uint32_t X, uint32_t Y, uint32_t Z) {
    return (X & Y) | (~X & Z);
}

uint32_t G(uint32_t X, uint32_t Y, uint32_t Z) {
    return (X & Z) | (Y & ~Z);
}

uint32_t H(uint32_t X, uint32_t Y, uint32_t Z) {
    return X ^ Y ^ Z;
}

uint32_t I(uint32_t X, uint32_t Y, uint32_t Z) {
    return Y ^ (X | ~Z);
}

//This is a helper function for left-rotate a 32-bit integer x by n bits.
uint32_t leftrotate(uint32_t x, uint32_t n) {
    return (x << n) | (x >> (32 - n));
}

// MD5 Transformation for one 512-bit block.
// Updates the MD5Buffer state using the current block of 16, 32-bit words.
// This function is performed iteratively for each block
// The final values are updated to md5buffer and then are used for final hash values
// Here the inputs are MD5buffer used here and the block - current message block(512 bits)
void transformBlock(MD5Buffer &md5buffer, const vector<uint32_t> &block) {
    // T: Constants derived from the sine function, defined in Step 3.4 - abs(sin(i + 1)) × 2^32
    static const uint32_t T[64] = {
        0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
        0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
        0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
        0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
        0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
        0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
        0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
        0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
        0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
        0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
        0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
        0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
        0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
        0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
        0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
        0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
    };

    // Shift amounts per operation, defined in Step 3.4
    static const uint32_t S[64] = {
         7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
         5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
         4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
         6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
    };

    // Copy the current state from the md5buffer values.
    // These four 32-bit values (A, B, C, D) represent the state that will be updated.
    uint32_t a = md5buffer.A;
    uint32_t b = md5buffer.B;
    uint32_t c = md5buffer.C;
    uint32_t d = md5buffer.D;

    // Process the block in 64 rounds.
    for (int i = 0; i < 64; i++) {
        uint32_t f, g;  
        // f is the result of the non-linear function; g determines the index of the word in block.
        // as defined on the site
        // Rounds 0-15 (first 16 rounds)
        if (i < 16) {
            f = F(b, c, d); 
            g = i;
        } 
        // Rounds 16-31:
        else if (i < 32) {
            f = G(b, c, d); 
            g = (5 * i + 1) % 16;
        } 
        // Rounds 32-47
        else if (i < 48) {
            f = H(b, c, d);
            g = (3 * i + 5) % 16;
        } 
        // Rounds 48-63
        else {
            f = I(b, c, d);
            g = (7 * i) % 16;
        }

        // Save the current value of d in a temporary variable. This temporary value will be used to rotate the state.
        uint32_t temp = d;
        // Shift the state values: d-> c, c -> b, a-> temp(d)
        d = c;
        c = b;
        // Compute the new value for b.
        // The expression inside leftrotate(...) is the sum of:
        //   - The previous value of a.
        //   - The result from the non-linear function f.
        //   - The constant T[i] for the current round.
        //   - The word from the current block[g], where g is determined based on the round.
        // The sum is then rotated left by S[i] bits.
        // The result of the rotation is added to the previous value of b.
        b = b + leftrotate(a + f + T[i] + block[g], S[i]);
        a = temp;

        // At this point, the roles of a, b, c, d have been rotated.
        // This completes one round of the 64 rounds.
    }

    // Update the current state of md5buffer with the results from this block.
    // States will be updated according to each and every block. This is a cummulative change.
    md5buffer.update(a, b, c, d);
    // no return statement - void function - the results changes are already updated to the state variable
}

// The main function - Main entry point.
// Responsible for taking the input and performing all the steps of the algorithm 
// Also the main process - the main execution starts here.
int main() {
    
    //Value Intialization
    string inputString;
    string bitString = "";
    int choice = 0; // 0, is kept, because in case of an error then it will ask again.

    // Input handling: choose between direct string input or reading from a text file.
    // Menu based system for the user to choose the mode of input and enter the input as well.
    while (true) {
        cout << "Enter 1 for string input or 2 for file input: ";
        cin >> choice;
        cin.ignore();  
        // Clear newline from input buffer else the same buffer will cause issue for the upcoming inputs

        if (choice == 1) {
            // String input(without the new line)

            cout << "Enter the string: ";
            getline(cin, inputString);
            break;
        } else if (choice == 2) {
            // File name input for a text file

            cout << "Enter the filename: ";
            string fileName;
            getline(cin, fileName);
            ifstream file(fileName);

            // Error checks for failed file opening
            if (!file) {
                cout << "Failed to open file. Please try again.\n";
                continue;
            }
            stringstream bufferStream;
            bufferStream << file.rdbuf();
            inputString = bufferStream.str();
            file.close();
            break;

        } else {
            // Error for Invalid choice.
            cout << "Invalid choice, please try again." << endl;
        }
    }

    // Now, we convert the string given by the user into its 8-bit binary representation in the string
    // format. I.E We parse the string character by character and process it and each character is converted 
    // to its 8 bit representataion and is used. For numbers in string, eg - 7 it is not 111 but its ASCII code.

    for (char c : inputString) {
        bitString += bitset<8>(c).to_string();
    }
    
    // We store the length of the orginal message, it will be later used for appendLength function
    // of the md5 algorithm, for the last 64 bits.
    uint64_t originalLength = bitString.size();

    // Padding the message and appendding the original length.
    // These two functions are performed to a bit string.
    // Step-1 Append Padding Bits
    paddingMessage(bitString);
    // Step-2 Append Length
    appendLength(bitString, originalLength);

    // Declaration of a MD5buffer variable and initialization of the MD5 buffer state.
    MD5Buffer md5buffer;
    // Step-3 Initialize MD Buffer
    md5buffer.initialize();

    // Step-4 Split the padded String into 512-bit blocks i.e 16 words of 32-bit word
    // Split the final padded bit string into 512-bit blocks.
    vector<vector<uint32_t>> messageBlocks = splitIntoBlocks(bitString);

    cout<<"No of 512-bit blocks(each block having 16 words of 32 bit word) created :"<<messageBlocks.size()<<endl;

    // Now we will process each 512-bit block using the MD5 transformation.
    // Step-4 Process Message in 16-Word(one word is 32 bit) Blocks
    
    for (const auto &block : messageBlocks) {
        transformBlock(md5buffer, block);
    }
    // Step 6 - Output
    // Output the final MD5 digest (in little-endian hexadecimal format).
    // I.E. for like  A register, the low end will come first and the high order at last, same for B,C,D 
    // then everything will end at D high end bytes
    cout << "MD5 digest: " << md5buffer.toHexString() << endl;

    return 0;
}
