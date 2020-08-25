/*
 * This file and its contents are licensed under the Apache License 2.0.
 * Please see the included NOTICE for copyright information and
 * LICENSE-APACHE for a copy of the license.
 */
#ifndef TIMESCALEDB_EXTENSION_H
#define TIMESCALEDB_EXTENSION_H
#include <postgres.h>
#include "extension_constants.h"
#include "export.h"

extern bool ts_extension_invalidate(Oid relid);
extern TSDLLEXPORT bool ts_extension_is_loaded(void);
extern void ts_extension_check_version(const char *so_version);
extern void ts_extension_check_server_version(void);
extern Oid ts_extension_schema_oid(void);
extern char *ts_extension_schema_name(void);

extern char *ts_extension_get_so_name(void);

#ifdef TS_DEBUG
void extension_test_set_unknown_state(void);
#endif

#endif /* TIMESCALEDB_EXTENSION_H */
