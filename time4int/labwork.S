  # labwork.S
  # Written 2015-2017 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1.
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.
  #
  # Please keep the two macros PUSH and POP unchanged
  # 
  # Changed by A Hammarstrand
  #

.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg,0($sp)
.endm

.macro	POP reg
	lw	\reg,0($sp)
	addi	$sp,$sp,4
.endm

.global delay
.global time2string
.global hexasc
.global enable_interrupt

.text
enable_interrupt:
      # The ei instruction sets all register the CPU needs to handle interrupts
      ei 
      j     return
       
delay:
    while:
        beq  $a0, $0, return
        addi $a0, $a0, -1          #a0 = ms
       
        addi $t0, $0, 4711              #t0 = i 
        for:
            beq  $t0, $0, while
            addi $t0, $t0, -1
            j    for
            nop

hexasc:
    andi   $a0, $a0, 0xf     #för att maska så att endast första nibble
    slti   $t0, $a0, 10
    
    beq    $0,$t0, letters
    addi   $v0, $a0, 0x30

    jr     $ra
    nop
    
    letters:
    addi   $v0, $a0, 0x37
    jr     $ra
    nop

time2string:
    PUSH $ra
    PUSH $s0
    PUSH $s1
                     
    move $s0, $a0           #flytta till s0 och s1
    move $s1, $a1
                     
    # Första siffran
    srl  $a0, $s1, 12
    jal  hexasc
    nop
    sb   $v0, 0($s0)
    
    # Andra siffran
    srl  $a0, $s1, 8
    jal  hexasc
    nop
    sb   $v0, 1($s0)       #nästa byte
    
    # Kolon
    addi $a0, $0, 0x3a
    sb   $a0, 2($s0)
    
    # Tredje siffran
    srl  $a0, $s1,4
    jal  hexasc
    nop
    sb   $v0, 3($s0)
    
    # Fjärde siffran
    move $a0, $s1
    jal  hexasc
    nop
    sb   $v0, 4($s0)
    
    # Null-byte
    sb   $0, 5($s0)       #null-byte fungerar som slutsignal
    
    POP $s1
    POP $s0
    POP $ra
   
return: 
    jr $ra
    nop
