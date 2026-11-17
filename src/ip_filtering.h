#ifndef PRECOMPUTED_ALLOWLIST_H
#define PRECOMPUTED_ALLOWLIST_H

typedef struct precomputed_allowlist {
    const char** array;
} precomputed_allowlist;

precomputed_allowlist* precompute_allowlist(const char** in_array, precomputed_allowlist* allowlist);
bool is_allowed(char* compate_target, precomputed_allowlist* allowlist);

#endif
