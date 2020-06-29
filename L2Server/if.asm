
GetEnchantAttributeValueWrapper PROTO

.data

.code

;.text:00000000008A89A8                 cmp     edx, 20h
;.text:00000000008A89AB                 jz      loc_8A8B00
;.text:00000000008A89B1                 cmp     edx, 19h
;.text:00000000008A89B4                 jnz     loc_8A8AE6
;.text:00000000008A89BA                 mov     rax, [rbx+0A90h]

SafeScrollEtcTypeHook proc
	cmp edx, 20h
	jz EnchantWindow
	cmp edx, 23h
	jz EnchantWindow
	cmp edx, 24h
	jz EnchantWindow
	cmp edx, 25h
	jz AttributeWindow
	cmp edx, 26h
	jz AttributeWindow
	mov rax, 8A89B1h
	jmp rax
	
	EnchantWindow:
		mov rax, 8A8B00h
		jmp rax
	
	AttributeWindow:
		mov rax, 8A89BAh
		jmp rax
SafeScrollEtcTypeHook endp

;.text:000000000069678D                 mov     cx, 14h
;.text:0000000000696791                 mov     [rsp+92h], cx
;.text:0000000000696799                 jmp     short loc_6967A7

AttributeEnchantValueWeaponHook proc
	push rdx
	push r8
	push r9
	push r10
	call GetEnchantAttributeValueWrapper
	mov cx, ax
	pop r10
	pop r9
	pop r8
	pop rdx
	test cx, cx
	jz NormalExit
	
	mov [rsp+92h], cx
	mov rax, 6967E0h
	jmp rax
	
	NormalExit:
		mov cx, 14h
		mov [rsp+92h], cx
		mov rax, 6967A7h
		jmp rax
AttributeEnchantValueWeaponHook endp

;.text:0000000000696147                 add     [rsp+r13*2+40h+arg_4C], 6	<--hook here
;.text:0000000000696151                 movsxd  rax, [rsp+40h+arg_5548]
;.text:0000000000696159                 movzx   r8d, word ptr ds:(loc_75F0BF+7)[r15+rax*4]
;.text:0000000000696162                 cmp     [rsp+r13*2+40h+arg_4C], r8w
;.text:000000000069616B                 jle     short loc_69617B
;.text:000000000069616D                 mov     [rsp+r13*2+40h+arg_4C], r8w
;.text:0000000000696176                 jmp     short loc_69617B

AttributeEnchantValueArmorHook proc
	
	push rdx
	push r9
	push r10
	call GetEnchantAttributeValueWrapper	
	pop r10
	pop r9
	pop rdx
	
	test ax, ax
	jz NormalExit
	add word ptr [rsp+r13*2+94h], ax
	mov rax, 69617Bh
	jmp rax
	
	NormalExit:
		add word ptr [rsp+r13*2+94h], 6
		mov rax, 696151h
		jmp rax
	
AttributeEnchantValueArmorHook endp

;.text:00000000008342B9                 movsxd  rcx, dword ptr [rbp+18h]
;.text:00000000008342BD                 mov     rax, rcx
;.text:00000000008342C0                 movzx   eax, word ptr [r13+rcx*4+804B96h]
;.text:00000000008342C9                 cmp     [rdi+132h], ax
;.text:00000000008342D0                 jl      short loc_83430D

AttributeEnchantLevelWeaponCheckHook proc
	movsxd rcx, dword ptr[rbp+18h]
	mov rax, rcx
	cmp ecx, 9
	jg SetMaxLevel
	mov rax, 8342C0h
	jmp rax
	
	SetMaxLevel:
		mov edx, ecx
		mov ecx, 9
		movzx   eax, word ptr [r13+rcx*4+804B96h]
		mov ecx, edx
		mov rdx, 8342C9h
		jmp rdx
		
AttributeEnchantLevelWeaponCheckHook endp


AttributeEnchantLevelArmorCheckHook proc
	movsxd rdx, dword ptr[rbp+18h]
	mov rax, rdx
	cmp edx, 9
	jg SetMaxLevel
	mov rax, 8343ABh
	jmp rax
	
	SetMaxLevel:
		mov ecx, edx
		mov edx, 9
		movzx   eax, word ptr [r13+rdx*4+804BBEh]
		mov edx, ecx
		mov rcx, 8343B4h
		jmp rcx
		
AttributeEnchantLevelArmorCheckHook endp

end