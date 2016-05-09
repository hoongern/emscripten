#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <emscripten.h>

int main()
{
    int result = 1;

    // Use mmap to read in.txt
    const char* path = "/file2.txt";
    struct stat mystat;
    
    stat(path, &mystat);
    assert(mystat.st_size > 0);
    int fd = open(path, O_RDONLY);
    assert(fd != -1);

    int filesize = 7;
    void* map = (void*)mmap(NULL, mystat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    assert(map != MAP_FAILED);

    printf("%s content=", path);
    for (int i = 0; i < filesize; i++) {
        printf("%c", ((char*)map)[i]);
    }
    printf("\n");

    int rc = munmap(map, filesize);
    assert(rc == 0);

    close(fd);

    //REPORT_RESULT();
    return 0;
}