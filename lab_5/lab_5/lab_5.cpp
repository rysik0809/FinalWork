#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char ascbin(const char* str);
void binasc(int value, char* buffer);
int func(char a, char b, char c, char d, char e);

int main() {
    char input[10] = { 0 };
    char args[5] = { 0 };
    char buffer[10] = { 0 };

    printf("function: y = a/b - c*d + e\n");
    printf("Input numbers, byte values (-128..127)\n\n");

    const char* prompts[] = { "a: ", "b: ", "c: ", "d: ", "e: " };
    for (int i = 0; i < 5; i++) {
        printf("%s", prompts[i]);
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';

        args[i] = ascbin(input);
    }

    if (args[1] == 0) {
        printf("\nError (/0)\n");
        return 1;
    }

    int result = func(args[0], args[1], args[2], args[3], args[4]);

    if (result > 127 || result < -128) {
        printf("\nError (overflow)\n");
        return 1;
    }

    binasc(result, buffer);
    printf("\nResult y = %s\n", buffer);

    return 0;
}

char ascbin(const char* str) {
    char result;
    __asm {
        push esi
        push ecx
        push edx

        mov esi, str     
        xor eax, eax      
        xor ecx, ecx      
        xor edx, edx      

        mov cl, [esi]
        cmp cl, '+'
        je positive
        cmp cl, '-'
        jne convert
        mov edx, 1        
        positive:
            inc esi          
        convert:
            mov cl, [esi]
            test cl, cl
            jz done           

            cmp cl, '0'
            jb done
            cmp cl, '9'
            ja done

            imul eax, 10      
            sub cl, '0'
            add eax, ecx      

            inc esi
            jmp convert
        done:
            test edx, edx
            jz store
            neg eax
        store:
            mov result, al    

            pop edx
            pop ecx
            pop esi
    }
    return result;
}

void binasc(int value, char* buffer) {
    __asm {
        push eax
        push ebx
        push ecx
        push edx
        push edi
        push esi

        mov eax, value      
        lea edi, [buffer]   
        add edi, 9          
        mov byte ptr[edi], 0 
        dec edi             

        mov ebx, 10         
        xor ecx, ecx        
        xor edx, edx        

        test eax, eax
        jnz not_zero
        mov byte ptr[edi], '0'
        jmp done

        not_zero :
            test eax, eax
            jns convert_loop
            neg eax             

        convert_loop :
            xor edx, edx
            div ebx             
            add dl, '0'         
            mov[edi], dl       
            dec edi             
            inc ecx           
            test eax, eax       
            jnz convert_loop    

            mov eax, value
            test eax, eax
            jns done
            mov byte ptr[edi], '-'
            dec edi

         done :
            inc edi             
            lea esi, [edi]      
            mov edi, buffer     
            mov ecx, 10         
            rep movsb          

            pop esi
            pop edi
            pop edx
            pop ecx
            pop ebx
            pop eax
    }
}

int func(char a, char b, char c, char d, char e) {
    int result;
    __asm {
        movsx eax, a      
        movsx ebx, b      
        cdq               
        idiv ebx          
        mov ecx, eax      

        movsx eax, c
        movsx ebx, d
        imul eax, ebx     

        sub ecx, eax

        movsx eax, e
        add ecx, eax

        mov result, ecx
    }
    return result;
}