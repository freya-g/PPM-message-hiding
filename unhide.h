#ifndef UNHIDE_H
#define UNHIDE_H

void close_program(FILE *fp, char *message);

int decode(int c);

int validate_properties(int *w, int *h, int *ch);

void read_header(FILE *fp, int *w, int *h, int *ch);

#endif