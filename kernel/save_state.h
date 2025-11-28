#ifndef SAVE_STATE_H
#define SAVE_STATE_H

#define MAX_SAVE_SLOTS 10

int save_system_state(const char* description, int slot);
int load_system_state(int slot);
void list_saved_states();
int delete_saved_state(int slot);
uint32_t calculate_checksum(void* data, uint32_t size);

#endif
