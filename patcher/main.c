#include <stdio.h>
#include <stdlib.h>

#define SEARCH_PATTERN "Direct3DC"

// Function to convert an integer to a byte array in the desired format
void int_to_byte_array(unsigned int value, unsigned char* array) {
    array[0] = (value & 0xFF);         // Extract the lowest byte
    array[1] = (value >> 8) & 0xFF;    // Shift right and extract the next byte
    array[2] = (value >> 16) & 0xFF;   // Shift right and extract the next byte
    array[3] = (value >> 24) & 0xFF;   // Shift right and extract the highest byte
}

int search_pattern(FILE* file, const unsigned char* pattern, size_t pattern_length, int offset) {
    unsigned char* buffer;
    size_t file_size;
    size_t i, j;

    // Get file size
    fseek(file, offset, SEEK_END);
    file_size = ftell(file);
    fseek(file, offset, SEEK_SET);

    // Allocate buffer to hold file content
    buffer = (unsigned char*)malloc(file_size);
    if (!buffer) {
        perror("Failed to allocate buffer");
        return;
    }

    // Read file content into buffer
    fread(buffer, 1, file_size, file);

    // Search for the pattern in the buffer
    for (i = 0; i <= file_size - pattern_length; i++) {
        for (j = 0; j < pattern_length; j++) {
            if (buffer[i + j] != pattern[j]) {
                break;
            }
        }
        if (j == pattern_length) {
            free(buffer);
            return i;
        }
    }

    // Free the buffer
    free(buffer);
    return 0;
}

int main(int argc, char* argv[])
{
    printf("THUG-SDL EXE PATCHER\n");
    int file_found = 1;
    FILE* file;

    if (argc != 2) {

        const char* filename = "THUG.exe";
        file = fopen(filename, "r+b");
        if (!file) {
            file_found = 0;
        }
    }
    else {
        file = fopen(argv[1], "r+b");
        if (!file) {
            file_found = 0;
        }
    }

    if (file_found) {
        // The hex pattern to search for
        const unsigned char pattern[] = { 0x09, 0x00, 0x44, 0x69, 0x72, 0x65, 0x63, 0x74, 0x33, 0x44, 0x43 };
        const unsigned char pattern_new_loc[] = { 0x50, 0x6F, 0x73, 0x74, 0x54, 0x68, 0x72, 0x65, 0x61, 0x64, 0x4D, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        const unsigned char new_dll_string[] = { 0x09, 0x00, 0x44, 0x69, 0x72, 0x65, 0x63, 0x74, 0x33, 0x44, 0x43, 0x72, 0x65, 0x61, 0x74, 0x65, 0x39, 0x00, 0x74, 0x68, 0x75, 0x67, 0x73, 0x64, 0x6C, 0x2E, 0x64, 0x6C, 0x6C, 0x00 };
        const unsigned char pattern_ptr[4]; //pointer to pattern[]
        const unsigned char pattern_ptr_dll[4]; //pointer to pattern+18 (dll filename)
        const unsigned char pattern_new_pointer_a[4];
        const unsigned char pattern_new_pointer_b[4];

        int d3d9string_pattern_offset;
        int ptr_a;
        int ptr_b;
        int error = 0;

        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-=-= Get pattern and pointer values -=-=-=-=-=-
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        // Get offset of original Direct3DCreate9 Text
        d3d9string_pattern_offset = search_pattern(file, pattern, sizeof(pattern), 0);
        if (d3d9string_pattern_offset)
            printf("DLL string found at offset: 0x%08x\n", d3d9string_pattern_offset);
        else
            error |= 1;

        // Two pointers exists to that offset. One more pointer exist to that offset +18. Convert the found offset to a byte array to search for it
        int_to_byte_array(d3d9string_pattern_offset, pattern_ptr);
        int_to_byte_array(d3d9string_pattern_offset + 18, pattern_ptr_dll);

        // There will be two occurrences. Search for the first one from the start of the file
        ptr_a = search_pattern(file, pattern_ptr, sizeof(pattern_ptr), 0);
        if (ptr_a)
            printf("Pointer A found at offset: 0x%08x\n", ptr_a);
        else
            error |= 1;

        // Repeat but start behind the first pointer to search the rest of the file
        ptr_b = search_pattern(file, pattern_ptr, sizeof(pattern_ptr), ptr_a + 4);
        if (ptr_b) {
            ptr_b = ptr_b + ptr_a + 4; // Correct the offset
            printf("Pointer B found at offset: 0x%08x\n", ptr_b);
        }
        else {
            error |= 1;
        }

        // Search for the dll pointer
        int ptr_dll = search_pattern(file, pattern_ptr_dll, sizeof(pattern_ptr_dll), 0);
        if (ptr_dll)
            printf("Pointer C at: 0x%08x\n", ptr_dll);
        else
            error |= 1;

        // Search for the new location of our d3d9 proxy dll. It will be patched after the PostThreadMessageA string (pattern_new_loc)
        int new_loc = search_pattern(file, pattern_new_loc, sizeof(pattern_new_loc), 0);
        if (new_loc)
            printf("New DLL string will be at: 0x%08x\n", new_loc + sizeof(pattern_new_loc));
        else
            error |= 1;


        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        // -=-=-=-=-= Write new pointers and dll string -=-=-=-=-=
        // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        if (!error) {
            // Get original file size
            fseek(file, 0, SEEK_END);
            size_t file_size = ftell(file);
            fseek(file, 0, SEEK_SET);

            // Allocate buffer to hold file content
            unsigned char* buffer = (unsigned char*)malloc(file_size);
            if (!buffer) {
                perror("Failed to allocate buffer");
                fclose(file);
                return EXIT_FAILURE;
            }

            // Read original file content into buffer
            fread(buffer, 1, file_size, file);
            fclose(file);

            // Write the modified buffer to the new file
            FILE* new_file = fopen("THUG-SDL.exe", "wb");
            if (!new_file) {
                perror("Failed to create new file");
                free(buffer);
                return EXIT_FAILURE;
            }
            fwrite(buffer, 1, file_size, new_file);

            // Seek to the specified offset
            if (fseek(file, (new_loc + sizeof(pattern_new_loc)), SEEK_SET) != 0)
                perror("Failed to seek to the specified offset");

            // Write the new string at the specified offset
            if (fwrite(new_dll_string, 1, sizeof(new_dll_string), new_file) != sizeof(new_dll_string))
                printf("Failed to write the new string\n");
            else
                printf("successfully patched new string\n");

            // patch the three pointers
            const unsigned char new_loc_ptr[4];
            const unsigned char new_dll_ptr[4];
            int_to_byte_array((new_loc + sizeof(pattern_new_loc)), new_loc_ptr);
            int_to_byte_array((new_loc + sizeof(pattern_new_loc) + 18), new_dll_ptr);
            //ptr a
            if (fseek(file, ptr_a, SEEK_SET) != 0)
                perror("Failed to seek to the specified offset");
            if (fwrite(new_loc_ptr, 1, sizeof(new_loc_ptr), new_file) != sizeof(new_loc_ptr))
                printf("Failed to write the new ptr_a\n");

            fseek(file, 0, SEEK_SET);
            if (fseek(file, ptr_b, SEEK_SET) != 0)
                perror("Failed to seek to the specified offset");
            if (fwrite(new_loc_ptr, 1, sizeof(new_loc_ptr), new_file) != sizeof(new_loc_ptr))
                printf("Failed to write the new ptr_a\n");

            fseek(file, 0, SEEK_SET);
            if (fseek(file, ptr_dll, SEEK_SET) != 0)
                perror("Failed to seek to the specified offset");
            if (fwrite(new_dll_ptr, 1, sizeof(new_loc_ptr), new_file) != sizeof(new_dll_ptr))
                printf("Failed to write the new ptr_b\n");

            fclose(new_file);
            fclose(file);
            free(buffer);
        }
        else {
            printf("Failed to patch the file. The offsets did not match the expected THUG.exe\n");
        }
    }
    else {
        printf("Failed to open file\n");
        printf("Usage: %s <filename>\n", argv[0]);
    }
    system("PAUSE");
    return 0;
}