
.data

.code

;.text:000000000093D381                 mov     r11, [rax+0A90h]
;.text:000000000093D388                 cmp     byte ptr [r11+954h], 0
;.text:000000000093D390                 jz      loc_93D419
;.text:000000000093D396                 cmp     byte ptr [rax+2368h], 0
OfflineShopCheck proc
	cmp byte ptr[r11+954h], 0
	jz CheckOfflineShop
	mov rsi, 93D396h
	jmp rsi
	
	CheckOfflineShop:
		mov rsi, [rax+3A10h]
		cmp dword ptr[rsi], 0
		jnz SetShop
		mov rsi, 93D419h
		jmp rsi
	
	SetShop:
		mov rsi, 93D396h
		jmp rsi
		
OfflineShopCheck endp

end