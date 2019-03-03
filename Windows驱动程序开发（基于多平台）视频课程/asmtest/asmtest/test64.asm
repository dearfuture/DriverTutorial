DbgPrint	proto

.data
szFmt1	db	"Çý¶¯¼ÓÔØ",0ah,0
szFmt2	db	"Çý¶¯Ð¶ÔØ",0ah,0

.code

Unload	proc
	sub rsp,28h

	lea rcx,szFmt2
	call DbgPrint

	add rsp,28h
	ret
Unload	endp

DriverEntry	proc
	mov rax,rcx
	mov rbx,rdx
	sub rsp,28h
	lea rbx,Unload
	mov [rax+68h],rbx
	
	lea rcx,szFmt1
	call DbgPrint
	
	add rsp,28h

	mov eax,0
	ret

DriverEntry	endp

end