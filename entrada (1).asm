add x2, x0, x1
sll x1, x2, x2
xor x4, x2, x3
addi x3, x2, -4
lw x5, 0(x1)
sw x5, 4(x2)
bne x1, x2, 8
