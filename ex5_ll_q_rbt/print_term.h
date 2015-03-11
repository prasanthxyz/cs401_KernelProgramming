#ifndef _PRINT_TERM_H_
#define _PRINT_TERM_H_

#include <asm/current.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/tty.h>

static void print_term(char *str)
{
	struct tty_struct *my_tty;
	my_tty = current->signal->tty;
	if(my_tty != NULL) {
		((my_tty->driver)->ops->write) (my_tty, str, strlen(str));
		((my_tty->driver)->ops->write) (my_tty, "\015\012", 2);
	}
}

#endif
