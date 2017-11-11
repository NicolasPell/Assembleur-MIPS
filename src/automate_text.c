/* AUTOMATE TEXT */


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

void majuscule(char *chaine) {

    int i = 0;
    for (i = 0 ; chaine[i] != '\0' ; i++)
    {
        chaine[i] = toupper(chaine[i]);
    }
}


struct dico_instr {
    char str_intr[10];
    int nb_op;
    char str_type[10];
};


typedef struct dico_instr DICO_INSTR;




int fonction_test_instruction(DICO_INSTR* Tab_dico, char* strlex, int* recup_indice) {
    majuscule(strlex);
    printf("\n\n");
    int i = 0;
    for(i=0; i<30; i++) {
        printf("Voici l'élement de Tab_dico[%d].str_intr = %s \n",i,Tab_dico[i].str_intr);
        if (strcmp(Tab_dico[i].str_intr,strlex)==0) {
            printf("l'instuction: %s a bien été trouvé dans le dictionnaire\n",strlex);
            *recup_indice = i;
            return(0);
        }
    }
    printf("\n\n");
    printf("l'instruction: %s  n'a pas été trouvé dans le dictionnaire\n",strlex);
    return(1);
}





DICO_INSTR* chargement_dico(FILE* pdico) {

    char token[10];
    int int_token=0;
    int k = 0;

    DICO_INSTR* Tab_dico = (DICO_INSTR*)calloc(30, sizeof(DICO_INSTR));

    while (feof(pdico) == 0) {

        fscanf(pdico, "%s", token);
        strcpy(Tab_dico[k].str_intr, token);

        fscanf(pdico, "%d", &int_token);
        Tab_dico[k].nb_op = int_token;

        fscanf(pdico, "%s", token);
        strcpy(Tab_dico[k].str_type,token);

        k++;

    }

    fseek(pdico,0, SEEK_SET); /* remise au debut du pointeur de fichier */
    return(Tab_dico);
}


LISTE_LEX automate_text(LISTE_LEX liste,int* gestion_err,int* fin,int* decalage_text, int* decalage_global, int* memory_set) {

    const char* word = ".word";
    const char* asciiz = ".asciiz";
    const char* bss = ".bss";
    const char* text = ".text";
    const char* data = ".data";
    const char* space = ".space";
    const char* byte = ".byte";
    const char* set = ".set";
    const char* point = ".";
    const char* nop = "NOP";
    const char* syscall = "SYSCALL";

    int recup_indice;

    INFO_MSG("strlex = %s\n",liste->lex.strlex);

    int S = initTEXT;
    puts("Je suis dans automate text\n");

    FILE* pdico;
    pdico = fopen("dico_instr.txt","r");

    if (pdico == NULL) {
        ERROR_MSG("Erreur à l'ouverture du fichier texte dictionnaire\n");
        *gestion_err = 1;
        return(liste);
    }

    DICO_INSTR* Tab_dico;

    Tab_dico = chargement_dico(pdico);

    si_dernier {
        DEBUG_MSG("Entrée dans automate data mais liste->suiv vide ligne %d\n",liste->lex.ligne);
        *gestion_err = 1;
        return (liste);
    }
    else {
        avance;
        DEBUG_MSG("strlex = %s\n",liste->lex.strlex);
    }

    while(liste) {

        switch(S) {

        case initTEXT:

            if (liste->lex.type == DIREC) {

                INFO_MSG("Je vais analyser la directive %s\n",liste->lex.strlex);

                if(strcmp(liste->lex.strlex,point)==0) {
                    ERROR_MSG("Directive . invalide\n");
                    *gestion_err=1;
                    return(liste);
                }
                else if(strcmp(liste->lex.strlex,word)==0) {
                    puts("Erreur: présence de .word dans la section .text\n");
                    *gestion_err=1;
                    return(liste);

                }

                else if(strcmp(liste->lex.strlex,asciiz)==0) {
                    puts("Erreur: présence de .asciiz dans la section .text\n");
                    *gestion_err=1;
                    return(liste);
                }

                else if(strcmp(liste->lex.strlex,space)==0) {
                    puts("Erreur: présence de .space dans la section .text\n");
                    *gestion_err=1;
                    return(liste);

                }

                else if(strcmp(liste->lex.strlex,byte)==0) {
                    puts("Erreur: présence de .byte dans la section .text\n");
                    *gestion_err=1;
                    return(liste);

                }

                /* si on change de section */

                else if(strcmp(liste->lex.strlex,text)==0) {
                    DEBUG_MSG("Nouvelle section .text ligne %d\n",liste->lex.ligne);
                    S=change_sectionTEXT;
                    break;
                }

                else if(strcmp(liste->lex.strlex,data)==0) {
                    DEBUG_MSG("Nouvelle section .data ligne %d\n",liste->lex.ligne);
                    S=change_sectionTEXT;
                    break;
                }

                else if(strcmp(liste->lex.strlex,bss)==0) {
                    DEBUG_MSG("Nouvelle section .bss ligne %d\n",liste->lex.ligne);
                    S=change_sectionTEXT;
                    break;
                }
            }

            else if (liste->lex.type == SYMBOL) {
                /* la fonction suivante teste si le SYMBOL est une instruction */
                if(fonction_test_instruction(Tab_dico,liste->lex.strlex, &recup_indice)==0) {
                    S = instructionTEXT;
                    DEBUG_MSG("S = instructionTEXT\n");
                    break;
                }
                else {
                    S = etiquetteTEXT;
                    DEBUG_MSG("S = etiquetteTEXT\n");
                }
            }
            else if (liste->lex.type == COMMENT) {
                si_dernier{ *fin=1; return(liste);}
                else {
                    avance;
                    S=initTEXT;
                    break;
                }
            }

            else {
                S = ERREUR;
                printf("ERREUR variable S\n");
                *gestion_err = 1;
                return(liste);
            }
            break;

        case etiquetteTEXT:
            si_dernier{
                *fin=1;
                DEBUG_MSG("Symbole n'étant ni étiquette ni instruction %d\n",liste->lex.ligne);
                return (liste);
            }
            else {
                avance;  /*on est au deux points*/
                if(liste->lex.type==DEUX_POINTS) {
                    si_dernier{
                        *fin=1;
                        DEBUG_MSG("Déclaration d'étiquette vide, fin de liste ligne %d\n",liste->lex.ligne);
                        return(liste);

                    }
                    else {
                        avance;
                        S=initTEXT;
                        break;
                    }
                }
                else {
                    *gestion_err = 1;
                    DEBUG_MSG("Instruction invalide ou etiquette mal definie ligne %d\n",liste->lex.ligne);
                }
            }


        case change_sectionTEXT:
            si_dernier{
                *fin=1;
                return (liste);
            }
            else {
                return(liste);
            }

        case instructionTEXT:

            /*INSTRUCT instruction;
            instruction.lex = liste->lex.strlex;
            instruction.nb_operande = Tab_dico[*recup_indice].nb_operande;
            instruction.ligne = liste->lex.ligne;
            instruction.decalage = *decalage_text;
            /* mettre le tableau d'operandes a remplir dans le while */

            INFO_MSG("strlex = %s\n",liste->lex.strlex);
            INFO_MSG("Je suis dans instructionTEXT\n");
            si_dernier{
                DEBUG_MSG("Fin de liste\n");
                S=ERREUR;
                *gestion_err=1;
                return(liste);
            }
            else {

                int compt_virgule;
                int compt_operande;

                DEBUG_MSG("Analyse l'instruction %s ligne %d\n",liste->lex.strlex,liste->lex.ligne);


                while(liste->suiv!=NULL) {

                    compt_virgule = 0;
                    compt_operande = 0;
                    avance;
                    DEBUG_MSG("strlex = %s\n",liste->lex.strlex);
                    if(liste->lex.type==HEXA ||liste->lex.type==DECIMAL || liste->lex.type==DECIMAL_ZERO || liste->lex.type==SYMBOL || liste->lex.type==REG) {
                        compt_operande = compt_operande + 1;
                        si_pas_dernier{
                            if((liste->suiv)->lex.type==VIRGULE) {
                                avance;
                                DEBUG_MSG("strlex = %s\n",liste->lex.strlex);
                                compt_virgule = compt_virgule + 1;
                                si_pas_dernier {
                                    if((liste->suiv)->lex.type==HEXA || (liste->suiv)->lex.type==DECIMAL || (liste->suiv)->lex.type==DECIMAL_ZERO || (liste->suiv)->lex.type==SYMBOL||(liste->suiv)->lex.type==REG) {
                                        compt_operande = compt_operande + 1;
                                    }
                                    else{
                                        S=ERREUR;
                                        DEBUG_MSG("Erreur virgule flottante ligne %d\n",liste->lex.ligne);
                                        *gestion_err=1;
                                        return(liste);
                                    }
                                }
                                else {
                                    *gestion_err=1;
                                    DEBUG_MSG("Erreur : virgule flottante ligne %d\n",liste->lex.ligne);
                                    return(liste);
                                }
                            }
                            else if((liste->suiv)->lex.type==DIREC) { /*nouvelle directive*/
                                *decalage_text = *decalage_text +4 ;
                                *decalage_global = *decalage_global +4 ;
                                DEBUG_MSG("decalage text = %d et decala_global = %d\n",*decalage_text, *decalage_global);
                                avance;
                                DEBUG_MSG("strlex = %s\n",liste->lex.strlex);
                                S=initTEXT;
                                break;
                            }
                            else if ((liste->suiv)->lex.type==SYMBOL) {
                                *decalage_text = *decalage_text +4 ;
                                *decalage_global = *decalage_global +4 ;
                                avance;
                                DEBUG_MSG("strlex = %s\n",liste->lex.strlex);
                                DEBUG_MSG("decalage text = %d et decala_global = %d\n",*decalage_text, *decalage_global);

                                DEBUG_MSG("Analyse l'instruction %s ligne %d\n",liste->lex.strlex,liste->lex.ligne);
                                if(fonction_test_instruction(Tab_dico,liste->lex.strlex, &recup_indice)==0) {
                                    if(strcmp(liste->lex.strlex,nop)==0 || strcmp(liste->lex.strlex,syscall)==0) {
                                        *decalage_text = *decalage_text + 4 ;
                                        *decalage_global = *decalage_global +4 ;
                                        si_dernier{ *fin=1; return(liste);}
                                        else {
                                            *decalage_text = *decalage_text +4 ;
                                            *decalage_global = *decalage_global +4 ;
                                            avance;
                                            DEBUG_MSG("strlex = %s\n",liste->lex.strlex);
                                            S=initTEXT;
                                            break;
                                        }
                                    }
                                    S=instructionTEXT;
                                    DEBUG_MSG("S = instructionTEXT\n");
                                }
                                else if(fonction_test_instruction(Tab_dico,liste->lex.strlex, &recup_indice)==1) {
                                    DEBUG_MSG("Instruction %s inconnue ligne %d\n",liste->lex.strlex,liste->lex.ligne);
                                    *gestion_err=1;
                                    return(liste);
                                }
                                else {
                                    S = etiquetteTEXT;
                                    DEBUG_MSG("S = etiquetteTEXT\n");
                                }
                            }
                            else{
                                ERROR_MSG("Absence virgule ou operande incorrecte dans .text ligne %d\n",liste->lex.ligne);
                                S=ERREUR;
                                *gestion_err = 1;
                                return(liste);
                            }
                        }
                        else {
                            *fin=1;
                            *decalage_text = *decalage_text + 4;
                            *decalage_global = *decalage_global + 4;
                            return(liste);
                        }
                    }
                    else {
                        DEBUG_MSG("strlex = %s \n",liste->lex.strlex);
                        ERROR_MSG("Mauvaise operande ligne %d\n",liste->lex.ligne);
                        S=ERREUR;
                        *gestion_err = 1;
                        return(liste);
                    }
                    if(compt_operande > 3) {
                        *gestion_err=1;
                        ERROR_MSG("Nombre d'operandes superieur a 3\n");
                        return(liste);
                    }
                }
                /* ajout d'un test car si pas d'entrée dans le while alors pas d'opérande => erreur ??? SAUF SI instruction sans opérandes ...
                if (compt_operande == 0) {
                  &gestion_err = 1;
                  ERROR_MSG;
                }
                */
                break;

            }
            /* ajout_queue_instruct(instruct,collection_instruct); */
            break;

        }
    }
    return(liste);
}
