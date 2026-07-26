# 对这10个数据按照从小到大的顺序（升序）进行排序，排序后的数据仍然放在数据存储器的第0、4、 8、 …、 36号地址的存储单元中。
main:
	# 存储数据
	addi $s0,$zero,8
	sw $s0,0($zero)
	addi $s0,$zero,1
	sw $s0,4($zero)
	addi $s0,$zero,5
	sw $s0,8($zero)
	addi $s0,$zero,2
	sw $s0,12($zero)
	addi $s0,$zero,7
	sw $s0,16($zero)
	addi $s0,$zero,9
	sw $s0,20($zero)
	addi $s0,$zero,6
	sw $s0,24($zero)
	addi $s0,$zero,4
	sw $s0,28($zero)
	addi $s0,$zero,3
	sw $s0,32($zero)
	addi $s0,$zero,10
	sw $s0,36($zero)

	# 比较
	addi $s1,$zero,0
	addi $s2,$zero,0
	addi $t0,$zero,9
# 外层循环
loop1:
	addi $s2,$zero,0
# 内层循环
loop2:
	addi $s0,$s2,0
	sll $s5,$s0,2
	lw $s3,0($s5)
	addi $s0,$s2,1
	sll $s6,$s0,2
	lw $s4,0($s6)
    # 大于则交换
	sgt $s7,$s3,$s4
	beq $s7,$zero,skip
	sw $s4,0($s5)
	sw $s3,0($s6)
skip:
	addi $s2,$s2,1
	sub $s0,$t0,$s1
	bne $s2,$s0,loop2
	addi $s1,$s1,1
	sub $s0,$t0,$s1
	bne $s0,$zero,loop1
finish:
	addi $v0,$zero,10
	syscall