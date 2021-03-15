

/*---------------------------------------------------------------------------------
	Copied from https://github.com/Car-byte/Base64
	Might be outdated updates for this file will be on that link

---------------------------------------------------------------------------------*/



#ifndef BASE64_H
#define BASE64_H

#include <iostream>
#include <string>
#include <array>
#include <map>

class Base64 {
public:

	Base64() {
		
		//could speed up by hardcoding instead of by loop
		char temp[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

		for (int i = 0; i < m_encoding_table.size(); i++) {
			m_encoding_table[i] = temp[i];
			m_decoding_table[temp[i]] = i;
		}
	}

	std::string ToBase64(const std::string& str) {

		std::string converted = "";
		converted.reserve((size_t)(str.size() * 1.5));

		size_t i = 0;
		
		while (i + 3 <= str.size()) {

			char output[4];

			output[0] = m_encoding_table[(int)((str[i] & 255) >> 2)];
			output[1] = m_encoding_table[(int)(((str[i] & 3) << 4) | ((str[i + 1] & 255) >> 4))];
			output[2] = m_encoding_table[(int)(((str[i + 1] & 15) << 2) | ((str[i + 2] & 255) >> 6))];
			output[3] = m_encoding_table[(int)(str[i + 2] & 63)];

			converted += output[0];
			converted += output[1];
			converted += output[2];
			converted += output[3];

			i = i + 3;
		}

		//last bytes of the str and add padding
		if (i < str.size()) {

			char output[4];

			output[0] = m_encoding_table[(int)((str[i] & 255) >> 2)];

			if (i + 1 < str.size()) {

				output[1] = m_encoding_table[(int)(((str[i] & 3) << 4) | ((str[i + 1] & 255) >> 4))];
				output[2] = m_encoding_table[(int)((str[i + 1] & 15) << 2)];
				output[3] = '=';
			}
			else {
				output[1] = m_encoding_table[(int)((str[i] & 3) << 4)];
				output[2] = '=';
				output[3] = '=';
			}

			converted += output[0];
			converted += output[1];
			converted += output[2];
			converted += output[3];
		}

		return converted;
	}

	std::string FromBase64(const std::string& str) {

		std::string converted = "";
		converted.reserve((size_t)(str.size() * 0.75));

		size_t i = 0;

		while (i + 4 <= str.size()) {

			char output[3];

			output[0] = ((m_decoding_table[str[i]] << 2) | (m_decoding_table[str[i + 1]] >> 4));
			output[1] = ((m_decoding_table[str[i + 1]] << 4) | (m_decoding_table[str[i + 2]] >> 2));
			output[2] = ((m_decoding_table[str[i + 2]] << 6) | (m_decoding_table[str[i + 3]]));

			//could increase speed by not checking every single one 
			if (output[0]) converted += output[0];
			if (output[1]) converted += output[1];
			if (output[2]) converted += output[2];

			i = i + 4;
		}

		return converted;
	}


private:

	std::array<char, 65> m_encoding_table;

	//use harcode functions instead of map to speed up 
	std::map<char, int> m_decoding_table;
};


namespace B64 {

	std::string ToBase64(const std::string& str) {

		char encoding_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

		std::string converted = "";
		converted.reserve((size_t)(str.size() * 1.5));

		size_t i = 0;

		while (i + 3 <= str.size()) {

			char output[4];

			output[0] = encoding_table[(int)((str[i] & 255) >> 2)];
			output[1] = encoding_table[(int)(((str[i] & 3) << 4) | ((str[i + 1] & 255) >> 4))];
			output[2] = encoding_table[(int)(((str[i + 1] & 15) << 2) | ((str[i + 2] & 255) >> 6))];
			output[3] = encoding_table[(int)(str[i + 2] & 63)];

			converted += output[0];
			converted += output[1];
			converted += output[2];
			converted += output[3];

			i = i + 3;
		}

		//last bytes of the str and add padding
		if (i < str.size()) {

			char output[4];

			output[0] = encoding_table[(int)(str[i] >> 2)];

			if (i + 1 < str.size()) {

				output[1] = encoding_table[(int)(((str[i] & 3) << 4) | ((str[i + 1] & 255) >> 4))];
				output[2] = encoding_table[(int)((str[i + 1] & 15) << 2)];
				output[3] = '=';
			}
			else {
				output[1] = encoding_table[(int)((str[i] & 3) << 4)];
				output[2] = '=';
				output[3] = '=';
			}

			converted += output[0];
			converted += output[1];
			converted += output[2];
			converted += output[3];
		}

		return converted;
	}


	std::string FromBase64(const std::string& str) {

		std::map<char, int> decoding_table;
		char encoding_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

		for (int i = 0; i < 65; i++) {
			decoding_table[encoding_table[i]] = i;
		}

		std::string converted = "";
		converted.reserve((size_t)(str.size() * 0.75));

		size_t i = 0;

		while (i + 4 <= str.size()) {

			char output[3];

			output[0] = ((decoding_table[str[i]] << 2) | (decoding_table[str[i + 1]] >> 4));
			output[1] = ((decoding_table[str[i + 1]] << 4) | (decoding_table[str[i + 2]] >> 2));
			output[2] = ((decoding_table[str[i + 2]] << 6) | (decoding_table[str[i + 3]]));

			//could increase speed by not checking every single one 
			if (output[0]) converted += output[0];
			if (output[1]) converted += output[1];
			if (output[2]) converted += output[2];

			i = i + 4;
		}

		return converted;
	}
}

#endif // !BASE64_H