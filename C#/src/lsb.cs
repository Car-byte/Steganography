using System;
using System.Drawing;
using System.IO;
using System.Collections.Generic;

namespace Steganography
{
    static class LSB
    {
        //variables from the class so we do not need to pass them as parameters to the functions
        static private int bitCount = 0;
        static private int byteCount = 0;
        static private byte currentByte = 0;
        static private bool atEnd = false;

        //encodes and image with the given txt file
        static public void Encode(string InputImage, string InputTxt, string OutputImage, bool encrypted = false)
        {
            //formating
            Console.WriteLine("\n\n\n");

            Bitmap image = null;
            FileStream input = null;
            ShellProgressBar.ProgressBar bar = null;
            int count = 0;

            //if the file is to be encrypted we encrypt the file and get the new encrypted one before moving on
            if (encrypted)
            {
                //gets the new encrypted file
                InputTxt = Encypt.GetEncryptedFile(InputTxt);

                //formating
                Console.WriteLine();
                Console.WriteLine();
            }

            try
            {
                //gets the file that is to be places in the image
                input = new FileStream(InputTxt, FileMode.Open);

                //get the progress bar
                bar = Progress.GetProgressBar((int)input.Length, InputTxt, InputImage, "encode");

                //update the progress bar
                bar.Tick("Reading image...");

                //read the image
                image = new Bitmap(InputImage);

                //update progress bar
                bar.Tick("Encoding image..");

                //get the first byte from the file
                int temp = input.ReadByte();
                LSB.currentByte = (byte)temp;

                //check if the file is empty
                if (temp == -1)
                {
                    Exception ex = new Exception("The file seems to be empty");
                    throw ex;
                }

                //loop through the whole image 
                for(int y = 0; y < image.Height; y++) 
                {
                    //update progress bar with the byte count
                    //one of the parameters in creating the progress bar is the size of the file
                    //so the bar knows the complete size and can calculate percent based on current count
                    bar.Tick(LSB.byteCount);

                    for (int x = 0; x < image.Width; x++)
                    {
                        //current pixel
                        Color imagePixel = image.GetPixel(x, y);

                        byte red = 0;
                        byte green = 0;
                        byte blue = 0;

                        //loop through the color channels RGB
                        for (int i = 0; i < 3; i++)
                        {
                            //check which channel we are on and encode based on that
                            //calls the setLSB methos which encodes the pixel channel to match that of the byte from the file
                            switch (i)
                            {
                                case 0:
                                    red = setLSB(imagePixel.R);
                                    break;
                                case 1:
                                    green = setLSB(imagePixel.G);
                                    break;
                                case 2:
                                    blue = setLSB(imagePixel.B);
                                    break;
                            }

                            //update our bit count so we know when we completed a whole byte
                            LSB.bitCount++;

                            //if we have complete a whole byte we need to get the next byte from the file
                            if (AtEndOfByte())
                            {
                                //update byte count
                                LSB.byteCount++;

                                //read the byte
                                temp = input.ReadByte();

                                //set current byte
                                LSB.currentByte = (byte)temp;

                                //if there was nothing left in the file
                                if (temp == -1)
                                {
                                    //update count
                                    count++;

                                    //if we have put 6 null bytes at the end of the encoding
                                    //this means we have completly encoded the txt file within the image
                                    if (count == 6)
                                    {
                                        //break out and close the file and set the end bool to true as we have finished
                                        if (input != null) input.Close();
                                        LSB.atEnd = true;
                                        break;
                                    }

                                    //set current byte to a null byte so that we can encode it within the image
                                    //6 null bytes are placed in the image to signify the end of the encoded message
                                    //this is so when we decode we know that we have finshed reading the encoded data
                                    LSB.currentByte = (byte)'\0';
                                }

                                //reset bit count as we got a new byte
                                LSB.bitCount = 0;
                            }

                        }

                        //once we have finished encoding one pixel we replace the old one with the new encoded one
                        image.SetPixel(x, y, Color.FromArgb(red, green, blue));

                        //if we are dont encoding exit the loops
                        if (LSB.atEnd && LSB.bitCount == 8)
                        {
                            x = image.Width - 1;
                            y = image.Height - 1;
                        }
                    }
                }

                //if we have finished looping through the image but did not finish encoding that means the txt file was too large for this image
                if (LSB.bitCount != 8 || LSB.atEnd == false)
                {
                    Exception ex = new Exception("The file you are trying to hide is too big try with a larger image or smaller message");
                    throw ex;
                }

                try
                {
                    //save the new enocded image
                    bar.Tick("Saving Image...");
                    image.Save(OutputImage, System.Drawing.Imaging.ImageFormat.Png);
                    bar.Tick(bar.MaxTicks);
                }
                catch (Exception e)
                {
                    //if we get an error while saving the image
                    Exception ex = new Exception("The image failed to save make sure the file does not already exist");
                    throw ex;
                }

            }
            catch (Exception ex)
            {
                Console.WriteLine("Error while encoding the image: " + ex.Message);
            }
            finally
            {
                //cleanup
                if (image != null) image.Dispose();
                if (input != null) input.Dispose();
                if (bar != null) bar.Dispose();
            }
        }

        //decodes an steg image
        static public void Decode(string InputImage, string outputTxt, bool encrypted = false)
        {
            //formating
            Console.WriteLine("\n\n");

            //decalare as null so we can check later for cleanup
            Bitmap image = null;
            FileStream output = null;
            ShellProgressBar.ProgressBar bar = null;
            string password = null;
            int count = 0;

            //get the password before we start to read the data
            if (encrypted)
            {
                //Some Console.WroteLine() for formating
                Console.WriteLine();
                Console.Write("Please enter the password: ");
                password = Console.ReadLine();
                Console.WriteLine("\n");
            }

            try
            {
                //Open file streams and read all neccessary data like the image and get the progress bar
                output = new FileStream(outputTxt, FileMode.Create);

                bar = Progress.GetProgressBar(100, InputImage, outputTxt, "decode");

                bar.Tick("Reading Image...");

                image = new Bitmap(InputImage);

                bar.Tick("Decoding image");

                //update the MaxTicks after reading the image
                bar.MaxTicks = image.Height / 2;

                List<byte> buffer = new List<byte>();

                //loop through the image pixels
                for (int y = 0; y < image.Height; y++)
                {
                    bar.Tick(y);

                    for (int x = 0; x < image.Width; x++)
                    {

                        Color imagePixel = image.GetPixel(x, y);

                        for (int channel = 0; channel < 3; channel++)
                        {
                            LSB.bitCount++;

                            switch (channel)
                            {
                                case 0:
                                    getLSB(imagePixel.R);
                                    break;
                                case 1:
                                    getLSB(imagePixel.G);
                                    break;
                                case 2:
                                    getLSB(imagePixel.B);
                                    break;
                            }

                            //if we are done with the byte
                            if (AtEndOfByte())
                            {
                                LSB.byteCount++;
                                buffer.Add(currentByte);

                                //if the byte is all 0's aka null byte
                                if (LSB.currentByte == '\0')
                                {
                                    //if we are at the end of the image with a count of 5
                                    count++;
                                    if (count == 5)
                                    {
                                        //mark that we reached the end and fufull exit conditions for the loop
                                        LSB.atEnd = true;
                                        x = image.Width - 1;
                                        y = image.Height - 1;
                                        break;
                                    }
                                }
                                else
                                {
                                    //if the byte is not null we write it to the output
                                    count = 0;
                                    output.Write(buffer.ToArray(), 0, buffer.ToArray().Length);
                                    buffer.Clear();
                                }

                                //reset bit count and current byte
                                LSB.bitCount = 0;
                                LSB.currentByte = 0;
                            }
                        }
                    }
                }

                //if we never find the end points of the encoded data
                if (LSB.atEnd == false)
                {
                    Exception ex = new Exception("The image does not seem to be encoded or the image was altered after encoding");
                    throw ex;
                }
                 
                //if the data was encrypted then we need to decrypt the data inorder to revert to original form
                if (encrypted)
                {
                    output.Close();

                    bar.Tick("Decypting the file...");

                    outputTxt = Encypt.GetDecryptedFile(outputTxt, password);                 
                }

                bar.Tick("Got " + LSB.byteCount + " bytes from " + InputImage + " and extracted to " + outputTxt);
                bar.Tick(bar.MaxTicks);

            }
            catch (Exception ex)
            {
                Console.WriteLine("Error while decoding image: " + ex.Message);
            }
            finally
            {
                //cleanup
                if (image != null) image.Dispose();
                if (output != null) output.Dispose();
                if (bar != null) bar.Dispose();
            }

        }


        //get the lsb aka sets the lsb of the currentByte to match the encoded pixel
        static private void getLSB(byte channel)
        {
            //if the bit in channel is set we bitwise or to force the currentByte to be set
            //we do not need to unset because every itteration of byte we reset currentByte to be 0
            if ((channel & 1) == 0000_0001)
            {
                LSB.currentByte |= 1;
            }
           
            //if we are not at the end we shif the pixel over to continue to encode it
            if (!AtEndOfByte())
            {
                LSB.currentByte = (byte)(LSB.currentByte << 1);
            }

        }

        //sets the lsb to match that of the currentByte
        static private byte setLSB(byte channel)
        {
            //checks if the lsb is set if it is then we bitwise or with 1 aka 0000_0001
            //if the bit is not set we bitwise and with 254 aka ~1 aka 1111_1110 to unset the lsb
            if ((1 & (LSB.currentByte >> (7 - LSB.bitCount))) == 0000_0001)
            {
                channel |= 1;
            }
            else
            {
                channel &= 254;
            }

            return channel;
        }

        //checks if we are at the end of a byte aka bitcount is 8
        static private bool AtEndOfByte()
        {
            return LSB.bitCount == 8;
        }
    }
}
