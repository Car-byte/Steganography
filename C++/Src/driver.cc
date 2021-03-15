

#include <iostream>
#include <string>

#include "steg.h"
#include "encryption.h"

//display the help menu
void Menu() {
    std::cout << std::endl;
    std::cout << "-------------------------------------------------------------------------" << std::endl;
    std::cout << "The first argument must always be either --xor or --d or --e" << std::endl;
    std::cout << "-------------------------------------------------------------------------" << std::endl;
    std::cout << "What arguments do" << std::endl << std::endl;
    std::cout << "--xor\tXor two images together and outputs the result as an image" << std::endl;
    std::cout << "--d\tDecodes an image that has been ecoded and extracts the txt file" << std::endl;
    std::cout << "--e\tEncoded an image with a given txt file" << std::endl;
    std::cout << "-------------------------------------------------------------------------" << std::endl << std::endl;
    std::cout << "--xor\tAfter this argument must be the two image that are to be xor together followed by the output file name" << std::endl << std::endl;
    std::cout << "--d\tAfter this argument must be the image to be decoded followed by output file name and an option --p argumnet if the file is encrypted" << std::endl << std::endl;
    std::cout << "--e\tAfter this argument must be the image to encode followed by txt file to encode in it followd by the output file name and then the option --p flag if you want to encrypt the file" << std::endl << std::endl;
    std::cout << "-------------------------------------------------------------------------" << std::endl << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "--xor image1.png image2.png outputImage.png" << std::endl;
    std::cout << "--e image.png secret.txt ouputImage.png" << std::endl;
    std::cout << "--e image.png secret.txt ouputImage.png --p" << std::endl;
    std::cout << "--d stegImage.png output.txt" << std::endl;
    std::cout << "--d stegImageEncrypted.png output.txt --p" << std::endl;
}



std::string lower_string(std::string str) {

    for (int i = 0; i < str.size(); i++)
        str[i] = std::tolower(str[i]);

    return str;
}


int main(int argc, char* argv[])
{
    if (argc <= 1) {
        std::cout << "No arguments were provided try --help for help with commands" << std::endl;
        std::exit(10);
    }

    //object for doing encoding, decoding and xor
    Steganography steg;

    std::string main_arg = lower_string((std::string)argv[1]);

    //parse the input and call functions accordingly
    if (main_arg == "help" || main_arg == "-help" || main_arg == "--help") {
        Menu();
    }
    else if (main_arg == "e" || main_arg == "-e" || main_arg == "--e") {

        if (argc <= 4) {
            std::cout << "Invalid number of arguments for encryption type --help for help" << std::endl;
        }
        else if (argc == 6 && (lower_string((std::string)argv[5]) == "p" || lower_string((std::string)argv[5]) == "-p" || lower_string((std::string)argv[5]) == "--p")) {
            steg.Encode(argv[2], argv[3], argv[4], true);
        }
        else {
            steg.Encode(argv[2], argv[3], argv[4]);
        }

    }
    else if (main_arg == "d" || main_arg == "-d" || main_arg == "--d") {

        if (argc <= 3) {
            std::cout << "Invalid number of arguments for decryption type --help for help" << std::endl;
        }
        else if (argc == 5 && (lower_string((std::string)argv[4]) == "p" || lower_string((std::string)argv[4]) == "-p" || lower_string((std::string)argv[4]) == "--p")) {
            steg.Decode(argv[2], argv[3], true);
        }
        else {
            steg.Decode(argv[2], argv[3]);
        }
    }
    else if (main_arg == "xor" || main_arg == "-xor" || main_arg == "--xor") {

        if (argc <= 3) {
            std::cout << "Invalid number of arguments for xor type --help for help" << std::endl;
        }
        else {
            steg.Xor(argv[1], argv[2], argv[3]);
        }
    }
    else {
        std::cout << "Invalid arguments try --help for help with arguments" << std::endl;
    }
	
	return 0;
}
