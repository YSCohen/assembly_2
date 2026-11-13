.text

    .globl sum_positive_ints
    .type sum_positive_ints, @function
sum_positive_ints:
    # DONE: Problem 1
    movq $0, %rax
    movq $0, %rdx

    p1_loop:
        cmpq %rsi, %rdx
        jge p1_ret
        movslq (%rdi), %rcx
        cmpq $0, %rcx
        jle p1_skip
        addq %rcx, %rax

    p1_skip:
        addq $1, %rdx
        addq $4, %rdi
        jmp p1_loop
    
    p1_ret:
        ret

    .globl reverse_longs
    .type reverse_longs, @function
reverse_longs:
    # TODO: Problem 2
    ret

    .globl sensor_score
    .type sensor_score, @function
sensor_score:
    # TODO: Problem 3
    movq $0, %rax
    ret

    .globl sensor_array_total
    .type sensor_array_total, @function
sensor_array_total:
    # TODO: Problem 4
    movq $0, %rax
    ret

    .globl make_arithmetic_sequence
    .type make_arithmetic_sequence, @function
make_arithmetic_sequence:
    # TODO: Problem 5
    movq $0, %rax
    ret

    .globl make_sensor
    .type make_sensor, @function
make_sensor:
    # TODO: Problem 6A
    movq $0, %rax
    ret

    .globl sensor_destroy
    .type sensor_destroy, @function
sensor_destroy:
    # TODO: Problem 6B
    ret

    .section .note.GNU-stack,"",@progbits
