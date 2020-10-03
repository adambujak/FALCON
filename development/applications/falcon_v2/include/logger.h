#ifndef LOGGER_H
#define LOGGER_H

void logger_task_setup (void);
void logger_write (char *string);
void logger_task (void *pvParameters);

#endif // LOGGER_H
