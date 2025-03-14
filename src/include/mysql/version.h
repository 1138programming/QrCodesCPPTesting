/* Copyright Abandoned 1996, 1999, 2001 MySQL AB
   This file is public domain and comes with NO WARRANTY of any kind */

/* Version numbers for protocol & mysqld */

#ifndef _mariadb_version_h_
#define _mariadb_version_h_

#ifdef _CUSTOMCONFIG_
#include <custom_conf.h>
#else
#define PROTOCOL_VERSION		10
#define MARIADB_CLIENT_VERSION_STR	"10.2.10"
#define MARIADB_BASE_VERSION		"mariadb-10.2"
#define MARIADB_VERSION_ID		(10000 * 10 + 100 * 2 + 10)
#define MYSQL_VERSION_ID		(10000 * 10 + 100 * 2 + 10)
#define MARIADB_PORT	        	3306
#define MARIADB_UNIX_ADDR               "/tmp/mysql.sock"
#define MYSQL_CONFIG_NAME		"my"

#define MARIADB_PACKAGE_VERSION "3.0.2"
#define MARIADB_PACKAGE_VERSION_ID (10000 * 3 + 100 * 0 + 2)
#define MARIADB_SYSTEM_TYPE "linux-gnu"
#define MARIADB_MACHINE_TYPE "x86_64"
#define MARIADB_PLUGINDIR ""

/* mysqld compile time options */
#ifndef MYSQL_CHARSET
#define MYSQL_CHARSET			""
#endif
#endif

/* Source information */
#define CC_SOURCE_REVISION ""

#endif /* _mariadb_version_h_ */
