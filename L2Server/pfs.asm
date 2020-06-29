
TraitExtendWrapper PROTO :QWORD

.code

TraitExtend proc
	call TraitExtendWrapper
	mov rax, 9BF99Ah
	jmp rax
TraitExtend endp

end