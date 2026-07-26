# RISC-V�����ۼӺͳ���           sum_riscv.asm               result=1+2+...+n               
# ���result����ڵ�ַΪ0�����ݴ洢����

main:
                ori a3,zero,100		# a3=100=n
	ori a5, zero, 1     		# a5=1          
	ori a2, zero, 1     		# a2=1        
                ori a4, zero, 0                          # a4=0          
loop:
	add a4, a4, a5     		# a4+a5  ->   a4       
	beq a5, a3, finish  		# �ж�a5=a3��          ��a5=a3=n��������ѭ��
    	add a5, a5, a2     		# a5+a2 -> a5           
    	jal zero, loop		# ��������ת��loopִ��
finish:
	sw a4, 0(zero)    		# a4 -> (0)                  ���ۼӽ��result���浽�洢����
end:
	jal zero, end		# ��������ת��endִ��
