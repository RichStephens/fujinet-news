program fujinews;
{$librarypath '../blibs/'}
uses atari, http_client, crt, b_system, joystick, efast, fn_cookies;

const 
{$i const.inc}
{$r resources.rc}
{$i interrupts.inc}

type TConfig = record
    currentTheme:byte;
end;

var
    url: string[120];
    api: string = 'N:https://fujinet.online/8bitnews/news.php?t=a&ps=39x23';
    //urlCat:  pChar = 'N:https://fujinet.online/8bitnews/news.php?t=a&ps=39x23&l=7&c=                   ';
    //urlPost: pChar = 'N:https://fujinet.online/8bitnews/news.php?t=a&ps=39x23&a=                ';
    responseBuffer: array [0..0] of byte absolute BUFFER_ADDRESS;
    s: string;
    bufPtr: word;
    ids: array [0..7] of string[8];
    b,idNum,idSel: byte;
    hPage:byte;
    dlistNum: byte;
    dliHeadCount: byte;
    selHead: byte;
    userInput: byte;
    inputDelay: byte;
    oldvbl: pointer;
    showSpinner: boolean;
    HELPFG: byte absolute $2DC;
    VDELAY: byte absolute $D01C;
    theme: array [0..THEME_LEN-1] of byte;
    themes: array [0..THEME_COUNT-1,0..THEME_LEN-1] of byte = (
    //  bg  menu    head    HEAD    content spinner
      ( $00,$04,$08,$02,$06,$92,$9f,$92,$9a,$04,$08 ),
      ( $00,$02,$06,$00,$06,$06,$0a,$00,$0a,$02,$04 ),
      ( $0c,$0a,$06,$0a,$06,$0f,$02,$0c,$02,$04,$08 ),
      ( $c0,$c0,$c4,$c2,$c6,$cc,$c2,$c2,$ca,$c4,$c8 ),
      ( $10,$10,$14,$12,$16,$1c,$12,$12,$1a,$14,$18 )
    );
    config: TConfig;
    
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
    

// *************************************************************************************** COOKIE ROUTINES

procedure LoadConfig;
begin
    InitCookie(APPKEY_CREATOR_ID, APPKEY_APP_ID, APPKEY_CONFIG_KEY);
    if GetCookie(pointer(BUFFER_ADDRESS)) = 1 then Move(responseBuffer[0], config, sizeOf(config));
    if config.currentTheme>=THEME_COUNT then config.currentTheme := 0;
end;

procedure SaveConfig;
begin
    InitCookie(APPKEY_CREATOR_ID, APPKEY_APP_ID, APPKEY_CONFIG_KEY);
    SetCookie(@config, sizeOf(config));
end;    
        
// **************************************************************************************** HELPERS  
// ****************************************************************************************
    
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
    
procedure SelectWindow(vram:word);
begin
    savmsc := vram;
    Gotoxy(1,1);    
end;

function GetMaxPage(var pages:string):integer;
var b: byte;
    copy:boolean;
begin
    b:=1;
    copy:=false;
    s[0]:=char(0);
    while (b<=byte(pages[0])) do begin
        if copy then begin
            inc(s[0]);
            s[byte(s[0])]:=pages[b];
        end;
        if pages[b]='/' then copy:=true;
        inc(b);
    end;
    Val(s, result, b);
end;      
        
procedure GetStrFromBuf(strPtr:word;delimiter:char;direct:boolean;lengthLimit:word);     
var b:word;
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
            dec(lengthLimit);
            if lengthLimit = 0 then exit;
        end;
        inc(bufPtr);
    end;
    inc(bufPtr);
end;
    
procedure ClearScr;
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
        Pause;
end;    

procedure InitPMG;
begin
    SDMCTL := %00101110;    
    GPRIOR := $21;    
    PMBASE := Hi(PMG);
    GRACTL := %00000011;
    sizep0 := 0;
    sizep1 := 0;
    sizem  := 0;
    hposp0 := SPINNER_LEFT;
    hposp1 := SPINNER_LEFT;
    hposm0 := VBAR_LEFT;
    hposm1 := VBAR_LEFT;
    vdelay := $0f;
end;

procedure SetTheme(tnum:byte);
begin
    Pause;
    Move(themes[tnum][0],@theme,THEME_LEN);
    color4:=theme[0];
end;

procedure SwitchTheme;
begin
    Inc(config.currentTheme);
    if config.currentTheme = THEME_COUNT then config.currentTheme := 0;
    SetTheme(config.currentTheme);
    SaveConfig;
end;

// *********************************************************************************************** IO
// ***********************************************************************************************
// ***********************************************************************************************

function GetUserInput:byte;
var key:char;
    joy:byte;
    longPress:byte;
    fireDown:boolean;
(*    
procedure ShowKey;
begin
    SelectWindow(VRAM_MENU);
    write(byte(key));
end;
*)
begin
    
    fireDown := false;
    
    repeat
        Pause;
        result := NONE;
        joy := (not stick0) and $f;
        
        if strig0 = 0 then fireDown := true;    // FIRE DOWN
        if fireDown then begin
            inc(longPress);
        end;
        
        if (strig0 = 1) and fireDown then begin // FIRE UP
            fireDown := false;
            if longPress>LONGPRESS_TIME then begin  // LONG PRESS
                result := I_QUIT;
            end else begin                          // SHORT PRESS
                result := I_ENTER;
            end;
            longPress := 0;
        end;

        if joy <> 0 then begin                      // JOY MOVED
            if joy and 1 <> 0 then result := I_UP;
            if joy and 2 <> 0 then result := I_DOWN;
            if joy and 4 <> 0 then result := I_LEFT;
            if joy and 8 <> 0 then result := I_RIGHT;
        end;
        
        if HELPFG = 17 then begin         // HELP
            result := I_HELP;
            HELPFG := 0;
        end;
        
        if (CONSOL and 2) = 0 then begin  // SELECT
            if inputDelay = 0 then SwitchTheme;
            result := I_IDLE;
        end;

        if inputDelay>0 then begin
            Dec(inputDelay);
            if result = NONE then inputDelay := 0;  // NO INPUT
            result := NONE;
        end else begin
            if result <> NONE then inputDelay := I_DELAY;
        end;
                
        if keypressed then begin
            key := ReadKey();
            //showKey;
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

procedure GetUrlData(page:byte);
begin
    url := Concat(url, '&p=');
    Str(page, s);
    url := Concat(url, s);
    url[Length(url)+1]:=char(0);
    SetScreen(0);
    showSpinner := true;
    HTTP_Get(@url[1], @responseBuffer[0]);
    showSpinner := false;
end;

procedure GetUrl;
begin
    str(0,s);
    Concat(s,'');
    Move(@api, @url, byte(api[0])+1);
end;

procedure GetArticle(id, artPage:byte);
var sp:^string;
begin
    GetUrl;
    url := Concat(url, '&a=');
    sp := ids[id];
    url := Concat(url, sp);
    GetUrlData(artPage);
end;

procedure GetCategoryHeaders(hPage:byte);
var sp:^string;
begin
    GetUrl;
    url := Concat(url, '&l=7&c=');
    sp := categories[idSel];
    url := Concat(url, sp);
    GetUrlData(hpage);
end;

// *********************************************************************************************** UI DRAW
// ***********************************************************************************************
// ***********************************************************************************************
// ***********************************************************************************************

procedure DrawPager(p,m:byte);
var ps,pl,b,i:byte;
begin
    if (p = 0) or (m = 0) then begin
        fillByte(pointer(PAGER_OFFSET),92,0);
    end else begin
        pl:=(PAGER_HEIGHT+1) div m;
        ps:=(p-1)*pl;
        for i:=0 to PAGER_HEIGHT do begin
            b:=1;
            if (i>=ps) and (i<=ps+pl+1) then b:=4;
            poke(PAGER_OFFSET + i, b);
        end;
    end;
end;

procedure ShowCategories;
begin
    savmsc := VRAM_CONTENT;
    for idNum := 0 to 8 do begin
        if idNum = idSel then begin
            s[0] := char(peek(word(categories[idNum])));
            for b := 1 to peek(word(categories[idNum])) do 
                s[b] := char(peek(word(categories[idNum]) + b) + 128);
        end else 
            Move(categories[idNum], @s[0], peek(word(categories[idNum])) + 1);
        Gotoxy( ((idNum and 1) shl 4) + 6, ((idNum shr 1) shl 1) + 12);
        Write(s);
    end;
end;

procedure ShowHelp;
begin
    SetScreen(0);
    ClearScr;
    SelectWindow(VRAM_MENU);
    Write(' FujiNews HELP');
    SelectWindow(VRAM_STATUS);
    Write(' '+'Press any key to continue'*);
    SelectWindow(VRAM_CONTENT);
    lmargin := 1;
    Writeln;
    Writeln('Navigate the application using');
    Writeln('the arrow keys or joystick. ');
    Writeln;
    Writeln('Return'*' opens documents,');
    Writeln;
    Writeln('Escape'*' gets you backward,');
    Writeln;
    Writeln('TAB'*' shows article details,');
    Writeln;
    Writeln('Press Fire'*' to enter article,');
    Writeln;
    Writeln('Hold Fire'*' for a second to go back,');
    Writeln;
    Writeln('SELECT'*' switches colour themes.');
    lmargin := 0;
    repeat
        userInput := getUserInput();
    until userInput <> NONE;
end;

procedure ShowMainMenu;
begin
    SetScreen(0);
    ClearScr;
    DrawPager(0,0);

    SelectWindow(VRAM_MENU);
    Write(' FujiNews v.0.98.en');
    SelectWindow(VRAM_STATUS);
    Write(' ','HELP'*' helps');

    savmsc := VRAM_CONTENT;
    // draw logo
    Move(logo[0*13],pointer(savmsc+40*1+2),13);
    Move(logo[1*13],pointer(savmsc+40*2+2),13);
    Move(logo[2*13],pointer(savmsc+40*3+2),13);
    Move(logo[3*13],pointer(savmsc+40*4+2),13);

    Gotoxy(17,3);
    Write('News reader client');
    Gotoxy(17,4);
    Write('by bocianu@gmail.com');
    Gotoxy(3,7);
    Write('server: fujinet.online/8bitnews/'*);
    Gotoxy(3,10);
    Write('Select News Category:');

    repeat
        ShowCategories();
        userInput := getUserInput();
        case userInput of
            I_RIGHT,I_SPACE :   Inc(idSel);
            I_LEFT :            Dec(idSel);
            I_DOWN :            Inc(idSel,2);
            I_UP :              Dec(idSel,2);
            I_HELP :            begin
                ShowHelp;
                userInput := I_RELOAD;
            end;
        end;
        if idSel>12 then idSel:=8;
        if idSel>8 then idSel:=0;
    until (userInput = I_ENTER) or (userInput = I_QUIT) or (userInput = I_RELOAD);
    selHead:=0;
    hPage:=1;
end;    

procedure ShowHeaders;
var hPages:string[20];
    maxPage:integer;
    reload:boolean;
begin
    repeat
        ClearScr;
        reload:=false;
        SelectWindow(VRAM_MENU);
        Write(' Fetching Headers '*);
        GetCategoryHeaders(hpage);
        SelectWindow(VRAM_STATUS);    
           
        if HTTP_error <> 1 then begin
            write(' Error: ', HTTP_error);
            Pause(100);
            userInput := I_QUIT;
        end else begin

            SetScreen(1);
            Write(' Received: ',HTTP_respSize,' bytes');
            SelectWindow(VRAM_MENU);
            Write(' Select Article    ');

            Gotoxy(40-peek(word(categories[idSel])),1);
            s[0] := char(peek(word(categories[idSel])));
            for b := 1 to peek(word(categories[idSel])) do 
            s[b] := char(peek(word(categories[idSel]) + b) + 128);
            Write(s);

            bufPtr := 0;
            GetStrFromBuf(word(@hPages), char($9b), false, 20);
            
            maxPage := GetMaxPage(hPages);
            DrawPager(hPage,maxPage);

            savmsc := VRAM_STATUS;
            Gotoxy(35 - Length(hPages),1);
            Write('Page ',hPages);
            
            // write headers
            idNum := 0;
            while(idNum < HEAD_MAX) and (bufPtr < HTTP_respSize) do begin
                GetStrFromBuf(word(ids[idNum]),'|', false,8);
                GetStrFromBuf(VRAM_CONTENT + idnum shl 8 + 21,'|', true,0);
                GetStrFromBuf(LINE_WIDTH + VRAM_CONTENT + idnum shl 8,char($9b),true,216);
                inc(idNum);
            end;
            dec(idNum);
            
            // if selected header do not exist, select last existing
            if selHead>idNum then selHead:=idNum;
            
            repeat
                userInput := getUserInput();
                case userInput of
                    I_DOWN,I_SPACE  : Inc(selHead);
                    I_UP            : Dec(selHead);
                    I_RIGHT: begin  // next
                        Inc(hPage);
                        reload := true;
                    end;
                    I_LEFT: begin           // prev
                        Dec(hPage);
                        reload := true;
                    end;
                end;
                // if goes below 0 switch to prev page
                if selHead=$ff then begin
                    selHead := HEAD_MAX;
                    Dec(hPage);
                    reload := true;
                // if goes above max header switch to next page
                end else if selHead>idNum then begin
                    selHead:=0;
                    Inc(hPage);
                    reload := true;
                end;
                // keep hpage in pages range - loop from last to first
                if reload then begin
                    if hPage<1 then hPage := maxPage;
                    if hPage>maxPage then hPage := 1;
                end;
                
            until (userInput = I_QUIT) or (userInput = I_ENTER) or reload;            
            if userInput = I_QUIT then selHead := NONE;
        end;
    until (userInput = I_QUIT) or (userInput = I_ENTER);            
end;

procedure ShowArticle(id: byte);
var artPage:byte;
    artTitle:string;
    artDate:string[20];
    artPages:string[20];
    artSource:string;
    quit, reload:boolean;
    maxPage:integer;
begin
    quit := false;
    reload := false;
    artPage := 1;
    SetScreen(0);
    repeat
        
        ClearScr;
        
        SelectWindow(VRAM_MENU);
        Write(' Fetching Article '*);
        SelectWindow(VRAM_STATUS);
        GetArticle(id, artPage);
        
        if HTTP_error <> 1 then begin
            write(' Error: ', HTTP_error);
            Pause(50);
            quit:=true;
        end else begin
    
            Write(' Received: ',HTTP_respSize,' bytes');
            bufPtr := 0;
            GetStrFromBuf(word(@artTitle), char($9b), false, 255); // title
            GetStrFromBuf(word(@artDate), char($9b), false, 20);
            GetStrFromBuf(word(@artSource), char($9b), false, 255);
            GetStrFromBuf(word(@artPages), char($9b), false, 20);
            
            maxPage := GetMaxPage(artPages);
            DrawPager(artPage,maxPage);

            savmsc := VRAM_STATUS;
            Gotoxy(35 - Length(artPages),1);
            Write('Page ',artPages);

            SelectWindow(VRAM_MENU);            
            Write(' ',artDate);
            Gotoxy(27,1);
            Write(' TAB '*' Details');
            
            SelectWindow(VRAM_CONTENT);

            while(bufPtr<HTTP_respSize) do begin
                s[0]:=char(0);
                GetStrFromBuf(word(@s), char($9b), false, 23*39);
                Writeln(s);
            end;    
                
            repeat
                userInput := getUserInput();
                case userInput of
                    I_QUIT: begin
                        quit := true;
                    end;
                    I_RIGHT, I_DOWN, I_SPACE, I_ENTER: begin  // next
                        Inc(artPage);
                        reload := true;
                    end;
                    I_LEFT, I_UP: begin           // prev
                        Dec(artPage);
                        reload := true;
                    end;
                    I_TAB: begin  // tab - title
                        ClearScr;
                        SelectWindow(VRAM_MENU);                        
                        Write(' '+'ARTICLE DETAILS:'*);
                        SelectWindow(VRAM_CONTENT);                        
                        Writeln('Date:'*' ',artDate);
                        Writeln;
                        Writeln('Title:'*);
                        Writeln(artTitle);
                        Writeln;
                        Writeln('Source:'*' ',artSource);
                        Writeln;
                        SelectWindow(VRAM_STATUS);
                        Write(' '+'Press any key to continue'*);
                        Readkey;
                        reload := true;
                    end;
                    else begin
                        reload := false;
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
    
    LoadConfig();
    SetTheme(config.currentTheme);
    // copy system charset to user location
    Move(pointer($e000), pointer(CHARSET), $400);
    // update with Fujinet logo characters
    Move(pointer(LOGO_CHARSET), pointer(CHARSET + $200), $100);

    Pause; 
    InitPMG;
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
        if (userInput <> I_QUIT) and (userInput <> I_RELOAD) then begin
            repeat 
                    ShowHeaders; // sets selHead (selected Header) of $ff - NONE
                    if selHead <> NONE then begin
                        ShowArticle(selHead);
                    end;
            until userInput = I_QUIT;
            userInput := NONE;
        end;
    until userInput = I_QUIT;

    Pause;
    SDMCTL := %00100010;    
    GRACTL := %00000000;    
    SetIntVec(iVBL, oldvbl);
    nmien := $40;
    TextMode(0);

end.
