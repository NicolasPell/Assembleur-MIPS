
/**
 * @file lex.c
 * @author François Portet <francois.portet@imag.fr>
 * @brief Lexical analysis routines for MIPS assembly syntax.
 *
 * These routines perform the analysis of the lexeme of an assembly source code file.
 */

#define _POSIX_C_SOURCE 200112L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>
#include<stdio.h>
#include<stdlib.h>

#define STRLEN 256

#include <global.h>
#include <notify.h>
#include <lex.h>

/**0
 * @param line String of the line of source code to be analysed.
 * @param nline the line number in the source code.
 * @return should return the collection of lexemes that represent the input line of source code.
 * @brief This function performs lexical analysis of one standardized line.
 *
 */

enum {START, initTEXT, initBSS, initDATA, VIRGULE, DEB_REG, REG, SYMBOL, DEUX_POINTS, DECIMAL_ZERO, HEXA, DIREC, DECIMAL, PARENTHESE, COMMENT, ETIQUETTE, INSTRUCTION, etiquetteDATA,lexDATA, wordDATA, asciizDATA, spaceDATA, byteDATA, change_sectionDATA, ERREUR};

/* Structure
typedef struct lexeme {
    int type;
    char* strlex;
    int ligne;
}LEXEME;

struct liste_lex{
	LEXEME lex;
	struct liste_lex* suiv;
};

typedef struct liste_lex* LISTE_LEX;

*/


/* Prototypes des fonctions */

int lex_load_file( char *file, unsigned int *nlines );
int lex_read_line( char *line, int nline, FILE* plex_bilan, LISTE_LEX liste_lexeme);
void lex_standardise(char* in, char* out );
char* whois(int S);
int automate(char* c);
int erreur_caractere();



/* Prototypes de fonctions annexes sur les listes chaînées */
LISTE_LEX creer_liste(void);
int est_vide(LISTE_LEX l);
void visualiser_liste(LISTE_LEX l);
LISTE_LEX ajout_tete(LEXEME lex, LISTE_LEX l);
LISTE_LEX supprimer_tete(LISTE_LEX l);
LISTE_LEX ajout_queue(LEXEME lex, LISTE_LEX l);
LISTE_LEX supprime(int n, LISTE_LEX l);
LISTE_LEX copie(LISTE_LEX l);
LISTE_LEX concat(LISTE_LEX l1, LISTE_LEX l2);
LEXEME creer_lexeme(void);



int lex_read_line( char *line, int nline, FILE* plex_bilan, LISTE_LEX liste_lexeme) {

    char *seps = " \n\t\r";
    char* token;
    char save[STRLEN];
    char* S_state= "";

    /* copy the input line so that we can do anything with it without impacting outside world*/
    memset( save, 0, STRLEN );
    memcpy( save, line, STRLEN );

    int S=0;        /* valeur int de l'état automate
    char* S_state;  /* etat correspondant à la valeur automate
    /* get each token*/
    for( token = strtok( line, seps ); NULL != token; token = strtok( NULL, seps )) {
        /* TODO : faire l'analyse lexicale de chaque token ici et les ajouter dans une collection*/
        fprintf( stderr, "TOKEN: '%s'\n", token );
        S = automate(token);
        /* Cette partie la ne fonctionne pas: Que faire?*/
        if (S != COMMENT) {
            LEXEME lex;
            sprintf(lex.strlex, "%s", token);
            /* Toute cette partie ce passe sans aucun soucis: */

            S_state = whois(S);
            fprintf(plex_bilan,"[%s]: %s a la ligne %d\n",S_state,token,nline);     /* écrit la collection de lexemes après analyse lexicale dans un fichier lex_bilan voir PDF */
            lex.type=S;
            lex.ligne=nline;
            ajout_queue(lex, liste_lexeme);
        }
    }
    /*getchar();*/
    return 1;
}

/**
 * @param file Assembly source code file name.
 * @param nlines Pointer to the number of lines in the file.
 * @return should return the collection of lexemes
 * @brief This function loads an assembly code from a file into memory.
 *
 */




int lex_load_file( char *file, unsigned int *nlines) {

    FILE*        fp   = NULL;
    char         line[STRLEN]; /* original source line */
    char         res[2*STRLEN]; /* standardised source line, can be longeur due to some possible added spaces*/

    FILE* plex_bilan;
    plex_bilan=fopen("./bilan.txt", "w+");
    LISTE_LEX liste_lexeme = creer_liste();
    LISTE_LEX tete_liste = creer_liste();
    tete_liste = liste_lexeme;
    if(plex_bilan == NULL) {
        printf("Erreur ouverture fichier bilan \n");
        return 0;
    }

    fp = fopen( file, "r" );
    if ( NULL == fp ) {
        printf("erreur à l'ouverture du fichier code_source \n");
        /*macro ERROR_MSG : message d'erreur puis fin de programme ! */
        /*ERROR_MSG("Error while trying to open %s file --- Aborts",file); */
        return 0;
    }

    *nlines = 0;

    while(!feof(fp)) {

        /*read source code line-by-line */
        if ( NULL != fgets( line, STRLEN-1, fp ) ) {
            line[strlen(line)-1] = '\0';  /* eat final '\n' */
            (*nlines)++;

            if ( 0 != strlen(line) ) {
                memset( res, 0, 2*STRLEN );
                lex_standardise( line, res );
                printf("\n\n\n Lecture de la ligne: %d \n\n\n",*nlines);
                lex_read_line(res,*nlines,plex_bilan,liste_lexeme);
            }
        }
    }
    liste_lexeme = liste_lexeme->suiv;
    visualiser_liste(liste_lexeme);
    int err =0 ;
    err = automate_init(liste_lexeme);
    printf("Valeur erreur = %d\n",err);

    fclose(fp);
    fclose(plex_bilan);
}



/**
 * @param in Input line of source code (possibly very badly written).
 * @param out Line of source code in a suitable form for further analysis.
 * @return nothing
 * @brief This function will prepare a line of source code for further analysis.
 */

/* note that MIPS assembly supports distinctions between lower and upper case*/

void lex_standardise(char* in,char* out ) {

    int i, j;

    /*char in[]= ".text   ADDI $2 ,  $4 ,-   6 ";
    char out[100]="";
    int compt = 0;      compteur sur i
    int compt2 = 0;     compteur sur j */


    for ( i= 0, j= 0; i< strlen(in); i++ ) {
        if(i!=strlen(in)-1) {
            while ((in[i] == ' ' && in [i+1] == ',') || (in[i] == ' ' && in [i+1] == ' ') || (in[i]== ' ' && in[i+1] == ':') || (in[i]== ' ' && in[i+1] == '.')) {
                i++;
            }
        }

        if(in[i] == ',') {				/*transforme les virgules selon la règle " , " */
            out[j]= ' ' ;
            out[j+1]=',' ;
            out[j+2]=' ' ;
            j = j+3 ;
            while (in[i+1] == ' ') {
                i++;
            }

        }

        else if((in[i] == ':')||(in[i] == '(')||(in[i] == ')')) {		/*transforme les deux points selon la règle " : " */
            out[j]= ' ' ;
            out[j+1]=in[i] ;
            out[j+2]=' ' ;
            j = j+3 ;
            while (in[i+1] == ' ') {
                i++;
            }

        }

        else if( in[i] == '.') {		/*transforme les points selon la règle ".text" */
            out[j]= '.' ;
            j++;
            while (in[i+1] == ' ') {
                i++;
            }

        }

        else if (in[i] == '-' && in[i+1] == ' ') {
            out[j] = '-';
            i++;
            j++;
            while (in[i+1] == ' ') {
                i++;
            }
        }
        else if (in[i] == '#') {
            while(i!= strlen(in)) {
                if ((in[i]) == ' ') { /* on remplace le caractère espace par un tiret du 8 */
                    out[j] = '_';
                }

                else {
                    out[j] = in [i];
                }
                i++;
                j++;
            }
            out[j] = '\0';
        }

        else {
            /* translate all spaces (i.e., tab) into simple spaces*/
            if (isblank((int) in[i])) {
                out[j]=' ';
                j++;
            }
            /*si aucun des cas précedents, copie les caractères de in vers out */
            else {
                out[j] = in[i];
                j++;
            }
        }
    }
    /*terminaison de la chaîne
    /*
    printf("chaine:%s\n",out);
    printf("longueur out  : %d\n",strlen(out));

    /*compteur de blancs (faible lisibilité sur la console)
    int k,l;
    for(k=0;k<strlen(out);k++)
    {
        if (out[k] == ' ') {
            compt++;
        }
    }
    for (l=0;l<strlen(in);l++){
        if (in[l] == ' ') {
            compt2++;
        }

    }
    printf("Nombre de blanks dans in : %d\n", compt2);
    printf("Nombre de blanks dans out : %d\n", compt); */

}

int automate(char* c) {
    int N = strlen(c);
    int S = START;
    int i=0;
    /* attention lorsque l'on va utiliser cette fonction penser à supprimer les \0 dans le fichier car sinon
    le pointeur c tombe sur le caractère antislash et ne repère pas qu'il s'agit d'un
    antislash-zéro (fin de string), ceci a été vérifié de nombreuses fois */

    for(i=0; i<N; i++) {
        switch(S) {

        case START:

            if (isspace(c[i])) {
                S = START;
            }
            else if (isdigit(c[i])) {
                S = (c[i]=='0')? DECIMAL_ZERO : DECIMAL ;
            }
            else if (isalpha(c[i])) {
                S=SYMBOL;
            }
            else if (c[i] == ',') {
                S=VIRGULE;
            }
            else if (c[i] == '.') {
                S=DIREC;
            }
            else if (c[i] == '0') {
                S=DECIMAL_ZERO;
            }
            else if (c[i] == ':') {
                S=DEUX_POINTS;
            }
            else if (c[i] == '-') {
                break;
            }
            else if (c[i] == '$') {
                S=REG;
            }
            else if (c[i] == '\0') {
                break;
            }
            else if (c[i] == '#') {
                S= COMMENT;
            }
            else if ((c[i] == '(')||(c[i] == ')')) {
                S=PARENTHESE;
            }
            else if (c[i] == '\"') {
                break;
            }
            else if (c[i] == '-') {
                break;
            }
            else {
                S=ERREUR;
            }
            break;
        case VIRGULE: /*on casse immédiatement dans le cas d'une virgule, il n'y a rien à tester */
            break ;
        case REG:
            if (isalnum(c[i])) {
                S=REG;
            }
            else {
                return erreur_caractere();
            }
            break;
        case SYMBOL:
            /* changement ici */
            if (isalnum(c[i])) {
                return(S);
            }
            else {
                return erreur_caractere();
            }
            break;
        case DEUX_POINTS:
            break;
        case DECIMAL_ZERO:
            if ( c[i] == 'x' || c[i] == 'X') {
                S = HEXA;
            }
            else {
                return erreur_caractere();
            }
            break;
        case DECIMAL:
            if (isdigit(c[i])) {
                S = DECIMAL;
            }
            else {
                return erreur_caractere();
            }
            break;
        case HEXA:
            if (isxdigit(c[i])||c[i] == '('|| c[i] == ')' || c[i] == '$') S = HEXA ;
            else {
                return erreur_caractere();
            }
            break;
        case DIREC:
            if(isalpha(c[i])) {   /*une directive ne contient que des caractères alphab */
                S = DIREC ;
            }
            else {
                return erreur_caractere();
            };
            break;
        case COMMENT: /*rien de particulier à vérifier dans le cas d'un commentaire */
            break;
        case PARENTHESE:
            break;
        case ERREUR:
            return erreur_caractere();
            break;
        }
    }
    return(S);
}

int erreur_caractere() {
    /* printf("Erreur lecture caractere '%c' dans la chaine '%s' ligne %d", c,s,i); */
    return(11);
}

char* whois(int S) {
    char* blabla;
    if (S==START) {
        blabla = "START  ";
        return blabla ;
    }
    if (S==VIRGULE) {
        blabla = "VIRGULE  ";
        return blabla ;
    }
    if (S==REG) {
        blabla ="REG  ";
        return blabla ;
    }
    if (S==SYMBOL) {
        blabla = "SYMBOL  ";
        return blabla ;
    }
    if (S==DEUX_POINTS) {
        blabla = "DEUX_POINTS  ";
        return blabla ;
    }
    if (S==DECIMAL_ZERO) {
        blabla = "DECIMAL_ZERO  ";
        return blabla ;
    }
    if (S==HEXA) {
        blabla = "HEXA  ";
        return blabla ;
    }
    if (S==DIREC) {
        blabla = "DIREC  ";
        return blabla ;
    }
    if (S==DECIMAL) {
        blabla = "DECIMAL  ";
        return blabla ;
    }
    if (S==COMMENT) {
        blabla = "COMMENT  ";
        return blabla;
    }
    if (S==ERREUR) {
        blabla = "ERREUR  ";
        return blabla ;
    }
}

/*void main() {

    LISTE_LEX liste1;
    liste1 = creer_liste();
    LISTE_LEX liste2;
    liste2 = creer_liste();

    LEXEME lex1 = {1,"text",1000};
    LEXEME lex2 = {2,"blabla",14500};
    LEXEME lex3 = {5,"tchoutchou",150};
    LEXEME lex4 = {6,"alalal",1500};
    LEXEME lex5 = {21,"si tu vois que moi...",13};

    liste1 = ajout_tete(lex1,liste1);
    liste1 = ajout_tete(lex2,liste1);
    visualiser_liste(liste1);
    puts("---Suppression de la tete---\n");
    liste1 = supprimer_tete(liste1);
    visualiser_liste(liste1);
    puts("---Ajout de lex2 et lex3 en queue---\n");
    liste1 = ajout_queue(lex2,liste1);
    liste1 = ajout_queue(lex3,liste1);
    liste1 = ajout_queue(lex5,liste1);
    visualiser_liste(liste1);
    puts("---Suppression du 2 e element---\n");
    liste1=supprime(2,liste1);
    visualiser_liste(liste1);
    liste2 = ajout_queue(lex3,liste2);
    liste2 = ajout_queue(lex4,liste2);
    visualiser_liste(liste2);

    LISTE_LEX liste3;
    liste3 = creer_liste();
    liste3 = concat(liste1,liste2);
    visualiser_liste(liste3);
} */
