/*
 * mk_dbg.h
 *
 *  Created on: Feb 16, 2015
 *      Author: sjguo
 *      Note: This file is from Markus Kusano's datalog pass
 */

#ifndef MK_DBG_H_
#define MK_DBG_H_

#pragma once

#ifdef MK_DBG
#define DEBUG_MSG(str) do { errs() << str; } while (false)
#else
#define DEBUG_MSG(str) do {} while (false)
#endif

#ifdef MK_DBG
#define DEBUG_MSG_RED(str) \
	do { \
		if(errs().has_colors()) { \
			errs().changeColro(raw_ostream::RED); \
		} \
		errs() << str; \
		errs().resetColor(); \
	} while(false)
#else
#define DEBUG_MSG_RED(str) do{} while(false)
#endif

#endif /* MK_DBG_H_ */
