/*
* Reveal hidden kernel modules by brute forcing the Virtual Memory
* Copyright (C) 2013  Panos Sakkos
* 
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __MODULE_UTILS__
#define __MODULE_UTILS__

int looks_like_module(struct module *module);

int modules_are_the_same(struct module *module1, struct module *module2);

int exists_in_the_modules_list(struct module *target_module);

#endif
