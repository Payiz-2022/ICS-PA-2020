.globl  _start
_start:
  movl %eax, %esp
  movl %eax, %ebp
  call call_main
