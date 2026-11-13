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
    # DONE: Problem 2
    # %rdi - long *a
    # %rsi - long n

    movq %rdi, %rdx
    subq $1, %rsi
    imulq $8, %rsi
    addq %rsi, %rdx

    p2_loop:
        cmpq %rdi, %rdx
        jle p2_ret
        movq (%rdi), %rcx
        movq (%rdx), %r8
        movq %rcx, (%rdx)
        movq %r8, (%rdi)
        addq $8, %rdi
        subq $8, %rdx
        jmp p2_loop

    p2_ret:
        ret

    .globl sensor_score
    .type sensor_score, @function
sensor_score:
    # DONE: Problem 3
    # %rdi - const Sensor *s
    #        int id
    #        long reading
    #        int weight
    #
    # (long)s->id + s->reading * (long)s->weight

    movslq  0(%rdi), %rax
    movslq 16(%rdi), %rsi
    imulq   8(%rdi), %rsi
    addq %rsi, %rax
    ret

    .globl sensor_array_total
    .type sensor_array_total, @function
sensor_array_total:
    # DONE: Problem 4
    # %rdi - const Sensor *a
    # %rsi - long n

    movq $0, %rax
    movq $0, %rdx # index

    p4_loop:
        cmpq %rsi, %rdx
        jge p4_ret
        pushq %rax
        pushq %rsi
        pushq %rdi
        pushq %rdx
        call sensor_score
        popq %rdx
        popq %rdi
        popq %rsi
        popq %rcx
        addq %rcx, %rax
        addq $1, %rdx
        addq $24, %rdi
        jmp p4_loop

    p4_ret:
        ret

    .globl make_arithmetic_sequence
    .type make_arithmetic_sequence, @function
make_arithmetic_sequence:
    # DONE: Problem 5
    # %rdi - long start
    # %rsi - long step
    # %rdx - long n

    cmpq $0, %rdx
    jle p5_ret_null

    pushq %rdi
    pushq %rsi
    pushq %rdx

    imulq $8, %rdx
    movq %rdx, %rdi
    call malloc

    popq %rdx
    popq %rsi
    popq %rdi

    cmpq $0, %rax
    je p5_ret_null

    movq %rax, %rcx
    movq $0, %r8
    p5_loop:
        cmpq %rdx, %r8
        jge p5_ret
        movq %rsi, %r9
        imulq %r8, %r9
        addq %rdi, %r9
        movq %r9, (%rcx)
        addq $8, %rcx
        addq $1, %r8
        jmp p5_loop

    p5_ret:
        ret

    p5_ret_null:
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
