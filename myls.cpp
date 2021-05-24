#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<dirent.h>
#include<limits.h>
#include<errno.h>
#include<string.h>
#include<sys/stat.h>
#include<pwd.h>
#include<time.h>
#include<libgen.h>
#include<sys/types.h>
#include<grp.h>

#include<bits/stdc++.h>

using namespace std;

vector<pair<long int, struct dirent*>> dir_entries;

void print_long(char *dir_arg, struct dirent *dir_entry);

int main(int argc, char *argv[])
{
    int flag_long = 0; //flag for l
    int flag_all = 0; // flag for a
    int flag_t = 0; //flag for t

    //get options
    int opt;
    while((opt = getopt(argc, argv, "alt")) != -1) {//filters input argument string and flag setting
        switch(opt) {
            case 'l':
                flag_long = 1;
                break;
            case 'a':
                flag_all = 1;
                break;
            case 't':
                flag_t = 1;
                break;
            default:
                fprintf(stderr, "myls: supports -l and -a options\n");
                exit(EXIT_FAILURE);
        }
    }//filter complete

    DIR *dir = opendir(".");
    if(dir == NULL) {//cannot open directory
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    struct dirent *dir_entry; //directory entry file/folder
    errno = 0;
    while((dir_entry = readdir(dir)) != NULL) {//read one by one untill end
        char *name = dir_entry->d_name;
		struct stat statbuf;
		stat(name, &statbuf); // stat info in statbuff
		dir_entries.push_back({statbuf.st_mtime, dir_entry});
    }
    if((dir_entry == NULL) && (errno != 0)) {// error number. if any problem while reading
        perror("readdir");
        exit(EXIT_FAILURE);
    }

    if(flag_t){ //sort if t is in option
        sort(dir_entries.rbegin(), dir_entries.rend());
    }





    for(auto it:dir_entries){
        char* name = it.second->d_name;

        if(!flag_all && (name[0] == '.')) continue; // dont read hidden file if a is not in option

        if(!flag_long) cout << name << "\t"; //print  only name if no l in option
        else print_long(".", it.second);// print long: details of dir_entry
    }
    if(!flag_long) cout << endl;

    closedir(dir);//close directory

    return 0;
}

void print_long(char *dir_arg, struct dirent *dir_entry)
{
    struct stat statbuf;
    char fp[PATH_MAX];
    sprintf(fp, "%s/%s", dir_arg, dir_entry->d_name); //file path = diractory + file name
    if(stat(fp, &statbuf) == -1) {//statbuf loads all info about dir_entry
        perror("stat");
        return;
    }

    //permission data/link
    printf((S_ISDIR(statbuf.st_mode)) ? "d" : "-"); //diractory
    printf((statbuf.st_mode & S_IRUSR) ? "r" : "-"); //user read permission
    printf((statbuf.st_mode & S_IWUSR) ? "w" : "-"); //user write permission
    printf((statbuf.st_mode & S_IXUSR) ? "x" : "-"); //user execution permission
    printf((statbuf.st_mode & S_IRGRP) ? "r" : "-"); //group read permission
    printf((statbuf.st_mode & S_IWGRP) ? "w" : "-"); //group write permission
    printf((statbuf.st_mode & S_IXGRP) ? "x" : "-"); //group ececution permission
    printf((statbuf.st_mode & S_IROTH) ? "r" : "-"); //author read permission
    printf((statbuf.st_mode & S_IWOTH) ? "w" : "-"); //author write permission
    printf((statbuf.st_mode & S_IXOTH) ? "x " : "- "); //author execution permission
    printf("%li ", statbuf.st_nlink); // how much hard link

    //group and user data
    struct passwd *pw;
    struct group *gid;
    pw = getpwuid(statbuf.st_uid);
    if(pw == NULL) {
        perror("getpwuid");
        printf("%d ", statbuf.st_uid);
    }else{
        printf("%s ", pw->pw_name);
    }
    gid = getgrgid(statbuf.st_gid);
    if(gid == NULL) {
        perror("getpwuid");
        printf("%d ", statbuf.st_gid);
    }else {
        printf("%s ", gid->gr_name);
    }

    //file size five digit
    printf("%5ld ", statbuf.st_size);

    //timestamp
    struct tm *tmp;
    char outstr[200];
    time_t t = statbuf.st_mtime;//last modification time
    tmp = localtime(&t);
    if(tmp == NULL) {
        perror("localtime");
        exit(EXIT_FAILURE);
    }
    strftime(outstr, sizeof(outstr), "%b %d %R", tmp);
    printf("%s ", outstr);

    //file name
    printf("%s\n", dir_entry->d_name);
}


