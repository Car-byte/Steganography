#include <iostream>
#include <string>
#include <fstream>

#include <cryptopp/aes.h>
#include <cryptopp/hmac.h>
#include <cryptopp/pwdbased.h>
#include <cryptopp/sha.h>
#include <cryptopp/modes.h>
#include <cryptopp/randpool.h>
#include <cryptopp/files.h>

#include "base64.h"
#include "encryption.h"


std::string Encryption::GetKey(const std::string password) {

	CryptoPP::SecByteBlock out(32);

	//salt is just to prevent basic rainbow table attacks
	std::string salt = B64::FromBase64("9jVMorKyaXe5MsTcYDVSoQ==");

	CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA1> pbkdf2;

	//derive a symmetrical encryption key from the password using pbkdf2
	pbkdf2.DeriveKey(out, out.size(), 0, (CryptoPP::byte *)password.data(), password.size(), (CryptoPP::byte*)salt.data(), salt.size(), 10000);

	return B64::ToBase64(std::string(out.begin(), out.end()));
}


//trims the file name and adds decrypted to it for decrypted files
std::string Encryption::TrimFileName(const std::string& file_path) {
		
	for (int i = 0; i < file_path.size(); i++) {

		if (file_path[i] == '.')
			return file_path.substr(0, i) + ".decrypted";
	}

	throw std::exception("File pathing error");
}



std::string Encryption::EncryptFile(const std::string& key_b64, const std::string& file_path) {
	
	std::ifstream input_file;
	std::ofstream output_file;

	try {

		input_file.open(file_path, std::ios::binary | std::ios::in);
		if (input_file.fail()) {
			throw std::exception("Unable to open text file");
		}

		//generate a random iv for aes
		CryptoPP::byte iv[16];
		CryptoPP::RandomPool rnd;
		rnd.GenerateBlock(iv, 16);

		std::string aes_key = B64::FromBase64(key_b64);

		output_file.open(file_path + ".aes", std::ios::binary | std::ios::out);
		if (output_file.fail()) {
			throw std::exception("Unable to create encryption file");
		}

		//pend the iv to the very start of the output file
		output_file.write((const char*)iv, 16);

		//create aes encryption data structure and set the key and iv
		CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption encrypt;
		encrypt.SetKeyWithIV((CryptoPP::byte *)aes_key.data(), aes_key.size(), iv);

		//the constructor performs the operations and gets disposed of automatically after getting called no cleanup for new needed
		//saving an object is marked as delete so we use the constructor to perform the operation
		CryptoPP::FileSource(input_file, true, new CryptoPP::StreamTransformationFilter(encrypt, new CryptoPP::FileSink(output_file)));
	}
	catch (std::exception ex) {
		output_file.close();
		input_file.close();
		throw std::exception("Fatal Error: error while encrypting file");
	}

	output_file.close();
	input_file.close();

	return file_path + ".aes";
}



std::string Encryption::DecryptFile(const std::string& password_plain_text, const std::string& file_path) {

	std::ifstream input_file;
	std::ofstream output_file;

	try {

		input_file.open(file_path, std::ios::binary | std::ios::in);
		if (input_file.fail())
			throw std::exception("Error while opening the input file");

		output_file.open(TrimFileName(file_path), std::ios::binary | std::ios::out);
		if (output_file.fail())
			throw std::exception("Error while creating the decryption file");

		//create iv and read the iv from the file
		char iv[16];
		input_file.read(iv, 16);

		std::string aes_key = B64::FromBase64(GetKey(password_plain_text));

		//create the aes decryption data structure and set the key with iv
		CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption decryption;
		decryption.SetKeyWithIV((CryptoPP::byte *)aes_key.data(), aes_key.size(), (CryptoPP::byte *)iv);

		//the constructor performs the operations and gets disposed of automatically after getting called no cleanup for new needed
		//saving an object is marked as delete so we use the constructor to perform the operation
		CryptoPP::FileSource(input_file, true, new CryptoPP::StreamTransformationFilter(decryption, new CryptoPP::FileSink(output_file)));

	}
	catch (std::exception ex) {
		output_file.close();
		input_file.close();
		throw std::exception("Error while opening files make sure that they exist");
	}

	output_file.close();
	input_file.close();

	return TrimFileName(file_path);
}


//wrapper function for encrypting a file 
std::string Encryption::GetEncryptedFile(const std::string& file_path) {

	std::string password;
	std::cout << "Create a password for the encryption: ";
	std::getline(std::cin, password);
	std::cout << std::endl;

	password = GetKey(password);

	if (password.empty())
		throw std::exception("Password was invalid or empty");


	return EncryptFile(password, file_path);
}


//wrapper function for decrypting a file
std::string Encryption::GetDecryptedFile(const std::string& file_path, const std::string& password) {
	
	std::string password_b64 = GetKey(password);

	if (password_b64.empty())
		throw std::exception("Password was invalid or empty");

	return DecryptFile(password, file_path);
}
