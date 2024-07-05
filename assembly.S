; -----------------------------------------
;   Programa em Assembly
; -----------------------------------------

#define __SFR_OFFSET 0

; Nesse arquivo são definidos todos os
; registradores do ATMega
#include "avr/io.h"

.global delay_1_ms

; -----------------------------------------

delay_1_ms:
        ldi r31, 4000>>8    ; 1 cycle
        ldi r30, 4000&255   ; 1 cycle
loop:   sbiw r30, 1         ; 4000*2 cycles
        brne loop           ; 3999*2+1 cycles
        ret

