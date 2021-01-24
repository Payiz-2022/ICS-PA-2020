.globl  _start
_start:
  movl %eax, %esp
  pushl %esp
  movl $0, %ebp
  call call_main
