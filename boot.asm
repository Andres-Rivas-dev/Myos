[BITS 16]
[ORG 0x7C00]

start:
	mov si, mensaje
	call print

hang:
	jmp hang

print:
	lodsb
	or al, al
	jz done
	mov ah, 0x0E
	int 0x10
	jmp print

done:
	ret

mensaje db 'Hola desde mi OS!', 0 ;

times 510-($-$$) db 0
dw 0xAA55
