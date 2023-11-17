section .data

section .text

	global _start

_start:

	mov rbp, rsp
	sub rsp, 72
	mov edi, 61
	mov eax, edi
x: 	mov [rbp-12], edi	; move the variable value to the stack
	mov edi, 12
	mov eax, edi
y: 	mov [rbp-24], edi	; move the variable value to the stack
	mov edi, [rbp-24]	; retrieve variable value from the stack
	mov [rbp-64], edi	; save argument one to the stack
	mov edi, [rbp-12]	; retrieve variable value from the stack
	mov [rbp-68], edi	; save argument two to the stack
	sub edi, [rbp-64]	; subtract
	mov [rbp-72], edi	; save result to the stack
	mov eax, edi	; move result into rax according to SystemV ABI
	mov rcx, 10	; set the divisor to 10
	xor rbx, rbx	; clear rbx to use as counter
divide_loop0:
	xor rdx, rdx	; clear remainder
	div rcx		; divide rax by 10, remainder in rdx
	add rdx, '0'	; convert integer to ascii
	push rdx	; push the remainder to the stack
	inc rbx		; increment the number of pushes to the stack

	cmp rax, 0	; check to make sure quotient is not 0
	jne divide_loop0	; if != 0, continue loop
print_result0:
; prepare the registers for syswrite
	mov rax, 1	; syscall number for syswrite
	mov rdi, 1	; file descriptor for stdout
	mov rdx, 1	; the number of bytes to write to the console

	mov rsi, rsp	; move address of first character into the buffer register
	add rsp, 8	; add 8 bytes to rsp register to simulate a pop
	syscall	; syswrite syscall
	dec rbx		; decrement counter register
	cmp rbx, 0	; if rbx > 0, there are more bytes to print
	jne print_result0	; loop back
	mov edi, [rbp-24]	; retrieve variable value from the stack
	mov [rbp-64], edi	; save argument one to the stack
	mov edi, [rbp-12]	; retrieve variable value from the stack
	mov [rbp-68], edi	; save argument two to the stack
	adc edi, [rbp-64]	; add the second argument to the first argument from the stack
	mov [rbp-72], edi	; save result to the stack
	mov eax, edi	; move to rax according to ABI
	mov rcx, 10	; set the divisor to 10
	xor rbx, rbx	; clear rbx to use as counter
divide_loop1:
	xor rdx, rdx	; clear remainder
	div rcx		; divide rax by 10, remainder in rdx
	add rdx, '0'	; convert integer to ascii
	push rdx	; push the remainder to the stack
	inc rbx		; increment the number of pushes to the stack

	cmp rax, 0	; check to make sure quotient is not 0
	jne divide_loop1	; if != 0, continue loop
print_result1:
; prepare the registers for syswrite
	mov rax, 1	; syscall number for syswrite
	mov rdi, 1	; file descriptor for stdout
	mov rdx, 1	; the number of bytes to write to the console

	mov rsi, rsp	; move address of first character into the buffer register
	add rsp, 8	; add 8 bytes to rsp register to simulate a pop
	syscall	; syswrite syscall
	dec rbx		; decrement counter register
	cmp rbx, 0	; if rbx > 0, there are more bytes to print
	jne print_result1	; loop back

	mov rax, 60
	syscall
