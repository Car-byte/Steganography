# Steganography
Steganography in C# with a C++ version on its way

Both are compatible with each other, you can encode with the C++ version then decode with the C# version and vice versa

[Steganography](https://en.wikipedia.org/wiki/Steganography) is the process of hiding data within data.

In this small project I use [LSB](https://en.wikipedia.org/wiki/Bit_numbering) steganograhy.

Other methods including DCT coming soon aka when I get a chance

Tested both on windows and mac, both the C# and C++ implementations are corss platform.


# Dependecys
For C# I used the [System.Drawing.Common](https://github.com/dotnet/runtime) for the Bitmap class to read images the [ShellProgressBar](https://github.com/Mpdreamz/shellprogressbar) for the progress bar and [Microsoft.AspNetCore.Cryptography.KeyDerivation](https://dotnet.microsoft.com/apps/aspnet) for the key derivative function all available wiht NuGet 

# Usage




# What is LSB or least significant bit?
Least significant bit is the last bit of a byte example: 10110101 the LSB is the very last one 1011010(1)

The reason being that the last place of a bit changes the byte the least of any position

10110101 converted to a number is 181 but 10110100 converted to a number is 180 the last bit only changes the numbers value by 1!
because changing the last bit only changes the byte by a tiny amount that means we can change it to whatever we want and it will
have barly and impact on the image.

We can go through an image byte by byte and change all of the last bits to our secrete message and therfore encoding our message into
the image.

take a look at these two images:

![alt text](https://raw.githubusercontent.com/Car-byte/Steganography/main/Examples/encoded.png)
![alt text](https://github.com/Car-byte/Steganography/blob/main/Examples/library.png?raw=true)

Can you tell wich one has a message encoded in it whith just your eyes? Im willing to bet you cannot.
Thats the whole goal while the goal of encyption is to make sure no one can read your message,
steganography is all about concealment, trying to make sure no one knows a message is whithin another message.


# Applications
This is cool and all but, why?

If you were a music company passing out copies of pre-released music and it gets leaked, you could figure out who exactly leaked it.

If you were a company that sells stock photos, you could be able to find out who is using your images without permission.

If you were a law firm and someone leaked evidence in a case, you could know exactly who leaked them.

And much much more...
