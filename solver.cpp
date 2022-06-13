#include "solver.h"

#include <algorithm>
#include <unordered_map>

Solver::Solver(std::istream &stream) {
    std::string buffer;

    std::cout << "Uneti formulu u DIMACS CNF formatu:" << std::endl;

    do {
        stream >> buffer;
        if(buffer == "c")
            stream.ignore(std::string::npos, '\n');
    } while(buffer != "p");

    stream >> buffer;

    unsigned clauseCount;
    stream >> varCount >> clauseCount;

    for(int i = 0; i < clauseCount; i++) {
        Clause clause;
        Literal literal;

        while(true) {
            stream >> literal;

            if(literal == NullLiteral)
                break;
            
            clause.push_back(literal);
        }

        formula.push_back(clause);
    }
}

void Solver::print_formula() {
    for(auto &clause: formula) {
        for(auto &literal: clause)
            std::cout << literal << " ";
        std::cout << std::endl;
    }
}

Literal Solver::unit_clause() {
    for(auto &clause: formula)
        if(clause.size() == 1)
            return clause.at(0);
    
    return NullLiteral;
}

void Solver::unit_propagation(Literal literal) {
    delete_clauses(literal);

    for(int i = 0; i < formula.size(); i++) {
        int n = formula[i].size();

        for(int j = 0; j < n; j++)
            if(formula[i][j] == -literal) {
                formula[i][j] = formula[i].back();
                formula[i].pop_back();
                n--;
                j--;
            }
    }
}

bool Solver::tautology(Clause &clause) {
    std::unordered_map<Literal, bool> literals = {};

    for(auto &literal: clause) {
        if(literal > 0) {
            if(literals.find(literal) != literals.end() && literals[literal] == false)
                return true;
            literals[literal] = true;
        }
        else {
            if(literals.find(-literal) != literals.end() && literals[-literal] == true)
                return true;
            literals[-literal] = false;
        }
    }

    return false;
}

void Solver::delete_tautology() {
    int n = formula.size();

    for(int i = 0; i < n; i++) {
        if(tautology(formula[i])) {
            formula[i] = formula.back();
            formula.pop_back();
            n--;
            i--;
        }
    }
}

bool Solver::pure_literal(Literal literal) {
    bool p = false, n = false;

    for(auto &clause: formula) {
        for(auto &l: clause) {
            if(l == literal)
                p = true;
            if(l == -literal)
                n = true;
        }
    }

    if(p && !n)
        return true;
    
    return false;
}

Literal Solver::contains_pure_literal() {
    Literal literal;

    for(literal = 1; literal <= varCount; literal++) {
        if(pure_literal(literal))
            return literal;
        if(pure_literal(-literal))
            return -literal;
    }

    return NullLiteral;
}

void Solver::delete_clauses(Literal literal) {
    int n = formula.size();

    for(int i = 0; i < n; i++) {
        if(std::find(formula[i].begin(), formula[i].end(), literal) != formula[i].end()) {
            formula[i] = formula.back();
            formula.pop_back();
            n--;
            i--;
        }
    }
}

bool Solver::contains_empty_clause() {
    for(auto &clause: formula)
        if(clause.empty())
            return true;
    
    return false;
}

Literal Solver::find_literal() {
    Literal literal;

    for(literal = 1; literal <= varCount; literal++) {
        bool p = false, n = false;

        for(auto &clause: formula) {
            for(auto &l : clause) {
                if(l == literal)
                    p = true;
                if(l == -literal)
                    n = true;
            }
        }

        if(p && n)
            return literal;
    }

    return NullLiteral;
}

void Solver::delete_duplicates(Clause &clause) {
    int n = clause.size();

    for(int i = 0; i < n; i++) {
        for(int j = i + 1; j < n; j++)
            if(clause[i] == clause[j]) {
                clause[j] = clause.back();
                clause.pop_back();
                j--;
                n--;
            }
    }
}

void Solver::resolve(Clause &clause1, Clause &clause2, Literal literal) {
    Clause resolvent(clause1);

    for(int i = 0; i < resolvent.size(); i++)
        if(resolvent[i] == literal) {
            resolvent[i] = resolvent.back();
            resolvent.pop_back();
            break;
        }
    
    resolvent.insert(resolvent.end(), clause2.begin(), clause2.end());

    for(int i = 0; i < resolvent.size(); i++)
        if(resolvent[i] == -literal) {
            resolvent[i] = resolvent.back();
            resolvent.pop_back();
            break;
        }

    delete_duplicates(resolvent);

    formula.push_back(resolvent);
}

void Solver::resolve_clauses(Literal literal) {
    int n = formula.size();

    for(int i = 0; i < n; i++) {
        if(std::find(formula[i].begin(), formula[i].end(), literal) != formula[i].end()) {
            for(int j = i + 1; j < n; j++) {
                if(std::find(formula[j].begin(), formula[j].end(), -literal) != formula[j].end())
                    resolve(formula[i], formula[j], literal);
            }
        }
        else if(std::find(formula[i].begin(), formula[i].end(), -literal) != formula[i].end()) {
            for(int j = i + 1; j < n; j++) {
                if(std::find(formula[j].begin(), formula[j].end(), literal) != formula[j].end())
                    resolve(formula[i], formula[j], -literal);
            }
        }
    }
}

bool Solver::dp_sat() {
    while(true) {
        Literal literal;

        while((literal = unit_clause()) != NullLiteral)
            unit_propagation(literal);
    
        delete_tautology();

        while((literal = contains_pure_literal()) != NullLiteral)
            delete_clauses(literal);

        if(formula.empty())
            return true;
        
        if(contains_empty_clause())
            return false;
        
        literal = find_literal();
        resolve_clauses(literal);

        delete_clauses(literal);
        delete_clauses(-literal);
    }
}