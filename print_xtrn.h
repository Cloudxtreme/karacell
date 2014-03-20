/*
Karacell 3 Library
Copyright 2014 Tigerspike Ltd
http://karacell.info

This collection of files constitutes the Karacell 3 Library. (This is a
library in the abstact sense; it's not intended to compile to a ".lib"
file.)

The Karacell 3 Library is free software: you can redistribute it and/or
modify it under the terms of the GNU Limited General Public License as
published by the Free Software Foundation, version 3.

The Karacell 3 Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Limited General Public License version 3 for more details.

You should have received a copy of the GNU Limited General Public
License version 3 along with the Karacell 3 Library (filename
"COPYING"). If not, see http://www.gnu.org/licenses/ .
*/
/*
String Printing Prototype Declaration. See print.c for full interface descriptions.
*/
extern void print_error(char *string_base);
extern void print_error_input_file(void);
extern void print_error_output_file(void);
extern void print_error_no_memory(void);
extern void print(char *string_base);
extern void print_new_line(void);
extern void print_warning(char *string_base);
