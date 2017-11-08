# allons au ru
#erreur ligne 7 OPERANDE registre

.set noreorder		
.text
    Lw $t0 , lunchtime
    LW $6M, -0x200($7)			#erreur $6
    ADDI $t1,$zero,8	

boucle:
    BEQ $t0 , $t1 , byebye
    NOP
    addi $t1 , $t1 , 1
    J boucle 
    NOP
byebye:
    JAL viteviteauru

.data
lunchtime: .word 12
.word menu

.bss 
menu:.space 24
