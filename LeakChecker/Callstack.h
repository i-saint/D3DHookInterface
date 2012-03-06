﻿#ifndef _ist_LeakChecker_Callstack_
#define _ist_LeakChecker_Callstack_

#include <string>

bool InitializeSymbol();
void FinalizeSymbol();

int GetCallstack(void **callstack, int callstack_size, int skip_size);
std::string AddressToString(void *address);

// utility
std::string CallstackToString(void **callstack, int callstack_size, int clamp_head=0, int clamp_tail=0, const char *indent="");

#endif // _ist_LeakChecker_Callstack_
