program fujinews;
{$librarypath '../blibs/'}
uses atari, http_client, crt, b_system;

const 
{$i const.inc}
{$r resources.rc}
{$i interrupts.inc}

var
    urlCat:  pChar = 'N:https://fujinet.online/8bitnews/news.php?t=a&ps=39x23&l=7&c=                   ';
    urlPost: pChar = 'N:https://fujinet.online/8bitnews/news.php?t=a&ps=39x23&a=                ';
    responseBuffer: array [0..0] of byte absolute BUFFER_ADDRESS;
    s: string;
    bufPtr: word;
    ids: array [0..80] of byte;
    b,idNum,idSel: byte;
    dlistNum: byte;
    dliHeadCount: byte;
    selHead: byte;
    showSpinner: boolean;
    userInput: byte;
    oldvbl: pointer;
    
    theme: array [0..8] of byte = (
        //border    //menuBG    //menuFG    //headBG    //headFG    //headBGon  //headFGon //contentBG //contentFG
        $0,         $04,        $08,        $02,         $06,        $92,         $9f,       $92,        $9a
    );
    
    cat0: string = 'top';
    cat1: string = 'world';
    cat2: string = 'science';
    cat3: string = 'business';
    cat4: string = 'technology';
    cat5: string = 'health';
    cat6: string = 'entertainment';
    cat7: string = 'politics';
    cat8: string = 'sports';
    categories: array [0..8] of pointer = (@cat0, @cat1, @cat2, @cat3, @cat4, @cat5, @cat6, @cat7, @cat8);

    logo: array [0..13*4-1] of byte = (
        $00, $00, $00, $00, $00, $5a, $5b, $5c, $00, $00, $00, $00, $00, 
        $40, $41, $42, $43, $44, $54, $55, $56, $4a, $4b, $4c, $4d, $4e, 
        $45, $46, $47, $48, $49, $57, $58, $59, $4f, $50, $51, $52, $53, 
        $00, $00, $00, $00, $00, $00, $5d, $5e, $5f, $00, $00, $00, $00
    );    
    
    
function ShouldISkip(c:byte):boolean;
begin
    result:=false;
    if c > 127 then exit(true);
end;

function Atascii2Antic(c: byte): byte; assembler; overload;
asm 
        lda c
        asl
        php
        cmp #2*$60
        bcs @+
        sbc #2*$20-1
        bcs @+
        adc #2*$60
@       plp
        ror
        sta result;
end;
        
procedure getStrFromBuf(strPtr:word;delimiter:char;direct:boolean);     
var b:byte;
    c:byte;
begin
    b := 1;
    if direct then b := 0;
    while (responseBuffer[bufPtr] <> byte(delimiter)) and (responseBuffer[bufPtr] <> $9b) do begin
        c := responseBuffer[bufPtr];
        if not ShouldISkip(c) then begin
            if direct then c := Atascii2Antic(c);
            poke(strPtr + b, c);
            if not direct then poke(strPtr, b);
            inc(b);
        end;
        inc(bufPtr);
    end;
    inc(bufPtr);
end;
    
procedure selectWindow(vram:word);
begin
    savmsc := vram;
    gotoxy(1,1);    
end;
    
procedure clearScr;
begin
    fillbyte(pointer(VRAM_ADDRESS),VRAM_SIZE,0);
end;   
    
procedure SetScreen(snum:byte);
var a:array [0..1] of word = (DISPLAY_LIST_ADDRESS1,DISPLAY_LIST_ADDRESS2);
begin
        Pause();
        dlistNum := snum;
        SDLSTL := a[snum];
        savmsc := VRAM_STATUS;
        nmien := $c0;
end;    

procedure showCategories;
begin

    savmsc := VRAM_CONTENT;
    for idNum := 0 to 8 do begin
            if idNum = idSel then begin
                s[0] := char(peek(word(categories[idNum])));
                for b := 1 to peek(word(categories[idNum])) do 
                    s[b] := char(peek(word(categories[idNum]) + b) + 128);
            end else 
                move(categories[idNum], @s[0], peek(word(categories[idNum])) + 1);
            gotoxy( ((idNum and 1) shl 4) + 6, ((idNum shr 1) shl 1) + 12);
            Write(s);
    end;
end;
    

function getUserInput:byte;
var key:char;

procedure ShowKey;
begin
    selectWindow(VRAM_MENU);
    write(byte(key));
end;

begin
    result := NONE;
    repeat
        pause;
        if keypressed then begin
            key := ReadKey();
            case key of
                'q','Q',char(27): result := I_QUIT;
                char(29),char(61): result := I_DOWN;
                char(28),char(45): result := I_UP;
                char(31),char(42): result := I_RIGHT;
                char(30),char(43): result := I_LEFT;
                char(32): result := I_SPACE;
                char(155): result := I_ENTER;
                char(127): result := I_TAB;
            end;
        end;
    until result <> NONE;
end;

procedure ShowMainMenu;
begin
    SetScreen(0);
    clearScr;

    selectWindow(VRAM_MENU);
    Write(' FujiNews v.0.7');
    selectWindow(VRAM_STATUS);
    Write(' ','HELP'*' helps');

    savmsc := VRAM_CONTENT;
    // draw logo
    move(logo[0*13],pointer(savmsc+40*1+2),13);
    move(logo[1*13],pointer(savmsc+40*2+2),13);
    move(logo[2*13],pointer(savmsc+40*3+2),13);
    move(logo[3*13],pointer(savmsc+40*4+2),13);

    Gotoxy(17,3);
    Write('News reader client');
    Gotoxy(17,4);
    Write('by bocianu@gmail.com');
    Gotoxy(3,7);
    Write('server: fujinet.online/8bitnews/'*);
    Gotoxy(3,10);
    Write('Select News Category:');

    repeat
        showCategories();
        userInput := getUserInput();
        case userInput of
            I_RIGHT:    Inc(idSel);
            I_LEFT:     Dec(idSel);
            I_DOWN:     Inc(idSel,2);
            I_UP:       Dec(idSel,2);
        end;
        if idSel>12 then idSel:=8;
        if idSel>8 then idSel:=0;
    until (userInput = I_ENTER) or (userInput = I_QUIT);

end;    

procedure GetCategoryHeaders;
begin
    for b:=1 to peek(word(categories[idSel])) do urlCat[61 + b] := char(peek(word(categories[idSel]) + b));
    urlCat[61 + b]:=char(0);
    pause;
    nmien := $40;
    HTTP_Get(urlCat, @responseBuffer);
    nmien := $c0;
end;

procedure GetArticle(id, artPage:byte);
var i:byte;
begin
    i := id shl 3;
    for b := 1 to ids[i] do urlPost[57 + b] := char(ids[i + b]);
    i := b + 56;
    s := '&p=';
    for b := 1 to byte(s[0]) do urlPost[i + b] := s[b];
    i := b + 59;
    Str(artPage, s);
    Str(artPage, s);
    for b := 1 to byte(s[0]) do urlPost[i + b] := s[b];
    
    urlPost[i + b] := char(0);

    pause;
    nmien := $40;
    HTTP_Get(urlPost, @responseBuffer);
    nmien := $c0;
end;

procedure ShowHeaders;
begin
    selHead := NONE;
    clearScr;
    GetCategoryHeaders;
    selectWindow(VRAM_STATUS);       
    if HTTP_error <> 1 then write(' Error: ', HTTP_error)
    else begin

        SetScreen(1);
        Write(' Received: ',HTTP_respSize,' bytes');

        selectWindow(VRAM_MENU);
        Write(' Select Article ');

        gotoxy(40-peek(word(categories[idSel])),1);
        s[0] := char(peek(word(categories[idSel])));
        for b := 1 to peek(word(categories[idSel])) do 
        s[b] := char(peek(word(categories[idSel]) + b) + 128);
        Write(s);

        bufPtr := 0;
        idNum := 0;
        
        while(idNum<7) do begin
            getStrFromBuf(word(@ids[idNum shl 3]),'|', false);
            getStrFromBuf(VRAM_CONTENT + idnum shl 8 + 21,'|', true);
            getStrFromBuf(LINE_WIDTH + VRAM_CONTENT + idnum shl 8,char($9b), true);
            inc(idNum);
        end;
        
        if selHead>6 then selHead:=0;
        
        repeat
            userInput := getUserInput();
            case userInput of
                I_DOWN,I_RIGHT,I_SPACE  : Inc(selHead);
                I_UP,I_LEFT             : Dec(selHead);
            end;
            if selHead>12 then selHead:=6;
            if selHead>6 then selHead:=0;
        until (userInput = I_QUIT) or (userInput = I_ENTER);            
        if userInput = I_QUIT then selHead := NONE;
    end;
end;

procedure ShowArticle(id: byte);
var artPage:byte;
    artTitle:string;
    artDate:string[20];
    artPages:string[20];
    artSource:string;
    copy, quit, reload:boolean;
    b: byte;
    maxPage:integer;
begin
    quit := false;
    reload := false;
    artPage := 1;
    SetScreen(0);
    repeat
        
        clearScr;
        selectWindow(VRAM_STATUS);
        GetArticle(id, artPage);
        
        if HTTP_error <> 1 then begin
            write(' Error: ', HTTP_error);
            pause(50);
            quit:=true;
        end else begin
    
            Write(' Received: ',HTTP_respSize,' bytes');
            bufPtr := 0;
            getStrFromBuf(word(@artTitle), char($9b), false); // title
            getStrFromBuf(word(@artDate), char($9b), false);
            getStrFromBuf(word(@artSource), char($9b), false);
            getStrFromBuf(word(@artPages), char($9b), false);
            
            b:=1;
            copy:=false;
            s[0]:=char(0);
            while (b<=byte(artPages[0])) do begin
                if copy then begin
                    inc(s[0]);
                    s[byte(s[0])]:=artPages[b];
                end;
                if artPages[b]='/' then copy:=true;
                inc(b);
            end;
            Val(s, maxPage, b);

            savmsc := VRAM_STATUS;
            Gotoxy(35 - Length(artPages),1);
            Write('Page ',artPages);

            selectWindow(VRAM_MENU);            
            Write(' ',artDate);
            gotoxy(27,1);
            Write(' TAB '*' Details');
            
            selectWindow(VRAM_CONTENT);

            while(bufPtr<HTTP_respSize) do begin
                s[0]:=char(0);
                getStrFromBuf(word(@s), char($9b), false);
                Writeln(s);
            end;    
                
            repeat

                userInput := getUserInput();

                case userInput of
                    I_QUIT: begin
                        quit := true;
                    end;
                    I_RIGHT, I_DOWN, I_SPACE: begin  // next
                        Inc(artPage);
                        reload := true;
                    end;
                    I_LEFT, I_UP: begin           // prev
                        Dec(artPage);
                        reload := true;
                    end;
                    I_TAB: begin  // tab - title
                        clearScr;
                        selectWindow(VRAM_MENU);                        
                        Write(' '+'ARTICLE DETAILS:'*);
                        selectWindow(VRAM_CONTENT);                        
                        Writeln('Date: ',artDate);
                        Writeln;
                        Writeln('Title:');
                        Writeln;
                        Writeln(artTitle);
                        Writeln;
                        Writeln('Source:');
                        Writeln;
                        Writeln(artSource);
                        Writeln;
                        selectWindow(VRAM_STATUS);
                        Write(' '+'Press any key to continue'*);
                        Readkey;
                        reload := true;
                    end;
                end;
                
                if reload then begin
                    if artPage<1 then artPage := maxPage;
                    if artPage>maxPage then artPage := 1;
                end;
                
            until quit or reload;            

        end;

    until quit;
    userInput := NONE;
end;

// ************************************************************************************************************
// ************************************************************************************************************
// ************************************************************************************************************
// ************************************************************************************************************
// ************************************************************************************************************

begin

    move(pointer($e000), pointer(CHARSET), $400);
    move(pointer(LOGO_CHARSET), pointer(CHARSET + $200), $100);

    pause;
    nmien := $0;
    GetIntVec(iVBL, oldvbl);
    SetIntVec(iVBL, @vbl);
    chbas := Hi(CHARSET); 
    nmien := $40;
    
    CursorOff;
    lmargin := 0;

    idSel := 0;
    
    repeat
        
        ShowMainMenu; // idSel is set (selected category Id)

        if userInput <> I_QUIT then begin
            repeat 
            
                    ShowHeaders; // sets selHead (selected Header) of $ff - NONE
                    if selHead <> NONE then begin
                        ShowArticle(selHead);
                    end;
                
            until userInput = I_QUIT;
            userInput := NONE;
        end;
        
    until userInput = I_QUIT;

    pause;
    SetIntVec(iVBL, oldvbl);
    nmien := $40;
    TextMode(0);

end.
