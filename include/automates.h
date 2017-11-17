#ifndef _AUTOMATES_H_
#define _AUTOMATES_H_

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



/* Prototypes des automates */

int automate_init(LISTE_LEX liste);
LISTE_LEX automate_bss(LISTE_LEX liste,LISTE_BSS collection_bss,char** table_symb,int* gestion_err, int* fin, int* decalage_bss, int* decalage_global, int* memory_set);
LISTE_LEX automate_data(LISTE_LEX liste,LISTE_DATA collection_data,char** table_symb,int* gestion_err, int* fin, int* decalage_data, int* decalage_global, int* memory_set);
LISTE_LEX automate_text(LISTE_LEX liste,LISTE_INSTRUCT collection_instruct,char** table_symb,int* gestion_err,int* fin,int* decalage_text, int* decalage_global, int* memory_set);


#endif /* _AUTOMATES_H_ */
