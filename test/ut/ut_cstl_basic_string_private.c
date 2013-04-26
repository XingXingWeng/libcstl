#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <google/cmockery.h>

#include "cstl/cstl_def.h"
#include "cstl/citerator.h"
#include "cstl/clist.h"
#include "cstl/cvector.h"
#include "cstl/cstl_basic_string_iterator.h"
#include "cstl/cstl_basic_string_private.h"
#include "cstl/cstl_basic_string.h"
#include "cstl_basic_string_aux.h"

#include "ut_def.h"
#include "ut_cstl_basic_string_private.h"

UT_SUIT_DEFINATION(cstl_basic_string_private, _create_basic_string_representation)

/*
 * test _create_basic_string_representation
 */
UT_CASE_DEFINATION(_create_basic_string_representation)
void test__create_basic_string_representation__invalid_elemsize(void** state)
{
    expect_assert_failure(_create_basic_string_representation(0, 0, 0));
}

void test__create_basic_string_representation__capacity_0(void** state)
{
    _basic_string_rep_t* prep = _create_basic_string_representation(0, 0, 1);
    assert_true(prep != NULL);
    assert_true(prep->_t_elemsize == 1);
    assert_true(prep->_t_length == 0);
    assert_true(prep->_t_capacity == 0);
    assert_true(prep->_n_refcount == -1);
}

void test__create_basic_string_representation__new_capacity_0(void** state)
{
    _basic_string_rep_t* prep = _create_basic_string_representation(0, 4, 1);
    assert_true(prep != NULL);
    assert_true(prep->_t_elemsize == 1);
    assert_true(prep->_t_length == 0);
    assert_true(prep->_t_capacity == 0);
    assert_true(prep->_n_refcount == -1);
}

void test__create_basic_string_representation__old_capacity_0(void** state)
{
    _basic_string_rep_t* prep = _create_basic_string_representation(8, 0, 1);
    assert_true(prep != NULL);
    assert_true(prep->_t_elemsize == 1);
    assert_true(prep->_t_length == 0);
    assert_true(prep->_t_capacity == 8);
    assert_true(prep->_n_refcount == -1);
}

void test__create_basic_string_representation__capacity_new_le_old(void** state)
{
    _basic_string_rep_t* prep = _create_basic_string_representation(8, 10, 1);
    assert_true(prep != NULL);
    assert_true(prep->_t_elemsize == 1);
    assert_true(prep->_t_length == 0);
    assert_true(prep->_t_capacity == 8);
    assert_true(prep->_n_refcount == -1);
}

void test__create_basic_string_representation__capacity_new_gt_old(void** state)
{
    _basic_string_rep_t* prep = _create_basic_string_representation(18, 10, 1);
    assert_true(prep != NULL);
    assert_true(prep->_t_elemsize == 1);
    assert_true(prep->_t_length == 0);
    assert_true(prep->_t_capacity == 20);
    assert_true(prep->_n_refcount == -1);
}

void test__create_basic_string_representation__capacity_new_gt_old_twice(void** state)
{
    _basic_string_rep_t* prep = _create_basic_string_representation(48, 10, 1);
    assert_true(prep != NULL);
    assert_true(prep->_t_elemsize == 1);
    assert_true(prep->_t_length == 0);
    assert_true(prep->_t_capacity == 48);
    assert_true(prep->_n_refcount == -1);
}

/*
 * test _basic_string_rep_increase_shared
 */
UT_CASE_DEFINATION(_basic_string_rep_increase_shared)
void test__basic_string_rep_increase_shared__null(void** state)
{
    expect_assert_failure(_basic_string_rep_increase_shared(NULL));
}
void test__basic_string_rep_increase_shared__shared(void** state)
{
    _basic_string_rep_t* prep = _create_basic_string_representation(0, 0, 1);
    prep->_n_refcount = 10;
    _basic_string_rep_increase_shared(prep);
    assert(prep->_n_refcount == 11);
    free(prep);
}
void test__basic_string_rep_increase_shared__not_shared(void** state)
{
    _basic_string_rep_t* prep = _create_basic_string_representation(0, 0, 1);
    prep->_n_refcount = 0;
    _basic_string_rep_increase_shared(prep);
    assert(prep->_n_refcount == 1);
    free(prep);
}

/*
 * test _basic_string_rep_reduce_shared
 */
UT_CASE_DEFINATION(_basic_string_rep_reduce_shared)
void test__basic_string_rep_reduce_shared__null(void** state)
{
    expect_assert_failure(_basic_string_rep_reduce_shared(NULL));
}
void test__basic_string_rep_reduce_shared__not_shared(void** state)
{
    _basic_string_rep_t* prep = _create_basic_string_representation(0, 0, 1);
    _basic_string_rep_set_sharable(prep);
    assert(_basic_string_rep_reduce_shared(prep) == NULL);
}
void test__basic_string_rep_reduce_shared__shared(void** state)
{
    _basic_string_rep_t* prep_reduce = NULL;
    _basic_string_rep_t* prep = _create_basic_string_representation(0, 0, 1);
    prep->_n_refcount = 10;
    prep_reduce = _basic_string_rep_reduce_shared(prep);
    assert(prep == prep_reduce);
    assert(prep->_n_refcount == 9);
    free(prep);
}

/*
 * test _basic_string_rep_clone
 */
UT_CASE_DEFINATION(_basic_string_rep_clone)
void test__basic_string_rep_clone__null(void** state)
{
    expect_assert_failure(_basic_string_rep_clone(NULL));
}
void test__basic_string_rep_clone__length_0(void** state)
{
    _basic_string_rep_t* prep = _create_basic_string_representation(0, 0, 1);
    _basic_string_rep_t* prep_clone = _basic_string_rep_clone(prep);
    assert(prep_clone != NULL);
    assert(prep_clone->_n_refcount == -1);
    assert(prep_clone->_t_capacity == 0);
    assert(prep_clone->_t_elemsize == 1);
    assert(prep_clone->_t_length == 0);
    free(prep);
    free(prep_clone);
}
void test__basic_string_rep_clone__length_not_0(void** state)
{
    _basic_string_rep_t* prep = _create_basic_string_representation(10, 0, 4);
    prep->_t_length = 4;
    memset(_basic_string_rep_get_data(prep), 0xcc, prep->_t_elemsize * prep->_t_length);
    _basic_string_rep_t* prep_clone = _basic_string_rep_clone(prep);
    assert(prep_clone != NULL);
    assert(prep_clone->_n_refcount == -1);
    assert(prep_clone->_t_capacity == 10);
    assert(prep_clone->_t_elemsize == 4);
    assert(prep_clone->_t_length == 4);
    assert(memcmp(_basic_string_rep_get_data(prep), _basic_string_rep_get_data(prep_clone), prep->_t_length * prep->_t_elemsize) == 0);
    free(prep);
    free(prep_clone);
}

/*
 * test _basic_string_rep_get_data
 */
UT_CASE_DEFINATION(_basic_string_rep_get_data)
void test__basic_string_rep_get_data__rep_null(void** state)
{
    expect_assert_failure(_basic_string_rep_get_data(NULL));
}

void test__basic_string_rep_get_data__successfully(void** state)
{
    _basic_string_rep_t* prep = _create_basic_string_representation(0, 0, 1);
    void* pv_data = _basic_string_rep_get_data(prep);
    assert_true(pv_data == prep+1);
}

/*
 * test _basic_string_rep_get_representation
 */
UT_CASE_DEFINATION(_basic_string_rep_get_representation)
void test__basic_string_rep_get_representation__data_null(void** state)
{
    expect_assert_failure(_basic_string_rep_get_representation(NULL));
}

void test__basic_string_rep_get_representation__successfully(void** state)
{
    _basic_string_rep_t* prep = _create_basic_string_representation(0, 0, 1);
    _basic_string_rep_t* prep_check = _basic_string_rep_get_representation(prep+1);
    assert_true(prep == prep_check);
}

/*
 * test _basic_string_rep_get_length
 */
UT_CASE_DEFINATION(_basic_string_rep_get_length)
void test__basic_string_rep_get_length__null(void** state)
{
    expect_assert_failure(_basic_string_rep_get_length(NULL));
}
void test__basic_string_rep_get_length__0(void** state)
{
    _basic_string_rep_t* prep = _create_basic_string_representation(0, 0, 1);
    assert_true(_basic_string_rep_get_length(prep) == 0);
}
void test__basic_string_rep_get_length__n(void** state)
{
    _basic_string_rep_t* prep = _create_basic_string_representation(100, 0, 1);
    prep->_t_length = 10;
    assert_true(_basic_string_rep_get_length(prep) == 10);
}
/*
 * test _basic_string_rep_set_length
 */
UT_CASE_DEFINATION(_basic_string_rep_set_length)
void test__basic_string_rep_set_lenght__null(void** state)
{
    expect_assert_failure(_basic_string_rep_set_length(NULL, 0));
}
void test__basic_string_rep_set_lenght__len_ge_capacity(void** state)
{
    _basic_string_rep_t* prep = _create_basic_string_representation(0, 0, 1);
    expect_assert_failure(_basic_string_rep_set_length(prep, 10));
}
void test__basic_string_rep_set_lenght__0(void** state)
{
    void* pv_data = NULL;
    int elem = 0;
    _basic_string_rep_t* prep = _create_basic_string_representation(0, 0, sizeof(int));
    _basic_string_rep_set_length(prep, 0);
    assert_true(_basic_string_rep_get_length(prep) == 0);
    pv_data = _basic_string_rep_get_data(prep);
    assert_true(memcmp(pv_data, &elem, sizeof(int)) == 0);
}
void test__basic_string_rep_set_lenght__n(void** state)
{
    void* pv_data = NULL;
    int elem = 0;
    _basic_string_rep_t* prep = _create_basic_string_representation(10, 0, sizeof(int));
    _basic_string_rep_set_length(prep, 6);
    assert_true(_basic_string_rep_get_length(prep) == 6);
    pv_data = _basic_string_rep_get_data(prep);
    assert_true(memcmp(((char*)pv_data) + prep->_t_elemsize * 6, &elem, prep->_t_elemsize) == 0);
}

/*
 * test _basic_string_rep_is_shared
 */
UT_CASE_DEFINATION(_basic_string_rep_is_shared)
void test__basic_string_rep_is_shared__null(void** state)
{
    expect_assert_failure(_basic_string_rep_is_shared(NULL));
}
void test__basic_string_rep_is_shared__true(void** state)
{
    _basic_string_rep_t* prep = _create_basic_string_representation(0, 0, 1);
    prep->_n_refcount = 1;
    assert_true(_basic_string_rep_is_shared(prep));
}
void test__basic_string_rep_is_shared__false(void** state)
{
    _basic_string_rep_t* prep = _create_basic_string_representation(0, 0, 1);
    assert_false(_basic_string_rep_is_shared(prep));
}
/*
 * test _basic_string_rep_set_sharable
 */
UT_CASE_DEFINATION(_basic_string_rep_set_sharable)
void test__basic_string_rep_set_sharable__null(void** state)
{
    expect_assert_failure(_basic_string_rep_set_sharable(NULL));
}
void test__basic_string_rep_set_sharable__successfully(void** state)
{
    _basic_string_rep_t* prep = _create_basic_string_representation(0, 0, 1);
    _basic_string_rep_set_sharable(prep);
    assert_true(prep->_n_refcount == 0);
}

/*
 * test _basic_string_rep_is_leaked
 */
UT_CASE_DEFINATION(_basic_string_rep_is_leaked)
void test__basic_string_rep_is_leaked__null(void** state)
{
    expect_assert_failure(_basic_string_rep_is_leaked(NULL));
}
void test__basic_string_rep_is_leaked__true(void** state)
{
    _basic_string_rep_t* prep = _create_basic_string_representation(0, 0, 1);
    assert_true(_basic_string_rep_is_leaked(prep));
}
void test__basic_string_rep_is_leaked__false(void** state)
{
    _basic_string_rep_t* prep = _create_basic_string_representation(0, 0, 1);
    prep->_n_refcount = 1;
    assert_false(_basic_string_rep_is_leaked(prep));
}
/*
 * test _basic_string_rep_set_leaked
 */
UT_CASE_DEFINATION(_basic_string_rep_set_leaked)
void test__basic_string_rep_set_leaked__null(void** state)
{
    expect_assert_failure(_basic_string_rep_set_leaked(NULL));
}
void test__basic_string_rep_set_leaked__successfully(void** state)
{
    _basic_string_rep_t* prep = _create_basic_string_representation(0, 0, 1);
    _basic_string_rep_set_leaked(prep);
    assert_true(prep->_n_refcount == -1);
}


/*
 * test _create_basic_string
 */
UT_CASE_DEFINATION(_create_basic_string)
void test__create_basic_string__null_typename(void** state)
{
    expect_assert_failure(_create_basic_string(NULL));
}

void test__create_basic_string__unregisted_type(void** state)
{
    assert_true(_create_basic_string("unregisted_type_t") == NULL);
}

void test__create_basic_string__c_builtin_type(void** state)
{
    basic_string_t* pt_basic_string = _create_basic_string("unsigned int");
    assert_true(pt_basic_string != NULL);
    basic_string_destroy(pt_basic_string);
}

void test__create_basic_string__libcstl_builtin_type(void** state)
{
    basic_string_t* pt_basic_string = _create_basic_string("list_t<int>");
    assert_true(pt_basic_string != NULL);
    basic_string_destroy(pt_basic_string);
}

void test__create_basic_string__registed_type(void** state)
{
    typedef struct _tag__create_basic_string__registed_type{int n_elem;}_create_basic_string__registed_type_t;
    basic_string_t* pt_basic_string = NULL;
    type_register(_create_basic_string__registed_type_t, NULL, NULL, NULL, NULL);
    pt_basic_string = _create_basic_string("_create_basic_string__registed_type_t");
    assert_true(pt_basic_string != NULL);
    basic_string_destroy(pt_basic_string);
}

/*
 * test _create_basic_string_auxiliary
 */
UT_CASE_DEFINATION(_create_basic_string_auxiliary)
void test__create_basic_string_auxiliary__null_basic_string_container(void** state)
{
    expect_assert_failure(_create_basic_string_auxiliary(NULL, "int"));
}

void test__create_basic_string_auxiliary__null_typename(void** state)
{
    basic_string_t bstr;
    expect_assert_failure(_create_basic_string_auxiliary(&bstr, NULL));
}

void test__create_basic_string_auxiliary__unregisted_type(void** state)
{
    basic_string_t bstr;
    assert_false(_create_basic_string_auxiliary(&bstr, "unregisted_type_t"));
}

void test__create_basic_string_auxiliary__c_builtin_type(void** state)
{
    basic_string_t* pt_basic_string = (basic_string_t*)malloc(sizeof(basic_string_t));
    assert_true(pt_basic_string != NULL);
    assert_true(_create_basic_string_auxiliary(pt_basic_string, "int"));
    assert_true(pt_basic_string->_pby_string == NULL);
    assert_true(pt_basic_string->_t_typeinfo._pt_type != NULL);
    assert_true(pt_basic_string->_t_typeinfo._t_style == _TYPE_C_BUILTIN);
    assert_true(strcmp(pt_basic_string->_t_typeinfo._s_typename, _INT_TYPE) == 0);

    basic_string_destroy(pt_basic_string);
}

void test__create_basic_string_auxiliary__libcstl_builtin_type(void** state)
{
    basic_string_t* pt_basic_string = (basic_string_t*)malloc(sizeof(basic_string_t));
    assert_true(pt_basic_string != NULL);
    assert_true(_create_basic_string_auxiliary(pt_basic_string, "list_t< int>    "));
    assert_true(pt_basic_string->_pby_string == NULL);
    assert_true(pt_basic_string->_t_typeinfo._pt_type != NULL);
    assert_true(pt_basic_string->_t_typeinfo._t_style == _TYPE_CSTL_BUILTIN);
    assert_true(strcmp(pt_basic_string->_t_typeinfo._s_typename, "list_t<int>") == 0);

    basic_string_destroy(pt_basic_string);
}

void test__create_basic_string_auxiliary__registed_type(void** state)
{
    typedef struct _tag_create_basic_string_auxiliary__registed_type{int n_elem;}_create_basic_string_auxiliary__registed_type_t;
    basic_string_t* pt_basic_string = (basic_string_t*)malloc(sizeof(basic_string_t));
    assert_true(pt_basic_string != NULL);
    type_register(_create_basic_string_auxiliary__registed_type_t, NULL, NULL, NULL, NULL);
    assert_true(_create_basic_string_auxiliary(pt_basic_string, "_create_basic_string_auxiliary__registed_type_t"));
    assert_true(pt_basic_string->_pby_string == NULL);
    assert_true(pt_basic_string->_t_typeinfo._pt_type != NULL);
    assert_true(pt_basic_string->_t_typeinfo._t_style == _TYPE_USER_DEFINE);
    assert_true(strcmp(pt_basic_string->_t_typeinfo._s_typename, "_create_basic_string_auxiliary__registed_type_t") == 0);

    basic_string_destroy(pt_basic_string);
}

/*
 * test _basic_string_init_elem and _basic_string_init_elem_varg
 */
UT_CASE_DEFINATION(_basic_string_init_elem__basic_string_init_elem_varg)
void test__basic_string_init_elem__basic_string_init_elem_varg__null_basic_string_container(void** state)
{
    expect_assert_failure(_basic_string_init_elem(NULL, 10, 100));
}

void test__basic_string_init_elem__basic_string_init_elem_varg__non_created(void** state)
{
    basic_string_t bstr;
    bstr._t_typeinfo._t_style = 999;

    expect_assert_failure(_basic_string_init_elem(&bstr, 10, 100));
}

void test__basic_string_init_elem__basic_string_init_elem_varg__successfully_0_count(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    _basic_string_init_elem(pt_basic_string, 0, 100);
    assert_true(basic_string_size(pt_basic_string) == 0);
    assert_true(basic_string_capacity(pt_basic_string) == 0);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_init_elem__basic_string_init_elem_varg__successfully(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);

    _basic_string_init_elem(pt_basic_string, 8, 100);
    assert_true(basic_string_size(pt_basic_string) == 8);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        assert_true(*(int*)basic_string_at(pt_basic_string, i) == 100);
    }
    assert_true(basic_string_capacity(pt_basic_string) == 8);
    /*assert_true(basic_string_capacity(pt_basic_string) == 24);*/

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_init_elem__basic_string_init_elem_varg__successfully_large_amounts_of_data(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);

    _basic_string_init_elem(pt_basic_string, 800, 100);
    assert_true(basic_string_size(pt_basic_string) == 800);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        assert_true(*(int*)basic_string_at(pt_basic_string, i) == 100);
    }
    assert_true(basic_string_capacity(pt_basic_string) == 800);
    /*assert_true(basic_string_capacity(pt_basic_string) == 1200);*/

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_init_elem__basic_string_init_elem_varg__successfully_multiple_specificed_element(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);

    _basic_string_init_elem(pt_basic_string, 8, 100, 200, 300, 400);
    assert_true(basic_string_size(pt_basic_string) == 8);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        assert_true(*(int*)basic_string_at(pt_basic_string, i) == 100);
    }
    assert_true(basic_string_capacity(pt_basic_string) == 8);
    /*assert_true(basic_string_capacity(pt_basic_string) == 24);*/

    basic_string_destroy(pt_basic_string);
}

/*
 * test _basic_string_destroy_auxiliary
 */
UT_CASE_DEFINATION(_basic_string_destroy_auxiliary)
void test__basic_string_destroy_auxiliary__null_basic_string_container(void** state)
{
    expect_assert_failure(_basic_string_destroy_auxiliary(NULL));
}

void test__basic_string_destroy_auxiliary__invalid_basic_string_container_finish_less_than_start(void** state)
{
    /* comment for 2.2
    basic_string_t bstr;
    bstr._vec_base._pby_finish = (_byte_t*)0x00;
    bstr._vec_base._pby_start = (_byte_t*)0x08;

    expect_assert_failure(_basic_string_destroy_auxiliary(&bstr));
    */
}

void test__basic_string_destroy_auxiliary__invalid_basic_string_container_endofstorage_less_than_start(void** state)
{
    /* comment for 2.2
    basic_string_t bstr;
    bstr._vec_base._pby_endofstorage = (_byte_t*)0x00;
    bstr._vec_base._pby_start = (_byte_t*)0x08;
    bstr._vec_base._pby_finish = (_byte_t*)0x10;

    expect_assert_failure(_basic_string_destroy_auxiliary(&bstr));
    */
}

void test__basic_string_destroy_auxiliary__successfully_non_init_container(void** state)
{
    /* comment for 2.2
    basic_string_t* pt_basic_string = create_basic_string(int);

    _basic_string_destroy_auxiliary(pt_basic_string);
    assert_true(pt_basic_string->_vec_base._pby_endofstorage == NULL);
    assert_true(pt_basic_string->_vec_base._pby_finish == NULL);
    assert_true(pt_basic_string->_vec_base._pby_start == NULL);

    free(pt_basic_string);
    */
}

void test__basic_string_destroy_auxiliary__successfully_empty_container(void** state)
{
    /* comment for 2.2
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init(pt_basic_string);

    _basic_string_destroy_auxiliary(pt_basic_string);
    assert_true(pt_basic_string->_vec_base._pby_endofstorage == NULL);
    assert_true(pt_basic_string->_vec_base._pby_finish == NULL);
    assert_true(pt_basic_string->_vec_base._pby_start == NULL);

    free(pt_basic_string);
    */
}

void test__basic_string_destroy_auxiliary__successfully(void** state)
{
    /* comment for 2.2
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init_elem(pt_basic_string, 10, 100);

    _basic_string_destroy_auxiliary(pt_basic_string);
    assert_true(pt_basic_string->_vec_base._pby_endofstorage == NULL);
    assert_true(pt_basic_string->_vec_base._pby_finish == NULL);
    assert_true(pt_basic_string->_vec_base._pby_start == NULL);

    free(pt_basic_string);
    */
}

/*
 * test _basic_string_find_elem and _basic_string_find_elem_varg
 */
UT_CASE_DEFINATION(_basic_string_find_elem__basic_string_find_elem_varg)

void test__basic_string_find_elem__basic_string_find_elem_varg__null_basic_string_container(void** state)
{
    expect_assert_failure(_basic_string_find_elem(NULL, 0, 2323));
}

void test__basic_string_find_elem__basic_string_find_elem_varg__non_init_basic_string_container(void** state)
{
    /* comment for 2.2
    basic_string_t* pt_basic_string = create_basic_string(int);

    pt_basic_string->_vec_base._t_typeinfo._t_style = 23423;
    expect_assert_failure(_basic_string_find_elem(pt_basic_string, 0, 111));

    pt_basic_string->_vec_base._t_typeinfo._t_style = _TYPE_C_BUILTIN;
    basic_string_destroy(pt_basic_string);
    */
}

void test__basic_string_find_elem__basic_string_find_elem_varg__invalid_position(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 10, 100);
    assert_true(_basic_string_find_elem(pt_basic_string, 34456, 100) == NPOS);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_elem__basic_string_find_elem_varg__invalid_position_end(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 10, 100);
    assert_true(_basic_string_find_elem(pt_basic_string, basic_string_size(pt_basic_string), 100) == NPOS);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_elem__basic_string_find_elem_varg__invalid_position_NPOS(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 10, 100);
    assert_true(_basic_string_find_elem(pt_basic_string, NPOS, 100) == NPOS);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_elem__basic_string_find_elem_varg__find_failure(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 10, 100);
    assert_true(_basic_string_find_elem(pt_basic_string, 0, 9999) == NPOS);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_elem__basic_string_find_elem_varg__find_failure_middle_pos(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 1, 9999);
    basic_string_push_back(pt_basic_string, 100);
    basic_string_push_back(pt_basic_string, 100);
    basic_string_push_back(pt_basic_string, 100);
    basic_string_push_back(pt_basic_string, 100);
    basic_string_push_back(pt_basic_string, 100);
    basic_string_push_back(pt_basic_string, 100);
    assert_true(_basic_string_find_elem(pt_basic_string, 3, 9999) == NPOS);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_elem__basic_string_find_elem_varg__find_successful(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, 97);
    basic_string_push_back(pt_basic_string, 400);
    basic_string_push_back(pt_basic_string, 97);
    basic_string_push_back(pt_basic_string, -3);
    basic_string_push_back(pt_basic_string, 97);
    basic_string_push_back(pt_basic_string, 700);
    assert_true(_basic_string_find_elem(pt_basic_string, 0, 97) == 0);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_elem__basic_string_find_elem_varg__find_successful_middle(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, 47);
    basic_string_push_back(pt_basic_string, 400);
    basic_string_push_back(pt_basic_string, 97);
    basic_string_push_back(pt_basic_string, -3);
    basic_string_push_back(pt_basic_string, 97);
    basic_string_push_back(pt_basic_string, 700);
    assert_true(_basic_string_find_elem(pt_basic_string, 0, 97) == 2);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_elem__basic_string_find_elem_varg__find_successful_back(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, 47);
    basic_string_push_back(pt_basic_string, 400);
    basic_string_push_back(pt_basic_string, 7);
    basic_string_push_back(pt_basic_string, -3);
    basic_string_push_back(pt_basic_string, 97);
    assert_true(_basic_string_find_elem(pt_basic_string, 0, 97) == 4);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_elem__basic_string_find_elem_varg__find_successful_middle_pos(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, 47);
    basic_string_push_back(pt_basic_string, 400);
    basic_string_push_back(pt_basic_string, 97);
    basic_string_push_back(pt_basic_string, -3);
    basic_string_push_back(pt_basic_string, 97);
    basic_string_push_back(pt_basic_string, 700);
    assert_true(_basic_string_find_elem(pt_basic_string, 3, 97) == 4);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_elem__basic_string_find_elem_varg__cstr_find_failure(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init_elem(pt_basic_string, 10, "100");
    assert_true(_basic_string_find_elem(pt_basic_string, 0, "9999") == NPOS);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_elem__basic_string_find_elem_varg__cstr_find_failure_middle_pos(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init_elem(pt_basic_string, 1, "9999");
    basic_string_push_back(pt_basic_string, "100");
    basic_string_push_back(pt_basic_string, "100");
    basic_string_push_back(pt_basic_string, "100");
    basic_string_push_back(pt_basic_string, "100");
    basic_string_push_back(pt_basic_string, "100");
    basic_string_push_back(pt_basic_string, "100");
    assert_true(_basic_string_find_elem(pt_basic_string, 3, "9999") == NPOS);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_elem__basic_string_find_elem_varg__cstr_find_successful(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, "97");
    basic_string_push_back(pt_basic_string, "400");
    basic_string_push_back(pt_basic_string, "97");
    basic_string_push_back(pt_basic_string, "-3");
    basic_string_push_back(pt_basic_string, "97");
    basic_string_push_back(pt_basic_string, "700");
    assert_true(_basic_string_find_elem(pt_basic_string, 0, "97") == 0);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_elem__basic_string_find_elem_varg__cstr_find_successful_middle(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, "47");
    basic_string_push_back(pt_basic_string, "400");
    basic_string_push_back(pt_basic_string, "97");
    basic_string_push_back(pt_basic_string, "-3");
    basic_string_push_back(pt_basic_string, "97");
    basic_string_push_back(pt_basic_string, "700");
    assert_true(_basic_string_find_elem(pt_basic_string, 0, "97") == 2);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_elem__basic_string_find_elem_varg__cstr_find_successful_back(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, "47");
    basic_string_push_back(pt_basic_string, "400");
    basic_string_push_back(pt_basic_string, "7");
    basic_string_push_back(pt_basic_string, "-3");
    basic_string_push_back(pt_basic_string, "97");
    assert_true(_basic_string_find_elem(pt_basic_string, 0, "97") == 4);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_elem__basic_string_find_elem_varg__cstr_find_successful_middle_pos(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, "47");
    basic_string_push_back(pt_basic_string, "400");
    basic_string_push_back(pt_basic_string, "97");
    basic_string_push_back(pt_basic_string, "-3");
    basic_string_push_back(pt_basic_string, "97");
    basic_string_push_back(pt_basic_string, "700");
    assert_true(_basic_string_find_elem(pt_basic_string, 3, "97") == 4);

    basic_string_destroy(pt_basic_string);
}

/*
 * test _basic_string_rfind_elem and _basic_string_rfind_elem_varg
 */
UT_CASE_DEFINATION(_basic_string_rfind_elem__basic_string_rfind_elem_varg)

void test__basic_string_rfind_elem__basic_string_rfind_elem_varg__null_basic_string_container(void** state)
{
    expect_assert_failure(_basic_string_rfind_elem(NULL, 0, 2323));
}

void test__basic_string_rfind_elem__basic_string_rfind_elem_varg__non_init_basic_string_container(void** state)
{
    /* comment for 2.2
    basic_string_t* pt_basic_string = create_basic_string(int);

    pt_basic_string->_vec_base._t_typeinfo._t_style = 23423;
    expect_assert_failure(_basic_string_rfind_elem(pt_basic_string, 0, 111));

    pt_basic_string->_vec_base._t_typeinfo._t_style = _TYPE_C_BUILTIN;
    basic_string_destroy(pt_basic_string);
    */
}

void test__basic_string_rfind_elem__basic_string_rfind_elem_varg__invalid_position(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 10, 100);
    assert_true(_basic_string_rfind_elem(pt_basic_string, 34456, 100) == 9);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_rfind_elem__basic_string_rfind_elem_varg__invalid_position_begin(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 10, 100);
    assert_true(_basic_string_rfind_elem(pt_basic_string, 0, 100) == 0);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_rfind_elem__basic_string_rfind_elem_varg__invalid_position_NPOS(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 10, 100);
    assert_true(_basic_string_rfind_elem(pt_basic_string, NPOS, 100) == 9);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_rfind_elem__basic_string_rfind_elem_varg__find_failure(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 10, 100);
    assert_true(_basic_string_rfind_elem(pt_basic_string, NPOS, 9999) == NPOS);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_rfind_elem__basic_string_rfind_elem_varg__find_failure_middle_pos(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, 100);
    basic_string_push_back(pt_basic_string, 100);
    basic_string_push_back(pt_basic_string, 100);
    basic_string_push_back(pt_basic_string, 100);
    basic_string_push_back(pt_basic_string, 100);
    basic_string_push_back(pt_basic_string, 100);
    basic_string_push_back(pt_basic_string, 9999);
    assert_true(_basic_string_rfind_elem(pt_basic_string, 3, 9999) == NPOS);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_rfind_elem__basic_string_rfind_elem_varg__find_successful(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, 97);
    basic_string_push_back(pt_basic_string, 400);
    basic_string_push_back(pt_basic_string, 97);
    basic_string_push_back(pt_basic_string, -3);
    basic_string_push_back(pt_basic_string, 97);
    basic_string_push_back(pt_basic_string, 700);
    assert_true(_basic_string_rfind_elem(pt_basic_string, 0, 97) == 0);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_rfind_elem__basic_string_rfind_elem_varg__find_successful_middle(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, 47);
    basic_string_push_back(pt_basic_string, 400);
    basic_string_push_back(pt_basic_string, 97);
    basic_string_push_back(pt_basic_string, -3);
    basic_string_push_back(pt_basic_string, 97);
    basic_string_push_back(pt_basic_string, 700);
    assert_true(_basic_string_rfind_elem(pt_basic_string, 3, 97) == 2);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_rfind_elem__basic_string_rfind_elem_varg__find_successful_back(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, 47);
    basic_string_push_back(pt_basic_string, 400);
    basic_string_push_back(pt_basic_string, 7);
    basic_string_push_back(pt_basic_string, -3);
    basic_string_push_back(pt_basic_string, 97);
    assert_true(_basic_string_rfind_elem(pt_basic_string, 100, 97) == 4);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_rfind_elem__basic_string_rfind_elem_varg__find_successful_middle_pos(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, 47);
    basic_string_push_back(pt_basic_string, 400);
    basic_string_push_back(pt_basic_string, 97);
    basic_string_push_back(pt_basic_string, -3);
    basic_string_push_back(pt_basic_string, 97);
    basic_string_push_back(pt_basic_string, 700);
    assert_true(_basic_string_rfind_elem(pt_basic_string, 4, 97) == 4);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_rfind_elem__basic_string_rfind_elem_varg__cstr_find_failure(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init_elem(pt_basic_string, 10, "100");
    assert_true(_basic_string_rfind_elem(pt_basic_string, NPOS, "9999") == NPOS);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_rfind_elem__basic_string_rfind_elem_varg__cstr_find_failure_middle_pos(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, "100");
    basic_string_push_back(pt_basic_string, "100");
    basic_string_push_back(pt_basic_string, "100");
    basic_string_push_back(pt_basic_string, "100");
    basic_string_push_back(pt_basic_string, "100");
    basic_string_push_back(pt_basic_string, "100");
    basic_string_push_back(pt_basic_string, "9999");
    assert_true(_basic_string_rfind_elem(pt_basic_string, 3, "9999") == NPOS);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_rfind_elem__basic_string_rfind_elem_varg__cstr_find_successful(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, "97");
    basic_string_push_back(pt_basic_string, "400");
    basic_string_push_back(pt_basic_string, "97");
    basic_string_push_back(pt_basic_string, "-3");
    basic_string_push_back(pt_basic_string, "97");
    basic_string_push_back(pt_basic_string, "700");
    assert_true(_basic_string_rfind_elem(pt_basic_string, 0, "97") == 0);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_rfind_elem__basic_string_rfind_elem_varg__cstr_find_successful_middle(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, "47");
    basic_string_push_back(pt_basic_string, "400");
    basic_string_push_back(pt_basic_string, "97");
    basic_string_push_back(pt_basic_string, "-3");
    basic_string_push_back(pt_basic_string, "97");
    basic_string_push_back(pt_basic_string, "700");
    assert_true(_basic_string_rfind_elem(pt_basic_string, 3, "97") == 2);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_rfind_elem__basic_string_rfind_elem_varg__cstr_find_successful_back(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, "47");
    basic_string_push_back(pt_basic_string, "400");
    basic_string_push_back(pt_basic_string, "7");
    basic_string_push_back(pt_basic_string, "-3");
    basic_string_push_back(pt_basic_string, "97");
    assert_true(_basic_string_rfind_elem(pt_basic_string, NPOS, "97") == 4);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_rfind_elem__basic_string_rfind_elem_varg__cstr_find_successful_middle_pos(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, "47");
    basic_string_push_back(pt_basic_string, "400");
    basic_string_push_back(pt_basic_string, "97");
    basic_string_push_back(pt_basic_string, "-3");
    basic_string_push_back(pt_basic_string, "97");
    basic_string_push_back(pt_basic_string, "700");
    assert_true(_basic_string_rfind_elem(pt_basic_string, 4, "97") == 4);

    basic_string_destroy(pt_basic_string);
}

/*
 * test _basic_string_find_first_not_of_elem and _basic_string_find_first_not_of_elem_varg
 */
UT_CASE_DEFINATION(_basic_string_find_first_not_of_elem__basic_string_find_first_not_of_elem_varg)

void test__basic_string_find_first_not_of_elem__basic_string_find_first_not_of_elem_varg__null_basic_string_container(void** state)
{
    expect_assert_failure(_basic_string_find_first_not_of_elem(NULL, 0, 2323));
}

void test__basic_string_find_first_not_of_elem__basic_string_find_first_not_of_elem_varg__non_init_basic_string_container(
    void** state)
{
    /* comment for 2.2
    basic_string_t* pt_basic_string = create_basic_string(int);

    pt_basic_string->_vec_base._t_typeinfo._t_style = 23423;
    expect_assert_failure(_basic_string_find_first_not_of_elem(pt_basic_string, 0, 111));

    pt_basic_string->_vec_base._t_typeinfo._t_style = _TYPE_C_BUILTIN;
    basic_string_destroy(pt_basic_string);
    */
}

void test__basic_string_find_first_not_of_elem__basic_string_find_first_not_of_elem_varg__invalid_position(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 10, 100);
    assert_true(_basic_string_find_first_not_of_elem(pt_basic_string, 34456, 100) == NPOS);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_first_not_of_elem__basic_string_find_first_not_of_elem_varg__invalid_position_end(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 10, 100);
    assert_true(_basic_string_find_first_not_of_elem(pt_basic_string, basic_string_size(pt_basic_string), 100) == NPOS);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_first_not_of_elem__basic_string_find_first_not_of_elem_varg__invalid_position_NPOS(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 10, 100);
    assert_true(_basic_string_find_first_not_of_elem(pt_basic_string, NPOS, 100) == NPOS);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_first_not_of_elem__basic_string_find_first_not_of_elem_varg__find_first_not_of_failure(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 10, 100);
    assert_true(_basic_string_find_first_not_of_elem(pt_basic_string, 0, 100) == NPOS);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_first_not_of_elem__basic_string_find_first_not_of_elem_varg__find_first_not_of_failure_middle_pos(
    void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 1, 9999);
    basic_string_push_back(pt_basic_string, 100);
    basic_string_push_back(pt_basic_string, 100);
    basic_string_push_back(pt_basic_string, 100);
    basic_string_push_back(pt_basic_string, 100);
    basic_string_push_back(pt_basic_string, 100);
    basic_string_push_back(pt_basic_string, 100);
    assert_true(_basic_string_find_first_not_of_elem(pt_basic_string, 3, 100) == NPOS);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_first_not_of_elem__basic_string_find_first_not_of_elem_varg__find_first_not_of_successful(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, 99);
    basic_string_push_back(pt_basic_string, 400);
    basic_string_push_back(pt_basic_string, 97);
    basic_string_push_back(pt_basic_string, -3);
    basic_string_push_back(pt_basic_string, 97);
    basic_string_push_back(pt_basic_string, 700);
    assert_true(_basic_string_find_first_not_of_elem(pt_basic_string, 0, 97) == 0);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_first_not_of_elem__basic_string_find_first_not_of_elem_varg__find_first_not_of_successful_middle(
    void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, 97);
    basic_string_push_back(pt_basic_string, 97);
    basic_string_push_back(pt_basic_string, 400);
    basic_string_push_back(pt_basic_string, -3);
    basic_string_push_back(pt_basic_string, 97);
    basic_string_push_back(pt_basic_string, 700);
    assert_true(_basic_string_find_first_not_of_elem(pt_basic_string, 0, 97) == 2);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_first_not_of_elem__basic_string_find_first_not_of_elem_varg__find_first_not_of_successful_back(
    void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, 97);
    basic_string_push_back(pt_basic_string, 97);
    basic_string_push_back(pt_basic_string, 97);
    basic_string_push_back(pt_basic_string, 97);
    basic_string_push_back(pt_basic_string, 47);
    assert_true(_basic_string_find_first_not_of_elem(pt_basic_string, 0, 97) == 4);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_first_not_of_elem__basic_string_find_first_not_of_elem_varg__find_first_not_of_successful_middle_pos(
    void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, 47);
    basic_string_push_back(pt_basic_string, 400);
    basic_string_push_back(pt_basic_string, 97);
    basic_string_push_back(pt_basic_string, 97);
    basic_string_push_back(pt_basic_string, -3);
    basic_string_push_back(pt_basic_string, 700);
    assert_true(_basic_string_find_first_not_of_elem(pt_basic_string, 3, 97) == 4);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_first_not_of_elem__basic_string_find_first_not_of_elem_varg__cstr_find_first_not_of_failure(
    void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init_elem(pt_basic_string, 10, "100");
    assert_true(_basic_string_find_first_not_of_elem(pt_basic_string, 0, "100") == NPOS);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_first_not_of_elem__basic_string_find_first_not_of_elem_varg__cstr_find_first_not_of_failure_middle_pos(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init_elem(pt_basic_string, 1, "9999");
    basic_string_push_back(pt_basic_string, "100");
    basic_string_push_back(pt_basic_string, "100");
    basic_string_push_back(pt_basic_string, "100");
    basic_string_push_back(pt_basic_string, "100");
    basic_string_push_back(pt_basic_string, "100");
    basic_string_push_back(pt_basic_string, "100");
    assert_true(_basic_string_find_first_not_of_elem(pt_basic_string, 3, "100") == NPOS);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_first_not_of_elem__basic_string_find_first_not_of_elem_varg__cstr_find_first_not_of_successful(
    void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, "99");
    basic_string_push_back(pt_basic_string, "400");
    basic_string_push_back(pt_basic_string, "97");
    basic_string_push_back(pt_basic_string, "-3");
    basic_string_push_back(pt_basic_string, "97");
    basic_string_push_back(pt_basic_string, "700");
    assert_true(_basic_string_find_first_not_of_elem(pt_basic_string, 0, "97") == 0);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_first_not_of_elem__basic_string_find_first_not_of_elem_varg__cstr_find_first_not_of_successful_middle(
    void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, "97");
    basic_string_push_back(pt_basic_string, "97");
    basic_string_push_back(pt_basic_string, "47");
    basic_string_push_back(pt_basic_string, "400");
    basic_string_push_back(pt_basic_string, "-3");
    basic_string_push_back(pt_basic_string, "97");
    basic_string_push_back(pt_basic_string, "700");
    assert_true(_basic_string_find_first_not_of_elem(pt_basic_string, 0, "97") == 2);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_first_not_of_elem__basic_string_find_first_not_of_elem_varg__cstr_find_first_not_of_successful_back(
    void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, "97");
    basic_string_push_back(pt_basic_string, "97");
    basic_string_push_back(pt_basic_string, "97");
    basic_string_push_back(pt_basic_string, "97");
    basic_string_push_back(pt_basic_string, "47");
    assert_true(_basic_string_find_first_not_of_elem(pt_basic_string, 0, "97") == 4);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_first_not_of_elem__basic_string_find_first_not_of_elem_varg__cstr_find_first_not_of_successful_middle_pos(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, "47");
    basic_string_push_back(pt_basic_string, "400");
    basic_string_push_back(pt_basic_string, "97");
    basic_string_push_back(pt_basic_string, "97");
    basic_string_push_back(pt_basic_string, "-3");
    basic_string_push_back(pt_basic_string, "97");
    basic_string_push_back(pt_basic_string, "700");
    assert_true(_basic_string_find_first_not_of_elem(pt_basic_string, 3, "97") == 4);

    basic_string_destroy(pt_basic_string);
}

/*
 * test _basic_string_find_last_not_of_elem and _basic_string_find_last_not_of_elem_varg
 */
UT_CASE_DEFINATION(_basic_string_find_last_not_of_elem__basic_string_find_last_not_of_elem_varg)

void test__basic_string_find_last_not_of_elem__basic_string_find_last_not_of_elem_varg__null_basic_string_container(void** state)
{
    expect_assert_failure(_basic_string_find_last_not_of_elem(NULL, 0, 2323));
}

void test__basic_string_find_last_not_of_elem__basic_string_find_last_not_of_elem_varg__non_init_basic_string_container(void** state)
{
    /* comment for 2.2
    basic_string_t* pt_basic_string = create_basic_string(int);

    pt_basic_string->_vec_base._t_typeinfo._t_style = 23423;
    expect_assert_failure(_basic_string_find_last_not_of_elem(pt_basic_string, 0, 111));

    pt_basic_string->_vec_base._t_typeinfo._t_style = _TYPE_C_BUILTIN;
    basic_string_destroy(pt_basic_string);
    */
}

void test__basic_string_find_last_not_of_elem__basic_string_find_last_not_of_elem_varg__invalid_position(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 10, 100);
    assert_true(_basic_string_find_last_not_of_elem(pt_basic_string, 34456, 101) == 9);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_last_not_of_elem__basic_string_find_last_not_of_elem_varg__invalid_position_begin(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 10, 100);
    assert_true(_basic_string_find_last_not_of_elem(pt_basic_string, 0, 101) == 0);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_last_not_of_elem__basic_string_find_last_not_of_elem_varg__invalid_position_NPOS(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 10, 100);
    assert_true(_basic_string_find_last_not_of_elem(pt_basic_string, NPOS, 101) == 9);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_last_not_of_elem__basic_string_find_last_not_of_elem_varg__find_failure(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 10, 100);
    assert_true(_basic_string_find_last_not_of_elem(pt_basic_string, NPOS, 100) == NPOS);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_last_not_of_elem__basic_string_find_last_not_of_elem_varg__find_failure_middle_pos(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, 9999);
    basic_string_push_back(pt_basic_string, 9999);
    basic_string_push_back(pt_basic_string, 9999);
    basic_string_push_back(pt_basic_string, 9999);
    basic_string_push_back(pt_basic_string, 100);
    basic_string_push_back(pt_basic_string, 100);
    basic_string_push_back(pt_basic_string, 100);
    basic_string_push_back(pt_basic_string, 100);
    basic_string_push_back(pt_basic_string, 100);
    basic_string_push_back(pt_basic_string, 100);
    assert_true(_basic_string_find_last_not_of_elem(pt_basic_string, 3, 9999) == NPOS);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_last_not_of_elem__basic_string_find_last_not_of_elem_varg__find_successful(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, 99);
    basic_string_push_back(pt_basic_string, 400);
    basic_string_push_back(pt_basic_string, 97);
    basic_string_push_back(pt_basic_string, -3);
    basic_string_push_back(pt_basic_string, 97);
    basic_string_push_back(pt_basic_string, 700);
    assert_true(_basic_string_find_last_not_of_elem(pt_basic_string, 0, 97) == 0);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_last_not_of_elem__basic_string_find_last_not_of_elem_varg__find_successful_middle(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, 47);
    basic_string_push_back(pt_basic_string, 400);
    basic_string_push_back(pt_basic_string, -3);
    basic_string_push_back(pt_basic_string, 97);
    basic_string_push_back(pt_basic_string, 97);
    basic_string_push_back(pt_basic_string, 700);
    assert_true(_basic_string_find_last_not_of_elem(pt_basic_string, 3, 97) == 2);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_last_not_of_elem__basic_string_find_last_not_of_elem_varg__find_successful_back(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, 47);
    basic_string_push_back(pt_basic_string, 400);
    basic_string_push_back(pt_basic_string, 7);
    basic_string_push_back(pt_basic_string, 97);
    basic_string_push_back(pt_basic_string, -3);
    assert_true(_basic_string_find_last_not_of_elem(pt_basic_string, 100, 97) == 4);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_last_not_of_elem__basic_string_find_last_not_of_elem_varg__find_successful_middle_pos(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, 47);
    basic_string_push_back(pt_basic_string, 400);
    basic_string_push_back(pt_basic_string, 97);
    basic_string_push_back(pt_basic_string, 97);
    basic_string_push_back(pt_basic_string, -3);
    basic_string_push_back(pt_basic_string, 700);
    assert_true(_basic_string_find_last_not_of_elem(pt_basic_string, 4, 97) == 4);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_last_not_of_elem__basic_string_find_last_not_of_elem_varg__cstr_find_failure(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init_elem(pt_basic_string, 10, "100");
    assert_true(_basic_string_find_last_not_of_elem(pt_basic_string, NPOS, "100") == NPOS);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_last_not_of_elem__basic_string_find_last_not_of_elem_varg__cstr_find_failure_middle_pos(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, "9999");
    basic_string_push_back(pt_basic_string, "9999");
    basic_string_push_back(pt_basic_string, "9999");
    basic_string_push_back(pt_basic_string, "9999");
    basic_string_push_back(pt_basic_string, "100");
    basic_string_push_back(pt_basic_string, "100");
    basic_string_push_back(pt_basic_string, "100");
    basic_string_push_back(pt_basic_string, "100");
    basic_string_push_back(pt_basic_string, "100");
    basic_string_push_back(pt_basic_string, "100");
    assert_true(_basic_string_find_last_not_of_elem(pt_basic_string, 3, "9999") == NPOS);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_last_not_of_elem__basic_string_find_last_not_of_elem_varg__cstr_find_successful(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, "99");
    basic_string_push_back(pt_basic_string, "400");
    basic_string_push_back(pt_basic_string, "97");
    basic_string_push_back(pt_basic_string, "-3");
    basic_string_push_back(pt_basic_string, "97");
    basic_string_push_back(pt_basic_string, "700");
    assert_true(_basic_string_find_last_not_of_elem(pt_basic_string, 0, "97") == 0);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_last_not_of_elem__basic_string_find_last_not_of_elem_varg__cstr_find_successful_middle(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, "47");
    basic_string_push_back(pt_basic_string, "400");
    basic_string_push_back(pt_basic_string, "-3");
    basic_string_push_back(pt_basic_string, "97");
    basic_string_push_back(pt_basic_string, "97");
    basic_string_push_back(pt_basic_string, "700");
    assert_true(_basic_string_find_last_not_of_elem(pt_basic_string, 3, "97") == 2);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_last_not_of_elem__basic_string_find_last_not_of_elem_varg__cstr_find_successful_back(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, "47");
    basic_string_push_back(pt_basic_string, "400");
    basic_string_push_back(pt_basic_string, "7");
    basic_string_push_back(pt_basic_string, "97");
    basic_string_push_back(pt_basic_string, "-3");
    assert_true(_basic_string_find_last_not_of_elem(pt_basic_string, NPOS, "97") == 4);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_find_last_not_of_elem__basic_string_find_last_not_of_elem_varg__cstr_find_successful_middle_pos(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init(pt_basic_string);
    basic_string_push_back(pt_basic_string, "47");
    basic_string_push_back(pt_basic_string, "400");
    basic_string_push_back(pt_basic_string, "97");
    basic_string_push_back(pt_basic_string, "97");
    basic_string_push_back(pt_basic_string, "-3");
    basic_string_push_back(pt_basic_string, "700");
    assert_true(_basic_string_find_last_not_of_elem(pt_basic_string, 4, "97") == 4);

    basic_string_destroy(pt_basic_string);
}

/*
 * test _basic_string_connect_elem and _basic_string_connect_elem_varg
 */
UT_CASE_DEFINATION(_basic_string_connect_elem__basic_string_connect_elem_varg)
void test__basic_string_connect_elem__basic_string_connect_elem_varg__null_basic_string_container(void** state)
{
    expect_assert_failure(_basic_string_connect_elem(NULL, 2323));
}

void test__basic_string_connect_elem__basic_string_connect_elem_varg__non_inited_basic_string_container(void** state)
{
    /* commnet for 2.2
    basic_string_t* pt_basic_string = create_basic_string(int);

    pt_basic_string->_vec_base._t_typeinfo._t_style = 23423;
    expect_assert_failure(_basic_string_connect_elem(pt_basic_string, 111));

    pt_basic_string->_vec_base._t_typeinfo._t_style = _TYPE_C_BUILTIN;
    basic_string_destroy(pt_basic_string);
    */
}

void test__basic_string_connect_elem__basic_string_connect_elem_varg__empty_container(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init(pt_basic_string);
    _basic_string_connect_elem(pt_basic_string, 45);
    assert_true(basic_string_size(pt_basic_string) == 1);
    assert_true(*(int*)basic_string_at(pt_basic_string, 0) == 45);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_connect_elem__basic_string_connect_elem_varg__non_empty_container(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 3, 100);
    _basic_string_connect_elem(pt_basic_string, 45);
    assert_true(basic_string_size(pt_basic_string) == 4);
    assert_true(*(int*)basic_string_at(pt_basic_string, 3) == 45);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_connect_elem__basic_string_connect_elem_varg__cstr_empty_container(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init(pt_basic_string);
    _basic_string_connect_elem(pt_basic_string, "45");
    assert_true(basic_string_size(pt_basic_string) == 1);
    assert_true(strcmp((char*)basic_string_at(pt_basic_string, 0), "45") == 0);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_connect_elem__basic_string_connect_elem_varg__cstr_non_empty_container(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init_elem(pt_basic_string, 3, "100");
    _basic_string_connect_elem(pt_basic_string, "45");
    assert_true(basic_string_size(pt_basic_string) == 4);
    assert_true(strcmp((char*)basic_string_at(pt_basic_string, 3), "45") == 0);

    basic_string_destroy(pt_basic_string);
}

/*
 * test _basic_string_assign_elem and _basic_string_assign_elem_varg
 */
UT_CASE_DEFINATION(_basic_string_assign_elem__basic_string_assign_elem_varg)
void test__basic_string_assign_elem__basic_string_assign_elem_varg__null_container(void** state)
{
    expect_assert_failure(_basic_string_assign_elem(NULL, 10, 232323));
}

void test__basic_string_assign_elem__basic_string_assign_elem_varg__non_inited_container(void** state)
{
    /* comment for 2.2
    basic_string_t* pt_basic_string = create_basic_string(int);


    pt_basic_string->_vec_base._t_typeinfo._t_style = 23423;
    expect_assert_failure(_basic_string_assign_elem(pt_basic_string, 111));

    pt_basic_string->_vec_base._t_typeinfo._t_style = _TYPE_C_BUILTIN;

    / *assert_true(false);* /

    basic_string_destroy(pt_basic_string);
    */
}

void test__basic_string_assign_elem__basic_string_assign_elem_varg__empty_container_assign_empty(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init(pt_basic_string);
    _basic_string_assign_elem(pt_basic_string, 0, 100);
    assert_true(basic_string_size(pt_basic_string) == 0);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_assign_elem__basic_string_assign_elem_varg__empty_container_assign_non_empty(void** state)
{
    int i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init(pt_basic_string);
    _basic_string_assign_elem(pt_basic_string, 3, 100);
    assert_true(basic_string_size(pt_basic_string) == 3);
    for(i = 0; i < 3; ++i)
    {
        assert_true(*(int*)basic_string_at(pt_basic_string, i) == 100);
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_assign_elem__basic_string_assign_elem_varg__non_empty_container_assign_empty(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 4, 100);
    _basic_string_assign_elem(pt_basic_string, 0, 700);
    assert_true(basic_string_size(pt_basic_string) == 0);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_assign_elem__basic_string_assign_elem_varg__non_empty_container_assign_less(void** state)
{
    int i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 4, 100);
    _basic_string_assign_elem(pt_basic_string, 2, 700);
    assert_true(basic_string_size(pt_basic_string) == 2);
    for(i = 0; i < 2; ++i)
    {
        assert_true(*(int*)basic_string_at(pt_basic_string, i) == 700);
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_assign_elem__basic_string_assign_elem_varg__non_empty_container_assign_equal(void** state)
{
    int i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 4, 100);
    _basic_string_assign_elem(pt_basic_string, 4, 700);
    assert_true(basic_string_size(pt_basic_string) == 4);
    for(i = 0; i < 4; ++i)
    {
        assert_true(*(int*)basic_string_at(pt_basic_string, i) == 700);
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_assign_elem__basic_string_assign_elem_varg__non_empty_container_assign_greater(void** state)
{
    int i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 4, 100);
    _basic_string_assign_elem(pt_basic_string, 100, 700);
    assert_true(basic_string_size(pt_basic_string) == 100);
    for(i = 0; i < 100; ++i)
    {
        assert_true(*(int*)basic_string_at(pt_basic_string, i) == 700);
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_assign_elem__basic_string_assign_elem_varg__cstr_empty_container_assign_empty(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init(pt_basic_string);
    _basic_string_assign_elem(pt_basic_string, 0, "100");
    assert_true(basic_string_size(pt_basic_string) == 0);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_assign_elem__basic_string_assign_elem_varg__cstr_empty_container_assign_non_empty(void** state)
{
    int i = 0;
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init(pt_basic_string);
    _basic_string_assign_elem(pt_basic_string, 3, "100");
    assert_true(basic_string_size(pt_basic_string) == 3);
    for(i = 0; i < 3; ++i)
    {
        assert_true(strcmp((char*)basic_string_at(pt_basic_string, i), "100") == 0);
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_assign_elem__basic_string_assign_elem_varg__cstr_non_empty_container_assign_empty(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init_elem(pt_basic_string, 4, "100");
    _basic_string_assign_elem(pt_basic_string, 0, "700");
    assert_true(basic_string_size(pt_basic_string) == 0);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_assign_elem__basic_string_assign_elem_varg__cstr_non_empty_container_assign_less(void** state)
{
    int i = 0;
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init_elem(pt_basic_string, 4, "100");
    _basic_string_assign_elem(pt_basic_string, 2, "700");
    assert_true(basic_string_size(pt_basic_string) == 2);
    for(i = 0; i < 2; ++i)
    {
        assert_true(strcmp((char*)basic_string_at(pt_basic_string, i), "700") == 0);
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_assign_elem__basic_string_assign_elem_varg__cstr_non_empty_container_assign_equal(void** state)
{
    int i = 0;
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init_elem(pt_basic_string, 4, "100");
    _basic_string_assign_elem(pt_basic_string, 4, "700");
    assert_true(basic_string_size(pt_basic_string) == 4);
    for(i = 0; i < 4; ++i)
    {
        assert_true(strcmp((char*)basic_string_at(pt_basic_string, i), "700") == 0);
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_assign_elem__basic_string_assign_elem_varg__cstr_non_empty_container_assign_greater(void** state)
{
    int i = 0;
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init_elem(pt_basic_string, 4, "100");
    _basic_string_assign_elem(pt_basic_string, 100, "700");
    assert_true(basic_string_size(pt_basic_string) == 100);
    for(i = 0; i < 100; ++i)
    {
        assert_true(strcmp((char*)basic_string_at(pt_basic_string, i), "700") == 0);
    }

    basic_string_destroy(pt_basic_string);
}

/*
 * test _basic_string_push_back and _basic_string_push_back_varg
 */
UT_CASE_DEFINATION(_basic_string_push_back__basic_string_push_back_varg)
void test__basic_string_push_back__basic_string_push_back_varg__null_container(void** state)
{
    expect_assert_failure(_basic_string_push_back(NULL, 23));
}

void test__basic_string_push_back__basic_string_push_back_varg__non_inited_container(void** state)
{
    /* comment for 2.2
    basic_string_t* pt_basic_string = create_basic_string(int);


    pt_basic_string->_vec_base._t_typeinfo._t_style = 23423;
    expect_assert_failure(_basic_string_push_back(pt_basic_string, 111));

    pt_basic_string->_vec_base._t_typeinfo._t_style = _TYPE_C_BUILTIN;

    / *assert_true(false);* /

    basic_string_destroy(pt_basic_string);
    */
}

void test__basic_string_push_back__basic_string_push_back_varg__empty(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init(pt_basic_string);
    _basic_string_push_back(pt_basic_string, 1);
    assert_true(basic_string_size(pt_basic_string) == 1);
    assert_true(*(int*)iterator_get_pointer(iterator_prev(basic_string_end(pt_basic_string))) == 1);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_push_back__basic_string_push_back_varg__non_empty(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 100, 100);
    _basic_string_push_back(pt_basic_string, 1);
    assert_true(basic_string_size(pt_basic_string) == 101);
    assert_true(*(int*)iterator_get_pointer(iterator_prev(basic_string_end(pt_basic_string))) == 1);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_push_back__basic_string_push_back_varg__cstr_empty(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init(pt_basic_string);
    _basic_string_push_back(pt_basic_string, "1");
    assert_true(basic_string_size(pt_basic_string) == 1);
    assert_true(strcmp((char*)iterator_get_pointer(iterator_prev(basic_string_end(pt_basic_string))), "1") == 0);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_push_back__basic_string_push_back_varg__cstr_non_empty(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init_elem(pt_basic_string, 100, "100");
    _basic_string_push_back(pt_basic_string, "1");
    assert_true(basic_string_size(pt_basic_string) == 101);
    assert_true(strcmp((char*)iterator_get_pointer(iterator_prev(basic_string_end(pt_basic_string))), "1") == 0);

    basic_string_destroy(pt_basic_string);
}

/*
 * test _basic_string_pop_back
 */
UT_CASE_DEFINATION(_basic_string_pop_back)
void test__basic_string_pop_back__null_container(void** state)
{
    expect_assert_failure(_basic_string_pop_back(NULL));
}

void test__basic_string_pop_back__non_inited_container(void** state)
{
    /* comment for 2.2
    basic_string_t* pt_basic_string = create_basic_string(int);

    pt_basic_string->_vec_base._t_typeinfo._t_style = 23423;
    expect_assert_failure(_basic_string_pop_back(pt_basic_string));

    pt_basic_string->_vec_base._t_typeinfo._t_style = _TYPE_C_BUILTIN;

    basic_string_destroy(pt_basic_string);
    */
}

void test__basic_string_pop_back__empty(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init(pt_basic_string);
    expect_assert_failure(_basic_string_pop_back(pt_basic_string));

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_pop_back__non_empty(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 100, 2323);
    basic_string_push_back(pt_basic_string, 9999);
    _basic_string_pop_back(pt_basic_string);
    assert_true(basic_string_size(pt_basic_string) == 100);
    assert_true(*(int*)iterator_get_pointer(iterator_prev(basic_string_end(pt_basic_string))) == 2323);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_pop_back__cstr_non_empty(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init_elem(pt_basic_string, 100, "2323");
    basic_string_push_back(pt_basic_string, "9999");
    _basic_string_pop_back(pt_basic_string);
    assert_true(basic_string_size(pt_basic_string) == 100);
    assert_true(strcmp((char*)iterator_get_pointer(iterator_prev(basic_string_end(pt_basic_string))), "2323") == 0);

    basic_string_destroy(pt_basic_string);
}

/*
 * test _basic_string_resize and _basic_string_resize_varg
 */
UT_CASE_DEFINATION(_basic_string_resize__basic_string_resize_varg)
void test__basic_string_resize__basic_string_resize_varg__null_basic_string_container(void** state)
{
    expect_assert_failure(_basic_string_resize(NULL, 10, 100));
}

void test__basic_string_resize__basic_string_resize_varg__non_inited(void** state)
{
    /* comment for 2.2
    basic_string_t* pt_basic_string = create_basic_string(int);
    pt_basic_string->_vec_base._pby_start = (_byte_t*)0x987;

    expect_assert_failure(_basic_string_resize(pt_basic_string, 10, 100));

    pt_basic_string->_vec_base._pby_start = NULL;
    basic_string_destroy(pt_basic_string);
    */
}

void test__basic_string_resize__basic_string_resize_varg__successfully_0_resize_0(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init(pt_basic_string);

    assert_true(basic_string_size(pt_basic_string) == 0);
    assert_true(basic_string_capacity(pt_basic_string) == 0);
    _basic_string_resize(pt_basic_string, 0, 100);
    assert_true(basic_string_size(pt_basic_string) == 0);
    assert_true(basic_string_capacity(pt_basic_string) == 0);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_resize__basic_string_resize_varg__successfully_0_resize_10(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init(pt_basic_string);

    assert_true(basic_string_size(pt_basic_string) == 0);
    assert_true(basic_string_capacity(pt_basic_string) == 0);
    _basic_string_resize(pt_basic_string, 10, 100);
    assert_true(basic_string_size(pt_basic_string) == 10);
    assert_true(basic_string_capacity(pt_basic_string) == 26);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        assert_true(*(int*)basic_string_at(pt_basic_string, i) == 100);
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_resize__basic_string_resize_varg__successfully_0_resize_1000(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init(pt_basic_string);

    assert_true(basic_string_size(pt_basic_string) == 0);
    assert_true(basic_string_capacity(pt_basic_string) == 0);
    _basic_string_resize(pt_basic_string, 1000, 100);
    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        assert_true(*(int*)basic_string_at(pt_basic_string, i) == 100);
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_resize__basic_string_resize_varg__successfully_10_resize_0(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init_elem(pt_basic_string, 10, 0);

    assert_true(basic_string_size(pt_basic_string) == 10);
    assert_true(basic_string_capacity(pt_basic_string) == 26);
    _basic_string_resize(pt_basic_string, 0, 100);
    assert_true(basic_string_size(pt_basic_string) == 0);
    assert_true(basic_string_capacity(pt_basic_string) == 26);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_resize__basic_string_resize_varg__successfully_10_resize_10(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init_elem(pt_basic_string, 10, 0);

    assert_true(basic_string_size(pt_basic_string) == 10);
    assert_true(basic_string_capacity(pt_basic_string) == 26);
    _basic_string_resize(pt_basic_string, 10, 100);
    assert_true(basic_string_size(pt_basic_string) == 10);
    assert_true(basic_string_capacity(pt_basic_string) == 26);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        assert_true(*(int*)basic_string_at(pt_basic_string, i) == 0);
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_resize__basic_string_resize_varg__successfully_10_resize_1000(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init_elem(pt_basic_string, 10, 0);

    assert_true(basic_string_size(pt_basic_string) == 10);
    assert_true(basic_string_capacity(pt_basic_string) == 26);
    _basic_string_resize(pt_basic_string, 1000, 100);
    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i < 10)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 0);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 100);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_resize__basic_string_resize_varg__successfully_1000_resize_0(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init_elem(pt_basic_string, 1000, 0);

    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    _basic_string_resize(pt_basic_string, 0, 100);
    assert_true(basic_string_size(pt_basic_string) == 0);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_resize__basic_string_resize_varg__successfully_1000_resize_10(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init_elem(pt_basic_string, 1000, 0);

    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    _basic_string_resize(pt_basic_string, 10, 100);
    assert_true(basic_string_size(pt_basic_string) == 10);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        assert_true(*(int*)basic_string_at(pt_basic_string, i) == 0);
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_resize__basic_string_resize_varg__successfully_1000_resize_1000(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init_elem(pt_basic_string, 1000, 0);

    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    _basic_string_resize(pt_basic_string, 1000, 100);
    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        assert_true(*(int*)basic_string_at(pt_basic_string, i) == 0);
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_resize__basic_string_resize_varg__successfully_1000_resize_1200(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init_elem(pt_basic_string, 1000, 0);

    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    _basic_string_resize(pt_basic_string, 1200, 100);
    assert_true(basic_string_size(pt_basic_string) == 1200);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i < 1000)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 0);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 100);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_resize__basic_string_resize_varg__successfully_1000_resize_2000(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init_elem(pt_basic_string, 1000, 0);

    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    _basic_string_resize(pt_basic_string, 2000, 100);
    assert_true(basic_string_size(pt_basic_string) == 2000);
    assert_true(basic_string_capacity(pt_basic_string) == 3000);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i < 1000)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 0);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 100);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_resize__basic_string_resize_varg__successfully_1000_resize_2000_multiple(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init_elem(pt_basic_string, 1000, 0);

    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    _basic_string_resize(pt_basic_string, 2000, 100, 110, 120, 130, 140, 150);
    assert_true(basic_string_size(pt_basic_string) == 2000);
    assert_true(basic_string_capacity(pt_basic_string) == 3000);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i < 1000)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 0);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 100);
        }
    }

    basic_string_destroy(pt_basic_string);
}

/*
 * test _basic_string_append_elem and _basic_string_append_elem_varg
 */
UT_CASE_DEFINATION(_basic_string_append_elem__basic_string_append_elem_varg)
void test__basic_string_append_elem__basic_string_append_elem_varg__null_container(void** state)
{
    expect_assert_failure(_basic_string_append_elem(NULL, 1, 100));
}

void test__basic_string_append_elem__basic_string_append_elem_varg__non_inited_container(void** state)
{
    /* comment for 2.2
    basic_string_t* pt_basic_string = create_basic_string(int);

    pt_basic_string->_vec_base._t_typeinfo._t_style = 23423;
    expect_assert_failure(_basic_string_append_elem(pt_basic_string, 0, 111));
    pt_basic_string->_vec_base._t_typeinfo._t_style = _TYPE_C_BUILTIN;

    basic_string_destroy(pt_basic_string);
    */
}

void test__basic_string_append_elem__basic_string_append_elem_varg__empty_append_empty(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init(pt_basic_string);
    _basic_string_append_elem(pt_basic_string, 0, 100);
    assert_true(basic_string_size(pt_basic_string) == 0);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_append_elem__basic_string_append_elem_varg__empty_append_non_empty(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init(pt_basic_string);
    _basic_string_append_elem(pt_basic_string, 10, 100);
    assert_true(basic_string_size(pt_basic_string) == 10);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        assert_true(*(int*)basic_string_at(pt_basic_string, i) == 100);
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_append_elem__basic_string_append_elem_varg__non_empty_append_empty(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 10, 100);
    _basic_string_append_elem(pt_basic_string, 0, 900);
    assert_true(basic_string_size(pt_basic_string) == 10);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        assert_true(*(int*)basic_string_at(pt_basic_string, i) == 100);
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_append_elem__basic_string_append_elem_varg__non_empty_append_non_empty(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 10, 100);
    _basic_string_append_elem(pt_basic_string, 10, 900);
    assert_true(basic_string_size(pt_basic_string) == 20);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i < 10)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 100);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 900);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_append_elem__basic_string_append_elem_varg__cstr_empty_append_empty(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init(pt_basic_string);
    _basic_string_append_elem(pt_basic_string, 0, "100");
    assert_true(basic_string_size(pt_basic_string) == 0);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_append_elem__basic_string_append_elem_varg__cstr_empty_append_non_empty(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init(pt_basic_string);
    _basic_string_append_elem(pt_basic_string, 10, "100");
    assert_true(basic_string_size(pt_basic_string) == 10);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        assert_true(strcmp((char*)basic_string_at(pt_basic_string, i), "100") == 0);
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_append_elem__basic_string_append_elem_varg__cstr_non_empty_append_empty(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init_elem(pt_basic_string, 10, "100");
    _basic_string_append_elem(pt_basic_string, 0, "900");
    assert_true(basic_string_size(pt_basic_string) == 10);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        assert_true(strcmp((char*)basic_string_at(pt_basic_string, i), "100") == 0);
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_append_elem__basic_string_append_elem_varg__cstr_non_empty_append_non_empty(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(char*);

    basic_string_init_elem(pt_basic_string, 10, "100");
    _basic_string_append_elem(pt_basic_string, 10, "900");
    assert_true(basic_string_size(pt_basic_string) == 20);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i < 10)
        {
            assert_true(strcmp((char*)basic_string_at(pt_basic_string, i), "100") == 0);
        }
        else
        {
            assert_true(strcmp((char*)basic_string_at(pt_basic_string, i), "900") == 0);
        }
    }

    basic_string_destroy(pt_basic_string);
}

/*
 * test _basic_string_insert_n and _basic_string_insert_n_varg
 */
UT_CASE_DEFINATION(_basic_string_insert_n__basic_string_insert_n_varg)
void test__basic_string_insert_n__basic_string_insert_n_varg__null_basic_string_container(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init(pt_basic_string);

    expect_assert_failure(_basic_string_insert_n(NULL, basic_string_begin(pt_basic_string), 10, 100));

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_insert_n__basic_string_insert_n_varg__non_inited(void** state)
{
    /* comment for 2.2
    basic_string_t* pt_basic_string = create_basic_string(int);
    pt_basic_string->_vec_base._pby_start = (_byte_t*)0x78;

    expect_assert_failure(_basic_string_insert_n(pt_basic_string, basic_string_begin(pt_basic_string), 10, 100));

    pt_basic_string->_vec_base._pby_start = NULL;
    basic_string_destroy(pt_basic_string);
    */
}

void test__basic_string_insert_n__basic_string_insert_n_varg__invalid_position(void** state)
{
    basic_string_iterator_t it_pos = _create_basic_string_iterator();
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init_elem(pt_basic_string, 10, 0);
    it_pos = basic_string_begin(pt_basic_string);
    it_pos._t_pos._pby_corepos = NULL;

    expect_assert_failure(_basic_string_insert_n(pt_basic_string, it_pos, 10, 100));

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_insert_n__basic_string_insert_n_varg__empty_insert_0(void** state)
{
    basic_string_iterator_t it_pos;
    basic_string_iterator_t it_iter;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init(pt_basic_string);

    assert_true(basic_string_size(pt_basic_string) == 0);
    assert_true(basic_string_capacity(pt_basic_string) == 0);
    it_pos = basic_string_begin(pt_basic_string);
    it_iter = _basic_string_insert_n(pt_basic_string, it_pos, 0, 100);
    assert_true(iterator_equal(it_iter, it_pos));
    assert_true(basic_string_size(pt_basic_string) == 0);
    assert_true(basic_string_capacity(pt_basic_string) == 0);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_insert_n__basic_string_insert_n_varg__empty_insert_10(void** state)
{
    basic_string_iterator_t it_pos;
    basic_string_iterator_t it_iter;
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init(pt_basic_string);

    assert_true(basic_string_size(pt_basic_string) == 0);
    assert_true(basic_string_capacity(pt_basic_string) == 0);
    it_pos = basic_string_begin(pt_basic_string);
    it_iter = _basic_string_insert_n(pt_basic_string, it_pos, 10, 100);
    assert_true(iterator_equal(it_iter, basic_string_begin(pt_basic_string)));
    assert_true(basic_string_size(pt_basic_string) == 10);
    assert_true(basic_string_capacity(pt_basic_string) == 26);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        assert_true(*(int*)basic_string_at(pt_basic_string, i) == 100);
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_insert_n__basic_string_insert_n_varg__empty_insert_1000(void** state)
{
    basic_string_iterator_t it_pos;
    basic_string_iterator_t it_iter;
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init(pt_basic_string);

    assert_true(basic_string_size(pt_basic_string) == 0);
    assert_true(basic_string_capacity(pt_basic_string) == 0);
    it_pos = basic_string_begin(pt_basic_string);
    it_iter = _basic_string_insert_n(pt_basic_string, it_pos, 1000, 100);
    assert_true(iterator_equal(it_iter, basic_string_begin(pt_basic_string)));
    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        assert_true(*(int*)basic_string_at(pt_basic_string, i) == 100);
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_insert_n__basic_string_insert_n_varg__begin_insert_0(void** state)
{
    basic_string_iterator_t it_pos;
    basic_string_iterator_t it_iter;
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init_elem(pt_basic_string, 1000, 0);

    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    it_pos = basic_string_begin(pt_basic_string);
    it_iter = _basic_string_insert_n(pt_basic_string, it_pos, 0, 100);
    assert_true(iterator_equal(it_iter, basic_string_begin(pt_basic_string)));
    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        assert_true(*(int*)basic_string_at(pt_basic_string, i) == 0);
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_insert_n__basic_string_insert_n_varg__begin_insert_10(void** state)
{
    basic_string_iterator_t it_pos;
    basic_string_iterator_t it_iter;
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init_elem(pt_basic_string, 1000, 0);

    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    it_pos = basic_string_begin(pt_basic_string);
    it_iter = _basic_string_insert_n(pt_basic_string, it_pos, 10, 100);
    assert_true(iterator_equal(it_iter, basic_string_begin(pt_basic_string)));
    assert_true(basic_string_size(pt_basic_string) == 1010);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i < 10)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 100);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 0);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_insert_n__basic_string_insert_n_varg__begin_insert_1000(void** state)
{
    basic_string_iterator_t it_pos;
    basic_string_iterator_t it_iter;
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init_elem(pt_basic_string, 1000, 0);

    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    it_pos = basic_string_begin(pt_basic_string);
    it_iter = _basic_string_insert_n(pt_basic_string, it_pos, 1000, 100);
    assert_true(iterator_equal(it_iter, basic_string_begin(pt_basic_string)));
    assert_true(basic_string_size(pt_basic_string) == 2000);
    assert_true(basic_string_capacity(pt_basic_string) == 3000);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i < 1000)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 100);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 0);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_insert_n__basic_string_insert_n_varg__middle_insert_0(void** state)
{
    basic_string_iterator_t it_pos;
    basic_string_iterator_t it_iter;
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init_elem(pt_basic_string, 1000, 0);

    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    it_pos = iterator_next_n(basic_string_begin(pt_basic_string), 300);
    it_iter = _basic_string_insert_n(pt_basic_string, it_pos, 0, 100);
    assert_true(iterator_equal(it_iter, iterator_next_n(basic_string_begin(pt_basic_string), 300)));
    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        assert_true(*(int*)basic_string_at(pt_basic_string, i) == 0);
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_insert_n__basic_string_insert_n_varg__middle_insert_10(void** state)
{
    basic_string_iterator_t it_pos;
    basic_string_iterator_t it_iter;
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init_elem(pt_basic_string, 1000, 0);

    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    it_pos = iterator_next_n(basic_string_begin(pt_basic_string), 300);
    it_iter = _basic_string_insert_n(pt_basic_string, it_pos, 10, 100);
    assert_true(iterator_equal(it_iter, iterator_next_n(basic_string_begin(pt_basic_string), 300)));
    assert_true(basic_string_size(pt_basic_string) == 1010);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i >= 300 && i < 310)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 100);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 0);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_insert_n__basic_string_insert_n_varg__middle_insert_1000(void** state)
{
    basic_string_iterator_t it_pos;
    basic_string_iterator_t it_iter;
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init_elem(pt_basic_string, 1000, 0);

    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    it_pos = iterator_next_n(basic_string_begin(pt_basic_string), 300);
    it_iter = _basic_string_insert_n(pt_basic_string, it_pos, 1000, 100);
    assert_true(iterator_equal(it_iter, iterator_next_n(basic_string_begin(pt_basic_string), 300)));
    assert_true(basic_string_size(pt_basic_string) == 2000);
    assert_true(basic_string_capacity(pt_basic_string) == 3000);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i >= 300 && i < 1300)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 100);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 0);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_insert_n__basic_string_insert_n_varg__end_insert_0(void** state)
{
    basic_string_iterator_t it_pos;
    basic_string_iterator_t it_iter;
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init_elem(pt_basic_string, 1000, 0);

    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    it_pos = basic_string_end(pt_basic_string);
    it_iter = _basic_string_insert_n(pt_basic_string, it_pos, 0, 100);
    assert_true(iterator_equal(it_iter, basic_string_end(pt_basic_string)));
    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        assert_true(*(int*)basic_string_at(pt_basic_string, i) == 0);
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_insert_n__basic_string_insert_n_varg__end_insert_10(void** state)
{
    basic_string_iterator_t it_pos;
    basic_string_iterator_t it_iter;
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init_elem(pt_basic_string, 1000, 0);

    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    it_pos = basic_string_end(pt_basic_string);
    it_iter = _basic_string_insert_n(pt_basic_string, it_pos, 10, 100);
    assert_true(iterator_equal(it_iter, iterator_next_n(basic_string_begin(pt_basic_string), 1000)));
    assert_true(basic_string_size(pt_basic_string) == 1010);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i >= 1000)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 100);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 0);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_insert_n__basic_string_insert_n_varg__end_insert_1000(void** state)
{
    basic_string_iterator_t it_pos;
    basic_string_iterator_t it_iter;
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init_elem(pt_basic_string, 1000, 0);

    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    it_pos = basic_string_end(pt_basic_string);
    it_iter = _basic_string_insert_n(pt_basic_string, it_pos, 1000, 100);
    assert_true(iterator_equal(it_iter, iterator_next_n(basic_string_begin(pt_basic_string), 1000)));
    assert_true(basic_string_size(pt_basic_string) == 2000);
    assert_true(basic_string_capacity(pt_basic_string) == 3000);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i >= 1000)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 100);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 0);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_insert_n__basic_string_insert_n_varg__end_insert_1000_multiple(void** state)
{
    basic_string_iterator_t it_pos;
    basic_string_iterator_t it_iter;
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init_elem(pt_basic_string, 1000, 0);

    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    it_pos = basic_string_end(pt_basic_string);
    it_iter = _basic_string_insert_n(pt_basic_string, it_pos, 1000, 100, 200, 300, 400, 500);
    assert_true(iterator_equal(it_iter, iterator_next_n(basic_string_begin(pt_basic_string), 1000)));
    assert_true(basic_string_size(pt_basic_string) == 2000);
    assert_true(basic_string_capacity(pt_basic_string) == 3000);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i >= 1000)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 100);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 0);
        }
    }

    basic_string_destroy(pt_basic_string);
}

/*
 * test _basic_string_insert_elem and _basic_string_insert_elem_varg
 */
UT_CASE_DEFINATION(_basic_string_insert_elem__basic_string_insert_elem_varg)
void test__basic_string_insert_elem__basic_string_insert_elem_varg__null_basic_string_container(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init(pt_basic_string);

    expect_assert_failure(_basic_string_insert_elem(NULL, 0, 10, 100));

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_insert_elem__basic_string_insert_elem_varg__non_inited(void** state)
{
    /* comment for 2.2
    basic_string_t* pt_basic_string = create_basic_string(int);
    pt_basic_string->_vec_base._pby_start = (_byte_t*)0x78;

    expect_assert_failure(_basic_string_insert_elem(pt_basic_string, 0, 10, 100));

    pt_basic_string->_vec_base._pby_start = NULL;
    basic_string_destroy(pt_basic_string);
    */
}

void test__basic_string_insert_elem__basic_string_insert_elem_varg__invalid_position(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init_elem(pt_basic_string, 10, 0);

    expect_assert_failure(_basic_string_insert_elem(pt_basic_string, 100, 10, 100));

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_insert_elem__basic_string_insert_elem_varg__empty_insert_0(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init(pt_basic_string);

    assert_true(basic_string_size(pt_basic_string) == 0);
    assert_true(basic_string_capacity(pt_basic_string) == 0);
    _basic_string_insert_elem(pt_basic_string, 0, 0, 100);
    assert_true(basic_string_size(pt_basic_string) == 0);
    assert_true(basic_string_capacity(pt_basic_string) == 0);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_insert_elem__basic_string_insert_elem_varg__empty_insert_10(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init(pt_basic_string);

    assert_true(basic_string_size(pt_basic_string) == 0);
    assert_true(basic_string_capacity(pt_basic_string) == 0);
    _basic_string_insert_elem(pt_basic_string, 0, 10, 100);
    assert_true(basic_string_size(pt_basic_string) == 10);
    assert_true(basic_string_capacity(pt_basic_string) == 26);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        assert_true(*(int*)basic_string_at(pt_basic_string, i) == 100);
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_insert_elem__basic_string_insert_elem_varg__empty_insert_1000(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init(pt_basic_string);

    assert_true(basic_string_size(pt_basic_string) == 0);
    assert_true(basic_string_capacity(pt_basic_string) == 0);
    _basic_string_insert_elem(pt_basic_string, 0, 1000, 100);
    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        assert_true(*(int*)basic_string_at(pt_basic_string, i) == 100);
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_insert_elem__basic_string_insert_elem_varg__begin_insert_0(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init_elem(pt_basic_string, 1000, 0);

    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    _basic_string_insert_elem(pt_basic_string, 0, 0, 100);
    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        assert_true(*(int*)basic_string_at(pt_basic_string, i) == 0);
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_insert_elem__basic_string_insert_elem_varg__begin_insert_10(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init_elem(pt_basic_string, 1000, 0);

    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    _basic_string_insert_elem(pt_basic_string, 0, 10, 100);
    assert_true(basic_string_size(pt_basic_string) == 1010);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i < 10)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 100);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 0);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_insert_elem__basic_string_insert_elem_varg__begin_insert_1000(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init_elem(pt_basic_string, 1000, 0);

    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    _basic_string_insert_elem(pt_basic_string, 0, 1000, 100);
    assert_true(basic_string_size(pt_basic_string) == 2000);
    assert_true(basic_string_capacity(pt_basic_string) == 3000);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i < 1000)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 100);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 0);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_insert_elem__basic_string_insert_elem_varg__middle_insert_0(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init_elem(pt_basic_string, 1000, 0);

    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    _basic_string_insert_elem(pt_basic_string, 300, 0, 100);
    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        assert_true(*(int*)basic_string_at(pt_basic_string, i) == 0);
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_insert_elem__basic_string_insert_elem_varg__middle_insert_10(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init_elem(pt_basic_string, 1000, 0);

    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    _basic_string_insert_elem(pt_basic_string, 300, 10, 100);
    assert_true(basic_string_size(pt_basic_string) == 1010);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i >= 300 && i < 310)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 100);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 0);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_insert_elem__basic_string_insert_elem_varg__middle_insert_1000(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init_elem(pt_basic_string, 1000, 0);

    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    _basic_string_insert_elem(pt_basic_string, 300, 1000, 100);
    assert_true(basic_string_size(pt_basic_string) == 2000);
    assert_true(basic_string_capacity(pt_basic_string) == 3000);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i >= 300 && i < 1300)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 100);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 0);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_insert_elem__basic_string_insert_elem_varg__end_insert_0(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init_elem(pt_basic_string, 1000, 0);

    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    _basic_string_insert_elem(pt_basic_string, 1000, 0, 100);
    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        assert_true(*(int*)basic_string_at(pt_basic_string, i) == 0);
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_insert_elem__basic_string_insert_elem_varg__end_insert_10(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init_elem(pt_basic_string, 1000, 0);

    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    _basic_string_insert_elem(pt_basic_string, 1000, 10, 100);
    assert_true(basic_string_size(pt_basic_string) == 1010);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i >= 1000)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 100);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 0);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_insert_elem__basic_string_insert_elem_varg__end_insert_1000(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init_elem(pt_basic_string, 1000, 0);

    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    _basic_string_insert_elem(pt_basic_string, 1000, 1000, 100);
    assert_true(basic_string_size(pt_basic_string) == 2000);
    assert_true(basic_string_capacity(pt_basic_string) == 3000);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i >= 1000)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 100);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 0);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_insert_elem__basic_string_insert_elem_varg__end_insert_1000_multiple(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init_elem(pt_basic_string, 1000, 0);

    assert_true(basic_string_size(pt_basic_string) == 1000);
    assert_true(basic_string_capacity(pt_basic_string) == 1500);
    _basic_string_insert_elem(pt_basic_string, 1000, 1000, 100, 200, 300, 400, 500);
    assert_true(basic_string_size(pt_basic_string) == 2000);
    assert_true(basic_string_capacity(pt_basic_string) == 3000);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i >= 1000)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 100);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 0);
        }
    }

    basic_string_destroy(pt_basic_string);
}

/*
 * test _basic_string_range_replace_elem and _basic_string_range_replace_elem_varg
 */
UT_CASE_DEFINATION(_basic_string_range_replace_elem__basic_string_range_replace_elem_varg)
void test__basic_string_range_replace_elem__basic_string_range_replace_elem_varg__null_container(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init(pt_basic_string);
    expect_assert_failure(_basic_string_range_replace_elem(
        NULL, basic_string_begin(pt_basic_string), basic_string_end(pt_basic_string), 10, 100));

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_range_replace_elem__basic_string_range_replace_elem_varg__non_inited_container(void** state)
{
    /* comment for 2.2
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_iterator_t it_begin;
    basic_string_iterator_t it_end;

    basic_string_init(pt_basic_string);
    it_begin = basic_string_begin(pt_basic_string);
    it_end = basic_string_end(pt_basic_string);

    pt_basic_string->_vec_base._t_typeinfo._t_style = 20000;
    expect_assert_failure(_basic_string_range_replace_elem(pt_basic_string, it_begin, it_end, 10, 1000));
    pt_basic_string->_vec_base._t_typeinfo._t_style = _TYPE_C_BUILTIN;

    basic_string_destroy(pt_basic_string);
    */
}

void test__basic_string_range_replace_elem__basic_string_range_replace_elem_varg__invalid_range(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_iterator_t it_begin;
    basic_string_iterator_t it_end;

    basic_string_init_elem(pt_basic_string, 10, 100);
    it_begin = basic_string_begin(pt_basic_string);
    it_end = basic_string_end(pt_basic_string);

    expect_assert_failure(_basic_string_range_replace_elem(pt_basic_string, it_end, it_begin, 0, 1000));

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_range_replace_elem__basic_string_range_replace_elem_varg__empty_replace_empty(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_iterator_t it_begin;
    basic_string_iterator_t it_end;

    basic_string_init(pt_basic_string);
    it_begin = basic_string_begin(pt_basic_string);
    it_end = basic_string_end(pt_basic_string);

    _basic_string_range_replace_elem(pt_basic_string, it_begin, it_end, 0, 1000);
    assert_true(basic_string_size(pt_basic_string) == 0);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_range_replace_elem__basic_string_range_replace_elem_varg__empty_replace_non_empty(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_iterator_t it_begin;
    basic_string_iterator_t it_end;

    basic_string_init(pt_basic_string);
    it_begin = basic_string_begin(pt_basic_string);
    it_end = basic_string_end(pt_basic_string);

    _basic_string_range_replace_elem(pt_basic_string, it_begin, it_end, 10, 1000);
    assert_true(basic_string_size(pt_basic_string) == 10);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        assert_true(*(int*)basic_string_at(pt_basic_string, i) == 1000);
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_range_replace_elem__basic_string_range_replace_elem_varg__begin_replace_empty(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_iterator_t it_begin;
    basic_string_iterator_t it_end;

    basic_string_init_elem(pt_basic_string, 10, 1000);
    it_begin = basic_string_begin(pt_basic_string);
    it_end = iterator_next_n(it_begin, 3);

    _basic_string_range_replace_elem(pt_basic_string, it_begin, it_end, 0, 1000);
    assert_true(basic_string_size(pt_basic_string) == 7);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_range_replace_elem__basic_string_range_replace_elem_varg__begin_replace_less(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_iterator_t it_begin;
    basic_string_iterator_t it_end;

    basic_string_init_elem(pt_basic_string, 10, 1000);
    it_begin = basic_string_begin(pt_basic_string);
    it_end = iterator_next_n(it_begin, 3);

    _basic_string_range_replace_elem(pt_basic_string, it_begin, it_end, 1, 8000);
    assert_true(basic_string_size(pt_basic_string) == 8);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i == 0)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 8000);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 1000);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_range_replace_elem__basic_string_range_replace_elem_varg__begin_replace_equal(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_iterator_t it_begin;
    basic_string_iterator_t it_end;

    basic_string_init_elem(pt_basic_string, 10, 1000);
    it_begin = basic_string_begin(pt_basic_string);
    it_end = iterator_next_n(it_begin, 3);

    _basic_string_range_replace_elem(pt_basic_string, it_begin, it_end, 3, 8000);
    assert_true(basic_string_size(pt_basic_string) == 10);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i < 3)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 8000);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 1000);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_range_replace_elem__basic_string_range_replace_elem_varg__begin_replace_greater(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_iterator_t it_begin;
    basic_string_iterator_t it_end;

    basic_string_init_elem(pt_basic_string, 10, 1000);
    it_begin = basic_string_begin(pt_basic_string);
    it_end = iterator_next_n(it_begin, 3);

    _basic_string_range_replace_elem(pt_basic_string, it_begin, it_end, 8, 8000);
    assert_true(basic_string_size(pt_basic_string) == 15);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i < 8)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 8000);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 1000);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_range_replace_elem__basic_string_range_replace_elem_varg__middle_replace_empty(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_iterator_t it_begin;
    basic_string_iterator_t it_end;

    basic_string_init_elem(pt_basic_string, 10, 1000);
    it_begin = basic_string_begin(pt_basic_string);
    it_begin = iterator_next(it_begin);
    it_end = iterator_next_n(it_begin, 3);

    _basic_string_range_replace_elem(pt_basic_string, it_begin, it_end, 0, 1000);
    assert_true(basic_string_size(pt_basic_string) == 7);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_range_replace_elem__basic_string_range_replace_elem_varg__middle_replace_less(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_iterator_t it_begin;
    basic_string_iterator_t it_end;

    basic_string_init_elem(pt_basic_string, 10, 1000);
    it_begin = basic_string_begin(pt_basic_string);
    it_begin = iterator_next(it_begin);
    it_end = iterator_next_n(it_begin, 3);

    _basic_string_range_replace_elem(pt_basic_string, it_begin, it_end, 1, 8000);
    assert_true(basic_string_size(pt_basic_string) == 8);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i == 1)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 8000);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 1000);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_range_replace_elem__basic_string_range_replace_elem_varg__middle_replace_equal(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_iterator_t it_begin;
    basic_string_iterator_t it_end;

    basic_string_init_elem(pt_basic_string, 10, 1000);
    it_begin = basic_string_begin(pt_basic_string);
    it_begin = iterator_next(it_begin);
    it_end = iterator_next_n(it_begin, 3);

    _basic_string_range_replace_elem(pt_basic_string, it_begin, it_end, 3, 8000);
    assert_true(basic_string_size(pt_basic_string) == 10);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i > 0 && i < 4)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 8000);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 1000);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_range_replace_elem__basic_string_range_replace_elem_varg__middle_replace_greater(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_iterator_t it_begin;
    basic_string_iterator_t it_end;

    basic_string_init_elem(pt_basic_string, 10, 1000);
    it_begin = basic_string_begin(pt_basic_string);
    it_begin = iterator_next(it_begin);
    it_end = iterator_next_n(it_begin, 3);

    _basic_string_range_replace_elem(pt_basic_string, it_begin, it_end, 8, 8000);
    assert_true(basic_string_size(pt_basic_string) == 15);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i > 0 && i < 9)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 8000);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 1000);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_range_replace_elem__basic_string_range_replace_elem_varg__end_replace_empty(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_iterator_t it_begin;
    basic_string_iterator_t it_end;

    basic_string_init_elem(pt_basic_string, 10, 1000);
    it_end = basic_string_end(pt_basic_string);
    it_begin = iterator_prev_n(it_end, 3);

    _basic_string_range_replace_elem(pt_basic_string, it_begin, it_end, 0, 1000);
    assert_true(basic_string_size(pt_basic_string) == 7);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_range_replace_elem__basic_string_range_replace_elem_varg__end_replace_less(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_iterator_t it_begin;
    basic_string_iterator_t it_end;

    basic_string_init_elem(pt_basic_string, 10, 1000);
    it_end = basic_string_end(pt_basic_string);
    it_begin = iterator_prev_n(it_end, 3);

    _basic_string_range_replace_elem(pt_basic_string, it_begin, it_end, 1, 8000);
    assert_true(basic_string_size(pt_basic_string) == 8);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i == 7)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 8000);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 1000);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_range_replace_elem__basic_string_range_replace_elem_varg__end_replace_equal(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_iterator_t it_begin;
    basic_string_iterator_t it_end;

    basic_string_init_elem(pt_basic_string, 10, 1000);
    it_end = basic_string_end(pt_basic_string);
    it_begin = iterator_prev_n(it_end, 3);

    _basic_string_range_replace_elem(pt_basic_string, it_begin, it_end, 3, 8000);
    assert_true(basic_string_size(pt_basic_string) == 10);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i > 6)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 8000);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 1000);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_range_replace_elem__basic_string_range_replace_elem_varg__end_replace_greater(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_iterator_t it_begin;
    basic_string_iterator_t it_end;

    basic_string_init_elem(pt_basic_string, 10, 1000);
    it_end = basic_string_end(pt_basic_string);
    it_begin = iterator_prev_n(it_end, 3);

    _basic_string_range_replace_elem(pt_basic_string, it_begin, it_end, 8, 8000);
    assert_true(basic_string_size(pt_basic_string) == 15);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i > 6)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 8000);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 1000);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_range_replace_elem__basic_string_range_replace_elem_varg__replace_all(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_iterator_t it_begin;
    basic_string_iterator_t it_end;

    basic_string_init_elem(pt_basic_string, 10, 1000);
    it_begin = basic_string_begin(pt_basic_string);
    it_end = basic_string_end(pt_basic_string);

    _basic_string_range_replace_elem(pt_basic_string, it_begin, it_end, 8, 8000);
    assert_true(basic_string_size(pt_basic_string) == 8);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        assert_true(*(int*)basic_string_at(pt_basic_string, i) == 8000);
    }

    basic_string_destroy(pt_basic_string);
}

/*
 * test _basic_string_replace_elem and _basic_string_replace_elem_varg
 */
UT_CASE_DEFINATION(_basic_string_replace_elem__basic_string_replace_elem_varg)
void test__basic_string_replace_elem__basic_string_replace_elem_varg__null_container(void** state)
{
    expect_assert_failure(_basic_string_replace_elem(NULL, 0, 0, 0, 0));
}

void test__basic_string_replace_elem__basic_string_replace_elem_varg__non_inited_container(void** state)
{
    /* comment for 2.2
    basic_string_t* pt_basic_string = create_basic_string(int);

    pt_basic_string->_vec_base._t_typeinfo._t_style = 9999;
    expect_assert_failure(_basic_string_replace_elem(pt_basic_string, 0, 0, 0, 0));
    pt_basic_string->_vec_base._t_typeinfo._t_style = _TYPE_C_BUILTIN;

    basic_string_destroy(pt_basic_string);
    */
}

void test__basic_string_replace_elem__basic_string_replace_elem_varg__empty_replace_empty(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init(pt_basic_string);
    _basic_string_replace_elem(pt_basic_string, 0, NPOS, 0, 100);
    assert_true(basic_string_size(pt_basic_string) == 0);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_replace_elem__basic_string_replace_elem_varg__empty_replace_non_empty(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init(pt_basic_string);

    _basic_string_replace_elem(pt_basic_string, 0, NPOS, 10, 1000);
    assert_true(basic_string_size(pt_basic_string) == 10);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        assert_true(*(int*)basic_string_at(pt_basic_string, i) == 1000);
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_replace_elem__basic_string_replace_elem_varg__begin_replace_empty(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 10, 1000);

    _basic_string_replace_elem(pt_basic_string, 0, 3, 0, 1000);
    assert_true(basic_string_size(pt_basic_string) == 7);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_replace_elem__basic_string_replace_elem_varg__begin_replace_less(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 10, 1000);

    _basic_string_replace_elem(pt_basic_string, 0, 3, 1, 8000);
    assert_true(basic_string_size(pt_basic_string) == 8);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i == 0)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 8000);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 1000);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_replace_elem__basic_string_replace_elem_varg__begin_replace_equal(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 10, 1000);

    _basic_string_replace_elem(pt_basic_string, 0, 3, 3, 8000);
    assert_true(basic_string_size(pt_basic_string) == 10);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i < 3)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 8000);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 1000);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_replace_elem__basic_string_replace_elem_varg__begin_replace_greater(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 10, 1000);

    _basic_string_replace_elem(pt_basic_string, 0, 3, 8, 8000);
    assert_true(basic_string_size(pt_basic_string) == 15);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i < 8)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 8000);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 1000);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_replace_elem__basic_string_replace_elem_varg__middle_replace_empty(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 10, 1000);

    _basic_string_replace_elem(pt_basic_string, 1, 3, 0, 1000);
    assert_true(basic_string_size(pt_basic_string) == 7);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_replace_elem__basic_string_replace_elem_varg__middle_replace_less(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 10, 1000);

    _basic_string_replace_elem(pt_basic_string, 1, 3, 1, 8000);
    assert_true(basic_string_size(pt_basic_string) == 8);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i == 1)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 8000);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 1000);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_replace_elem__basic_string_replace_elem_varg__middle_replace_equal(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 10, 1000);

    _basic_string_replace_elem(pt_basic_string, 1, 3, 3, 8000);
    assert_true(basic_string_size(pt_basic_string) == 10);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i > 0 && i < 4)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 8000);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 1000);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_replace_elem__basic_string_replace_elem_varg__middle_replace_greater(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 10, 1000);

    _basic_string_replace_elem(pt_basic_string, 1, 3, 8, 8000);
    assert_true(basic_string_size(pt_basic_string) == 15);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i > 0 && i < 9)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 8000);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 1000);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_replace_elem__basic_string_replace_elem_varg__end_replace_empty(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 10, 1000);

    _basic_string_replace_elem(pt_basic_string, 7, 3, 0, 1000);
    assert_true(basic_string_size(pt_basic_string) == 7);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_replace_elem__basic_string_replace_elem_varg__end_replace_less(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 10, 1000);

    _basic_string_replace_elem(pt_basic_string, 7, 100, 1, 8000);
    assert_true(basic_string_size(pt_basic_string) == 8);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i == 7)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 8000);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 1000);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_replace_elem__basic_string_replace_elem_varg__end_replace_equal(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 10, 1000);

    _basic_string_replace_elem(pt_basic_string, 7, NPOS, 3, 8000);
    assert_true(basic_string_size(pt_basic_string) == 10);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i > 6)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 8000);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 1000);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_replace_elem__basic_string_replace_elem_varg__end_replace_greater(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 10, 1000);

    _basic_string_replace_elem(pt_basic_string, 7, NPOS, 8, 8000);
    assert_true(basic_string_size(pt_basic_string) == 15);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        if(i > 6)
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 8000);
        }
        else
        {
            assert_true(*(int*)basic_string_at(pt_basic_string, i) == 1000);
        }
    }

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_replace_elem__basic_string_replace_elem_varg__replace_all(void** state)
{
    size_t i = 0;
    basic_string_t* pt_basic_string = create_basic_string(int);

    basic_string_init_elem(pt_basic_string, 10, 1000);

    _basic_string_replace_elem(pt_basic_string, 0, NPOS, 8, 8000);
    assert_true(basic_string_size(pt_basic_string) == 8);
    for(i = 0; i < basic_string_size(pt_basic_string); ++i)
    {
        assert_true(*(int*)basic_string_at(pt_basic_string, i) == 8000);
    }

    basic_string_destroy(pt_basic_string);
}

/*
 * test _basic_string_init_elem_auxiliary
 */
UT_CASE_DEFINATION(_basic_string_init_elem_auxiliary)
void test__basic_string_init_elem_auxiliary__null_basic_string_container(void** state)
{
    int elem = 0;
    expect_assert_failure(_basic_string_init_elem_auxiliary(NULL, &elem));
}

void test__basic_string_init_elem_auxiliary__null_elem(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);

    expect_assert_failure(_basic_string_init_elem_auxiliary(pt_basic_string, NULL));

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_init_elem_auxiliary__non_created_basic_string(void** state)
{
    /*int elem = 0;*/
    /*basic_string_t bstr;*/
    /*bstr._vec_base._pby_start = (_byte_t*)0x78;*/

    /*expect_assert_failure(_basic_string_init_elem_auxiliary(&bstr, &elem));*/
    assert_true(true);
}

void test__basic_string_init_elem_auxiliary__successfully_int(void** state)
{
    basic_string_t* pt_basic_string = create_basic_string(int);
    basic_string_init_elem(pt_basic_string, 10, 100);

    _basic_string_init_elem_auxiliary(pt_basic_string, pt_basic_string->_pby_string);
    assert_true(*(int*)basic_string_at(pt_basic_string, 0) == 0);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_init_elem_auxiliary__successfully_cstr(void** state)
{
    /*
    basic_string_t* pt_basic_string = create_basic_string(char*);
    basic_string_init_elem(pt_basic_string, 10, "abcdefg");

    _basic_string_init_elem_auxiliary(pt_basic_string, pt_basic_string->_pby_string);
    assert_true(strcmp((char*)basic_string_at(pt_basic_string, 0), "") == 0);

    basic_string_destroy(pt_basic_string);
    */
    assert_true(false);
}

void test__basic_string_init_elem_auxiliary__successfully_iterator(void** state)
{
    iterator_t it_iter = _create_basic_string_iterator();
    basic_string_t* pt_basic_string = create_basic_string(iterator_t);
    basic_string_init_elem(pt_basic_string, 10, &it_iter);

    _basic_string_init_elem_auxiliary(pt_basic_string, pt_basic_string->_pby_string);
    memset(&it_iter, 0x00, sizeof(iterator_t));
    assert_true(memcmp((iterator_t*)basic_string_at(pt_basic_string, 0), &it_iter, sizeof(iterator_t)) == 0);

    basic_string_destroy(pt_basic_string);
}

void test__basic_string_init_elem_auxiliary__successfully_container(void** state)
{
    vector_t bstr;
    basic_string_t* pt_basic_string = create_basic_string(vector_t<int>);

    _basic_string_init_elem_auxiliary(pt_basic_string, &bstr);
    assert_true(bstr._pby_start == NULL);
    assert_true(bstr._pby_finish == NULL);
    assert_true(bstr._pby_endofstorage == NULL);
    assert_true(bstr._t_typeinfo._t_style == _TYPE_C_BUILTIN);
    assert_true(bstr._t_typeinfo._pt_type != NULL);
    assert_true(strcmp(bstr._t_typeinfo._s_typename, _INT_TYPE) == 0);
    assert_true(_alloc_is_inited(&bstr._t_allocator));

    basic_string_destroy(pt_basic_string);
}

