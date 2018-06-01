.code16

.global start
start:
	cli                             #关闭中断
    inb $0x92, %al                  #启动A20总线
	orb $0x02, %al
	outb %al, $0x92
	data32 addr32 lgdt gdtDesc      #加载GDTR
	movl %cr0, %eax                 #启动保护模式
	orb $0x01, %al
	movl %eax, %cr0
	data32 ljmp $0x08, $start32     #长跳转切换至保护

.code32
start32:
	movw $0x18, %ax
	movw %ax, %gs
	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %ss
	movl $(128<<20), %esp	
	jmp bootMain

.p2align 2
gdt:
	.word 0,0 # empty entry
	.byte 0,0,0,0

	.word 0xffff,0 
	.byte 0,0x9a,0xcf,0

	.word 0xffff,0 
	.byte 0,0x92,0xcf,0

	.word 0xffff,0x8000 
	.byte 0x0b,0x92,0xcf,0

gdtDesc:
	.word (gdtDesc - gdt -1)
	.long gdt
