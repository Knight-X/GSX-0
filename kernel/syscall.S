.syntax unified
.cpu cortex-m3
.fpu softvfp
.thumb

.type read, %function
.global read
read:
	push {r7, lr}
	mov r7, #0x4
	svc 0
	nop
	pop {r7, lr}
	bx lr

.type write, %function
.global write
write:
	push {r7, lr}
	mov r7, #0x3
	svc 0
	nop
	pop {r7, lr}
	bx lr
