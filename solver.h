#ifndef __SOLVER_H__
#define __SOLVER_H__

#include <iostream>
#include <vector>

using Literal = int;
using Clause = std::vector<Literal>;
using NormalForm = std::vector<Clause>;
const Literal NullLiteral = 0;

class Solver {
public:
    Solver(std::istream &stream);
    void print_formula();
    Literal unit_clause();
    void unit_propagation(Literal literal);
    bool tautology(Clause &clause);
    void delete_tautology();
    bool pure_literal(Literal literal);
    Literal contains_pure_literal();
    void delete_clauses(Literal literal);
    bool contains_empty_clause();
    Literal find_literal();
    void delete_duplicates(Clause &clause);
    void resolve(Clause &clause1, Clause &clause2, Literal literal);
    void resolve_clauses(Literal literal);
    bool dp_sat();

private:
    unsigned varCount;
    NormalForm formula;
};

#endif