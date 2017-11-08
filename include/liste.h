#ifndef _LISTE_H_
#define _LISTE_H_

#include <stdio.h>
#include "lex.h" 

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

#endif /* _LEX_H_ */
