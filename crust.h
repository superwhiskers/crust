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

#pragma once

#include <stdlib.h>

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

/* destroys a Result */
void result_destroy(struct Result result) {
	free(result.data);
}

/* destroys an Option */
void option_destroy(struct Option option) {
	free(option.data);
}
