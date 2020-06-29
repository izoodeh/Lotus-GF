
IsValidPointer PROTO :QWORD
.data

.code

;.text:000000000086DE87                 mov     rcx, [rbx]
;.text:000000000086DE8A                 mov     [r14+rsi], eax
;.text:000000000086DE8E                 mov     rax, [rcx]
;.text:000000000086DE91                 jnz     short loc_86DEA0
;.text:000000000086DE93                 mov     edx, [r13+0]
;.text:000000000086DE97                 call    qword ptr [rax+30h]
;.text:000000000086DE9A                 mov     [r14+rsi], edi
;.text:000000000086DE9E                 jmp     short loc_86DEAE

;.text:000000000086DEA0                 mov     r8d, [r13+0]
;.text:000000000086DEA4                 xor     r9d, r9d
;.text:000000000086DEA7                 lea     edx, [r9+1]
;.text:000000000086DEAB                 call    qword ptr [rax+40h]

ValidateObjectInTimer proc
push rdi
push rcx
push rdx
push r8
push r9
push r10
push r11
mov [r14+rsi], eax
mov     rdi, rcx
jnz CheckPointer2
jmp CheckPointer1

CheckPointer1:
	call IsValidPointer
	test eax, eax
	jz InvalidPointer1
	mov rax, [rdi]
	pop r11
	pop r10
	pop r9
	pop r8
	pop rdx
	pop rcx
	pop rdi
	mov rdx, 86DE93h
	jmp rdx

CheckPointer2:
	call IsValidPointer
	test eax, eax
	jz InvalidPointer2
	mov rax, [rdi]
	pop r11
	pop r10
	pop r9
	pop r8
	pop rdx
	pop rcx
	pop rdi
	mov rdx, 86DEA0h
	jmp rdx
	
InvalidPointer1:
	pop r11
	pop r10
	pop r9
	pop r8
	pop rdx
	pop rcx
	pop rdi
	mov rdx, 86DE9Ah
	jmp rdx
	
InvalidPointer2:
	pop r11
	pop r10
	pop r9
	pop r8
	pop rdx
	pop rcx
	pop rdi
	mov rdx, 86DEAEh
	jmp rdx
	

ValidateObjectInTimer endp


end