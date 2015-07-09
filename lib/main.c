// This file is for test 
//
#include "all.h"
#include "strbuf.h"

#define FILE_MODE   (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define MAXLINE 1024

int main(int argc, char **argv)
{

    if(argc<3){
        fprintf(stderr, "Usage: %s <old str> <new str>\n", argv[0]);
        exit(-1);
    }
    //const char old[] = "/usr/share/centrify";
    //const char new[] = "/usr/local/share/centrify";
    const char* old = argv[1];
    const char* new = argv[2];
    char cmd[MAXLINE];
    memset(cmd, 0, MAXLINE);

    /*
     * Get data from stdin, expect fomat 
     * file: line is matched 
     * */
    struct strbuf test = STRBUF_INIT;
    while (strbuf_getline(&test, stdin, '\n') != EOF){
    
        /*Analyze the input, generate the replace line*/
        if (strchr(test.buf, ':') == NULL){
            fprintf(stderr, "input line doesn't contain : \n");
            continue;
        }
        struct strbuf** strs = strbuf_split_max(&test, ':', 2);
        struct strbuf*  file = strs[0];
        struct strbuf*  match = strs[1];
        strbuf_setlen(file, file->len-1);

        printf("file: %s\n", file->buf);
        printf("match: %s\n", match->buf);

        struct strbuf replace = STRBUF_INIT;
        strbuf_add(&replace, match->buf, match->len);

        char *p;
        if ((p = strstr(replace.buf, old))){
            int pos = p - replace.buf;
            strbuf_splice(&replace, pos, strlen(old), new, strlen(new));
        }

        printf("replace: %s\n", replace.buf);


        /*This is for my specific used!!*/
        snprintf(cmd, MAXLINE, "p4 edit -c 124193 %s",file->buf);
        system(cmd);

        /*Generate the new file name*/
        struct strbuf newfile = STRBUF_INIT;
        strbuf_add(&newfile, file->buf, file->len);
        strbuf_add(&newfile, ".in", 3);
        printf("newfile: %s\n", newfile.buf);

        /*open the orginal file and create a new file*/
        FILE *fp = fopen(file->buf, "r");
        if(!fp){
            perror("fopen failed");
            exit(1);
        }

        int newfd;
        if ((newfd = open(newfile.buf, O_CREAT|O_EXCL|O_WRONLY, FILE_MODE)) < 0){
            perror("open newfile failed");
            exit(1);
        }


        char* line = NULL;
        size_t line_len = 0;
        while (getline(&line, &line_len, fp) != -1){
            int append = 0;
            line_len = strlen(line);
            if (line[line_len-1] == '\n'){
                line[--line_len]='\0';
                append = 1;
            }
            if (!strcmp(line, match->buf)){
                write(newfd, replace.buf, replace.len);
            }
            else{
                write(newfd, line, line_len);
            }

            if (append){
                write(newfd, "\n", 1);
            }
        }
        
        free(line);
        fclose(fp);
        close(newfd);


        snprintf(cmd, MAXLINE, "mv %s %s", newfile.buf, file->buf);
        system(cmd);

        strbuf_list_free(strs);
        strbuf_release(&replace);
        strbuf_release(&test);
        strbuf_release(&newfile);
    }
    return 0;
}
