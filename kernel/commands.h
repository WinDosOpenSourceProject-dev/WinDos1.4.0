#ifndef COMMANDS_H
#define COMMANDS_H

void cmd_copy(const char* source, const char* destination);
void cmd_del(const char* filename);
void cmd_move(const char* source, const char* destination);
void cmd_cd(const char* dirname);
void cmd_md(const char* dirname);
void cmd_rd(const char* dirname);
void cmd_deltree(const char* dirname);
void cmd_print(const char* filename);
void cmd_date(const char* new_date);
void cmd_time(const char* new_time);
void cmd_con(const char* filename);
void cmd_install();
void cmd_save(const char* description);
void cmd_load(int slot);
void cmd_savelist();
void cmd_savedel(int slot);

#endif
