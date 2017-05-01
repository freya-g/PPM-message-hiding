# Message Hiding and Unhiding in PPM files
### Language: C
### By Yifei (Freya) Gao


This directory contains 2 programs: hide and unhide. The "hide" program takes a message from the standard input and encrypts it into the input PPM file, where the "unhide" program decodes and retrieves the message from the PPM file.


The two programs accept input files of standard PPM format (the magic number at the start of the file should be "P6"). If the user provides a standard "P3" PPM file, the program will return an error and terminate.
The programs are intended to process header in the following format:

P6

w h

255

where 'w' and 'h' are the width and height, and 255 is the max channel value of the PPM image. Multiple lines of comments starting with '#' can be inserted in between "P6" and "w h", and "w h" and "255". If width or height is less than 1, or if the max channel value is not "255", the input PPM file will be deemed invalid and the program will terminate.

- - - - -
### Compiling

To compile "hide" or "unhide, run one of the following commands on the program folder in the command line:
	make hide
	make unhide

Or to compile all programs:
	make

To return the folder to its original state and clear all executables, run the following command on the program folder in the command line:
	make clean

- - - - -

### To run "hide":
	./hide inputFile.ppm outputFile.ppm
where "inputFile.ppm" is the input PPM filename, and "outputFile.ppm" will be the output file after encoding the message inside. When the system prompts, enter the string you wish to encode into the image.


### To run "unhide":
	./unhide inputFile.ppm
where "inputFile.ppm" is the input filename which has a message encoded within it using "hide".

- - - - -

### Limitations:
1. The "hide" and "unhide" programs were specifically programmed to take the format of standard PPM file header mentioned above where it uses a counter to keep in track of the non-comment rows that has been processed. However, this also means variations of the header may be incorrectly processed, such as the case where all 3 values (w, h, and ch) are on the same line, separated by space.

2. The "hide" program starts writing to the output file at the same time as it reads through the input file. This uses less memory than writing everything to memory and then writing to the file in one go. However, if an error terminates the program before it finishes writing, the output file will be left incomplete, whereas the other method will make sure the message can be successfully encoded before writing to file at all.

3. The "unhide" function cannot automatically detect if a message is encoded inside the PPM file, and relies on the EOF character encoded by the "hide" program to correctly retrieve the message. Therefore, if "unhide" is used to run on a file without an encoded message, it may still produce incorrect output if it coincidentally finds EOF from the least significant bits.

Other conditions that will cause the program to terminate with appropriate error messages are:
- not enough command line arguments provided
- input file not found
- no standard input given for "hide"
- input message is longer than what is able to be fit into the PPM image in "hide"
- reaching EOF while encoding or decoding message due to inconsistency of header info and actual image size