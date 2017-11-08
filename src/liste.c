
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






LISTE_LEX creer_liste(void) {
    LISTE_LEX p = calloc(1, sizeof(*p));
    return p;
}

int est_vide(LISTE_LEX l) {
    return !l;
}

void visualiser_liste(LISTE_LEX l) {
    LISTE_LEX p=l;
    int i=1;
    puts("\n\n*** Liste de lexemes : ***\n");
    while(p!=NULL) {
        printf("Lexeme %d :\n",i);
        printf("\t\t%d\t %s \t%d\n\n", p->lex.type,p->lex.strlex, p->lex.ligne);
        p=p->suiv;
        i++;
    }
}

LISTE_LEX ajout_tete(LEXEME lex1, LISTE_LEX l) {
    LISTE_LEX p = NULL;
    p = calloc(1, sizeof(*p));
    p->lex = lex1;
    p->suiv=l;
    return p;
}


LISTE_LEX supprimer_tete(LISTE_LEX l) {
    if(est_vide(l)) {
        return NULL;
    }
    else {
        LISTE_LEX p;
        p = l->suiv;
        free(l);
        return p;
    }

}

LISTE_LEX ajout_queue(LEXEME lex1,LISTE_LEX l) {
    LISTE_LEX p = calloc(1, sizeof(*p));
    p->lex = lex1;
    p->suiv = NULL;
    if(est_vide(l)) {
        return p;
    }
    else {
        LISTE_LEX tete_de_l = l;
        while(!est_vide(l->suiv)) {
            l = l->suiv;
        }
        l->suiv = p;
        return tete_de_l;
    }
}

LISTE_LEX supprime(int n, LISTE_LEX l) {
    int i;
    LISTE_LEX p = l;
    LISTE_LEX temp = NULL;
    if(est_vide(l)) {
        return l;
    }
    if(n==1) {
        return supprimer_tete(l);
    }

    for(i=0; i<n-2 && !est_vide(p-> suiv); i++) {
        p = p->suiv;
    }

    if(p->suiv == NULL) {
        printf("Pas d'élement\n");
        return l;
    }

    else {
        temp = p->suiv;
        p->suiv = temp->suiv;
    }
    free(temp);
    return l;
}


LISTE_LEX copie(LISTE_LEX l) {

    /*la fin de la liste copie */
    LISTE_LEX copie = creer_liste();

    if(est_vide(l)) {
        return NULL;
    }

    while(l->suiv != NULL) {
        /* A chaque itération : */
        LEXEME lex1 = l->lex;
        copie = ajout_queue(lex1,copie);
        copie = copie->suiv;
        l = l->suiv;
    }
    LEXEME lex1 = l->lex;
    copie = ajout_queue(lex1,copie);
    copie ->suiv = NULL;
    return copie;
}


LISTE_LEX concat(LISTE_LEX l1, LISTE_LEX l2) {

    LISTE_LEX p = creer_liste();
    p->suiv = l1;
    LISTE_LEX tete = p;

    while( (l1->suiv) != NULL) {
        p->lex=l1->lex;
        l1 = l1->suiv;
        p = p->suiv;
    }
    p -> suiv= l2;
    return(p);
}


