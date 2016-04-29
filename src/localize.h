#pragma once
#include "hash.h"

#define _(str) locale_str(HASH_DJB2(str))

void locale_init(uint32_t resource_locale);

void locale_deinit();

char *locale_str(int hashval);
