

#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <iostream>
#include <string>

static class Encryption {
public:

	std::string GetEncryptedFile(const std::string& file_path);
	std::string GetDecryptedFile(const std::string& file_path, const std::string& password);

private:

	std::string EncryptFile(const std::string& key, const std::string& file_path);
	std::string DecryptFile(const std::string& key, const std::string& file_path);
	std::string GetKey(const std::string password);
	std::string TrimFileName(const std::string& file_path);
};



#endif // !ENCRYPTION_H