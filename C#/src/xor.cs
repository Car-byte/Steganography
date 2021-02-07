using System;
using System.Drawing;
using ShellProgressBar;

namespace Steganography
{
    static class Xor
    {

        public static void XorImages(string im1, string im2, string output)
        {
            //formating
            Console.WriteLine("\n\n");

            Bitmap image1 = null;
            Bitmap image2 = null;
            ProgressBar bar = null;

            try
            {
                //read the images
                image1 = new Bitmap(im1);
                image2 = new Bitmap(im2);

                if (image1.Height != image2.Height || image1.Width != image2.Width)
                {
                    Console.Write("The images you are trying to xor are not the same size do you want to continue? (yes or no): ");
                    string input = Console.ReadLine();
                    input = input.ToLower();
                    
                    if (input != "y" && input != "ye" && input != "yes")
                    {
                        Exception ex = new Exception("User aborted");
                        throw ex;
                    }

                    //formating
                    Console.WriteLine("\n\n");
                }

                //gets ProgressBar bar for display
                bar = Progress.GetProgressBar(image1.Height, im1, im2, "xor");

                //loop through the images pixels and xor them together
                for (int y = 0; y < image1.Height && y < image2.Height; y++)
                {
                    bar.Tick();

                    for (int x = 0; x < image1.Width && x < image2.Width; x++)
                    {
                        //get pixel before passing to function for easier reading
                        Color pixel1 = image1.GetPixel(x, y);
                        Color pixel2 = image2.GetPixel(x, y);

                        //set the current pixel to be the xored one by calling XorPixel
                        image1.SetPixel(x, y, XorPixel(pixel1, pixel2));
                    }
                }

                bar.Tick("Saving...");

                //save the image
                image1.Save(output, System.Drawing.Imaging.ImageFormat.Png);

                bar.Tick("Done!");

                //formating
                Console.WriteLine("\n\n");
            }
            catch (Exception ex)
            {
                Console.WriteLine("Image failed to load, is the image name you entered correct? Error: " + ex.Message);
            }
            finally
            {
                if (image1 != null) image1.Dispose();
                if (image2 != null) image2.Dispose();
                if (bar != null) bar.Dispose();
            }
        }

        //xores the two pixels together
        private static Color XorPixel(in Color pixel1, in Color pixel2)
        {
            byte r = (byte)(pixel1.R ^ pixel2.R);
            byte g = (byte)(pixel1.G ^ pixel2.G);
            byte b = (byte)(pixel1.B ^ pixel2.B);
            return Color.FromArgb(r, g, b);
        }
    }
}
