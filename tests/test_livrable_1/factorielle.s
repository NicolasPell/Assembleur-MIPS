.text            
.set noreorder		#pas de réordonnancement

#Attention: ce programme factorielle ne fonctionne que pour des nombres 16 bits
#Ce programme a été modifié à partir du site internet: gallium.inria.fr/~remy/poly/compil/1/index.html

fact:   blez $a0, fact_0     # si a0 <= 0 saut à fact_0
        sub  $sp, $sp, 8     # réserve deux mots en pile
        sw   $ra, 0($sp)     # sauve l'adresse de retour
        sw   $a0, 4($sp)     # et la valeur de a0
        sub  $a0, $a0, 1     # décrément a0
        jal  fact            # v0 <- appel récursif (a0-1)
        lw   $a0, 4($sp)     # récupére a0
        mult $v0, $a0   	 # HI et LO <- a0 * v0
		MFLO $v0             # 
        lw   $ra, 0($sp)     # récupére l'adresse de retour
        add  $sp, $sp, 8     # libère la pile
        j    $ra             # retour à l'appelant
                             
fact_0: lui   $v0, 1         # v0 <- 1
        j    $ra             # retour à l'appelant
