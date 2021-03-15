


#ifndef STEG_H
#define STEG_H

#include <iostream>
#include <string>

#include <opencv2/highgui/highgui.hpp>

class Steganography {
public:

	Steganography();
	~Steganography();
	void Encode(std::string image_file_path, std::string text_file_path, std::string output_file_path, bool encrypted = false);
	void Decode(std::string image_file_path, std::string output_file_path, bool encrypted = false);
	void Xor(std::string image1_file_path, std::string image2_file_path, std::string output_file_path);

private:

	char SetLsb(char channel);
	void GetLsb(char channel);
	bool FullByte(); 
	std::string& TrimFilePath(std::string& str);


	uint32_t m_byte_count;
	uint16_t m_bit_count;
	bool m_at_end;
	char m_current_byte;
	cv::Mat* m_primary_image;
	cv::Mat* m_secondary_image; //used for xor
};


#endif // !STEG_H