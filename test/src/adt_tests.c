/*
 * This file and its contents are licensed under the Apache License 2.0.
 * Please see the included NOTICE for copyright information and
 * LICENSE-APACHE for a copy of the license.
 */
#include "catalog.h"
#include <access/xact.h>
#include <postgres.h>
#include <fmgr.h>

#include "export.h"

#include "test_utils.h"
#include <utils/syscache.h>

TS_FUNCTION_INFO_V1(ts_test_adts);
TS_FUNCTION_INFO_V1(ts_test_extension_cache);

#define VEC_PREFIX int32
#define VEC_ELEMENT_TYPE int32
#define VEC_DECLARE 1
#define VEC_DEFINE 1
#define VEC_SCOPE static inline
#include <adts/vec.h>

#include <adts/bit_array.h>

static void
i32_vec_test(void)
{
	int32_vec *vec = int32_vec_create(CurrentMemoryContext, 0);
	int i;
	uint32 old_capacity;
	for (i = 0; i < 100; i++)
		int32_vec_append(vec, i);

	AssertInt64Eq(vec->num_elements, 100);

	if (vec->max_elements < 100)
		elog(ERROR, "vec capacity %d, should be at least 100", vec->max_elements);

	for (i = 0; i < 100; i++)
		AssertInt64Eq(*int32_vec_at(vec, i), i);

	AssertPtrEq(int32_vec_last(vec), int32_vec_at(vec, vec->num_elements - 1));

	old_capacity = vec->max_elements;
	int32_vec_delete_range(vec, 30, 19);
	AssertInt64Eq(vec->num_elements, 81);
	AssertInt64Eq(vec->max_elements, old_capacity);

	for (i = 0; i < 30; i++)
		AssertInt64Eq(*int32_vec_at(vec, i), i);

	for (; i < 51; i++)
		AssertInt64Eq(*int32_vec_at(vec, i), i + 19);

	AssertPtrEq(int32_vec_last(vec), int32_vec_at(vec, vec->num_elements - 1));

	int32_vec_clear(vec);
	AssertInt64Eq(vec->num_elements, 0);
	AssertInt64Eq(vec->max_elements, old_capacity);

	int32_vec_free_data(vec);
	AssertInt64Eq(vec->num_elements, 0);
	AssertInt64Eq(vec->max_elements, 0);
	AssertPtrEq(vec->data, NULL);

	/* free_data is idempotent */
	int32_vec_free_data(vec);
	AssertInt64Eq(vec->num_elements, 0);
	AssertInt64Eq(vec->max_elements, 0);
	AssertPtrEq(vec->data, NULL);

	int32_vec_free(vec);
}

/* including BitArray should give us uint64_vec */
static void
uint64_vec_test(void)
{
	uint64_vec vec;
	int i;
	uint64_vec_init(&vec, CurrentMemoryContext, 100);
	for (i = 0; i < 30; i++)
		uint64_vec_append(&vec, i + 3);

	AssertInt64Eq(vec.num_elements, 30);
	AssertInt64Eq(vec.max_elements, 100);
	for (i = 0; i < 30; i++)
		AssertInt64Eq(*uint64_vec_at(&vec, i), i + 3);

	uint64_vec_free_data(&vec);
	AssertInt64Eq(vec.num_elements, 0);
	AssertInt64Eq(vec.max_elements, 0);
	AssertPtrEq(vec.data, NULL);
}

static void
bit_array_test(void)
{
	BitArray bits;
	BitArrayIterator iter;
	int i;
	bit_array_init(&bits);

	for (i = 0; i < 65; i++)
		bit_array_append(&bits, i, i);

	bit_array_append(&bits, 0, 0);
	bit_array_append(&bits, 0, 0);
	bit_array_append(&bits, 64, 0x9069060909009090);
	bit_array_append(&bits, 1, 0);
	bit_array_append(&bits, 64, ~0x9069060909009090);
	bit_array_append(&bits, 1, 1);

	bit_array_iterator_init(&iter, &bits);
	for (i = 0; i < 65; i++)
		AssertInt64Eq(bit_array_iter_next(&iter, i), i);

	AssertInt64Eq(bit_array_iter_next(&iter, 0), 0);
	AssertInt64Eq(bit_array_iter_next(&iter, 0), 0);
	AssertInt64Eq(bit_array_iter_next(&iter, 64), 0x9069060909009090);
	AssertInt64Eq(bit_array_iter_next(&iter, 1), 0);
	AssertInt64Eq(bit_array_iter_next(&iter, 64), ~0x9069060909009090);
	AssertInt64Eq(bit_array_iter_next(&iter, 1), 1);

	bit_array_iterator_init_rev(&iter, &bits);
	AssertInt64Eq(bit_array_iter_next_rev(&iter, 1), 1);
	AssertInt64Eq(bit_array_iter_next_rev(&iter, 64), ~0x9069060909009090);
	AssertInt64Eq(bit_array_iter_next_rev(&iter, 1), 0);
	AssertInt64Eq(bit_array_iter_next_rev(&iter, 64), 0x9069060909009090);
	AssertInt64Eq(bit_array_iter_next_rev(&iter, 0), 0);
	AssertInt64Eq(bit_array_iter_next_rev(&iter, 0), 0);
	for (i = 64; i >= 0; i--)
		AssertInt64Eq(bit_array_iter_next_rev(&iter, i), i);
}

Datum
ts_test_adts(PG_FUNCTION_ARGS)
{
	i32_vec_test();
	uint64_vec_test();
	bit_array_test();
	PG_RETURN_VOID();
}


Datum
ts_test_extension_cache(PG_FUNCTION_ARGS)
{
	Oid table1 = catalog_get_table_id(ts_catalog_get(), HYPERTABLE);
	Oid table2 = catalog_get_table_id(ts_catalog_get(), CHUNK);
	elog(WARNING, "invalidating");
	Oid tables[]= {
  112,
  113,
  174,
  175,
  548,
  549,
  826,
  827,
  828,
 1136,
 1137,
 1213,
 1214,
 1232,
 1233,
 1247,
 1249,
 1255,
 1259,
 1260,
 1261,
 1262,
 1417,
 1418,
 2187,
 2224,
 2328,
 2336,
 2337,
 2396,
 2397,
 2579,
 2600,
 2601};
	ResetCatalogCaches();
	for (int i = 0; i< 34;i++)
    	CacheInvalidateRelcacheByRelid(tables[i], CMD_INSERT);
	extension_test_set_unknown_state();
	elog(WARNING, "invalidating");
//	CacheInvalidateRelcacheAll();
	CommandCounterIncrement();
	extension_test_set_unknown_state();
//	ts_extension_is_loaded();
	elog(WARNING, "accepting");
	AcceptInvalidationMessages();
	PG_RETURN_VOID();
}