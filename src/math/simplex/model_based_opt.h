/*++
Copyright (c) 2016 Microsoft Corporation

Module Name:

    model_based_opt.h

Abstract:

    Model-based optimization for linear real arithmetic.

Author:

    Nikolaj Bjorner (nbjorner) 2016-27-4

Revision History:


--*/

#ifndef __MODEL_BASED_OPT_H__
#define __MODEL_BASED_OPT_H__

#include "util.h"
#include "rational.h"

namespace opt {
   
    enum ineq_type {
        t_eq,
        t_lt,
        t_le
    };

    enum bound_type {
        unbounded,
        strict,
        non_strict
    };
    
    class model_based_opt {
    public:
        struct var {
            unsigned m_id;
            rational m_coeff;
            var(unsigned id, rational const& c): m_id(id), m_coeff(c) {}
            struct compare {
                bool operator()(var x, var y) {
                    return x.m_id < y.m_id;
                }
            };
        };
    private:
        struct row {
            row(): m_type(t_le), m_value(0), m_alive(false) {}
            vector<var> m_vars;         // variables with coefficients
            rational    m_coeff;        // constant in inequality
            ineq_type   m_type;         // inequality type
            rational    m_value;        // value of m_vars + m_coeff under interpretation of m_var2value.
            bool        m_alive;        // rows can be marked dead if they have been processed.
        };

        vector<row>             m_rows;
        unsigned                m_objective_id;
        vector<unsigned_vector> m_var2row_ids;
        vector<rational>        m_var2value;
        vector<var>             m_new_vars;
        
        bool invariant();
        bool invariant(row const& r);

        row& objective() { return m_rows[0]; }
        

        bool find_bound(unsigned x, unsigned& bound_index, unsigned_vector& other, bool is_pos);
        
        rational get_coefficient(unsigned row_id, unsigned var_id);

        bool resolve(unsigned row_id1, rational const& a1, unsigned row_id2, unsigned x);

        void multiply(rational const& c, unsigned row_id);

        void add(unsigned row_id1, unsigned row_id2);

        void set_row(row& r, vector<var> const& coeffs, rational const& c, ineq_type rel);

    public:

        model_based_opt();

        // add a fresh variable with value 'value'.
        unsigned add_var(rational const& value);

        // add a constraint. We assume that the constraint is 
        // satisfied under the values provided to the variables.
        void add_constraint(vector<var> const& coeffs, rational const& c, ineq_type r);

        // Set the objective function (linear).
        void set_objective(vector<var> const& coeffs, rational const& c);

        // find a maximal value for the objective function over the current values.
        // in other words, the returned maximal value may not be globally optimal,
        // but the current evaluation of variables are used to select a local
        // optimal.
        bound_type maximize(rational& value);


        void display(std::ostream& out) const;
        void display(std::ostream& out, row const& r) const;

    };

}

#endif 
