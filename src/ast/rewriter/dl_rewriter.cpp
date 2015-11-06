/*++
Copyright (c) 2010 Microsoft Corporation

Module Name:

    dl_rewriter.cpp

Abstract:

    <abstract>

Author:

    Nikolaj Bjorner (nbjorner) 2010-08-10

Revision History:

--*/

#include"dl_rewriter.h"
    
br_status dl_rewriter::mk_app_core(func_decl * f, unsigned num_args, expr* const* args, expr_ref& result) {
    return mk_app_core(f->get_family_id(), f->get_decl_kind(), num_args, args, f->get_num_parameters(), f->get_parameters(), result);
}

br_status dl_rewriter::mk_app_core(family_id fid, decl_kind k, unsigned num_args, expr * const * args, 
                      unsigned np, parameter const* params, expr_ref & result) {

    ast_manager& m = result.get_manager();
    uint64 v1, v2;
    SASSERT(fid == m_util.get_family_id());
    switch(k) {
    case datalog::OP_DL_LT:
        if (m_util.is_numeral_ext(args[0], v1) &&
            m_util.is_numeral_ext(args[1], v2)) {
            result = (v1 < v2)?m.mk_true():m.mk_false();
            return BR_DONE;
        }
        // x < x <=> false
        if (args[0] == args[1]) {
            result = m.mk_false();
            return BR_DONE;
        }
        // x < 0 <=> false
        if (m_util.is_numeral_ext(args[1], v2) && v2 == 0) {
            result = m.mk_false();
            return BR_DONE;
        }
        // 0 < x <=> 0 != x
        if (m_util.is_numeral_ext(args[1], v1) && v1 == 0) {
            result = m.mk_not(m.mk_eq(args[0], args[1]));
            return BR_DONE;
        }
        break;
        
    default:
        break;
    }
    return BR_FAILED;
}
    


