; -------------------------------------------------
; 	将GDT的基地址加载到GDTR
;
; 	代码借鉴自[hurlex-doc](https://github.com/hurley25/hurlex-doc)
;
; -------------------------------------------------

;用于外部引用
[GLOBAL gdtr_flush]

gdtr_flush:
	mov eax, [esp+4]  ; 参数存入 eax 寄存器		正是gdtr压栈的位置
	lgdt [eax]        ; 加载到 GDTR [修改原先GRUB设置]

	mov ax, 0x10      ; 加载我们的数据段描述符		相当于置位数据段
	mov ds, ax        ; 更新所有可以更新的段寄存器
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp 0x08:.flush   ; 远跳转，0x08是我们的代码段描述符
			  ; 远跳目的是清空流水线并串行化处理器
.flush:
	ret
