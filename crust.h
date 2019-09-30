/*
crust - simple emulations of some rust types in c
copyright (c) 2019 superwhiskers <whiskerdev@protonmail.com>

this program is free software: you can redistribute it and/or modify
it under the terms of the gnu lesser general public license as published by
the free software foundation, either version 3 of the license, or
(at your option) any later version.

this program is distributed in the hope that it will be useful,
but without any warranty; without even the implied warranty of
merchantability or fitness for a particular purpose.  see the
gnu lesser general public license for more details.

you should have received a copy of the gnu lesser general public license
along with this program.  if not, see <https://www.gnu.org/licenses/>.
*/

#ifndef CRUST_H
#define CRUST_H

#include <libunwind.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef CRUST_FUNCTION_NAME_LENGTH
#define CRUST_FUNCTION_NAME_LENGTH 50
#endif

/* possible types of a Result */
typedef enum ResultType {
	ResultOk,
	ResultErr,
} ResultType;

/* emulation of rust's Result enum in c */
typedef struct Result {
	enum ResultType type;
	void *data;
} Result;

/* shorthand for (Result){ ResultOk, (data) } */
#define Ok(data)                                                               \
	(Result) {                                                             \
		ResultOk, (data)                                               \
	}

/* shorthand for (Result){ ResultErr, (data) } */
#define Err(data)                                                              \
	(Result) {                                                             \
		ResultErr, (data)                                              \
	}

/* destroys a Result */
void result_destroy(struct Result result) {
	free(result.data);
}

/* checks if a Result is of the Err variant */
int result_is_err(struct Result result) {
	if (result.type == ResultErr) {
		return 1;
	}
	return 0;
}

/* checks if a Result is of the Ok variant */
int result_is_ok(struct Result result) {
	if (result.type == ResultOk) {
		return 1;
	}
	return 0;
}

/* possible types of an Option */
typedef enum OptionType {
	OptionSome,
	OptionNone,
} OptionType;

/* emulation of rust's Option enum in c */
typedef struct Option {
	enum OptionType type;
	void *data;
} Option;

/* shorthand for (Option){ OptionSome, (data) } */
#define Some(data)                                                             \
	(Option) {                                                             \
		OptionSome, (data)                                             \
	}

/* shorthand for (Option){ OptionNone, 0 } */
#define None                                                                   \
	(Option) {                                                             \
		OptionNone, 0                                                  \
	}

/* destroys an Option */
void option_destroy(struct Option option) {
	free(option.data);
}

/* checks if an Option is of the Some variant */
int option_is_some(struct Option option) {
	if (option.type == OptionSome) {
		return 1;
	}
	return 0;
}

/* checks if an Option is of the None variant */
int option_is_none(struct Option option) {
	if (option.type == OptionNone) {
		return 1;
	}
	return 0;
}

/* panics from a function and prints a stack trace. exits with code 1 */
#define panic(message)                                                         \
	_Generic((message), char * : panic_with_code)((message), 1)

/* panics from a function and prints a stack trace, then exits the program with the specified exit code */
void panic_with_code(char *message, int code) {
	unw_cursor_t cursor;
	unw_context_t uc;
	unw_word_t ip, sp, op;
	char *name = (char *)(malloc(CRUST_FUNCTION_NAME_LENGTH));

	unw_getcontext(&uc);
	unw_init_local(&cursor, &uc);

	printf("panic: %s\n", message);

	int i = 1;
	while (unw_step(&cursor) > 0) {
		unw_get_reg(&cursor, UNW_REG_IP, &ip);
		unw_get_reg(&cursor, UNW_REG_SP, &sp);
		unw_get_proc_name(&cursor, name, CRUST_FUNCTION_NAME_LENGTH,
				  &op);
		printf("%d: %s() +0x%lx\n", i, name, (long)(op));
		printf("    ip = %lx, sp = %lx\n", ip, sp);
		i++;
	}

	exit(code);
}

#endif
