#ifndef _COLLECT_H_
#define _COLLECT_H_

#include <stdio.h>
#include <lex.h>
#include <liste.h>

/* Tous les prototypes de fonctions n�cessaires */



/*on d�finit ici la collection d'instru */

/*
union union_classe_Op{
	char classe_SYMB[20];
	char classe_SA[10];
	char classe_REG[10];
  int classe_IMM[10];
  int classe_BAS[10];
  int classe_ABS[10];
  int classe_REL[10];
};
typedef union union_classe_Op union_classe_Op;


typedef struct instruction {
  char INSTR[4];
	int tag;
	int ligne;
	int decalage;
	union_classe_Op Operande;
} INSTRUCT;
*/

typedef struct instruction {
    LEXEME lex;
    int nb_operande;
    int ligne;
    int decalage;
    LISTE_LEX* Tab_Op[3];
} INSTRUCT;



struct liste_instruct {
    INSTRUCT instruction;
    struct liste_instruct* suiv;
};


typedef struct liste_instruct* LISTE_INSTRUCT;


/* on d�finit ici la collection de donn�es de la section data */

union union_Op{
	char SYMBOL[20]; /* tag = 1 */
	char chaine_asciiz[250]; /*apres un asciiz, tag = 2 */
	int decimal_data; /* apr�s un .word ou un .byte pour g�rer un nombre decimal dans la section data, tag = 3*/
	unsigned int decimal_bss; /* apr�s un .space, tag = 4*/
	char hexa_word[50]; /* apr�s un .word, tag = 5*/
	char hexa_byte[50]; /* apr�s un .byte, tag = 6 */
};
typedef union union_Op union_Op;


typedef struct data{
	char DIR[20];
	int tag;
	int ligne;
	int decalage;
	union_Op Operande;
} DATA;




struct liste_data {
    DATA data;
    struct liste_data* suiv;
};

typedef struct liste_data* LISTE_DATA;


/* on d�finit ici la collection de donn�es de la section bss */

typedef struct BSS{
	char DIR[20];
	int tag;
	int ligne;
	int decalage;
	union_Op Operande;
} BSS;

struct liste_bss {
    BSS bss;
    struct liste_bss* suiv;
};

typedef struct liste_bss* LISTE_BSS;


#endif /* _COLLECT_H_ */
