.data
hello:  .asciiz "hello\n"   # hello pointe vers "hello\n\0"
        .text
        .globl __start
__start:
        li   $v0, 4         # la primitive print_string
        la   $a0, hello     # a0  l'adresse de hello
        syscall
