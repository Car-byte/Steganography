using System;
using ShellProgressBar;

namespace Steganography
{
    static class Progress
    {
        static public ShellProgressBar.ProgressBar GetProgressBar(int max, in string name1, in string name2, string activity)
        {
            //set the options
            ProgressBarOptions options = new ProgressBarOptions();
            options.ProgressCharacter = '\u2588';
            options.ProgressBarOnBottom = true;
            options.ForegroundColor = ConsoleColor.DarkGreen;
            options.BackgroundColor = ConsoleColor.Gray;
            options.DisplayTimeInRealTime = false;

            string message = null;

            //choose what to display based on what function called it
            if (activity == "xor")
            {
                message = "Starting... xoring " + name1 + " and " + name2;
            }
            else if (activity == "decode")
            {
                message = "Starting... decoding " + name1 + " and extracting to " + name2;
            }
            else
            {
                message = "Starting... encoding " + name1 + " into " + name2;
            }

            //return the progress bar
            return new ShellProgressBar.ProgressBar(max, message, options);
        }
    }
}
