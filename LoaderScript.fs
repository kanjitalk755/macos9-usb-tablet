: b64                       ( end-ptr ptr -- ) \ appends result to data area
do                          \ iterate over input
    0                       \ accum

    4 0 do                  \ decode 4 bytes
        6 lshift
        i j + c@            \ accum byte
        2e -
        dup b > 7 and -
        dup 25 > 6 and -
        or                  \ accum
    loop

    3 0 do                  \ encode 3 bytes
        dup c, 8 rshift
    loop

    drop
4 +loop
;

\ Maintain the LZSS dictionary
variable lzd                    \ lzss dictionary base
variable lzi                    \ lzss dictionary index
: lzp                           ( byte-ptr -- )
    c@ dup c,
    lzd @ lzi @ + c!
    lzi @ 1+ fff and lzi !
;

: lz                            ( end-ptr ptr scratch-ptr -- ) \ appends result to data area
lzd !                           ( end-ptr ptr )
fee lzi !                       \ dict ptr doesn't start at zero??
1 -rot                          ( control-byte end-ptr ptr )
do                              ( control-byte )
    dup 1 = if
        \ get a new control-byte
        drop i c@ 100 +
        1                       ( +loop-increment )
    else
        dup 2 / swap 1 and if
            \ LITERAL
            i lzp
            1                   ( +loop-increment )
        else
            \ DICTIONARY
            i 1+ c@ i c@ over   ( byte2 byte1 byte2 )
            f0 and 4 lshift +   ( byte2 dict-offset )
            swap f and          ( dict-offset count-3 )

            -3 do
                dup lzd @ +     ( dict-offset new-char-ptr )
                lzp             ( dict-offset )
                1+ fff and      ( next-dict-offset )
            loop
            drop                ( )

            2                   ( +loop-increment )
        then
    then
+loop
drop
;

20 parse ~                      ( b64-ptr b64-len )
over + swap                     ( b64-end-ptr b64-ptr ) \ our preferred form

here >r 2000 allot              \ make space for compression slop

here >r
b64
here r>                         ( lzss-end-ptr lzss-ptr )

here                            ( lzss-end-ptr lzss-ptr lzss-scratch-ptr )
r@ here!                        \ rewind over the slop area
lz
here r>                         ( final-ptr final-base )

tuck -                          ( ptr len ) \ OF's preferred form

dev /
" pef,AAPL,MacOS,PowerPC,prepare" property
" Tab" encode-string " code,AAPL,MacOS,name" property
\ 2000000 encode-int " AAPL,debug" property
