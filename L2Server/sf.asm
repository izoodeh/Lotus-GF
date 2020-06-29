
wcscmp PROTO :QWORD, :QWORD
.data
sSlotStoreBuff dw 's','l','o','t','_','s','t','o','r','e','_','b','u','f','f',0
.code


;.text:00000000005586A5                 mov     rax, [rbp+0A90h]
;.text:00000000005586AC                 mov     ecx, [rax+660h]
;.text:00000000005586B2                 test    ecx, ecx
;.text:00000000005586B4                 jz      loc_5589AC
;.text:00000000005586BA                 mov     rax, [rdx+0A90h]
;.text:00000000005586C1                 cmp     ecx, [rax+660h]
;.text:00000000005586C7                 jnz     loc_5589AC				;hook here + 1 nop
;.text:00000000005586CD                 lea     rcx, [rbp+3298h]
;.text:00000000005586D4                 call    sub_7695A0

CheckAllianceHook proc
	cmp ecx, [rax+660h]
	jnz CheckAlliance
	jmp Alliance
	
	CheckAlliance:
		mov rax, [rbp+0A90h]
		mov ecx, [rax+680h]
		test ecx, ecx
		jz NotAlliance
		mov rax, [rdx+0A90h]
		cmp ecx, [rax+680h]
		jnz NotAlliance
		jmp Alliance
	
	Alliance:
		mov rcx, 5586CDh
		jmp rcx
		
	NotAlliance:
		mov rax, 5589ACh
		jmp rax
		
CheckAllianceHook endp


CSkillEffect_i_dispel_by_category_new_slot_type proc
	call wcscmp
	cmp eax, ebx
	jnz NewSlotCheck
	mov dword ptr[rbp+10h], 1
	mov rdx, 9BE9D8h
	jmp rdx
	
	NewSlotCheck:
		movsxd rax, dword ptr [rsi+34h]
		mov rcx, [rsi+18h]
		lea rdx, sSlotStoreBuff
		mov rcx, [rcx+rax*8-20h]
		call wcscmp
		cmp eax, ebx
		jz SetNewSlotType
		mov rax, 9BE9B8h
		jmp rax
		
	SetNewSlotType:
		mov dword ptr[rbp+10h], 3
		mov rdx, 9BE9D8h
		jmp rdx
		
CSkillEffect_i_dispel_by_category_new_slot_type endp

;.text:000000000090A423                 jz      short loc_90A43C
;.text:000000000090A425                 mov     eax, [rdx]
;.text:000000000090A427                 mov     [rdi], eax
;.text:000000000090A429                 mov     eax, [rdx+4]
;.text:000000000090A42C                 mov     [rdi+4], eax
;.text:000000000090A42F                 jmp     short loc_90A43C
;rcx - pItem
;can use rax, r8, r9
; 19025 until 19038
; and 19060 until 19073

AugmentationGlowBlockUserInfo proc
	mov r8d, [rcx+1Ch]
	cmp r8d, 19025
	jge NextCheck1
	jmp NormalExecution
	
	NextCheck1:
		cmp r8d, 19038
		jle BlockGlow
		cmp r8d, 19060
		jge NextCheck2
		jmp NormalExecution
		
	NextCheck2:
		cmp r8d, 19073
		jle BlockGlow
		jmp NormalExecution
	
	BlockGlow:
		mov qword ptr[rdi], 0h
		mov r8, 90A43Ch
		jmp r8
	
	NormalExecution:
		mov eax, [rdx]
		mov [rdi], eax
		mov eax, [rdx+4]
		mov [rdi+4], eax
		mov r8, 90A43Ch
		jmp r8
AugmentationGlowBlockUserInfo endp

;text:000000000090C93B                 lea     rcx, [rax+0C0h]
;.text:000000000090C942                 cmp     rsi, rcx
;.text:000000000090C945                 jz      short loc_90C951
;
;.text:000000000090C947                 mov     eax, [rcx]			<--- hook start
;.text:000000000090C949                 mov     [rsi], eax
;.text:000000000090C94B                 mov     eax, [rcx+4]
;.text:000000000090C94E                 mov     [rsi+4], eax
;.text:000000000090C951
;.text:000000000090C951 loc_90C951:                             ; CODE XREF: User__SendCharInfo+559j
;.text:000000000090C951                                         ; User__SendCharInfo+565j
;.text:000000000090C951                 mov     eax, [rbx+78h]

AugmentationGlowBlockCharInfo proc
	mov eax, [rax+1Ch]
	cmp eax, 19025
	jge NextCheck1
	jmp NormalExecution
	
	NextCheck1:
		cmp eax, 19038
		jle BlockGlow
		cmp eax, 19060
		jge NextCheck2
		jmp NormalExecution
	
	NextCheck2:
		cmp eax, 19073
		jle BlockGlow
		jmp NormalExecution
	
	BlockGlow:
		mov qword ptr[rsi], 0h
		mov rax, 90C951h
		jmp rax
	
	NormalExecution:
		mov eax, [rcx]
		mov [rsi], eax
		mov eax, [rcx+4]
		mov [rsi+4], eax
		mov rax, 90C951h
		jmp rax
AugmentationGlowBlockCharInfo endp
end