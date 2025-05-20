; x64 Native Tools Command Prompt for VS 2022
; C:\ASM_PROJEKT>ml64 DmytroMahaliuk_55722_comprs_decomprs.asm /link /entry:main
; https://metanit.com/assembler/tutorial/1.4.php

; obliczenia dla opcji pesymistycznej: (liczba elementów / bufor) * (bufor + 2)
; obliczenia dla opcji średniej: (liczba elementów / bufor) * (bufor / 2)
; obliczenia dla opcji optymistycznej: (liczba elementów / bufor) * 2

includelib kernel32.lib  
 
extrn GetStdHandle: proc
extrn ReadFile: proc
extrn WriteFile: proc
extrn CreateFileA: proc
extrn CloseHandle: proc
 
.data
buffer byte 32 dup (?)              ; bufor do odczytu
bufferLen = $ - buffer              ; długość bufora  ($ oznacza bieżący adres w momencie wykonywania tej linii w kodzie)

debuffer byte 2 dup (?)             ; bufor do odczytu
debufferLen = $ - debuffer          ; długość bufora

bufferName byte 64 dup (?)          ; bufor do odczytu danych
bufferNameLen = $ - bufferName

filename byte 64 dup (?)            ; bufor dla ciągu znaków z zerem kończącym
filenameout byte 64 dup (?)         ; bufor dla ciągu znaków z zerem kończącym

precompresbufferLen dq 0            ; 8 bajtów pamięci

compressedBuffer db 34 dup (0)      ; Bufor dla skompresowanych danych
compressedLen = $ - compressedBuffer; Długość skompresowanych danych

badOpenMsg byte "error", 10
badOpenMsgLen = $ - badOpenMsg
goodWriteMsg byte "success", 10
goodWriteMsgLen = $ - goodWriteMsg

fHandle qword 0                     ; deskryptor pliku  (8 bajtów pamięci)
fHandleout qword 0                  ; deskryptor pliku wyjściowego
stdin qword 0                       ; deskryptor wejścia konsoli
stdinSet byte 0                     ; flaga dla stdin
stdout qword 0                      ; deskryptor wyjścia konsoli
stdoutSet byte 0                    ; czy deskryptor wyjścia konsoli został ustawiony
decompresflag byte 0                ; flaga dekompresji


.code

; Procedura zapisu danych do pliku lub konsoli
; Parametry:
; RSI - adres ciągu znaków
; RCX - długość ciągu znaków
; RAX - deskryptor pliku
; Wynik - w RAX zwracana jest liczba zapisanych bajtów
write proc
  sub  rsp, 56          ; 40 bajtów (5 parametrów WriteLine) + 8 bajtów (bytesWritten) + 8 bajtów wyrównania
  mov rdx, rsi          ; Drugi parametr - ciąg znaków
  mov r8, rcx           ; Trzeci parametr - długość ciągu znaków
  mov  rcx, rax         ; Pierwszy parametr WriteFile - do rejestru RCX przenosimy deskryptor pliku - wyjścia konsoli
  lea  r9, bytesWritten       ; Czwarty parametr WriteFile - adres do uzyskania zapisanych bajtów
  mov qword ptr [rsp + 32], 0  ; Piąty parametr WriteFile
  call WriteFile
     
  test rax, rax ; sprawdzamy, czy wystąpił błąd
  mov eax, bytesWritten ; jeśli wszystko w porządku, umieszczamy w RAX liczbę zapisanych bajtów
  jnz exit 
  mov rax, -1 ; Przez RAX zwracamy kod błędu
exit:
  add  rsp, 56
  ret
bytesWritten equ [rsp+40]
write endp

; Procedura wyświetlania dowolnego ciągu znaków na konsoli
; Parametry
; RSI - adres ciągu znaków
; RCX - liczba znaków
; Wynik - w RAX liczba zapisanych bajtów lub -1, jeśli wystąpił błąd
writeToConsole proc
  cmp stdoutSet, 1
  jz writeData    ; jeśli deskryptor wyjścia konsoli jest ustawiony
  sub rsp, 32
  push rcx            ; zapisujemy liczbę znaków
  mov rcx, -11         ; Argument dla GetStdHandle - STD_OUTPUT
  call GetStdHandle     ; wywołujemy funkcję GetStdHandle
  pop rcx         ; przywracamy RCX - liczbę znaków
  add rsp, 32
  mov stdout, rax   ; umieszczamy w stdout deskryptor wyjścia konsoli
  mov stdoutSet, 1  ; deskryptor wyjścia konsoli jest ustawiony
writeData:
  mov rax, stdout
  call write
  ret
writeToConsole endp

; Procedura odczytu danych
; Parametry: 
; RAX - deskryptor pliku, 
; RDI - bufor do odczytu
; RCX - długość bufora
; Wynik - w RAX liczba odczytanych bajtów lub -1, jeśli wystąpił błąd
read proc
  sub rsp, 56
  mov r8, rcx   ; Trzeci parametr ReadFile - rozmiar bufora
  mov rcx, rax  ; Pierwszy parametr ReadFile - deskryptor pliku
  mov rdx, rdi  ; Drugi parametr ReadFile - adres bufora
  lea r9, bytesRead   ; Czwarty parametr ReadFile - liczba odczytanych bajtów
  mov qword ptr [rsp + 32], 0      ; Piąty parametr ReadFile - 0
  call ReadFile       ; wywołanie funkcji ReadFile
  test rax, rax       ; sprawdzamy, czy wystąpił błąd
  mov eax, bytesRead  ; jeśli nie ma błędu, w RAX umieszczana jest liczba odczytanych bajtów
  jnz exit            ; jeśli RAX jest różne od zera
  mov rax, -1         ; umieszczamy kod błędu
exit:
  add rsp, 56
  ret
bytesRead equ [rsp+40] ; obszar w stosie do przechowywania liczby odczytanych bajtów
read endp

; Otwarcie do zapisu: 040000000h
; Otwarcie do odczytu: 080000000h
; Otwarcie do zapisu i odczytu: 0C0000000h
; Otwarcie do zapisu i dopisywania (jeśli plik istnieje): 080000004h

; mov [rsp + 32]:
; 1: utworzyć nowy plik, jeśli nie istnieje, w przeciwnym razie otworzyć istniejący
; 2: zawsze tworzyć od nowa, nawet jeśli plik o takiej nazwie już istnieje
; 3: otworzyć istniejący plik
; 4: zawsze otworzyć plik. Jeśli nie istnieje, zostanie utworzony

; Procedura otwierania istniejącego pliku do odczytu
openFile_r proc
  sub rsp, 56
  mov rcx, rsi 
  mov rdx, 080000000h 
  xor r8, r8 
  xor r9, r9 
  mov r10, 3 ; Otwórz istniejący plik, jeśli nie istnieje - błąd
  mov [rsp + 32], r10
  mov r10, 128  
  mov [rsp + 40], r10 
  mov [rsp + 48], r9    
  call CreateFileA
 
  add rsp, 56
  ret
openFile_r endp

; Процедура открытия файла для записи
openFile_w proc
  sub rsp, 56
  mov rcx, rsi 
  mov rdx,  040000000h 
  xor r8, r8 
  xor r9, r9 
  mov r10, 2 ; Tworzy plik lub nadpisuje istniejący
  mov [rsp + 32], r10
  mov r10, 128  
  mov [rsp + 40], r10 
  mov [rsp + 48], r9    
  call CreateFileA
 
  add rsp, 56
  ret
openFile_w endp
 
; Procedura zamykania pliku
closeFile proc
  sub rsp, 40
  mov rcx, rax      
  call CloseHandle
  add rsp, 40
  ret
closeFile endp

; Procedura odczytu danych z konsoli
; Parametry: 
; RDI - bufor do odczytu
; RCX - długość bufora
; Wynik - w RAX liczba odczytanych bajtów lub -1, jeśli wystąpił błąd
readFromConsole proc
  cmp stdinSet, 1     ; jeśli deskryptor standardowego wejścia jest ustawiony
  jz readData         ; przechodzimy do odczytu danych
  sub rsp, 32
  push rcx            ; zapisujemy liczbę znaków
  mov  rcx, -10         ; Argument dla GetStdHandle - STD_INPUT
  call GetStdHandle     ; wywołujemy funkcję GetStdHandle
  mov stdin, rax        ; zapisujemy deskryptor standardowego wejścia
  mov stdinSet, 1
  pop rcx
  add rsp, 32
readData:
  mov rax, stdin
  call read
  ret
readFromConsole endp

; Kopiowanie danych i dodanie znaku null
copyBufferWithNullTerminator proc
  sub rcx, 1
  mov rbx, rcx          
copyLoop:

  mov al, byte ptr [rsi] 
  mov byte ptr [rdi], al 
  inc rsi
  inc rdi
  dec rcx

  jnz copyLoop
  dec rdi
  mov byte ptr [rdi], 0
  mov rax, rbx          
  ret
copyBufferWithNullTerminator endp

rleCompress proc
    ; Rejestry:
    ; rsi - wskaźnik na wejściowy ciąg znaków
    ; rdi - wskaźnik na bufor do kompresji
    ; rcx - długość wejściowego ciągu
    ; rbx - licznik powtórzeń znaku
    ; rax - bieżący znak
    ; rdx - następny znak
    ; r10
    
    lea rsi, buffer             ; Wskaźnik na wejściowy ciąg znaków
    lea rdi, compressedBuffer        ; Wskaźnik na bufor wyjściowy
    mov rcx, precompresbufferLen             ; Długość wejściowego ciągu
    mov r10,0

    xor rbx, rbx                     ; Licznik powtórzeń
    mov al, [rsi]                    ; Pierwszy znak

    inc rsi
    dec rcx
nextChar:
    cmp rcx, 0                       ; Sprawdzamy, czy pozostały jakieś znaki
    je finish

    mov dl, byte ptr [rsi]           ; Następny znak
    cmp al, dl                       ; Porównanie bieżącego i następnego znaku
    jne writeToBuffer                ; Jeśli różne, zapisujemy do bufora

    inc rbx                          ; Zwiększamy licznik powtórzeń
    inc rsi
    dec rcx
    jmp nextChar
writeToBuffer:
    inc rbx                          ; Dodajemy 1 do licznika (zliczamy bieżący znak)
    add bl, 32                       ; Konwertujemy na ASCII (32 + licznik)
    mov byte ptr [rdi], bl           ; Zapisujemy licznik do bufora
    inc rdi
    mov byte ptr [rdi], al           ; Zapisujemy bieżący znak
    inc rdi

    add r10,2

    mov al, dl                       ; Aktualizujemy bieżący znak
    xor rbx, rbx                     ; Zerujemy licznik powtórzeń
    inc rsi
    dec rcx
    jmp nextChar
finish:
    inc rbx                          ; Zliczamy ostatni znak
    add bl, 32
    mov byte ptr [rdi], bl           ; Zapisujemy licznik
    inc rdi
    mov byte ptr [rdi], al           ; Zapisujemy ostatni znak

    add r10,2

    lea rsi, compressedBuffer        ; Obliczamy długość skompresowanego bufora
    sub rdi, rsi                     ; rdi - rsi = długość skompresowanego bufora

    mov rax, precompresbufferLen             ; Sprawdzamy, czy używać kompresji
    add rax, 2                       ; Długość oryginalna + 2 dla znaczników '\'
    cmp rdi, rax
    jge noCompressionNeeded          ; Jeśli długość skompresowanego jest większa lub równa, kopiujemy oryginalny ciąg

    ret                              ; Kończymy, jeśli kompresja jest efektywniejsza
noCompressionNeeded:
    lea rdi, compressedBuffer        ; Czyszczenie bufora
    mov rcx, 34
    xor al, al
clearBuffer:
    mov byte ptr [rdi], al           
    inc rdi
    loop clearBuffer

    mov r10,precompresbufferLen
    add r10,2

    lea rdi, compressedBuffer       ; Kopiowanie oryginalnego ciągu z znacznikami '\'
    mov byte ptr [rdi], '\'         ; Dodajemy znacznik początku
    inc rdi

    lea rsi, buffer                 ; Kopiujemy oryginalny ciąg
    mov rcx, precompresbufferLen
copyOriginal:
    mov al, [rsi]
    mov [rdi], al
    inc rsi
    inc rdi
    loop copyOriginal

    mov byte ptr [rdi], '\'         ; Znacznik końca

    ret
rleCompress endp

rleDeCompress proc
    ; Rejestry:
    ; rsi - wskaźnik na skompresowany bufor
    ; rdi - wskaźnik na przywrócony bufor
    ; rcx - długość skompresowanego bufora
    ; rbx - licznik powtórzeń znaków
    ; rax, rdx - rejestry tymczasowe
    ; r10

    lea rsi, debuffer                   ; Wskaźnik na skompresowany bufor
    lea rdi, compressedBuffer           ; Wskaźnik na przywrócony bufor
    mov rcx, precompresbufferLen        ; Długość skompresowanego bufora
    mov r10, 0

    cmp decompresflag, 1
    je processOriginalLoop

    mov al, byte ptr [rsi]                     ; Pierwszy znak
    cmp al, '\'                      ; Sprawdzamy znacznik oryginalnych danych
    je processOriginal                ; Jeśli znacznik, przechodzimy do oryginalnych danych

    inc rsi
    sub al, 32                        ; Odejmujemy 32, aby odzyskać licznik powtórzeń

    mov dl, byte ptr [rsi]                     ; Odczytujemy znak
    inc rsi
writeLoop:
  
    mov [rdi], dl                     ; Zapisujemy znak do przywróconego bufora
    inc r10 
    inc rdi
    dec al                            ; Zmniejszamy licznik
    jnz writeLoop                     ; Powtarzamy aż do wyczerpania

    jmp endDeCompression0             ; Przechodzimy do obróbki kolejnego bloku
processOriginal:
    inc rsi  
    dec rcx                         ; Pomijamy znacznik '\'
processOriginalLoop:
    cmp rcx, 0                        ; Sprawdzamy pozostałą długość
    je endDeCompression1               ; Jeśli wszystko przetworzone, wychodzimy

    mov dl, [rsi]                     ; Odczytujemy oryginalny znak
    inc rsi
    dec rcx
    cmp dl, '\'                      ; Sprawdzamy na zamykający znacznik
    je endDeCompression0               ; Jeśli znaleziono, wychodzimy

    mov [rdi], dl                     ; Zapisujemy znak do przywróconego bufora
    inc r10
    inc rdi
    jmp processOriginalLoop           ; Powtarzamy aż do zamykającego znacznika
endDeCompression0:
    mov decompresflag, 0
    ret
endDeCompression1:
    mov decompresflag, 1
    ret
rleDeCompress endp
 
main proc
  lea rdi, bufferName      
  mov rcx, bufferNameLen        
  call readFromConsole  

  mov rcx, rax          
  lea rsi, bufferName       
  lea rdi, filename  
  call copyBufferWithNullTerminator

  lea rsi, filename
  call openFile_r       
  mov fHandle, rax        

  cmp eax, -1     
  je badOpen

  lea rdi, bufferName      
  mov rcx, bufferNameLen        
  call readFromConsole  

  mov rcx, rax          
  lea rsi, bufferName       
  lea rdi, filenameout  
  call copyBufferWithNullTerminator

  lea rsi, filenameout
  call openFile_w   
  mov fHandleout, rax  

  cmp eax, -1     
  je badOpen    

  lea rdi, bufferName      
  mov rcx, bufferNameLen        
  call readFromConsole 

  lea rsi, bufferName

  mov al, [rsi]

  cmp al , 'd'
  je dereadLoop
  cmp al , 'c'
  je readLoop
  jmp badOpen
readLoop: 

  mov rax, fHandle  
  lea rdi, buffer
  mov rcx, bufferLen
  call read       
 
  test eax, eax   
  jz done  

  mov precompresbufferLen , rax
  call rleCompress                      
 
  mov rcx, r10    
  mov rax,fHandleout        
  lea rsi, compressedBuffer     
  call write       

  jmp readLoop
dereadLoop:

  mov rax, fHandle  
  lea rdi, debuffer
  mov rcx, debufferLen
  call read    

  test eax, eax   
  jz done  

  mov precompresbufferLen , rax
  call rleDeCompress                     
 
  mov rcx, r10    
  mov rax,fHandleout        
  lea rsi, compressedBuffer     
  call write  

  jmp dereadLoop
badOpen:
  mov rax, fHandle  
  call closeFile  

  mov rax, fHandleout  
  call closeFile  

  lea rsi, badOpenMsg
  mov rcx, badOpenMsgLen
  call writeToConsole

  ret
done:
  mov rax, fHandle  
  call closeFile  

  mov rax, fHandleout  
  call closeFile  
 
  lea rsi, goodWriteMsg
  mov rcx, goodWriteMsgLen
  call writeToConsole

  ret
main endp
end
