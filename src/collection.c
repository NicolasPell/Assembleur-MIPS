
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
#include <collection.h>
#include <lex.h>
#include <liste.h>



/* Toutes les fonctions pour la manipulation de la collection data */

LISTE_DATA creer_liste_data(void) {
    LISTE_DATA p = calloc(1, sizeof(*p));
    return p;
}

int est_vide_data(LISTE_DATA l) {
    return !l;
}

void visualiser_liste_data(LISTE_DATA l) {
    LISTE_DATA p=l;
    int i=1;
    puts("\n\n*** Liste de DATAs : ***\n");
    while(p!=NULL) {

        printf("DATA %d :\n",i);
        printf("\t\tdirective: %s\t decalage: %d \t a la ligne: %d", p->data.DIR,p->data.decalage, p->data.ligne);

        switch(p->data.tag) {

        case 1:
            printf("\tOperande de type SYMBOL: %s \n\n", p->data.Operande.SYMBOL);

            break;

        case 2:
            printf("\tOperande de type chaine_asciiz: %s \n\n", p->data.Operande.chaine_asciiz);
            break;

        case 3:
            printf("\tOperande de type decimal_data: %s \n\n", p->data.Operande.decimal_data);

            break;

        case 4:
            printf("\tOperande de type decimal_bss: %s \n\n", p->data.Operande.decimal_bss);

            break;

        case 5:
            printf("\tOperande de type hexa_word: %s \n\n", p->data.Operande.hexa_word);

            break;

        case 6:
            printf("\tOperande de type hexa_byte: %s \n\n", p->data.Operande.hexa_byte);

            break;

        }
        p=p->suiv;
        i++;
    }
}
LISTE_DATA ajout_tete_data(DATA data1, LISTE_DATA l) {
    LISTE_DATA p = NULL;
    p = calloc(1, sizeof(*p));
    p->data = data1;
    p->suiv=l;
    return p;
}

LISTE_DATA ajout_queue_data(DATA data1,LISTE_DATA l) {
    LISTE_DATA p = calloc(1, sizeof(*p));
    p->data = data1;
    p->suiv = NULL;
    if(est_vide_data(l)) {
        return p;
    }
    else {
        LISTE_DATA tete_de_l = l;
        while(!est_vide_data(l->suiv)) {
            l = l->suiv;
        }
        l->suiv = p;
        return tete_de_l;
    }
}

LISTE_BSS creer_liste_bss(void) {
    LISTE_BSS p = calloc(1, sizeof(*p));
    return p;
}

int est_vide_bss(LISTE_BSS l) {
    return !l;
}

void visualiser_liste_bss(LISTE_BSS l) {
    LISTE_BSS p=l;
    int i=1;
    puts("\n\n*** Liste de BSSs : ***\n");
    while(p!=NULL) {
        printf("BSS %d :\n",i);
        printf("\t\tdirective: %s\t decalage: %d \t a la ligne: %d\n", p->bss.DIR,p->bss.decalage, p->bss.ligne);
        p=p->suiv;
        i++;
    }
}

LISTE_BSS ajout_tete_bss(BSS bss1, LISTE_BSS l) {
    LISTE_BSS p = NULL;
    p = calloc(1, sizeof(*p));
    p->bss = bss1;
    p->suiv=l;
    return p;
}

LISTE_BSS ajout_queue_bss(BSS BSS1, LISTE_BSS l) {
    LISTE_BSS p = calloc(1, sizeof(*p));
    p->bss = BSS1;
    p->suiv = NULL;
    if(est_vide_bss(l)) {
        return p;
    }
    else {
        LISTE_BSS tete_de_l = l;
        while(!est_vide_bss(l->suiv)) {
            l = l->suiv;
        }
        l->suiv = p;
        return tete_de_l;
    }
}

LISTE_INSTRUCT creer_liste_instruct(void) {
    LISTE_INSTRUCT p = calloc(1, sizeof(*p));
    return p;
}

int est_vide_instruct(LISTE_INSTRUCT l) {
    return !l;
}

void visualiser_liste_instruct(LISTE_INSTRUCT l) {
    LISTE_INSTRUCT p=l;
    int i=1;
    puts("\n\n*** Liste de INSTRUCTs : ***\n");
    while(p!=NULL) {
        printf("INSTRUCT %d :\n",i);
        printf("\t\tLexeme: %s\t decalage: %d\t  nb_op: %d\t a la ligne: %d", p->instruction.lex.strlex,p->instruction.decalage,p->instruction.nb_operande, p->instruction.ligne );
        p=p->suiv;
        i++;
    }
}

LISTE_INSTRUCT ajout_tete_instruct(INSTRUCT instruct1, LISTE_INSTRUCT l) {
    LISTE_INSTRUCT p = NULL;
    p = calloc(1, sizeof(*p));
    p->instruction = instruct1;
    p->suiv=l;
    return p;
}

LISTE_INSTRUCT ajout_queue_instruct(INSTRUCT instruct1, LISTE_INSTRUCT l) {
    LISTE_INSTRUCT p = calloc(1, sizeof(*p));
    p->instruction = instruct1;
    p->suiv = NULL;
    if(est_vide_instruct(l)) {
        return p;
    }
    else {
        LISTE_INSTRUCT tete_de_l = l;
        while(!est_vide_instruct(l->suiv)) {
            l = l->suiv;
        }
        l->suiv = p;
        return tete_de_l;
    }
}
