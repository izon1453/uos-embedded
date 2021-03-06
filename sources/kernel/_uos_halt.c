/*
 * Copyright (C) 2000-2005 Serge Vakulenko, <vak@cronyx.ru>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 *
 * As a special exception to the GPL, permission is granted for additional
 * uses of the text contained in this file.  See the accompanying file
 * "COPY-UOS.txt" for details.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <kernel/internal.h>

void tasks_print(struct _stream_t *stream){
    unsigned char n;
    task_t *t;
    bool_t have_current = 0;

    task_print (stream, 0);
    task_print (stream, task_idle);
    n = 0;
    list_iterate (t, &task_active) {
        if (t == task_current)
            have_current = 1;
        if ((t != task_idle) && (t != task_current))
            task_print (stream, t);
        if (! uos_valid_memory_address (t))
            break;
        if (++n > 32 || list_is_empty (&t->item)) {
            debug_puts ("...\n");
            break;
        }
    }
    if ((task_current != 0) && (task_current != task_idle)){
        if (have_current == 0)
            debug_putchar(0, '*');
        task_print (stream, task_current);
    }
}

void uos_debug_dump(){
    tasks_print(&debug);

    size_t sp = (size_t)__builtin_alloca (0);
    size_t low_stack = (size_t)task_current->stack_context + task_stack_avail(task_current);
    if ( (sp < low_stack)
#ifndef NDEBUG
        || (sp >= (size_t)task_current->stack_limit)
#endif
      )
        // if now out of task stack, use total affected stack
        sp = low_stack;

    debug_dump_stack (task_name (task_current), (void*)sp,
        (void*) task_current->stack_context, __builtin_return_address (0));
    debug_printf ("\n*** Please report this information");
}

/*
 * Halt uOS, return to the parent operating system (if any).
 * Optionally print debugging information about the system state.
 */
#undef uos_halt

#ifndef UOS_ON_HALT
#define UOS_ON_HALT(errcode)
#else
__attribute__((weak, noinline)) 
void uos_on_halt(int errcode)
{}
#endif

#ifdef NDEBUG
__NORETURN
#endif
void
uos_halt (int dump_flag)
{
    UOS_ON_HALT(dump_flag);
#if LINUX386
	exit (0);
#else
    arch_intr_off ();
	if (dump_flag)
	    uos_debug_dump();

	/* Halt CPU. */
	debug_puts ("\n*** System halted.\n");
	for (;;)
		continue;
#endif
}
