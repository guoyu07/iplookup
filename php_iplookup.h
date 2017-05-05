/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_IPLOOKUP_H
#define PHP_IPLOOKUP_H

extern zend_module_entry iplookup_module_entry;

#define phpext_iplookup_ptr &iplookup_module_entry

#define PHP_IPLOOKUP_VERSION "0.1.0" /* Replace with version number for your extension */

#ifdef PHP_WIN32
#	define PHP_IPLOOKUP_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_IPLOOKUP_API __attribute__ ((visibility("default")))
#else
#	define PHP_IPLOOKUP_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#define BE_32(x) ((((uint8_t*)(x))[0]<<24) |\
                  (((uint8_t*)(x))[1]<<16) |\
                  (((uint8_t*)(x))[2]<<8) |\
                  ((uint8_t*)(x))[3])

#define LE_32(x) ((((uint8_t*)(x))[3]<<24) |\
                  (((uint8_t*)(x))[2]<<16) |\
                  (((uint8_t*)(x))[1]<<8) |\
                  ((uint8_t*)(x))[0])

#define LE_24(x) ((((uint8_t*)(x))[2]<<16) |\
                  (((uint8_t*)(x))[1]<<8) |\
                  ((uint8_t*)(x))[0])

#define REDIRECT_TYPE_1 0x01
#define REDIRECT_TYPE_2 0x02
#define INDEX_LENGTH 7
#define CHAR_NUM 108


/*
  	Declare any global variables you may need between the BEGIN
	and END macros here:

ZEND_BEGIN_MODULE_GLOBALS(iplookup)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(iplookup)
*/

/* In every utility function you add that needs to use variables
   in php_iplookup_globals, call TSRMLS_FETCH(); after declaring other
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as IPLOOKUP_G(variable).  You are
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define IPLOOKUP_G(v) TSRMG(iplookup_globals_id, zend_iplookup_globals *, v)
#else
#define IPLOOKUP_G(v) (iplookup_globals.v)
#endif

#endif	/* PHP_IPLOOKUP_H */

uint32_t get_index_offset(FILE *);
int update_qqwry_file(const char *)
uint32_t ip2long(const char*);
uint32_t find_index(const uint32_t,FILE *);
int find_location_by_index(FILE *,const uint32_t,char *);
int nt is_cz88(const char*);
zval *get_location(FILE *,const char*);

static uint32_t start_index_offset,end_index_offset,total_ip_num;
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
