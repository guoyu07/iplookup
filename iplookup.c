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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_iplookup.h"

/* If you declare any globals in php_iplookup.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(iplookup)
*/
ZEND_DECLARE_MODULE_GLOBALS(iplookup)

/* True global resources - no need for thread safety here */
static int le_iplookup;

static uint32_t start_index_offset,end_index_offset,total_ip_num;

zend_class_entry *iplookup_ce;

/* {{{ PHP_INI
 */
/*Remove comments and fill if you need to have entries in php.ini*/
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("iplookup.qqwry_file",      "", PHP_INI_ALL, OnUpdateString, qqwry_file, zend_iplookup_globals, iplookup_globals)
    //STD_PHP_INI_ENTRY("iplookup.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_iplookup_globals, iplookup_globals)
PHP_INI_END()

ZEND_BEGIN_ARG_INFO_EX(arginfo_iplookup___construct, 0, 0, 1)
        ZEND_ARG_INFO(0, file)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_iplookup_get_index, 0, 0, 1)
        ZEND_ARG_INFO(0, ip)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_iplookup_search_ip, 0, 0, 1)
        ZEND_ARG_INFO(0, ip)
ZEND_END_ARG_INFO()

/* }}} */

/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_iplookup_compiled(string arg)
   Return a string to confirm that the module is compiled in */
ZEND_METHOD(IpLookUp,__construct)
{
	char *file = NULL;
	int file_len;
	php_stream *stream;
	zval *resource = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &file, &file_len) == FAILURE) {
		//如果没有传递参数 从配置文件中获取参数
		file = IPLOOKUP_G(qqwry_file);
		file_len = sizeof(file) - 1;
	}

	//判断是否有文件名
	if(file == NULL)
    {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "file not configure");
        return;
    }

	//判断文件是否存在 如果不存在下载最新的文件
	if(! VCWD_ACCESS(file,F_OK) )
    {
        if(update_qqwry_file(file) == 0)
        {
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "file update failed: %s", file);
            RETURN_NULL();
        }
    }

    zend_update_property_string(iplookup_ce,getThis(),"file",sizeof("file")-1,file TSRMLS_CC);

    //打开文件
    stream = php_stream_open_wrapper(file, "r", ENFORCE_SAFE_MODE | REPORT_ERRORS,NULL);
    if(! stream)
    {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "file open failed: %s", file);
        RETURN_NULL();
    }

    php_stream_seek(stream,0L,SEEK_SET);

    start_index_offset = get_index_offset(stream);
    end_index_offset   = get_index_offset(stream);

    total_ip_num = (start_index_offset - end_index_offset) / INDEX_LENGTH;

    //MAKE_STD_ZVAL(resource);
    //保存资源到对象中
    php_stream_to_zval(stream, resource);
    zend_update_property(iplookup_ce,getThis(),"fp",sizeof("fp")-1,resource TSRMLS_CC);
    zend_update_property_long(iplookup_ce,getThis(),"startIndexOffset",sizeof("startIndexOffset")-1,(long)start_index_offset TSRMLS_CC);
    zend_update_property_long(iplookup_ce,getThis(),"endIndexOffset",sizeof("endIndexOffset")-1,(long)end_index_offset TSRMLS_CC);
    zend_update_property_long(iplookup_ce,getThis(),"totalIpNum",sizeof("totalIpNum")-1,(long)total_ip_num TSRMLS_CC);
	RETURN_NULL();
}

ZEND_METHOD(IpLookUp,total_ip_num)
{
    zval *totalIpNum;

    totalIpNum = zend_read_property(iplookup_ce,getThis(),"totalIpNum",sizeof("totalIpNum")-1,0 TSRMLS_CC);

    //RETURN_NULL();
    //RETVAL_LONG(Z_STRVAL_P(totalIpNum);
    //return;
    RETURN_LONG(Z_STRVAL_P(totalIpNum));
}

ZEND_METHOD(IpLookUp,get_index)
{
    php_stream *stream = NULL;
    char *ip;
    int ip_len;
    zval *resource;
    uint32_t index;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &ip, &ip_len) == FAILURE) {
		return;
	}

    resource = zend_read_property(iplookup_ce,getThis(),"fp",sizeof("fp")-1,0 TSRMLS_CC);

    php_stream_from_zval(stream,&resource);

    if( ! stream)
    {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "file resource not exists!");
        RETURN_NULL();
    }

    index = find_index(ip2long(ip),stream);

    RETURN_LONG((long)index);
}

ZEND_METHOD(IpLookUp,search_ip)
{
    php_stream *stream = NULL;
    char *ip;
    int ip_len;
    zval *resource;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &ip, &ip_len) == FAILURE) {
		return;
	}

    resource = zend_read_property(iplookup_ce,getThis(),"fp",sizeof("fp")-1,0 TSRMLS_CC);

    php_stream_from_zval(stream,&resource);

    if( ! stream)
    {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "file resource not exists!");
        RETURN_NULL();
    }

    RETURN_ZVAL(get_location(stream,ip),0,1);
}

ZEND_METHOD(IpLookUp,update_qqwry_file)
{
    zval *file;
    file = zend_read_property(iplookup_ce,getThis(),"file",sizeof("file")-1,0);

    RETURN_LONG((long)update_qqwry_file(Z_STRVAL_P(file)));
}

ZEND_METHOD(IpLookUp,__destruct)
{
    php_stream *stream = NULL;
    zval *resource;
    resource = zend_read_property(iplookup_ce,getThis(),"fp",sizeof("fp")-1,0 TSRMLS_CC);

    php_stream_from_zval(stream,&resource);

    if( ! stream)
    {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "file resource not exists!");
        return;
    }

    php_stream_close(stream);
    return;
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and
   unfold functions in source code. See the corresponding marks just before
   function definition, where the functions purpose is also documented. Please
   follow this convention for the convenience of others editing your code.
*/


/* {{{ php_iplookup_init_globals
 */
/* Uncomment this function if you have INI entries*/
static void php_iplookup_init_globals(zend_iplookup_globals *iplookup_globals)
{
	iplookup_globals->qqwry_file = NULL;
}

/* {{{ iplookup_functions[]
 *
 * Every user visible function must have an entry in iplookup_functions[].
 */
const zend_function_entry iplookup_functions[] = {
	PHP_ME(IpLookUp,__construct,	arginfo_iplookup___construct,ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)		/* For testing, remove later. */
	PHP_ME(IpLookUp,total_ip_num,NULL,ZEND_ACC_PUBLIC)
	PHP_ME(IpLookUp,get_index,arginfo_iplookup_get_index,ZEND_ACC_PUBLIC)
	PHP_ME(IpLookUp,search_ip,arginfo_iplookup_search_ip,ZEND_ACC_PUBLIC)
	PHP_ME(IpLookUp,update_qqwry_file,NULL,ZEND_ACC_PUBLIC)
	PHP_ME(IpLookUp,__destruct,NULL,ZEND_ACC_PUBLIC|ZEND_ACC_DTOR)
	PHP_FE_END	/* Must be the last line in iplookup_functions[] */
};
/* }}} */

/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(iplookup)
{
	/* If you have INI entries, uncomment these lines
	REGISTER_INI_ENTRIES();
	*/
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce,"IpLookUp",iplookup_functions);
	iplookup_ce = zend_register_internal_class(&ce TSRMLS_CC);

	//注册文件资源
	//le_iplookup = zend_register_list_destructors_ex(NULL,NULL,"FILE",module_number);

	//定义类属性
	zend_declare_property_null(iplookup_ce,"fp",sizeof("fp")-1,ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(iplookup_ce,"file",sizeof("file")-1,ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_long(iplookup_ce,"startIndexOffset",sizeof("startIndexOffset")-1,0,ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_long(iplookup_ce,"endIndexOffset",sizeof("endIndexOffset")-1,0,ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_long(iplookup_ce,"totalIpNum",sizeof("totalIpNum")-1,0,ZEND_ACC_PUBLIC TSRMLS_CC);
	REGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(iplookup)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(iplookup)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(iplookup)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(iplookup)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "iplookup support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ iplookup_module_entry
 */
zend_module_entry iplookup_module_entry = {
	STANDARD_MODULE_HEADER,
	"iplookup",
	NULL,
	PHP_MINIT(iplookup),
	PHP_MSHUTDOWN(iplookup),
	PHP_RINIT(iplookup),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(iplookup),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(iplookup),
	PHP_IPLOOKUP_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

uint32_t get_index_offset(php_stream *stream)
{
    unsigned char head[4];
    php_stream_read(stream,head,4);

    return (uint32_t)LE_32(&head[0]);
}

uint32_t ip2long(const char *ip) {
    uint32_t ip_long=0;
    uint8_t ip_len=strlen(ip);
    uint32_t ip_sec=0;
    int8_t ip_level=3;
    uint8_t i,n;
    for (i=0;i<=ip_len;i++) {
        if (i!=ip_len && ip[i]!='.' && ip[i]<48 || ip[i]>57) {
            continue;
        }
        if (ip[i]=='.' || i==ip_len) {
            /*too many .*/
            if (ip_level==-1) {
                return 0;
            }
            for (n=0;n<ip_level;n++) {
                ip_sec*=256;
            }
            ip_long+=ip_sec;
            if (i==ip_len) {
                break;
            }
            ip_level--;
            ip_sec=0;
        } else {
            /*char '0' == int 48*/
            ip_sec=ip_sec*10+(ip[i]-48);
        }
    }
    return ip_long;
}

uint32_t find_index(const uint32_t ip,php_stream *stream) {
    uint32_t index_ip;
    unsigned char index_bytes[7];
    uint32_t index_mid;
    while (1){
        if ((end_index_offset - start_index_offset) == INDEX_LENGTH) {
            break;
        }
        //printf("index:%u:%u\n",index_start,index_end);
        index_mid = (end_index_offset - start_index_offset) / INDEX_LENGTH;
        if (index_mid % 2 == 0) {
            index_mid = index_mid / 2;
        } else {
            index_mid=(index_mid + 1) / 2;
        }
        index_mid = start_index_offset + index_mid * INDEX_LENGTH;
        php_stream_seek(stream,index_mid,SEEK_SET);
        php_stream_read(stream,index_bytes,7);
        index_ip=(uint32_t)LE_32(&index_bytes[0]);
        if (index_ip == ip) {
            break;
        } else if (index_ip < ip) {
            start_index_offset = index_mid;
        } else {
            end_index_offset = index_mid;
        }
    }
    if (index_ip > ip) {
        php_stream_seek(stream,start_index_offset,SEEK_SET);
        php_stream_read(stream,index_bytes,INDEX_LENGTH);
    }
    return (uint32_t)LE_24(&index_bytes[4]);
}

int find_location_by_index(php_stream *stream,const uint32_t data_index,char *location)
{
    unsigned char c;
    unsigned char data_index_bytes[3];
    uint32_t jump_data_index=0;
    if (data_index) {
        php_stream_seek(stream,data_index,SEEK_SET);
    }
    c=php_stream_getc(stream);
    switch (c) {
        case REDIRECT_TYPE_2:
        case REDIRECT_TYPE_1:
            php_stream_read(stream,data_index_bytes,3);
            jump_data_index=LE_24(&data_index_bytes[0]);
            php_stream_seek(stream,jump_data_index,SEEK_SET);
            break;
        default:
            location[strlen(location)]=c;
    }
    if (c) {
        while (c=php_stream_getc(stream)) {
            location[strlen(location)]=c;
        }
    }
    return 1;
}

int is_cz88(const char *str) {
    int i;
    int l=strlen(str)-7;
    for (i=0;i<l;i++) {
        if (str[i]=='C'
            && str[i+1]=='Z'
            && str[i+2]=='8'
            && str[i+3]=='8'
            && str[i+4]=='.'
            && str[i+5]=='N'
            && str[i+6]=='E'
            && str[i+7]=='T'
        ) {
            return 1;
        }
    }
    return 0;
}

zval *get_location(php_stream *stream,const char * ip)
{
    uint32_t ip_long;
    zval *result = (zval *)ecalloc(1,sizeof(zval));
    unsigned char data_index_bytes[3];
    uint32_t data_index;
    uint32_t addr2_offset;
    char country[CHAR_NUM],area[CHAR_NUM];
    unsigned char c;

    ip_long = ip2long(ip);
    if(result == NULL){
        return NULL;
    }
    array_init(result);

    if ( !stream ) {
        return 0;
    }
    php_stream_seek(stream,0,SEEK_SET);
    data_index = find_index(ip_long,stream);
    //fprintf(stderr,"index:%u:%u\n",ftell(qqwry_file),data_index);

    /*ip 4 + mode byte 1*/
    php_stream_seek(stream,data_index+4,SEEK_SET);
    c = php_stream_getc(stream);
    if (c == REDIRECT_TYPE_1) {
        php_stream_read(stream,data_index_bytes,3);
        data_index = LE_24(&data_index_bytes[0]);
        php_stream_seek(stream,data_index,SEEK_SET);
        c = php_stream_getc(stream);
        /*制造一个假的4bytes位移，抵充ip*/
        data_index -= 4;
    }

    if (c == REDIRECT_TYPE_2) {
        /*
         * ip 4 + mode byte 1 + addr1 offset 3
         * 这里ip的4个bytes不一定是真的，有可能是上一条注释里提到的情况
         */
        addr2_offset = data_index + 8;
        php_stream_read(stream,data_index_bytes,3);

        data_index = LE_24(&data_index_bytes[0]);
        php_stream_seek(stream,data_index,SEEK_SET);
        while (c = php_stream_getc(stream)) {
            country[strlen(country)] = c;
        }
        find_location_by_index(stream,addr2_offset,area);
    } else {
        country[strlen(country)] = c;
        while (c = php_stream_getc(stream)) {
            country[strlen(country)] = c;
        }
        find_location_by_index(stream,0,area);
    }
    if (is_cz88(country)) {
        country[0]='\0';
    }
    if (is_cz88(area)) {
        area[0]='\0';
    }

    //数组中添加元素
    //add_assoc_string(result,"all",string_merge);
    add_assoc_string(result,"country",country,1);
    add_assoc_string(result,"area",area,1);

    return result;
}


int update_qqwry_file(const char *file)
{
    return 1;
}

#ifdef COMPILE_DL_IPLOOKUP
ZEND_GET_MODULE(iplookup)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
