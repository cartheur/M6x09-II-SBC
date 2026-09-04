* ASSIST09 terminal workflow smoke test.
* Load this S-record into RAM at $1000, then execute G 1000.

        ORG     $1000

PDATA   EQU     3
MONITR  EQU     8
EOT     EQU     $04

START   LDX     #MESSAGE
        SWI
        FCB     PDATA
        SWI
        FCB     MONITR

MESSAGE FCC     /ASSIST09 RAM SMOKE TEST PASSED/
        FCB     EOT
