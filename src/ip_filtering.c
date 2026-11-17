#include <stdbool.h>
#include <string.h>

#include "ip_filtering.h"

precomputed_allowlist* precompute_allowlist(const char** in_array, precomputed_allowlist* allowlist) {
    allowlist->array = in_array;
    return allowlist;
}

bool is_allowed(char* compate_target, precomputed_allowlist* allowlist) {
    for (int i = 0; allowlist->array[i] != NULL; i++) {
        if (strcmp(allowlist->array[i], compate_target)) {
            return true;
        }
    }

    return false;
}
