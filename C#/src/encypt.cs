using System;
using System.Security.Cryptography;
using Microsoft.AspNetCore.Cryptography.KeyDerivation;
using System.IO;

namespace Steganography
{
    static class Encypt
    {
        //gets the pasword from the user and then encryps the file the user wants to encode
        public static string GetEncryptedFile(string file)
        {
            Console.Write("Create password for the image: ");
            string password = Console.ReadLine();

            //transform password to 256 bit key
            password = GetEncrptyKey(password);

            if (String.IsNullOrEmpty(password))
            {
                Exception ex = new Exception("Error while getting encryption key");
                throw ex;
            }

            //encrypt the file and return the name of it
            return EncrypytFile(password, file);
        }

        public static string GetDecryptedFile(string file, string password)
        {
            password = GetEncrptyKey(password);

            if (String.IsNullOrEmpty(password))
            {
                Exception ex = new Exception("Error while getting encryption key");
                throw ex;
            }

            //decrypt the file with the encryption key
            return DecryptFile(password, file);
        }

        private static string GetEncrptyKey(string password)
        {
            //possibly randomize salt and pend to the txt file but I just used to combat ranbow tables
            byte[] salt = Convert.FromBase64String("9jVMorKyaXe5MsTcYDVSoQ==");
           

            try
            {
                //transform the ket and return it
                return Convert.ToBase64String(KeyDerivation.Pbkdf2(password, salt, KeyDerivationPrf.HMACSHA1, 10000, 256 / 8));
            }
            catch (Exception ex)
            {
                //Console.WriteLine(ex.Message);
                return "";
            }           
        }

        //encryps a file
        private static string EncrypytFile(string key, string file)
        {

            Aes aes = null;
            FileStream encFile = null;
            FileStream input = null;
            CryptoStream cryptStream = null;

            try
            {
                //create aes and set the settings
                aes = Aes.Create();
                aes.KeySize = 256;
                aes.GenerateIV();
                aes.BlockSize = 128;
                aes.Mode = CipherMode.CBC;
                aes.Padding = PaddingMode.PKCS7;
                aes.Key = Convert.FromBase64String(key);

                //create the new file that will be written to
                encFile = new FileStream(file + ".aes", FileMode.Create);

                //write the iv to the front of the file
                encFile.Write(aes.IV, 0, aes.IV.Length);

                //create the file streams neccesary one for encrypting and one for reading the already existing file
                input = new FileStream(file, FileMode.Open);
                cryptStream = new CryptoStream(encFile, aes.CreateEncryptor(), CryptoStreamMode.Write);

                byte[] buffer = new byte[1048576];
                int reading;

                try
                {

                    try
                    {
                        while ((reading = input.Read(buffer, 0, buffer.Length)) > 0)
                        {
                            cryptStream.Write(buffer, 0, reading);
                        }

                        //make sure all the data is put into the file 
                        cryptStream.Flush();
                    }
                    catch
                    {
                        Exception ex = new Exception("Error while encrypting the file");
                        throw ex;
                    }
                }
                catch (Exception e)
                {
                    Exception ex = new Exception("Error while reading " + file);
                    throw ex;
                }
                
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error while encrypting file " + file + " " + ex.Message);
            }
            finally
            {
                //cleanup
                if (aes != null) aes.Dispose();
                if (input != null) input.Dispose();
                if (cryptStream != null) cryptStream.Dispose();
                if (encFile != null) encFile.Dispose();
            }

            return file + ".aes";
        }

        //decrypts a file and outputs it with the .decrypted extension
        private static string DecryptFile(string key, string file)
        {
            Aes aes = null;
            FileStream encryptedFile = null;
            CryptoStream CryptoFile = null;
            FileStream output = null;

            try
            {
                //aes settings
                aes = Aes.Create();
                aes.KeySize = 256;
                aes.BlockSize = 128;
                aes.Mode = CipherMode.CBC;
                aes.Padding = PaddingMode.PKCS7;
                aes.Key = Convert.FromBase64String(key);

                //open the encrypted file
                encryptedFile = new FileStream(file, FileMode.Open);

                //get the iv that is appened to the front of the file and set it for the aes algorithm
                byte[] iv = new byte[16];
                encryptedFile.Read(iv, 0, iv.Length);
                aes.IV = iv;

                //get the file streams needed to decrypt the file one for the output and one for decrypting
                CryptoFile = new CryptoStream(encryptedFile, aes.CreateDecryptor(), CryptoStreamMode.Read);
                output = new FileStream(trimFileName(file), FileMode.Create);

                byte[] buffer = new byte[1048576];
                int reading;
                
                try
                {
                    //decrypt the file
                    try
                    {
                        while ((reading = CryptoFile.Read(buffer, 0, buffer.Length)) > 0)
                        {
                            output.Write(buffer, 0, reading);
                        }
                    }
                    catch 
                    {
                        Exception ex = new Exception("Error while decrypting the text file, the file may not be encrypted");
                        throw ex;
                    }

                    //make sure all has been output to the file
                    output.Flush();
                }
                catch (Exception ex)
                {
                    Console.WriteLine("Error while decrypting: " + ex.Message);
                }

            }
            catch (Exception ex)
            {
                Console.WriteLine("Error while decrypting file " + file + " Error: " + ex.Message);
            }
            finally
            {
                //cleanup
                if (aes != null) aes.Dispose();
                if (output != null) output.Dispose();
                if (CryptoFile != null) CryptoFile.Dispose();
                if (output != null) output.Dispose();
            }
     
            //return the name of the decrypted file
            return trimFileName(file);
        }


        //take away the file extensions and append .decrypted to the name
        static private string trimFileName(string fileName)
        {
            string[] name = fileName.Split('.');

            //if there is an error in the file name
            if (String.IsNullOrEmpty(name[0]))
            {
                Exception ex = new Exception("Error with the file name");
                throw ex;
            }

            return name[0] + ".decrypted";
        }

    }
}
