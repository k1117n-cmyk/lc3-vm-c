.ORIG x3000

        AND R1, R1, #0
        ADD R1, R1, #3
LOOP    LEA R0, MSG
        PUTS
        ADD R1, R1, #-1
        BRp LOOP
        HALT

MSG     .STRINGZ "Hi\n"

.END
