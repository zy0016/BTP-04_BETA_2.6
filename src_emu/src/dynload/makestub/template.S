

@ Generated by gcc 2.9-xscale-010413 for ARM/elf
	.file	"template.c"
.gcc2_compiled.:
	.section .rodata
	.align	3
	.type	 funcname,object
	.size	 funcname,64
funcname:
	.ascii	"template\000"
.data
	.align	2
	.type	 target_addr.0,object
	.size	 target_addr.0,4
target_addr.0:
	.word	0
.text
	.align	2
	.global	template
	.type	 template,function
template:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, current_function_anonymous_args = 0

	ldr	ip, .L6
	ldr	ip, [ip, #0]
	cmp	ip, #0	
	movne pc, ip

.L3:
	stmdb	sp!, {r0-r3,lr}
	ldr	r0, .L6+4
	bl	stub_find_func_addr
	ldr	r3, .L6
	str	r0, [r3, #0]
	mov ip, r0
	ldmia	sp!, {r0-r3,lr}
	mov pc, ip

	.align	2
.L6:
	.word	target_addr.0
	.word	funcname
.Lfe1:
	.size	 template,.Lfe1-template
