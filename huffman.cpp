#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <vector>
#include "stdlib.h"
using namespace std;


/**
 * The struct of the Huffman table nodes.
 */
struct Node{
    char ch;
    int freq;
    Node* left;
    Node* right;

    Node(char ch, int freq) {
        this->ch = ch;
        this->freq = freq;
        left = 0;
        right = 0;
    }

    Node(Node* left, Node* right) {
        ch = '\0';
        freq = left->freq + right->freq;
        this->left = left;
        this->right = right;
    }
};


/**
 * Open the input file. If file can't be opened print the message about error and exit from the program.
 *
 * @param inputFileName - The input file name.
 * @return - The opened ifstream.
 */
ifstream* openInputFile(char* inputFileName) {
    ifstream* result = new ifstream(inputFileName, ios::binary);
    if (!result->is_open()) {
        cerr << "File open error." << endl;
        exit(EXIT_FAILURE);
    }
    return result;
}


/**
 * Open the output file. If file can't be opened print the message about error and exit from the program.
 *
 * @param outputFileName - The input file name.
 * @return - The opened ofstream.
 */
ofstream* openOutputFile(char* outputFileName) {
    ofstream* result = new ofstream(outputFileName, ios::binary);
    if (!result->is_open()) {
        cerr << "File open error." << endl;
        exit(EXIT_FAILURE);
    }
    return result;
}


/**
 * Close the opened input file.
 *
 * @param inputFileStream - The opened ifstream.
 */
void closeFile(ifstream* inputFileStream) {
    inputFileStream->close();
    delete inputFileStream;
}


/**
 * Close the opened output file.
 *
 * @param outputFileStream - The opened ofstream.
 */
void closeFile(ofstream* outputFileStream) {
    outputFileStream->close();
    delete outputFileStream;
}


/**
 * Create the chars frequency table. Chars are readed from input file.
 *
 * @param inputFileName - The input file name.
 * @return - The created chars frequency table.
 */
map<char, int>* createCharsFrequencyTable(char* inputFileName) {
    map<char, int>* result = new map<char, int>();
    ifstream* inputFileStream = openInputFile(inputFileName);
    char ch;
    while (inputFileStream->get(ch))
        (*result)[ch]++;
    closeFile(inputFileStream);
    return result;
}


/**
 * Set the compare parameter for the list sorting function. Do sort ascending the field freq of the nodes.
 *
 * @param firstNode - The first node.
 * @param secondNode - The second (next) node.
 * @return - True if first node freq field is less than second node freq field.
 */
bool compareNodes(Node* firstNode, Node* secondNode) {
    return (firstNode->freq < secondNode->freq);
}


/**
 * Create the Huffman tree from the chars frequency table.
 *
 * @param charsFrequencyTable - The chars frequency table.
 * @return - The pointer to the root of the created Huffman tree.
 */
Node* createHuffmanTree(map<char, int>* charsFrequencyTable) {
    // Fill the start Huffman tree nodes list from the chars frequency table
    list<Node*> nodesList;
    for (map<char, int>::iterator it = charsFrequencyTable->begin(); it != charsFrequencyTable->end(); it++) {
        Node* newNode = new Node(it->first, it->second);
        nodesList.push_back(newNode);
    }
    delete charsFrequencyTable;
    // Create the Huffman tree
    while (nodesList.size() > 1) {
        nodesList.sort(compareNodes);
        Node* firstNode = nodesList.front();
        nodesList.pop_front();
        Node* secondNode = nodesList.front();
        nodesList.pop_front();
        Node* newNode = new Node(firstNode, secondNode);
        nodesList.push_back(newNode);
    }
    Node* result = nodesList.front();
    return result;
}


/**
 * Recursive postfix Huffman tree traverse.
 *
 * @param huffmanTree - The Huffman tree.
 * @param code - The current builded Huffman code.
 * @param huffmanTable - The created Huffman table.
 */
void traverseHuffmanTree(Node* huffmanTree, vector<bool> & code, map<char, vector<bool> >* huffmanTable) {
    if (huffmanTree->left) {
        code.push_back(0);
        traverseHuffmanTree(huffmanTree->left, code, huffmanTable);
    }
    if (huffmanTree->right) {
        code.push_back(1);
        traverseHuffmanTree(huffmanTree->right, code, huffmanTable);
    }
    if (char ch = huffmanTree->ch)
        (*huffmanTable)[ch] = code;
    code.pop_back();
}


/**
 * Create the Huffman table from the Huffman tree.
 *
 * @param huffmanTree - The Huffman tree.
 * @return - The created Huffman table.
 */
map<char, vector<bool> >* createHuffmanTable(Node* huffmanTree) {
    vector<bool> code;
    map<char, vector<bool> >* result = new map<char, vector<bool> >();
    traverseHuffmanTree(huffmanTree, code, result);
    delete huffmanTree;
    return result;
}


/**
 * Create the compressed file.
 *
 * @param inputFileName - The input file name.
 * @param outputFileName - The output file name.
 * @param huffmanTable - The Huffman table.
 */
void createCompressedFile(char* inputFileName, char* outputFileName, map<char, vector<bool> >* huffmanTable) {
    // Fill the buffer by the coded data that are readed from the input file
    ifstream* inputFileStream = openInputFile(inputFileName);
    char ch;
    vector<bool> buffer;
    while (inputFileStream->get(ch)) {
        vector<bool> code = (*huffmanTable)[ch];
        for (vector<bool>::iterator it = code.begin(); it != code.end(); it++)
            buffer.push_back(*it);
    }
    closeFile(inputFileStream);

    // Store the buffer size and fill the buffer with zeros to the size of a multiple byte
    int bufferSize = buffer.size();
    while (buffer.size() % 8 != 0)
        buffer.push_back(0);
    ofstream* outputFileStream = openOutputFile(outputFileName);

    // Save the size of the Huffman table to the output file
    outputFileStream->put(huffmanTable->size());

    // Save the Huffman table to the output file (6 bytes for each element)
    for (map<char, vector<bool> >::iterator iter = huffmanTable->begin(); iter != huffmanTable->end(); iter++) {
        // 1st byte - The current char ASCII-code
        outputFileStream->put((*iter).first);
        // 2nd byte - The current char Huffman code size
        vector<bool> code = (*iter).second;
        outputFileStream->put(code.size());
        // Fill the current char Huffman code with zeros to the size of the int
        while (code.size() != sizeof(int) * 8)
            code.push_back(0);
        // 3rd - 6th bytes - The filled with zeros current char Huffman code
        unsigned int codedInt = 0;
        for (vector<bool>::iterator it = code.begin(); it != code.end(); it++)
            codedInt = (codedInt << 1) | *it;
        outputFileStream->write((char*) &codedInt, sizeof(int));
    }

    // Save the stored buffer size to the output file
    outputFileStream->write((char*) &bufferSize, sizeof(int));

    // Save the buffer to the output file
    int counter = 0;
    char codedByte = 0;
    for (vector<bool>::iterator it = buffer.begin(); it != buffer.end(); it++) {
        codedByte = (codedByte << 1) | *it;
        counter++;
        if ((counter % 8) == 0) {
            outputFileStream->put(codedByte);
            codedByte = 0;
        }
    }
    closeFile(outputFileStream);
}


/**
 * Restore the Huffman table from the input (compressed) file.
 *
 * @param inputFileName - The input (compressed) file name.
 * @return - The restored Huffman table.
 */
map<vector<bool>, char>* restoreHuffmanTable(char* inputFileName) {
    map<vector<bool>, char>* result = new map<vector<bool>, char>();
    ifstream* inputFileStream = openInputFile(inputFileName);

    // Read the table size from the input file
    char tableSize = 0;
    inputFileStream->get(tableSize);

    // Read the Huffman table from the input file(6 bytes for each element)
    for (int i = 0; i < tableSize; i++) {
        // 1st byte - The current char ASCII-code
        char ch;
        inputFileStream->get(ch);
        // 2nd byte - The current char Huffman code size
        char codeLength = 0;
        inputFileStream->get(codeLength);
        // 3rd - 6th bytes - The filled with zeros current char Huffman code
        unsigned int codedInt = 0;
        inputFileStream->read((char*) &codedInt, sizeof(int));
        // Restore the current char Huffman code to original size
        vector<bool> code;
        for (int j = 0; j < codeLength; j++) {
            code.push_back(codedInt & 2147483648);
            codedInt <<= 1;
        }
    (*result)[code] = ch;
    }
    closeFile(inputFileStream);
    return result;
}


/**
 * Create the decompressed file.
 *
 * @param inputFileName - The input (compressed) file name.
 * @param outputFileName - The output file name.
 * @param huffmanTable - The restored Huffman table.
 */
void createDecompressedFile(char* inputFileName, char* outputFileName, map<vector<bool>, char>* huffmanTable) {
    // Open the input file and jump over the saved Huffman table at the file beginning
    ifstream* inputFileStream = openInputFile(inputFileName);
    char tableSize = huffmanTable->size();
    inputFileStream->seekg(tableSize * 6 + 1);

    // Read the buffer size form the input file
    unsigned int bufferSize = 0;
    inputFileStream->read((char*) &bufferSize, sizeof(int));

    // Read the buffer from the input file
    vector<bool> buffer;
    char codedByte = 0;
    while (inputFileStream->get(codedByte))
        for (int i = 0; i < 8; i++) {
            buffer.push_back(codedByte & 128);
            codedByte <<= 1;
        }
    closeFile(inputFileStream);

    // Restore the buffer to the original size
    while(buffer.size() != bufferSize)
        buffer.pop_back();

    // Restore the original chars from the Huffman table and save it to the output file
    ofstream* outputFileStream = openOutputFile(outputFileName);
    vector<bool> code;
    for (vector<bool>::iterator it = buffer.begin(); it != buffer.end(); it++){
        code.push_back(*it);
        if (huffmanTable->count(code)) {
            outputFileStream->put((*huffmanTable)[code]);
            code.clear();
        }
    }
    delete huffmanTable;
    closeFile(outputFileStream);
}


/**
 * Compress the input file and store result in the output file.
 *
 * @param inputFileName - The input file name.
 * @param outputFileName - The output file name.
 */
void compressFile(char* inputFileName, char* outputFileName) {
    cout << "compressing... ";
    map<char, int>* charsFrequencyTable = createCharsFrequencyTable(inputFileName);
    Node* huffmanTree = createHuffmanTree(charsFrequencyTable);
    map<char, vector<bool> >* huffmanTable = createHuffmanTable(huffmanTree);
    createCompressedFile(inputFileName, outputFileName, huffmanTable);
    std::cout << "ready" << std::endl;
}


/**
 * Decompress the input (compressed) file and store result in the output file.
 *
 * @param inputFileName - The input file name.
 * @param outputFileName - The output file name.
 */
void decompressFile(char* inputFileName, char* outputFileName) {
    cout << "decompressing... ";
    map<vector<bool>, char>* huffmanTable = restoreHuffmanTable(inputFileName);
    createDecompressedFile(inputFileName, outputFileName, huffmanTable);
    std::cout << "ready" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cout << "Input format error!";
        return -1;
    }
    if (*argv[1] == 'c') {
        compressFile(argv[2], argv[3]);
    } else if (*argv[1] == 'd') {
        decompressFile(argv[2], argv[3]);
    } else {
        cout << "Input format error.";
        return -1;
    }
    return 0;
}
