/*
 * Part 2:    unhide.c
 * author:    Yifei (Freya) Gao
 * startDate: 23 Mar 2017
 * modified:  21 Apr 2017
This "unhide" program takes one command line parameter:
1. name of input file in .PPM format to be decoded
It unhides a message that has been encoded inside a PPM file using the program hide.c, and prints it to stdout.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "unhide.h"

/// \brief frees dynamically allocated variable, closes opened files, and exits with an error.
/// \param fp - input file
/// \param message - dynamically allocated memory to store the decoded message
void close_program(FILE *fp, char *message) {
    free(message);
    fclose(fp);
    exit(EXIT_FAILURE);
}

/// \brief returns the last bit (0 or 1) in c
/// \param c byte to read least significant bit from
/// \return least significant bit of c
int decode(int c) {
    int msg_bit;
    msg_bit = c & 1;  // mask all bits other than least sig bit
    return msg_bit;
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
        return 0;
    }
    else {
        return 1;
    }
}

/// \brief validates the header of the ppm file
/// \param fp - input file pointer
/// \param w - width of picture
/// \param h - height of picture
void read_header(FILE *fp, int *w, int *h, int *ch) {
    // if magic number isn't P6, exit
    char mn[10];
    fscanf(fp, "%s ", mn);
    if ( strcmp( mn, "P6" ) != 0 )
    {
        fprintf(stderr, "File type is not P6.\n");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

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
                    exit(EXIT_FAILURE);
                }
            }
            if ( headerCounter == 3)
            {
                if (fscanf(fp, "%d ", ch) != 1) {
                    fprintf(stderr, "Error parsing header.\n");
                    fclose(fp);
                    exit(EXIT_FAILURE);
                }
            }
        }
        else    // found '#', skip line
        {
            while (c != '\n')
            {
                c = fgetc(fp);
            }
        }
    }
}

/// \brief Reads an encoded ppm file and prints out the hidden message inside it.
int main (int argc, char *argv[]) {
    FILE *fp;

    // if did not receive at least 1 argument, exit.
    if ( argc != 2 )
    {
        fprintf(stderr, "Please provide one arguments: input file name.\n");
        exit(EXIT_FAILURE);
    }
    // open file if granted permission to read file
    if (access(argv[1], R_OK) != -1) {
        fp = fopen(argv[1], "r");
    } else {
        fprintf(stderr, "Error: Cannot open file %s.\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    // parse header
    int w = -1, h = -1, ch = -1;
    read_header(fp, &w, &h, &ch);

    // validate image dimension and channel number.
    if (validate_properties(&w, &h, &ch) == 0)
    {
        fprintf(stderr, "Inappropriate input PPM file.");
        fclose(fp);
        exit(EXIT_FAILURE);
    }


    // calculate image size
    int img_size = w * h * 3;

    // allocate space for hidden message
    char *message = (char *) malloc(img_size/8);
    // check if malloc() succeeded
    if (message == NULL) {
        fprintf(stderr, "Error: failed to allocate memory to store message.");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    // process the body of the image
    int c;
    int msg_byte;
    char msg_char;
    int msg_bit;
    int message_found = 0;
    for (int i=0; i<img_size/8; i++)  // one loop completes one byte of message
    {
        if (message_found == 1)
        {
            break;
        }
        // 1. if hit EOF (early): show error
        // 2. decode every 8 bytes
        // 3. stop when message ends (any decoded byte == EOF)
        msg_byte = 0;
        for (int j=0; j<8; j++)
        {
            if ( ( c = fgetc(fp) ) == EOF )    // file ends earlier than intended
            {
                fprintf(stderr, "Input file has invalid dimensions.\n");
                close_program(fp, message);
            }
            msg_byte = msg_byte << 1;  // shift message byte once
            msg_bit = decode(c);    // decode byte
            msg_byte = msg_byte | msg_bit;   // save bit to byte
        }
        if (msg_byte == 255) // found EOF encoded
        {
            message_found = 1;
//            msg_char = msg_byte;
//            message[i] = msg_char;
        }
        else
        {
            msg_char = msg_byte;
            message[i] = msg_char;
        }
    }

    fclose(fp);

    // test:

    FILE *test;
    test = fopen("test.txt", "w");
    for (int i=0; i<strlen(message); i++) {
        fputc(message[i], test);
    }
    fclose(test);


    if (message_found == 0)
    {
        printf("\n%s\n", message);
        free(message);
        fprintf(stderr, "Message could not be found.\n");   // something went wrong
        exit(EXIT_FAILURE);
    }
    else
    {
        fprintf(stdout, "Decoded message in %s is:\n", argv[1]);
        printf("%s\n", message);

        free(message);
        return 0;
    }

}