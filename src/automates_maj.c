
#define _POSIX_C_SOURCE 200112L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>
#include "liste.h"
#include "automates.h"
#include "collection.h"
#include "global.h"
#include "lex.h"
#include "notify.h"

#define si_pas_dernier if(liste->suiv!=NULL)
#define si_dernier if(liste->suiv==NULL)
#define avance liste=liste->suiv

enum {START, initTEXT, initBSS, initDATA, VIRGULE, DEB_REG, REG, SYMBOL, DEUX_POINTS, DECIMAL_ZERO, HEXA, DIREC, DECIMAL, PARENTHESE, COMMENT, ETIQUETTE, INSTRUCTION, etiquetteDATA,lexDATA, wordDATA, asciizDATA, spaceDATA, byteDATA, change_sectionDATA, spaceBSS, change_sectionBSS, etiquetteTEXT, deb_instructionTEXT, instructionTEXT, change_sectionTEXT, ERREUR};

/* test git */
/* AUTOMATE INIT */

int automate_init(LISTE_LEX liste) {

    int S = START;
    int memory_set=0;	/*vérifie que .set n'est pas dans une section et n'apparaît qu'une fois*/
    int decalage_bss=0;
    int decalage_data=0;
    int decalage_text=0;
    int decalage_global=0;
    int gestion_err = 0;
    int fin = 0;

    const char* word = ".word";
    const char* asciiz = ".asciiz";
    const char* bss = ".bss";
    const char* text = ".text";
    const char* data = ".data";
    const char* space = ".space";
    const char* byte = ".byte";
    const char* set = ".set";

    LISTE_DATA collection_data = creer_liste_data();
    LISTE_BSS collection_bss = creer_liste_bss();
    LISTE_INSTRUCT collection_instruct = creer_liste_instruct();

    /* creation de la table des symboles */

    char table_symb[200][50] ;

    /* memset de la table_symb */

    int i =0;
    for(i=0;i<200;i++){
    	strcpy(table_symb[i]," ");
    }

    DEBUG_MSG("Automate initial en cours ...\n");

    while(liste) {

        switch(S) {

        case START:

            if(gestion_err==1) {
                DEBUG_MSG("Erreur ligne %d\n",liste->lex.ligne);
                S=ERREUR;
                return(gestion_err);
            }

            else if(fin==1) {
                DEBUG_MSG("Fin de la liste ligne %d\n", liste->lex.ligne);
                DEBUG_MSG("Decalage global = %d\n",decalage_global);
                DEBUG_MSG("Decalage .data = %d\n",decalage_data);
                DEBUG_MSG("Decalage .bss = %d\n",decalage_bss);
                DEBUG_MSG("Decalage .text = %d\n",decalage_text);
                return(gestion_err);
            }

            else if(strcmp(liste->lex.strlex,bss)!=0 && strcmp(liste->lex.strlex,data)!=0 && strcmp(liste->lex.strlex,text)!=0 && strcmp(liste->lex.strlex,set)!=0) {
                DEBUG_MSG("Erreur : directive non définie dans une section ligne %d\n",liste->lex.ligne);
                gestion_err=1;
                return(gestion_err);
            }
            else if (liste->lex.type == COMMENT) {
                si_dernier{ DEBUG_MSG("Fin de la liste ligne %d\n", liste->lex.ligne); return (gestion_err);}
                else {
                    avance;
                    DEBUG_MSG("strlex = %s\n",liste->lex.strlex);
                }
            }
            else if (liste->lex.type == DIREC) {
                DEBUG_MSG("Entrée dans le if DIREC de automate initial\n");
                if (strcmp(liste->lex.strlex,bss)==0) {

                    liste = automate_bss(liste, collection_bss, table_symb, &gestion_err, &fin, &decalage_bss, &decalage_global, &memory_set);
                    if (gestion_err == 1) {
                        S=ERREUR;
                        return(gestion_err);
                    }
                    else {
                        S = START;
                    }
                }
                else if (strcmp(liste->lex.strlex,text)==0) {

                    liste = automate_text(liste, collection_instruct, table_symb, &gestion_err, &fin, &decalage_text,&decalage_global,&memory_set);
                    if (gestion_err == 1) {
                        S=ERREUR;
                        return(gestion_err);
                    }
                    else {
                        S = START;
                    }
                }
                else if (strcmp(liste->lex.strlex,data)==0) {

                    liste = automate_data(liste, collection_data, table_symb, &gestion_err, &fin, &decalage_data, &decalage_global, &memory_set);

                    if(gestion_err == 1) {
                        S=ERREUR;
                        return(gestion_err);
                    }
                    else {
                        S = START;
                    }
                }
                else if (strcmp(liste->lex.strlex,set)==0) {
                    si_pas_dernier {
                        /* test de .set noreorder */
                        avance;
                        DEBUG_MSG("strlex = %s\n",liste->lex.strlex);
                        if (strcmp(liste->lex.strlex,"noreorder")==0 && memory_set != 1) {
                            memory_set = 1;
                            S = START;
                        }
                        else {
                            DEBUG_MSG("Erreur directive .set : absence de noreorder ou plusieurs définitions ligne %d \n",liste->lex.ligne);
                            return(gestion_err) ;
                        }
                    }
                    else {
                        DEBUG_MSG("Erreur directive .set : absence de noreorder ligne %d \n",liste->lex.ligne);
                        gestion_err = 1;
                        return(gestion_err) ;
                    }

                }
            }
            else if (liste->suiv==NULL) {
                DEBUG_MSG("Fin de la liste ligne %d\n", liste->lex.ligne);
                return(gestion_err);
            }
            else {
                S=ERREUR;
                DEBUG_MSG("Erreur : directive inconnue ligne %d\n",liste->lex.ligne);
                gestion_err = 1;
                return(gestion_err);
            }
            break;
        }
    }
    return(gestion_err);
}











/* AUTOMATE DATA */

LISTE_LEX automate_data(LISTE_LEX liste,LISTE_DATA collection_data,char** table_symb,int* gestion_err, int* fin, int* decalage_data, int* decalage_global, int* memory_set) {

    const char* word = ".word";
    const char* asciiz = ".asciiz";
    const char* bss = ".bss";
    const char* text = ".text";
    const char* data = ".data";
    const char* space = ".space";
    const char* byte = ".byte";
    const char* set = ".set";
    int indi=0;
    DATA data1;

    char* ptr;
    long res;

    DEBUG_MSG("strlex = %s\n",liste->lex.strlex);

    si_dernier {
        DEBUG_MSG("Entrée dans automate data mais liste->suiv vide ligne %d\n",liste->lex.ligne);
        *gestion_err = 1;
        return (liste);
    }
    else {
        avance;
        DEBUG_MSG("strlex = %s\n",liste->lex.strlex);
    }

    int S = initDATA ;
    DEBUG_MSG("Je suis dans automate data ligne %d\n",liste->lex.ligne);

    while(liste) {

        switch(S) {

        case initDATA:

            DEBUG_MSG("Je suis dans initDATA\n");

            if (liste->lex.type == DIREC) {

                DEBUG_MSG("Je vais analyser la directive %s ligne %d\n",liste->lex.strlex,liste->lex.ligne);

                if(strcmp(liste->lex.strlex,word)==0) {
                    S=wordDATA;
                    break;
                }
                else if(strcmp(liste->lex.strlex,asciiz)==0) {
                    S=asciizDATA;
                    break;
                }
                else if(strcmp(liste->lex.strlex,space)==0) {
                    S=spaceDATA;
                    break;
                }
                else if(strcmp(liste->lex.strlex,byte)==0) {
                    S=byteDATA;
                    break;
                }
                /* si on change de section */
                else if(strcmp(liste->lex.strlex,text)==0) {
                    DEBUG_MSG("Nouvelle section .text ligne %d\n",liste->lex.ligne);
                    S=change_sectionDATA;
                    break;
                }
                else if(strcmp(liste->lex.strlex,data)==0) {

                    DEBUG_MSG("Nouvelle section .data ligne %d\n",liste->lex.ligne);
                    S=change_sectionDATA;
                    break;
                }
                else if(strcmp(liste->lex.strlex,bss)==0) {
                    DEBUG_MSG("Nouvelle section .bss ligne %d\n",liste->lex.ligne);
                    S=change_sectionDATA;
                    break;
                }
                else {
                    DEBUG_MSG("Erreur : directive invalide dans .data ligne %d\n",liste->lex.ligne);
                    S=ERREUR;
                    *gestion_err = 1;
                    return(liste);
                }
            }
            else if (strcmp(liste->lex.strlex,".set")==0) {
                si_pas_dernier {
                    avance;
                    DEBUG_MSG("strlex = %s\n",liste->lex.strlex);
                    if ( (strcmp(liste->lex.strlex,".noreorder")==0) && (*memory_set != 1) && (S!=initBSS) && (S!=initDATA) )	{
                        *memory_set = 1;
                        S = initDATA ;
                    }
                    else {
                        DEBUG_MSG("Error directive .set : missing noreorder or multiple .set or in section ligne %d\n",liste->lex.ligne);
                        *gestion_err=1;
                        return(gestion_err) ;

                    }
                }
            }

            else if (liste->lex.type == COMMENT) {
                S = COMMENT;
                break;
            }
            else if (liste->lex.type == SYMBOL) {
                si_dernier{ *fin=1; return (liste);}
                else {
                    if ((liste->suiv)->lex.type==DEUX_POINTS) {
                        S=etiquetteDATA;
                        break;
                    }
                    else {	/*test validité du lexeme */
                        if (strcmp(liste->lex.strlex,word)==0 || strcmp(liste->lex.strlex,space)==0 || strcmp(liste->lex.strlex,asciiz) || strcmp(liste->lex.strlex,byte)==0 ) {
                            S=lexDATA;
                            break;
                        }
                        else {
                            DEBUG_MSG("Erreur : absence deux points ou mauvaise directive dans .data ligne %d\n",liste->lex.ligne);
                            S=ERREUR;
                            *gestion_err = 1;
                            return(liste);
                        }
                    }
                }
            }

        case etiquetteDATA:



            remplissage_tab_symb(table_symb,liste->lex.strlex,&indi);
            indi = 0;
            getchar();
        		visualiser_table_symb(table_symb);getchar();

            avance;
            si_pas_dernier {
                avance;
                DEBUG_MSG("strlex = %s\n",liste->lex.strlex);
            }
            else {
                *fin=1;
                return(liste);
            }
            /*on teste la validité du lexeme suivant l'étiquette */
            if ( strcmp(liste->lex.strlex,word)==0 || strcmp(liste->lex.strlex,byte)==0 || strcmp(liste->lex.strlex,space)==0 || strcmp(liste->lex.strlex,asciiz)==0 ) {
                *decalage_data=*decalage_data+4;
                *decalage_global=*decalage_global+4;
                S= lexDATA;
                break;
            }
            /*ou alors on boucle si on retombe sur une étiquette */

            else if (liste->lex.type == SYMBOL) {
                si_dernier{ *fin=1; return (liste);}
                else {
                    if ((liste->suiv)->lex.type==DEUX_POINTS) {
                        S=etiquetteDATA;
                        break;
                    }
                    else {
                        DEBUG_MSG("Absence de : pour étiquette ligne %d\n",liste->lex.ligne);
                        S=ERREUR;
                        *gestion_err = 1;
                        return(liste);
                    }
                }
            }
            else {
                DEBUG_MSG("Erreur étiquette dans .data ligne %d\n",liste->lex.ligne);
                S=ERREUR;
                *gestion_err = 1;
                return(liste);
            }

        case COMMENT:
            si_pas_dernier{
                avance;
                DEBUG_MSG("strlex = %s\n",liste->lex.strlex);
                S = initDATA;
                break ;
            }
            else {
                *fin=1;
                return (liste);
            }

        case INSTRUCTION:  /*pas d'instruction en .data */
            DEBUG_MSG("Erreur présence instruction dans la section data ligne %d\n",liste->lex.ligne);
            S = ERREUR ;
            *gestion_err = 1;
            return(liste);

        case lexDATA:
            DEBUG_MSG("Je suis dans lexDATA ligne %d\n",liste->lex.ligne);
            if(strcmp(liste->lex.strlex,word)==0) {
                S=wordDATA;
                break;
            }
            else if(strcmp(liste->lex.strlex,asciiz)==0) {
                S=asciizDATA;
                break;
            }
            else if(strcmp(liste->
                           lex.strlex,space)==0) {
                S=spaceDATA;
                break;
            }
            else if(strcmp(liste->
                           lex.strlex,byte)==0) {
                S=byteDATA;
                break;
            }
            else {
                DEBUG_MSG("Lexeme invalide dans .data ligne %d\n",liste->lex.ligne);
                S=ERREUR;
                *gestion_err = 1;
                return(liste);
            }

        case wordDATA:

            DEBUG_MSG("Je suis dans wordDATA ligne %d\n",liste->lex.ligne);

            si_dernier{
                *fin=1;
                S=ERREUR;
                *gestion_err = 1;
                DEBUG_MSG(".word vide dans la section data ligne %d\n",liste->lex.ligne);
                return(liste);
            }
            else {
                int compt_virgule = 0;
                int compt_operande = 0;

                while(liste->suiv!=NULL) {
                    avance;
                    DEBUG_MSG("strlex = %s\n",liste->lex.strlex);

                    if(liste->lex.type==HEXA || liste->lex.type==DECIMAL || liste->lex.type==DECIMAL_ZERO || liste->lex.type==SYMBOL) {
                        compt_operande = compt_operande + 1;

                        /* Alignement du word */
                        if (*decalage_data % 4 != 0) {
                          int val;
                          val = 4 - (*decalage_data % 4);
                          *decalage_data += val ;   /* on incrémente jusqu'au prochain multiple de 4 */
                          *decalage_global+=val;

                          *decalage_data += 4 ;   /* chaque instruction est codée sur 4 bits pour .word */
                          *decalage_global+=4;
                        }
                        else {
                        *decalage_data +=4;
                        *decalage_global+=4;
                        }

                        DEBUG_MSG("decalage data = %d et decala_global = %d\n",*decalage_data, *decalage_global);

                        strcpy(data1.DIR,word);

                        printf("voici la directive copiee dans le data1: %s\n",data1.DIR);

                        /* ajout de la directive dans la structure data1 */
                        switch(liste->lex.type) {

                        case HEXA:
                            strcpy(data1.Operande.hexa_word,liste->lex.strlex);
                            data1.tag = 5;
                            break;

                        case SYMBOL:
                            strcpy(data1.Operande.SYMBOL,liste->lex.strlex);
                            data1.tag = 1;
                            break;

                        case DECIMAL:
                            data1.Operande.decimal_data=(int)liste->lex.strlex;
                            data1.tag = 3;
                            break;

                        case DECIMAL_ZERO:
                            data1.Operande.decimal_data=(int)liste->lex.strlex;
                            data1.tag = 3;
                            break;

                        }

                        data1.decalage = *decalage_data;
                        data1.ligne = liste->lex.ligne;
                        ajout_queue_data(data1,collection_data);
                        visualiser_liste_data(collection_data);

                        /* fin de l'ajout */

                        si_pas_dernier{
                            if((liste->suiv)->lex.type==VIRGULE) {
                                avance;
                                compt_virgule = compt_virgule + 1;
                                DEBUG_MSG("Compteur virgule = %d\n",compt_virgule);
                                si_pas_dernier{
                                    if((liste->suiv)->lex.type==HEXA || (liste->suiv)->lex.type==DECIMAL || (liste->suiv)->lex.type==DECIMAL_ZERO || (liste->suiv)->lex.type==SYMBOL) {

                                    }
                                    else{
                                        S=ERREUR;
                                        DEBUG_MSG("Erreur virgule flottante ligne %d\n",liste->lex.ligne);
                                        *gestion_err=1;
                                        return(liste);
                                    }

                                }
                                else {
                                    S=ERREUR;
                                    DEBUG_MSG("Erreur virgule flottante ligne %d\n",liste->lex.ligne);
                                    *gestion_err=1;
                                    return(liste);
                                }
                            }
                            else if((liste->suiv)->lex.type==DIREC) {
                                avance;
                                DEBUG_MSG("strlex = %s\n",liste->lex.strlex);
                                S=initDATA;
                                break;
                            }
                            else{
                                S=ERREUR;
                                *gestion_err = 1;
                                return(liste)	;
                            }
                        }
                        else {
                            *fin=1;
                            return(liste);
                        }
                    }
                    else {
                        DEBUG_MSG("Erreur opérande dans .word ligne %d\n",liste->lex.ligne);
                        S=ERREUR;
                        *gestion_err = 1;
                        return(liste);

                    }
                }
            }
            break;

        case asciizDATA:

            DEBUG_MSG("Je suis dans asciizDATA ligne %d\n",liste->lex.ligne);

            si_dernier{
                *fin=1;
                S=ERREUR;
                *gestion_err = 1;
                DEBUG_MSG(".asciiz vide dans la section data ligne %d\n",liste->lex.ligne);
                return(liste);
            }
            else {
                avance;
                DEBUG_MSG("strlex = %s\n",liste->lex.strlex);
                if(liste->lex.type==SYMBOL) {

                    while(liste->lex.type==SYMBOL) {
                        res = strlen( (liste->lex.strlex) +1 );
                        *decalage_data +=res;
                        *decalage_global+=res;

                        DEBUG_MSG("decalage data = %d et decala_global = %d\n",*decalage_data, *decalage_global);

                        strcpy(data1.DIR,asciiz);

                        printf("voici la directive copiee dans le data1: %s\n",data1.DIR);
                        switch(liste->lex.type) {

                        case HEXA:
                            strcpy(data1.Operande.hexa_word,liste->lex.strlex);
                            data1.tag = 5;
                            break;

                        case SYMBOL:
                            strcpy(data1.Operande.chaine_asciiz,liste->lex.strlex);
                            data1.tag = 2;
                            break;

                        case DECIMAL:
                            data1.Operande.decimal_data=(int)liste->lex.strlex;
                            data1.tag = 3;
                            break;

                        case DECIMAL_ZERO:
                            data1.Operande.decimal_data=(int)liste->lex.strlex;
                            data1.tag = 5;
                            break;

                        }

                        data1.ligne=liste->lex.ligne;
                        data1.decalage = *decalage_data;
                        ajout_queue_data(data1,collection_data);
                        visualiser_liste_data(collection_data);

                        si_dernier{
                            *fin = 1;
                            return(liste);
                        }
                        else {
                            avance;
                            DEBUG_MSG("strlex = %s\n",liste->lex.strlex);
                            while(liste->lex.type==VIRGULE && liste->suiv !=NULL && (liste->suiv)->lex.type==SYMBOL) {
                                avance;
                                DEBUG_MSG("strlex = %s\n",liste->lex.strlex);
                                res = strlen( (liste->lex.strlex) +1 );
                                *decalage_data +=res;
                                *decalage_global+=res;

                                DEBUG_MSG("decalage data = %d et decala_global = %d\n",*decalage_data, *decalage_global);

                                strcpy(data1.DIR,asciiz);

                                printf("voici la directive copiee dans le data1: %s\n",data1.DIR);
                                switch(liste->lex.type) {

                                case HEXA:
                                    strcpy(data1.Operande.hexa_word,liste->lex.strlex);
                                    data1.tag = 5;

                                    break;

                                case SYMBOL:
                                    strcpy(data1.Operande.chaine_asciiz,liste->lex.strlex);
                                    data1.tag = 2;
                                    break;

                                case DECIMAL:
                                    data1.Operande.decimal_data=(int)liste->lex.strlex;
                                    data1.tag = 3;
                                    break;

                                case DECIMAL_ZERO:
                                    data1.Operande.decimal_data=(int)liste->lex.strlex;
                                    data1.tag = 5;
                                    break;

                                }

                                data1.ligne=liste->lex.ligne;
                                data1.decalage = *decalage_data;
                                ajout_queue_data(data1,collection_data);
                                visualiser_liste_data(collection_data);
                                if(liste->suiv !=NULL) {
                                    avance;
                                    DEBUG_MSG("strlex = %s\n",liste->lex.strlex);
                                }
                                else {
                                    si_dernier{
                                        *fin = 1;
                                        return(liste);
                                    }
                                    DEBUG_MSG("Erreur: dans .asciiz virgule sans opérande ligne %d\n",liste->lex.ligne);
                                    S=ERREUR;
                                    *gestion_err=1;
                                    return(liste);
                                }
                            }
                            S=initDATA;
                            break;
                        }
                    }
                }
                else {
                    DEBUG_MSG("Erreur : mauvaise opérande pour .asciiz ligne %d\n",liste->lex.ligne);
                    S=ERREUR;
                    *gestion_err=1;
                    return(liste);
                }
            }
            break;

        case spaceDATA:      /*pe nser à vérif 1 seule opérande et si négatif */

            DEBUG_MSG("Je suis dans spaceDATA ligne %d\n",liste->lex.ligne);

            si_dernier{
                *fin=1;
                S=ERREUR;
                *gestion_err = 1;
                DEBUG_MSG(".space vide dans la section data ligne %d\n",liste->lex.ligne);
                return(liste);
            }
            else {
                avance;
                DEBUG_MSG("strlex = %s\n",liste->lex.strlex);
                while(liste->lex.type==HEXA || liste->lex.type==DECIMAL || liste->lex.type==DECIMAL_ZERO) {

                    if (liste->lex.type == HEXA) {
                        res = strtol(liste->lex.strlex,&ptr,16);

                        if (*ptr!='\0') {
                            printf("Hexa trop long pour strtol\n");
                            return(liste);
                        }
                        if (res < 0) {
                            *gestion_err = 1;
                            ERROR_MSG("Erreur conversion strtol");
                        }
                    }
                    else if (liste->lex.type == DECIMAL || liste->lex.type == DECIMAL_ZERO){
                        res = strtol(liste->lex.strlex,&ptr,10);
                        if (*ptr!='\0') {
                            printf("Decimal trop long pour strtol\n");
                            return(liste);
                        }
                        if (res < 0) {
                            *gestion_err = 1;
                            ERROR_MSG("Erreur conversion strtol");
                        }
                    }
                    else{
                      /* utiliser la table des symboles ici */
                    }

                    *decalage_data +=res;
                    *decalage_global+=res;

                    DEBUG_MSG("decalage data = %d et decala_global = %d\n",*decalage_data, *decalage_global);

                    strcpy(data1.DIR,space);

                    printf("voici la directive copiee dans le data1: %s\n",data1.DIR);

                    /* ajout de la directive dans la structure data1 */
                    switch(liste->lex.type) {

                    case HEXA:
                        strcpy(data1.Operande.hexa_word,liste->lex.strlex);
                        data1.tag = 5;
                        break;

                    case DECIMAL:
                        data1.Operande.decimal_data=(int)liste->lex.strlex;
                        data1.tag = 3;
                        break;

                    case DECIMAL_ZERO:
                        data1.Operande.decimal_data=(int)liste->lex.strlex;
                        data1.tag = 3;
                        break;
                    }
                    data1.decalage = *decalage_data;
                    data1.ligne = liste->lex.ligne;
                    ajout_queue_data(data1,collection_data);
                    visualiser_liste_data(collection_data);

                    /* fin de l'ajout */

                    si_pas_dernier{
                        avance;
                        DEBUG_MSG("strlex = %s\n",liste->lex.strlex);
                        if (liste->lex.type==VIRGULE) {
                            DEBUG_MSG("Erreur : multiples opérandes pour .space ligne %d\n",liste->lex.ligne);
                            S=ERREUR;
                            *gestion_err = 1;
                            return(liste);
                        }
                    }
                    else {
                        *fin = 1;
                        return (liste);
                    }
                }

                S=initDATA;
                break;
            }

        case byteDATA:

            DEBUG_MSG("Je suis dans byteDATA\n");
            si_dernier{
                DEBUG_MSG("Fin de liste\n");
                S=ERREUR;
                *gestion_err=1;
                DEBUG_MSG(".byte vide dans la section data ligne %d\n",liste->lex.ligne);
                return(liste);
            }
            else {
                int compt_virgule = 0;
                int compt_operande = 0;

                while(liste->suiv!=NULL) {
                    avance;
                    DEBUG_MSG("strlex = %s\n",liste->lex.strlex);

                    if(liste->lex.type==HEXA || liste->lex.type==DECIMAL || liste->lex.type==DECIMAL_ZERO || liste->lex.type==SYMBOL) {
                        compt_operande = compt_operande + 1;

                        if (liste->lex.type == HEXA) {
                            res = strtol(liste->lex.strlex,&ptr,16);
                            if (*ptr!='\0') {
                                printf("Hexa trop long pour strtol\n");
                                return(liste);
                            }
                        }
                        else {
                            res = strtol(liste->lex.strlex,&ptr,10);
                            if (*ptr!='\0') {
                                printf("Decimal trop long pour strtol\n");
                                return(liste);
                            }
                        }
                        /* test de la taille de l'operande censée être codée sur 1 octet */
                        if(res>255 || res<0) {
                            *gestion_err = 1;
                            ERROR_MSG("Operande of .byte out of boundaries ligne %d - expected size of byte\n", liste->lex.ligne);
                            return(liste);
                        }

                        *decalage_data +=res;
                        *decalage_global+=res;

                        DEBUG_MSG("decalage data = %d et decala_global = %d\n",*decalage_data, *decalage_global);

                        strcpy(data1.DIR,byte);

                        printf("voici la directive copier dans le data1: %s\n",data1.DIR);
                        data1.ligne=liste->lex.ligne;
                        data1.decalage = *decalage_data;

                        switch(liste->lex.type) {

                        case HEXA:
                            strcpy(data1.Operande.hexa_byte,liste->lex.strlex);
                            data1.tag = 6;
                            break;

                        case SYMBOL:
                            strcpy(data1.Operande.SYMBOL,liste->lex.strlex);
                            data1.tag = 1;
                            break;

                        case DECIMAL:
                            data1.Operande.decimal_data=(int)liste->lex.strlex;
                            data1.tag = 3;
                            break;

                        case DECIMAL_ZERO:
                            data1.Operande.decimal_data=(int)liste->lex.strlex;
                            data1.tag = 3;
                            break;

                        }

                        ajout_queue_data(data1,collection_data);
                        visualiser_liste_data(collection_data);
                        /* fin de l'ajout */

                        si_pas_dernier{
                            if((liste->suiv)->lex.type==VIRGULE) {
                                avance;
                                DEBUG_MSG("strlex = %s\n",liste->lex.strlex);
                                compt_virgule = compt_virgule + 1;
                                si_pas_dernier{
                                    if((liste->suiv)->lex.type==HEXA || (liste->suiv)->lex.type==DECIMAL || (liste->suiv)->lex.type==DECIMAL_ZERO || (liste->suiv)->lex.type==SYMBOL) {

                                    }
                                    else{
                                        S=ERREUR;
                                        DEBUG_MSG("Erreur virgule flottante ligne %d\n",liste->lex.ligne);
                                        *gestion_err=1;
                                        return(liste);
                                    }
                                }
                                else {
                                    *gestion_err = 1;

                                    return(liste);
                                }
                            }
                            else if((liste->suiv)->lex.type==DIREC) {
                                DEBUG_MSG(".byte a %d opérandes et %d virgules \n",compt_operande,compt_virgule);
                                avance;
                                DEBUG_MSG("strlex = %s\n",liste->lex.strlex);
                                S=initDATA;
                                break;
                            }
                            else{
                                DEBUG_MSG("Erreur : absence virgule ou opérande incorrecte dans .byte ligne %d\n",liste->lex.ligne);
                                S=ERREUR;
                                *gestion_err = 1;
                                return(liste)	;
                            }
                        }
                        else {
                            *fin=1;
                            return(liste);

                        }
                    }
                    else {
                        DEBUG_MSG("Erreur : mauvaise opérande dans .byte ligne %d\n",liste->lex.ligne);
                        S=ERREUR;
                        *gestion_err = 1;
                        return(liste);
                    }
                }
            }
            break;

        case change_sectionDATA:
            si_dernier{
                *fin=1;
                return (liste);
            }
            else {
                return(liste);
            }

        case ERREUR:
            DEBUG_MSG("Erreur : sortie de automate data ligne %d\n",liste->lex.ligne);
            *gestion_err = 1;
            return(liste);
        }

    }
    return(liste);
}



/* AUTOMATE BSS */

LISTE_LEX automate_bss(LISTE_LEX liste, LISTE_BSS collection_bss,char** table_symb, int* gestion_err, int* fin, int* decalage_bss, int* decalage_global, int* memory_set) {

    const char* word = ".word";
    const char* asciiz = ".asciiz";
    const char* bss = ".bss";
    const char* text = ".text";
    const char* data = ".data";
    const char* space = ".space";
    const char* byte = ".byte";
    const char* set = ".set";

    DEBUG_MSG("strlex = %s\n",liste->lex.strlex);

    si_dernier {
        DEBUG_MSG("Entrée dans automate data mais liste->suiv vide ligne %d\n",liste->lex.ligne);
        *gestion_err = 1;
        return (liste);
    }
    else {
        avance;
        DEBUG_MSG("strlex = %s\n",liste->lex.strlex);
    }

    int S = initBSS ;
    DEBUG_MSG("Je suis dans automate bss ligne %d\n",liste->lex.ligne);

    while(liste) {

        switch(S) {

        case initBSS:

            si_dernier{*fin = 1; return(liste);}

            if (liste->lex.type == DIREC) {
                DEBUG_MSG("Je vais analyser la directive %s ligne %d\n",liste->lex.strlex,liste->lex.ligne);
                /* si changement de section */
                if(strcmp(liste->lex.strlex,text)==0) {
                    DEBUG_MSG("Nouvelle section .text ligne %d\n",liste->lex.ligne);
                    S=change_sectionBSS;
                    break;
                }
                else if(strcmp(liste->lex.strlex,data)==0) {
                    DEBUG_MSG("Nouvelle section .data ligne %d\n",liste->lex.ligne);
                    S=change_sectionBSS;
                    break;
                }
                else if(strcmp(liste->lex.strlex,bss)==0) {
                    DEBUG_MSG("Nouvelle section .bss ligne %d\n",liste->lex.ligne);
                    S=change_sectionBSS;
                    break;
                }
                else if(strcmp(liste->lex.strlex,space)==0) {
                    S=spaceBSS;
                    break;
                }
                else if (liste->lex.type==COMMENT) {
                    S = COMMENT;
                    break;
                }
                else if (strcmp(liste->lex.strlex,".set")==0) {
                    si_pas_dernier {
                        avance;
                        DEBUG_MSG("strlex = %s\n",liste->lex.strlex);
                        if ( (strcmp(liste->lex.strlex,".noreorder")==0) && (*memory_set != 1) && (S!=initBSS) && (S!=initDATA) )	{
                            *memory_set = 1;
                            S = initBSS ;
                        }
                        else {
                            DEBUG_MSG("Error directive .set : missing noreorder or multiple .set or in section\n");
                            S=ERREUR;
                            *gestion_err = 1;
                            return(liste);
                        }
                    }
                    else {
                        S=ERREUR;
                        *gestion_err = 1;
                        return(liste);
                    }
                }
                else {
                    DEBUG_MSG("Directive %s incorrecte dans .bss ligne %d\n",liste->lex.strlex,liste->lex.ligne);
                    S=ERREUR;
                    *gestion_err = 1;
                    return(liste);
                }
            }
            else {
                DEBUG_MSG("Erreur : directive %s invalide dans .bss ligne %d\n",liste->lex.strlex,liste->lex.ligne);
                S=ERREUR;
                *gestion_err = 1;
                return(liste);
            }

        case spaceBSS:

            DEBUG_MSG("Je suis dans spaceBSS\n");
            BSS bss1;
            char* ptr;
            long res;

            si_dernier{
                *fin=1;
                S=ERREUR;
                *gestion_err = 1;
                DEBUG_MSG(".space vide dans la section bss ligne %d\n",liste->lex.ligne);
                return(liste);
            }
            else {
              avance;
              DEBUG_MSG("strlex = %s\n",liste->lex.strlex);
              while(liste->lex.type==HEXA || liste->lex.type==DECIMAL || liste->lex.type==DECIMAL_ZERO) {

                  if (liste->lex.type == HEXA) {
                      res = strtol(liste->lex.strlex,&ptr,16);
                      if (*ptr!='\0') {
                          printf("Hexa trop long pour strtol\n");
                          return(liste);
                      }
                      if (res < 0) {
                          *gestion_err = 1;
                          ERROR_MSG("Erreur conversion strtol");
                      }
                  }
                  else if (liste->lex.type == DECIMAL || liste->lex.type == DECIMAL_ZERO){
                      res = strtol(liste->lex.strlex,&ptr,10);
                      if (*ptr!='\0') {
                          printf("Decimal trop long pour strtol\n");
                          return(liste);
                      }
                      if (res < 0) {
                          *gestion_err = 1;
                          ERROR_MSG("Erreur conversion strtol");
                      }
                  }
                  else {
                    /* utiliser la table des symboles ici */
                  }

                  *decalage_bss+=res;
                  *decalage_global+=res;
                  DEBUG_MSG("decalage bss = %d et decala_global = %d\n",*decalage_bss, *decalage_global);

                  printf("je suis la\n");
                  strcpy(bss1.DIR,space);
                  printf("je suis ici\n");
                  printf("voici la directive copiee dans le bss1: %s\n",bss1.DIR);


  /* ajout de la directive dans la structure bss1 */

            switch(liste->lex.type) {

            case HEXA:
                strcpy(bss1.Operande.hexa_word,liste->lex.strlex);
                bss1.tag = 5;
                break;

            case SYMBOL:
                strcpy(bss1.Operande.SYMBOL,liste->lex.strlex);
                bss1.tag = 1;
                break;

            case DECIMAL:
                bss1.Operande.decimal_bss=(unsigned int)liste->lex.strlex;
                bss1.tag = 4;
                break;

            case DECIMAL_ZERO:
                bss1.Operande.decimal_bss=(unsigned int)liste->lex.strlex;
                bss1.tag = 4;
                break;

            }
            bss1.decalage = *decalage_bss;
            bss1.ligne = liste->lex.ligne;
            ajout_queue_data(bss1,collection_bss);
            visualiser_liste_data(collection_bss);

            /* fin de l'ajout */


            si_pas_dernier{
                  avance;
                DEBUG_MSG("strlex = %s\n",liste->lex.strlex);
                      if (liste->lex.type==VIRGULE) {
                          DEBUG_MSG("Erreur : multiples opérandes pour .space ligne %d\n",liste->lex.ligne);
                          S=ERREUR;
                          *gestion_err = 1;
                          return(liste);
                      }
                  }
                  else {
                      *fin = 1;
                      return (liste);
                  }
              }
          S=initBSS;
          break;
      }

        case COMMENT:
            si_pas_dernier{
                avance;
                DEBUG_MSG("strlex = %s\n",liste->lex.strlex);
                S = initBSS;
                break ;
            }
            else {
                *fin=1;
                return (liste);
            }

        case change_sectionBSS:
            si_dernier{ *fin=1; return (liste);}
            else {
                return(liste);
            }

        case ERREUR:
            DEBUG_MSG("Erreur : sortie de automate bss ligne %d\n",liste->lex.ligne);
            *gestion_err = 1;
            return(liste);


        }
    }
    return(liste);
}
