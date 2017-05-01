/*
 * Part 1:    hide.c
 * author:    Yifei (Freya) Gao
 * startDate: 14 Mar 2017
 * modified:  21 Apr 2017
This "hide" program takes two command line parameters:
1. name of input file in .PPM format
2. name of output file
It hides a user input message from stdin and encodes the message inside a PPM file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hide.h"

/// \brief frees dynamically allocated variable, closes opened files, and exits with an error.
/// \param fp - input file
/// \param wp - output file
/// \param message - dynamically allocated memory to store the decoded message
void close_program(FILE *fp, FILE *wp, char *message)
{
    free(message);
    fclose(fp);
    fclose(wp);
    exit(EXIT_FAILURE);
}

/// \brief modifies original byte by encoding 1 bit of message byte into it.
/// \param orig - byte from input ppm file to hide the message bit
/// \param msg - byte of message to be encoded
/// \param j - indicates the jth right
/// \return the modified ppm byte with 1 bit of message encoded in it
int encode(int orig, int msg, int j) {
    int newchar;
    orig = orig & 254;  // remove least significant bit
    msg = msg >> (7-j); // shift message byte such that desired bit is the rightmost bit
    msg = msg & 1;  // mask all bits other than least sig bit
    newchar = orig | msg;
    return newchar;
}

/// \brief validates if properties inside header of PPM image are valid
/// \param w - width of picture
/// \param h - height of picture
/// \param ch - max channel value
/// \return 0 if properties are valid, and -1 if otherwise
int validate_properties(int *w, int *h, int *ch)
{
    if ((*w < 1) | (*h < 1) | (*ch != 255))
    {
        fprintf(stderr, "Inappropriate input PPM file.");
        return -1;
    }
    else {
        return 0;
    }
}

/// \brief validates the header of the ppm file, writes header to output file stores variables w, h
/// \param fp - input file pointer
/// \param wp - output file pointer
/// \param w - width of picture
/// \param h - height of picture
/// \param ch - channel number of picture
void parse_header(FILE *fp, FILE *wp, int *w, int *h, int *ch) {
    // if magic number isn't P6, exit
    char mn[10];
    fscanf(fp, "%s ", mn);
    if ( strcmp( mn, "P6" ) != 0 )
    {
        fprintf(stderr, "File type is not P6.\n");
        fclose(fp);
        fclose(wp);
        exit(EXIT_FAILURE);
    }
    fprintf(wp, "%s\n", mn);

    // validate rest of header
    int c;
    for ( int headerCounter = 1; headerCounter < 3; )
    {
        // check for '#'
        c = fgetc(fp);
        fseek(fp, -1, SEEK_CUR);
        if (c != '#')
        {
            headerCounter++;
            if ( headerCounter == 2)
            {
                if (fscanf(fp, "%d %d", w, h) != 2) {
                    fprintf(stderr, "Error parsing header.\n");
                    fclose(fp);
                    fclose(wp);
                    exit(EXIT_FAILURE);
                }
//                fscanf(fp, "%d %d", w, h);
                fprintf(wp, "%d ", *w);
                fprintf(wp, "%d\n", *h);
            }
            if ( headerCounter == 3)
            {
                if (fscanf(fp, "%d ", ch) != 1) {
                    fprintf(stderr, "Error parsing header.\n");
                    fclose(fp);
                    fclose(wp);
                    exit(EXIT_FAILURE);
                }
//                fscanf(fp, "%d ", ch);
                fprintf(wp, "%d\n", *ch);
            }
        }
        else    // found '#', write everything to file
        {
            while (c != '\n')
            {
                c = fgetc(fp);
                fputc(c, wp);
            }
        }
    }
}

/// \brief Reads a ppm file, reads a string of user input message,
/// and produces an output ppm file where the message is encoded into the input file.
int main (int argc, char *argv[]) {

    FILE *fp;
    FILE *wp;

    // if did not receive exactly 2 arguments, exit.
    if ( argc != 3 )
    {
        fprintf(stderr, "Please provide at least two arguments: input and output file names.\n");
        exit(EXIT_FAILURE);
    }

    // open file if granted permission to read file
    if (access(argv[1], R_OK) != -1) {
        fp = fopen(argv[1], "r");
    } else {
        fprintf(stderr, "Error: Cannot open file %s.\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    // access() checks permission to write file
    if (access(argv[2], W_OK) != -1) {
        wp = fopen(argv[2], "w");
    } else {
        fprintf(stderr, "Error: Cannot create file %s.\n", argv[2]);
        exit(EXIT_FAILURE);
    }

    // parse header
    int w = -1, h = -1, ch = -1;
    parse_header(fp, wp, &w, &h, &ch);


    // validate image dimension and channel number.
    if (validate_properties(&w, &h, &ch) == -1)
    {
        fprintf(stderr, "Inappropriate input PPM file.");
        fclose(fp);
        fclose(wp);
        exit(EXIT_FAILURE);
    }

    // allocate and read message from input.
    int img_size = w * h * 3;
    char *message = (char *) malloc(img_size/8);
    // check if malloc() succeeded
    if (message == NULL) {
        fprintf(stderr, "Error: failed to allocate memory to store message.");
        fclose(fp);
        fclose(wp);
        exit(EXIT_FAILURE);
    }

    printf("Please input your message to be encoded, ending with EOF: \n");
//    fgets(message, img_size-8, stdin);
    int c = 0;  // variable to store each char
    int i = 0;  // index in the message
    while (c != EOF) {
        if (i >= img_size/8) {
            // throw an error when user types past their size limit
            fprintf(stderr, "Message is too long to be encoded into the given file.\n");
            close_program(fp, wp, message);
        }
        c = fgetc(stdin);
        message[i] = c;
        i++;
    }

    // process the body of the image
    // i = ith byte in message
    int m;  // m contains 1 byte of message
//    int c;
    int c_mod;  // modified c
    for (int i=0; i<strlen(message); i++)   // each byte gets one loop
    {
        m = message[i];

        for (int j=0; j<8; j++)
        {
            if ( ( c = fgetc(fp) ) == EOF )
            {
                fprintf(stderr, "Input file has invalid dimensions.\n");
                close_program(fp, wp, message);
            }
            c_mod = encode(c, m, j);
            fputc(c_mod, wp);
        }
    }

    // write rest of all bytes to output file
    while ( ( c = fgetc(fp) ) != EOF )
    {
        fputc(c, wp);
    }


    free(message);
    fclose(fp);
    fclose(wp);
    fprintf(stdout, "\n\nMessage has been successfully encoded into %s.\n\n", argv[1]);
    return 0;
}