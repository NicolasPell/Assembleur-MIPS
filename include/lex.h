/**
 * @file lex.h
 * @author François Portet <francois.portet@imag.fr>
 * @brief Lexem-related stuff.
 *
 * Contains lexem types definitions, some low-level syntax error codes,
 * the lexem structure definition and the associated prototypes.
 */

#ifndef _LEX_H_
#define _LEX_H_

#include <stdio.h>


typedef struct lexeme {
    int type;
    char strlex[50];
    int ligne;
}LEXEME;

struct liste_lex{
	LEXEME lex;
	struct liste_lex* suiv;
};

typedef struct liste_lex* LISTE_LEX;


/* Prototypes des fonctions */
int lex_load_file( char *file, unsigned int *nlines );
int lex_read_line( char *line, int nline, FILE* plex_bilan, LISTE_LEX liste_lexeme);
void lex_standardise(char* in, char* out );
char* whois(int S);
int automate(char* c);
int erreur_caractere();



#endif /* _LEX_H_ */

