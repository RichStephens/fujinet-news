(* declare your interrupt routines here *)

procedure dlitop;assembler;interrupt;
asm {
    phr ; store registers
    
    lda adr.theme+1
    ldx adr.theme+2
    sta wsync
    sta atari.colpf2
    stx atari.colpf1

    lda dlistNum
    beq dl0

dl1
    mwa #dliheader atari.vdslst
    lda #0 
    sta dliHeadCount
    jmp dli_end
  
dl0
    mwa #dlicontent atari.vdslst
    
dli_end    
    plr ; restore registers
};
end;

procedure dliheader;assembler;interrupt;
asm {
    phr ; store registers
    
    ldx adr.theme
    lda adr.theme
    add #4
    sta wsync
    stx atari.colpf2
    sta atari.colpf1

    :7 sta wsync

    lda dliHeadCount
    cmp selHead
    beq selected

    lda adr.theme+3
    ldx adr.theme+4
    jmp @+
selected
    lda adr.theme+5
    ldx adr.theme+6
@
    sta wsync
    sta atari.colpf2
    stx atari.colpf1


    inc dliHeadCount
    lda dliHeadCount
    cmp #7
    bne @+
    mwa #dlibottom atari.vdslst
@    
    plr ; restore registers
};
end;

procedure dlicontent;assembler;interrupt;
asm {
    phr ; store registers
    
    lda adr.theme+7
    ldx adr.theme+8
    sta wsync
    sta atari.colpf2
    stx atari.colpf1

    mwa #dlibottom atari.vdslst
    
    plr ; restore registers
};
end;

procedure dlibottom;assembler;interrupt;
asm {
    phr ; store registers
    
    lda adr.theme+1
    ldx adr.theme+2
    sta wsync
    sta atari.colpf2
    stx atari.colpf1

    mwa #dlitop atari.vdslst
    
    ldy #0
    lda showSpinner
    bne drawspinner

clearSpinner
@
    lda #0
    sta PMG + $200 + SPINNER_TOP,y
    sta PMG + $280 + SPINNER_TOP,y
    iny
    cpy #7
    bne @-
    
    jmp spinnerEnd
    
drawspinner    
    lda rtclok+2 
    lsr 
    and #7
    asl:asl:asl
    tax
@
    lda SPRITES,x
    sta PMG + $200 + SPINNER_TOP,y
    lda SPRITES+64,x
    sta PMG + $280 + SPINNER_TOP,y
    inx 
    iny
    cpy #7
    bne @-

spinnerEnd
    
    plr ; restore registers
};
end;


procedure vbl;assembler;interrupt;
asm {
    phr ; store registers
    
    mwa #dlitop atari.VDSLST

    lda adr.theme
    sta atari.color4
    lda #2
    sta atari.color2
    lda #0
    sta atari.color1
    
    

    plr ; restore registers
    jmp $E462 ; jump to system VBL handler
};
end;
