#pragma once
#include "hash.h"

#define _(str) locale_str(HASH_DJB2(str))

void locale_load(uint32_t resource_locale);

char *locale_str(int hashval);
