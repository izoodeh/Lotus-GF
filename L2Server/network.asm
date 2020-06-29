
PacketHandlerWrapper PROTO :QWORD, :QWORD, :BYTE
PacketDecryptWrapper PROTO :QWORD, :QWORD, :DWORD
SendKeyPacketWrapper PROTO :QWORD, :QWORD, :BYTE, :BYTE, :QWORD, :DWORD, :DWORD, :BYTE, :DWORD
.data

.code

PacketHandlerHook proc
	mov rcx, rsi
	jmp PacketHandlerWrapper
PacketHandlerHook endp

PacketDecryptHook proc
	mov rdx, rsi	;pSocket
	jmp PacketDecryptWrapper
PacketDecryptHook endp

SendKeyPacketHook proc
	mov rcx, rdi
	jmp SendKeyPacketWrapper
SendKeyPacketHook endp

end