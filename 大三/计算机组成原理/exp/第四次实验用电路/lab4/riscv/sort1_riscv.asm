main: 
    # 存储数据
    ori s0,zero,10
    sw s0,0(zero)
    add t0,s0,zero
    ori s0,zero,-1
    add t0,t0,s0

    ori s0,zero,8
    sw s0,4(zero)
    ori s0,zero,1
    sw s0,8(zero)
    ori s0,zero,5
    sw s0,12(zero)
    ori s0,zero,2
    sw s0,16(zero)
    ori s0,zero,7
    sw s0,20(zero)
    ori s0,zero,9
    sw s0,24(zero)
    ori s0,zero,6
    sw s0,28(zero)
    ori s0,zero,4
    sw s0,32(zero)
    ori s0,zero,3
    sw s0,36(zero)
    ori s0,zero,10
    sw s0,40(zero)

    # 比较
    ori s1,zero,0
    ori s2,zero,0
    ori s9,zero,1
    ori s10,zero,4
# 外层循环
loop1:
    ori s8,zero,4
    ori s2,zero,0
# 内层循环
loop2:
    add s5,s8,zero
    lw s3,0(s5)
    add s8,s8,s10
    add s6,s8,zero
    lw s4,0(s6)
    slt s7,s4,s3
    beq s7,zero,go
    beq zero,zero,skip
go:
    sw s4,0(s5)
    sw s3,0(s6)
skip:
    add s2,s2,s9
    add s0,s1,s2
    slt s7,t0,s0
    beq s7,zero,loop2
    add s1,s1,s9
    slt s7,t0,s1
    beq s7,zero,loop1
end:
    jal zero,end