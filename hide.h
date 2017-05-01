#ifndef HIDE_H
#define HIDE_H

void close_program(FILE *fp, FILE *wp, char *message);

int encode(int orig, int msg, int j);

int validate_properties(int *w, int *h, int *ch);

void parse_header(FILE *fp, FILE *wp, int *w, int *h, int *ch);

#endif