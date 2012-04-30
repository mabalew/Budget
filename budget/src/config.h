#ifndef CONFIG_H
#define CONFIG_H

int add_config(char *param_name, char *value);
int get_config(char *param_name, char *value);
int update_config(char *param_name, char *value);
int config_exists(char *param_name, int *count);
#endif
