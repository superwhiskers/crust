/*

crust - simple emulations of some rust features and bits of functional programming in c
copyright (c) 2019 superwhiskers <whiskerdev@protonmail.com>

this source code form is subject to the terms of the mozilla public
license, v. 2.0. if a copy of the mpl was not distributed with this
file, you can obtain one at http://mozilla.org/MPL/2.0/.

*/

#ifndef CRUST_H
#define CRUST_H

#include <stdio.h>
#include <stdlib.h>

/*
features:
- optionals (result, option) OPTIONALS
- map, filter, and reduce    MAP_FILTER_REDUCE
- panic                      PANIC
*/

/*
optionals
*/
#ifdef CRUST_FEATURE_OPTIONALS

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

#endif

/*
map, filter, and reduce
*/
#ifdef CRUST_FEATURE_MAP_FILTER_REDUCE

/* the type for a function to pass to map. the only parameter is the value to map */
typedef void(map_function)(void *);

/* the map function implemented in c. it modifies the array in-place */
void map(char *array, int array_length, int size, map_function *function) {
	for (int i = 0; i < array_length * size; i += size) {
		function(array + i);
	}
}

/* the type for a function to pass to filter. the only parameter is the value to filter */
typedef _Bool(filter_function)(void *);

/* the filter function implemented in c. it modifies the array in-place. it returns the new length (in indices) so you can reallocate */
int filter(char *array, int array_length, int size, filter_function *function) {
  int length = 0;
  for (int i = 0; i < array_length * size; i += size) {
    if (function(array + i)) {
      // this is literal black magic
      for (int l = 0; l < size; l++) {
        // just to have an explaination, this basically uses the fact that 
        // we know we are accessing a single byte, and copies over the data
        // byte-by-byte to the destination until we've copied over the known length
        *(array + (length * size) + l) = *(array + i + l);
      }
      length++;
    }
  }
  return length; 
}

/* the type for a function to pass to reduce. the first parameter is the accumulator, the second is the value to operate on */
typedef void(reduce_function)(void *, void *);

/* the reduce function implemented in c. the accumulator must be provided by the callee */
void *reduce(char *array, int array_length, int size, void *accumulator,
	     reduce_function *function) {
	for (int i = 0; i < array_length * size; i += size) {
		function(accumulator, array + i);
	}
	return accumulator;
}

#endif

/*
panic
*/
#ifdef CRUST_FEATURE_PANIC

#include <libunwind.h>

#ifndef CRUST_FUNCTION_NAME_LENGTH
#define CRUST_FUNCTION_NAME_LENGTH 50
#endif

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

#endif
