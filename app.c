#include <stdio.h>
#include <string.h>
int main(int argc, char const *argv[])
{
    // for (int i = 0; i < argc; i++)
    // {
    //     puts(argv[i]);        
    // }
    char buf[200];

    FILE * fp = popen("md5sum carpeta/file", "r");
    fgets(buf,200,fp);
    buf[strlen(buf)] = '\0';
    pclose(fp);
    puts(buf);
    return 0;
}
