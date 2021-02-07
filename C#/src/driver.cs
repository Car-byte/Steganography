using System;

namespace Steganography
{
    class driver
    {
        static void Main(string[] args)
        {
            for (int i = 0; i < args.Length; i++)
            {
                args[i] = args[i].ToLower();
            }
        
            //parse the command line arguments
            if (args.Length == 0)
            {
                Console.WriteLine("No arguments were entered type --help for help with usage");
            }
            else if (args[0] == "--help" || args[0] == "-help" || args[0] == "help")
            {
                Menu();
            }
            else if (args[0] == "--e" || args[0] == "-e" || args[0] == "-e")
            {
                if (args.Length < 4)
                {
                    Console.WriteLine("Invalid Number of arguments type --help for help");
                }
                else if (args.Length == 5 && (args[4] == "--p" || args[4] == "-p" || args[4] == "p"))
                {
                    LSB.Encode(args[1], args[2], args[3], true);
                }
                else
                {
                    LSB.Encode(args[1], args[2], args[3]);
                }

            }
            else if (args[0] == "--d" || args[0] == "-d" || args[0] == "d")
            {
                if (args.Length < 3)
                {
                    Console.WriteLine("Invalid Number of arguments type --help for help");
                }
                else if (args.Length == 4 && (args[3] == "--p" || args[3] == "-p" || args[3] == "p"))
                {
                    LSB.Decode(args[1], args[2], true);
                }
                else
                {
                    LSB.Decode(args[1], args[2]);
                }

            }
            else if (args[0] == "--xor" || args[0] == "-xor" || args[0] == "xor")
            {
                if (args.Length < 4)
                {
                    Console.WriteLine("Invalid Number of arguments type --help for help");
                }
                else
                {
                    Xor.XorImages(args[1], args[2], args[3]);
                }

            }
            else
            {
                Console.WriteLine("Arguments are not valid type --help for help with arguments");
            }
        }

        //explain usage and commands
        static private void Menu()
        {
            Console.WriteLine("\n");
            Console.WriteLine("-------------------------------------------------------------------------");
            Console.WriteLine("The first argument must always be either --xor or --d or --e");
            Console.WriteLine("-------------------------------------------------------------------------");
            Console.WriteLine("What arguments do\n");
            Console.WriteLine("--xor\tXor two images together and outputs the result as an image");
            Console.WriteLine("--d\tDecodes an image that has been ecoded and extracts the txt file");
            Console.WriteLine("--e\tEncoded an image with a given txt file");
            Console.WriteLine("-------------------------------------------------------------------------\n");
            Console.WriteLine("--xor\tAfter this argument must be the two image that are to be xor together followed by the output file name\n");
            Console.WriteLine("--d\tAfter this argument must be the image to be decoded followed by output file name and an option --p argumnet if the file is encrypted\n");
            Console.WriteLine("--e\tAfter this argument must be the image to encode followed by txt file to encode in it followd by the output file name and then the option --p flag if you want to encrypt the file\n");
            Console.WriteLine("-------------------------------------------------------------------------\n");
            Console.WriteLine("Examples:");
            Console.WriteLine("--xor image1.png image2.png outputImage.png");
            Console.WriteLine("--e image.png secret.txt ouputImage.png");
            Console.WriteLine("--e image.png secret.txt ouputImage.png --p");
            Console.WriteLine("--d stegImage.png output.txt");
            Console.WriteLine("--d stegImageEncrypted.png output.txt --p");
        }

    }
}
