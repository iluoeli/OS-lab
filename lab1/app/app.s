.code32

.global start
start:

	movl $((80*5+0)*2), %edi                #在第5行第0列打印
	movb $0x0c, %ah                         #黑底红字
	movb $0x48, %al                           #0x48为H的ASCII码
	movw %ax, %gs:(%edi)                    #写显存

	movl $((80*5+1)*2), %edi                
	movb $0x0c, %ah                        
	movb $0x65, %al                          
	movw %ax, %gs:(%edi)                    

	movl $((80*5+2)*2), %edi                
	movb $0x0c, %ah                        
	movb $0x6c, %al                          
	movw %ax, %gs:(%edi)                    

	movl $((80*5+3)*2), %edi                
	movb $0x0c, %ah                        
	movb $0x6c, %al                          
	movw %ax, %gs:(%edi)                    

	movl $((80*5+4)*2), %edi                
	movb $0x0c, %ah                        
	movb $0x6f, %al                          
	movw %ax, %gs:(%edi)                    

	movl $((80*5+5)*2), %edi                
	movb $0x0c, %ah                        
	movb $0x2c, %al                          
	movw %ax, %gs:(%edi)                    

	movl $((80*5+6)*2), %edi                
	movb $0x0c, %ah                        
	movb $0x20, %al                          
	movw %ax, %gs:(%edi)                    

	movl $((80*5+7)*2), %edi                
	movb $0x0c, %ah                        
	movb $0x57, %al                          
	movw %ax, %gs:(%edi)                    

	movl $((80*5+8)*2), %edi                
	movb $0x0c, %ah                        
	movb $0x6f, %al                          
	movw %ax, %gs:(%edi)                    

	movl $((80*5+9)*2), %edi                
	movb $0x0c, %ah                        
	movb $0x72, %al                          
	movw %ax, %gs:(%edi)                    

	movl $((80*5+10)*2), %edi                
	movb $0x0c, %ah                        
	movb $0x6c, %al                          
	movw %ax, %gs:(%edi)                    

	movl $((80*5+11)*2), %edi                
	movb $0x0c, %ah                        
	movb $0x64, %al                          
	movw %ax, %gs:(%edi)                    

	movl $((80*5+12)*2), %edi                
	movb $0x0c, %ah                        
	movb $0x21, %al                          
	movw %ax, %gs:(%edi)                    

loop:
	jmp loop

