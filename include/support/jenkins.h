#ifndef USS_JENKINS_H
#define USS_JENKINS_H

#include <stddef.h>
#include <stdint.h>

uint32_t jenkins_hash(char *key, size_t len);

#endif //USS_JENKINS_H
