#include <stdio.h>
#include <sys/mman.h>
#include <emscripten.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

bool test_mapshared_and_filedescriptor_refcounts()
{
    EM_ASM({
        FS.writeFile('/file1', new Uint8Array([100]), { encoding: 'binary', flags: 'w' });
        FS.writeFile('/file2', new Uint8Array([101]), { encoding: 'binary', flags: 'w' });
    });

    bool result = true;
    FILE *f1 = fopen("file1", "rw");
    uint8_t *f1_ptr = (uint8_t *) mmap(NULL, 1, PROT_READ, MAP_SHARED, fileno(f1), 0);

    // File should be able to be closed after mmap, ref http://pubs.opengroup.org/onlinepubs/7908799/xsh/mmap.html
    // "The mmap() function adds an extra reference to the file associated with the file descriptor fildes which is
    //  not removed by a subsequent close() on that file descriptor. This reference is removed when there are no
    //  more mappings to the file."
    fclose(f1);

    if (*f1_ptr != 100)
    {
        printf("Read file1 after mmap, expected: 100, actual: %d\n", *f1_ptr);
        result = false;
    }

    FILE *f2 = fopen("file2", "r");
    uint8_t number;

    *f1_ptr = 102;
    munmap(f1_ptr, 1); // Munmap should write back to the original file

    // the contents of file2 should not be affected
    fread(&number, sizeof(uint8_t), 1, f2);
    if (number != 101)
    {
        printf("Read file2, expected: 101, actual: %d\n", number);
        result = false;
    }
    fclose(f2);

    // verify the write to file1 we did while mmap'ed
    f1 = fopen("file1", "r");
    fread(&number, sizeof(uint8_t), 1, f1);
    if (number != 102)
    {
        printf("Re-read file1 after munmap, expected: 102, actual: %d\n", number);
        result = false;
    }
    fclose(f1);

    return result;
}

bool test_basic()
{
    printf("*\n");
    FILE *f = fopen("data.dat", "r");
    char *m;
    m = (char*)mmap(NULL, 9000, PROT_READ, MAP_PRIVATE, fileno(f), 0);
    bool result = !strncmp(m, "data from the file .", 20);
    munmap(m, 9000);
    printf("\n");
    fclose(f);
    return result;
}

int main() {
    int result = test_basic() && test_mapshared_and_filedescriptor_refcounts() ? 1 : 0;
    REPORT_RESULT();
}
