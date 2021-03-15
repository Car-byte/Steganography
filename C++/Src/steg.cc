#include <iostream>
#include <fstream>

#include <opencv2/highgui/highgui.hpp>

#include "steg.h"
#include "encryption.h"


//default constructor will set all values to 0
Steganography::Steganography() {
	this->m_at_end = false;
	this->m_bit_count = 0;
	this->m_byte_count = 0;
	this->m_current_byte = 0;
	this->m_primary_image = nullptr;
	this->m_secondary_image = nullptr;
}


//delete the images
Steganography::~Steganography() {
	//deleting nullptr does nothing and is safe
	delete m_primary_image;
	delete m_secondary_image;
}

void Steganography::Encode(std::string image_path, std::string text_file_path, std::string output_image_path, bool encrypted) {

	Encryption encryption;
	std::ifstream input_file;
	int null_byte_count = 0;

	try {

		if (encrypted)
			text_file_path = encryption.GetEncryptedFile(text_file_path);

		input_file.open(text_file_path, std::ios::binary | std::ios::in);
		if (input_file.fail())
			throw std::exception("Error while opening the text file");

		//read the image
		this->m_primary_image = new cv::Mat;
		*m_primary_image = cv::imread(image_path);

		//get the first byte
		input_file.get(m_current_byte);

		//if the file is empty
		if (input_file.eof())
			throw std::exception("The file text file seems to be empty"); 

		//loop through the image
		for (int y = 0; y < m_primary_image->rows; y++) {

			for (int x = 0; x < m_primary_image->cols; x++) {

				for (int channel = 0; channel < m_primary_image->channels(); channel++) {

					char image_byte = m_primary_image->at<cv::Vec3b>(cv::Point(x, y)).val[channel];

					//change the lsb of the image_byte to match the message
					image_byte = SetLsb(image_byte);

					m_primary_image->at<cv::Vec3b>(cv::Point(x, y)).val[channel] = image_byte;

					m_bit_count++;					

					//if at end of byte
					if (FullByte()) {
						m_byte_count++;

						if (m_at_end) {

							x = m_primary_image->cols - 1;
							y = m_primary_image->rows - 1;
							channel = m_primary_image->channels();
							break;
						}

						m_bit_count = 0;
						input_file.get(m_current_byte);

						//if at end of file we need to add null bytes to the end for extraction
						if (input_file.eof()) {

							null_byte_count++;
							if (null_byte_count == 6)
								m_at_end = true;

							m_current_byte = '\0';
						}

					}
				}
			}
		}

		input_file.close();

		//if we never fully encoded the image
		if (!FullByte() || !m_at_end)
			throw std::exception("The image is not big enough to encode the text file decrease the text file size or increase the image size");


		try {
			//if our save is not successful
			if (!cv::imwrite(output_image_path, *m_primary_image))
				throw std::exception("Error while saving file");

		}
		catch (std::exception ex){
			throw std::exception("File failed to save");
		}

	}
	catch (std::exception ex) {

		input_file.close();
		std::cout << "Fatal error while performing encoding" << std::endl;
	}
}


void Steganography::Decode(std::string image_file_path, std::string output_file_path, bool encrypted) {

	Encryption encryption;
	std::string password;
	int null_byte_count = 0;
	std::ofstream output_file;
	std::string buffer = "";

	if (encrypted) {

		std::cout << "Please enter the password for " + image_file_path << ": ";
		getline(std::cin, password);
		password = TrimFilePath(password);
	}

	try {

		output_file.open(output_file_path, std::ios::binary | std::ios::out);
		if (output_file.fail())
			throw std::exception("Failed to create output file");

		//read the image
		this->m_primary_image = new cv::Mat;
		*m_primary_image = cv::imread(image_file_path);

		//loop through the image
		for (int y = 0; y < m_primary_image->rows; y++) {

			for (int x = 0; x < m_primary_image->cols; x++) {

				for (int channel = 0; channel < m_primary_image->channels(); channel++) {

					m_bit_count++;

					//get the current byte from the image
					char image_byte = m_primary_image->at<cv::Vec3b>(cv::Point(x, y)).val[channel];

					//match the current byte to match the lsb of the image_byte
					GetLsb(image_byte);

					//if we have gone through a whole byte
					if (FullByte()) {

						m_byte_count++;
						buffer.push_back(m_current_byte);

						if (m_current_byte == '\0') {

							null_byte_count++;

							//if we reached the end of the message
							if (null_byte_count == 5) {

								m_at_end = true;
								y = m_primary_image->rows - 1;
								x = m_primary_image->cols - 1;
								channel = m_primary_image->channels() - 1;
								break;
							}
						}
						else {

							null_byte_count = 0;
							output_file.write(buffer.c_str(), buffer.size());
							buffer.clear();
						}

						//reset bit count and current byte to 0
						//important that current byte get sets to 0 as for the GetLSB and SetLSB methods
						//dont have a check for if the value is 1 but needs to be 0
						m_bit_count = 0;
						m_current_byte = 0;
					}
				}
			}
		}

		//if never reached the end of the image, this means we did not read a message
		if (!m_at_end)
			throw std::exception("The image seems to not contain encoded data or the image was edited");

		//if the file is encrypted then we need to decrypt it
		if (encrypted) {

			output_file.close();
			output_file_path = encryption.GetDecryptedFile(output_file_path, password);
		}

		//output how many bytes we recovered
		std::cout << "Got " << m_byte_count << " from " + image_file_path + " and extracted to " + output_file_path << std::endl;
	}
	catch (std::exception ex) {
		std::cout << "Fatal error while performing decoding" << std::endl;
	}
}


//just returns if bit count is 8
bool Steganography::FullByte() {
	return m_bit_count == 8;
}



char Steganography::SetLsb(char image_byte) {

	//if the bit at current position is set we set it on the image byte
	if (1 & (m_current_byte >> (7 - m_bit_count))) {
		image_byte |= 1;
	}
	else {
		//else we set it to 0
		image_byte &= ~1;
	}

	return image_byte;
}


void Steganography::GetLsb(char image_byte) {

	//if bit is set we set but in current byte
	if (1 & image_byte)
		m_current_byte |= 1;
	
	//if not a complete byte shift to the left to make room for next bit
	if (!FullByte())
		m_current_byte = m_current_byte << 1;
}


//trims a string from all blank space from left and right ends
std::string& Steganography::TrimFilePath(std::string& str) {

	str.erase(0, str.find_first_not_of(' '));
	str.erase(str.find_last_not_of(' ') + 1);
	return str;
}



void Steganography::Xor(std::string image1_file_path, std::string image2_file_path, std::string output_file_path) {

	try {

		//read images
		this->m_primary_image = new cv::Mat;
		this->m_secondary_image = new cv::Mat;
		*m_primary_image = cv::imread(image1_file_path);
		*m_secondary_image = cv::imread(image2_file_path);

		//if the images are not the same size we make sure that the user wants to continue
		if ((m_primary_image->rows != m_secondary_image->rows) || (m_primary_image->cols != m_secondary_image->cols)) {

			std::string ans = "";
			std::cout << "The images are a different size, are you sure you want to continue? (yes or no): ";
			std::getline(std::cin, ans);

			for (int i = 0; i < ans.size(); i++) {
				ans[i] = std::tolower(ans[i]);
			}

			if (ans == "n" || ans == "no" || ans == "noo" || ans == "nah") {
				throw std::exception("User aborted");
			}
		}

		//loop through out images and xor each pixel
		for (int y = 0; y < m_primary_image->rows && y < m_secondary_image->rows; y++) {

			for (int x = 0; x < m_primary_image->cols && x < m_secondary_image->cols; x++) {

				for (int channel = 0; channel < m_primary_image->channels() && channel < m_secondary_image->channels(); channel++) {

					char image_1_byte = m_primary_image->at<cv::Vec3b>(cv::Point(x, y)).val[channel];
					char image_2_byte = m_secondary_image->at<cv::Vec3b>(cv::Point(x, y)).val[channel];

					m_primary_image->at<cv::Vec3b>(cv::Point(x, y)).val[channel] = (image_1_byte ^ image_2_byte);
				}
			}
		}

		try {
			//if our writing to the image fails
			if (!cv::imwrite(output_file_path, *m_primary_image))
				throw std::exception("Error while saving file");

		}
		catch (std::exception ex) {
			throw std::exception("Error while saving image");
		}


	}
	catch (std::exception ex){
		std::cout << "Fatal error while performing xor" << std::endl;
	}
}
