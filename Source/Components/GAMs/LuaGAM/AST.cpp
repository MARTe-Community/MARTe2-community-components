#include "AST.h"
#include <AdvancedErrorManagement.h>
#include <stdio.h>

/*********************/
/**   DEFINITIONS   **/
/*********************/

#define AST_ERROR(token, error)                                                \
  REPORT_ERROR_STATIC(ErrorManagement::InitialisationError,                    \
                      "[Line:%i, Col:%i, Token:`%s`]\t%s", token->row,         \
                      token->col, token->raw.cstr(), error);

namespace MARTe {
namespace LUA {
namespace Rules {

/**
 * @brief Build block node
 * @param[in] tok_it token iterator
 * @param[out] ok control flag reference for correct node building
 * @return Block node
 */
Nodep block(tokens_t::iterator *&tok_it, bool &ok);

/**
 * @brief Build statement noe
 * @param[in] token_i token index
 * @param[out] ok control flag reference for correct node building
 * @return Statement node
 */
Nodep stat(tokens_t::iterator *&tok_it, bool &ok);

/**
 * @brief Build attribute namelist
 * @param[in] token_i token index
 * @param[out] ok control flag reference for correct node building
 * @return Atribute name list node
 */
Nodep attnamelist(tokens_t::iterator *&tok_it, bool &ok);

/**
 * @brief Build return statement node
 * @param[in] token_i token index
 * @param[out] ok control flag reference for correct node building
 * @return Return statement node
 */
Nodep retstat(tokens_t::iterator *&tok_it, bool &ok);

/**
 * @brief Build function name
 * @param[in] token_i token index
 * @param[out] ok control flag reference for correct node building
 * @return Functionbody node
 */
Nodep funcname(tokens_t::iterator *&tok_it, bool &ok);

/**
 * @brief Build variable list
 * @param[in] token_i token index
 * @param[out] ok control flag reference for correct node building
 * @return Variable list node
 */
Nodep varlist(tokens_t::iterator *&tok_it, bool &ok);

/**
 * @brief Build variable or function call
 * @param[in] token_i token index
 * @param[out] ok control flag reference for correct node building
 * @param[in] expected LuaNode type expected to be returned (VAR or
 * FUNCTIONCALL)
 * @return Variable/functioncall node
 */
Nodep var_or_funcall(tokens_t::iterator *&tok_it, bool &ok, LuaNode expected);

/**
 * @brief Build name list
 * @param[in] token_i token index
 * @param[out] ok control flag reference for correct node building
 * @return Name list node
 */
Nodep namelist(tokens_t::iterator *&tok_it, bool &ok);

/**
 * @brief Build expression list
 * @param[in] token_i token index
 * @param[out] ok control flag reference for correct node building
 * @return Expression list list node
 */
Nodep explist(tokens_t::iterator *&tok_it, bool &ok);

/**
 * @brief Build expression
 * @param[in] token_i token index
 * @param[out] ok control flag reference for correct node building
 * @return Expression node
 */
Nodep exp(tokens_t::iterator *&tok_it, bool &ok);

/**
 * @brief Build prefixed expression
 * @param[in] token_i token index
 * @param[out] ok control flag reference for correct node building
 * @return Prefixed expression node
 */
Nodep prefixexp(tokens_t::iterator *&tok_it, bool &ok);

/**
 * @brief Build arguments
 * @param[in] token_i token index
 * @param[out] ok control flag reference for correct node building
 * @return Arguments node
 */
Nodep args(tokens_t::iterator *&tok_it, bool &ok);

/**
 * @brief Build functionbody
 * @param[in] token_i token index
 * @param[out] ok control flag reference for correct node building
 * @return Function body node
 */
Nodep funcbody(tokens_t::iterator *&tok_it, bool &ok);

/**
 * @brief Build parameter list
 * @param[in] token_i token index
 * @param[out] ok control flag reference for correct node building
 * @return Parameter list node
 */
Nodep parlist(tokens_t::iterator *&tok_it, bool &ok);

/**
 * @brief Build table constructor
 * @param[in] token_i token index
 * @param[out] ok control flag reference for correct node building
 * @return Table constructor node
 */
Nodep table(tokens_t::iterator *&tok_it, bool &ok);

/**
 * @brief Build generic Node instance
 * @param[in] token_i token index
 * @param[in] type LuaNode value
 * @return Node instance of declared type
 */
Nodep node(tokens_t::iterator *&tok_it, LuaNode type);
} // namespace Rules
} // namespace LUA
} // namespace MARTe

/*********************/
/** END DEFINITIONS **/
/*********************/

namespace MARTe {
namespace LUA {

void ast_t::add(Nodep n) { nodes.append(n); }

ast_t &ast_t::operator+=(const Nodep &n) {
  nodes.append(n);
  return *this;
}

NodepList *ast_t::operator->() { return &nodes; }

Nodep &ast_t::operator[](const uint32 &ind) const { return nodes[ind]; }

void ast_t::print() {
  for (NodepList::iterator *it = nodes.iterate(); it; it = it->next()) {
    printf("%s\n", it->value()->toString(0, 4, true).cstr());
  }
}

ast_t generate_ast(tokens_t tokens, bool &ok) {
  uint32 token_i = 0;
  ast_t ast;
  tokens_t::iterator *tok_it = tokens->iterate();
  while (tok_it && tok_it->value()->type != ENDCODE && ok) {
    ast += Rules::block(tok_it, ok);
  }
  ok &= ast->len() > 0;
  return ast;
}

Nodep Rules::block(tokens_t::iterator *&tok_it, bool &ok) {
  Nodep block = new Node(BLOCK);
  while (tok_it && tok_it->value()->type != RETURN &&
         tok_it->value()->type != ENDCODE && tok_it->value()->type != END &&
         tok_it->value()->type != ELSEIF && tok_it->value()->type != UNTIL &&
         tok_it->value()->type != ELSE && ok) {
    block->append(Rules::stat(tok_it, ok));
  }
  if (tok_it->value()->type == RETURN && ok) {
    block->append(Rules::retstat(tok_it, ok));
  }

  if (!ok) {
    block->del();
  }

  return block;
}

Nodep Rules::stat(tokens_t::iterator *&tok_it, bool &ok) {
  Nodep stat;
  if (tok_it && tok_it->value()->type != ENDCODE && ok) {

    // Comment
    if (tok_it->value()->type == COMM) {
      stat = Rules::node(tok_it, COMMENT);
    }

    // Semicolon/break
    else if (tok_it->value()->type == SEMCOL ||
             tok_it->value()->type == BREAK) {
      stat = Rules::node(tok_it, STAT);
    }

    // Reqiure
    else if (tok_it->value()->type == REQUIRE) {
      stat = Rules::node(tok_it, STAT);
      stat->append(Rules::node(tok_it, MODULE));
    }

    // Label
    else if (tok_it->value()->type == COLCOL) {
      stat = Rules::node(tok_it, STAT);
      if (tok_it->value()->type == ID) {
        stat->append(Rules::node(tok_it, LABEL));
      } else {
        AST_ERROR(tok_it->value(), "Missing name for `label` statement");
        ok = false;
      }
      if (tok_it->value()->type != COLCOL) {
        AST_ERROR(tok_it->value(),
                  "Missing closing double semicolon for `label` statement");
        ok = false;
      } else {
        tok_it = tok_it->next();
      }
    }

    // Goto
    else if (tok_it->value()->type == GOTO) {
      stat = Rules::node(tok_it, STAT);
      if (tok_it->value()->type == ID) {
        stat->append(Rules::node(tok_it, NAME));
      } else {
        AST_ERROR(tok_it->value(), "Missing name for `goto` statement");
        ok = false;
      }
    }

    // Do
    else if (tok_it->value()->type == DO) {
      stat = Rules::node(tok_it, STAT);
      stat->append(Rules::block(tok_it, ok));
      if (tok_it->value()->type != END) {
        AST_ERROR(tok_it->value(), "Missing `end` for `do` statement");
        ok = false;
      } else {
        tok_it = tok_it->next();
      }
    }

    // While loop
    else if (tok_it->value()->type == WHILE) {
      stat = Rules::node(tok_it, STAT);
      stat->append(Rules::exp(tok_it, ok));
      if (ok) {
        if (tok_it->value()->type == DO) {
          tok_it = tok_it->next();
          stat->append(Rules::block(tok_it, ok));
          if (ok && tok_it->value()->type != END) {
            AST_ERROR(tok_it->value(), "Missing `end` after `while` statement");
            ok = false;
          }
          if (ok) {
            tok_it = tok_it->next();
          }
        } else {
          AST_ERROR(tok_it->value(),
                    "Missing `do` block for `while` statement");
          ok = false;
        }
      }
    }

    // Repeat until
    else if (tok_it->value()->type == REPEAT) {
      stat = Rules::node(tok_it, STAT);
      stat->append(Rules::block(tok_it, ok));
      if (ok) {
        if (tok_it->value()->type == UNTIL) {
          tok_it = tok_it->next();
          stat->append(Rules::exp(tok_it, ok));
        } else {
          AST_ERROR(tok_it->value(),
                    "Missing `until` token for `repeat` statement");
          ok = false;
        }
      }
    }

    // If then
    else if (tok_it->value()->type == IF) {
      stat = Rules::node(tok_it, STAT);
      stat->append(Rules::exp(tok_it, ok));
      if (ok) {
        if (tok_it->value()->type == THEN) {
          tok_it = tok_it->next();
          stat->append(Rules::block(tok_it, ok));
        } else {
          AST_ERROR(tok_it->value(), "Expecting `then` after `if`");
          ok = false;
        }
      }
      while (ok && tok_it->value()->type == ELSEIF) {
        Nodep elseif = Rules::node(tok_it, STAT);
        stat->append(elseif);
        elseif->append(Rules::exp(tok_it, ok));
        if (ok) {
          if (tok_it->value()->type == THEN) {
            tok_it = tok_it->next();
            elseif->append(Rules::block(tok_it, ok));
          } else {
            AST_ERROR(tok_it->value(), "Expecting `then` after `elseif`");
            ok = false;
          }
        }
      }
      if (ok && tok_it->value()->type == ELSE) {
        Nodep else_ = Rules::node(tok_it, STAT);
        stat->append(else_);
        else_->append(Rules::block(tok_it, ok));
      }
      if (ok && tok_it->value()->type != END) {
        AST_ERROR(tok_it->value(), "Missing `end` after `if` statement");
        ok = false;
      }
      if (ok) {
        tok_it = tok_it->next();
      }
    }

    // For statement
    else if (tok_it->value()->type == FOR) {
      stat = Rules::node(tok_it, STAT);
      if (tok_it->value()->type != ID) {
        AST_ERROR(tok_it->value(), "Expecting name or namelist");
        ok = false;
      } else {
        if (tok_it->next() && (tok_it->next()->value()->type == COMMA ||
                               tok_it->next()->value()->type == IN)) {
          stat->append(Rules::namelist(tok_it, ok));
          if (ok) {
            if (tok_it->value()->type == IN) {
              tok_it = tok_it->next();
              stat->append(Rules::explist(tok_it, ok));
            } else {
              AST_ERROR(tok_it->value(), "Missing `in` after namelist");
              ok = false;
            }
          }
        } else {
          stat->append(Rules::node(tok_it, NAME));
          if (tok_it->value()->type == ASSIGN) {
            tok_it = tok_it->next();
            stat->append(Rules::exp(tok_it, ok));
          } else {
            AST_ERROR(tok_it->value(), "Missing variable assignment");
            ok = false;
          }
          if (ok) {
            if (tok_it->value()->type == COMMA) {
              tok_it = tok_it->next();
              stat->append(Rules::exp(tok_it, ok));
            } else {
              AST_ERROR(tok_it->value(), "Missing comma");
              ok = false;
            }
          }
          if (ok) {
            if (tok_it->value()->type == COMMA) {
              tok_it = tok_it->next();
              stat->append(Rules::exp(tok_it, ok));
            }
          }
        }
      }
      if (ok) {
        if (tok_it->value()->type == DO) {
          tok_it = tok_it->next();
          stat->append(Rules::block(tok_it, ok));
          if (ok && tok_it->value()->type != END) {
            AST_ERROR(tok_it->value(), "Missing `end after `for` statement");
            ok = false;
          }
        } else {
          AST_ERROR(tok_it->value(), "Missing `do` after `for` conditions");
          ok = false;
        }
      }
      if (ok) {
        tok_it = tok_it->next();
      }
    }

    // Function
    else if (tok_it->value()->type == FUNCTION) {
      stat = Rules::node(tok_it, STAT);
      stat->append(Rules::funcname(tok_it, ok));
      stat->append(Rules::funcbody(tok_it, ok));
    }

    // Local definition
    else if (tok_it->value()->type == LOCAL) {
      stat = Rules::node(tok_it, LOCALSTAT);

      // Function
      if (tok_it->value()->type == FUNCTION) {
        stat->append(Rules::node(tok_it, LOCALFUNCTION));
        if (tok_it->value()->type == ID) {
          stat->append(Rules::node(tok_it, NAME));
        } else {
          AST_ERROR(tok_it->value(),
                    "Missing local function name for `local` statement");
          ok = false;
        }
        stat->append(Rules::funcbody(tok_it, ok));
      }

      // Build attnamelist
      else if (tok_it->value()->type == ID) {
        stat->append(Rules::attnamelist(tok_it, ok));
        if (tok_it->value()->type == ASSIGN) {
          tok_it = tok_it->next();
          stat->append(Rules::explist(tok_it, ok));
        }
      }

      else {
        AST_ERROR(tok_it->value(),
                  "Missing local function or attribute name list for "
                  "`local` statement");
        ok = false;
      }
    }

    else if (tok_it->value()->type != OPAR && tok_it->value()->type != ID) {
      AST_ERROR(tok_it->value(), "Invalid statement");
      ok = false;
    }

    // Varlist/functioncall
    else {
      bool ret = true;
      tokens_t::iterator *init_it = tok_it;
      stat = new Node(STAT);

      // Varlist
      stat->append(Rules::varlist(tok_it, ret));
      if (ret) {
        if (tok_it->value()->type == ASSIGN) {
          tok_it = tok_it->next();
        } else {
          ret = false;
        }
        if (ret) {
          stat->append(Rules::explist(tok_it, ret));
        }
      }

      // Functioncall
      if (!ret) {
        ret = true;
        stat->del();
        tok_it = init_it;
        stat->append(Rules::var_or_funcall(tok_it, ret, FUNCTIONCALL));
      }

      if (!ret) {
        AST_ERROR(tok_it->value(), "Invalid statement");
        ok = false;
      }
    }
  }

  if (!ok) {
    stat.del();
  }

  return stat;
}

Nodep Rules::varlist(tokens_t::iterator *&tok_it, bool &ok) {
  tokens_t::iterator *init_it = tok_it;
  Nodep varlist;
  if (tok_it->value()->type != ENDCODE) {
    varlist = new Node(VARLIST);
    varlist->append(Rules::var_or_funcall(tok_it, ok, VAR));
    while (tok_it->value()->type == COMMA && ok) {
      tok_it = tok_it->next();
      varlist->append(Rules::var_or_funcall(tok_it, ok, VAR));
    }
  }

  if (!ok) {
    varlist.del();
    tok_it = init_it;
  }

  return varlist;
}

Nodep Rules::var_or_funcall(tokens_t::iterator *&tok_it, bool &ok,
                            LuaNode expected) {
  tokens_t::iterator *init_it = tok_it;
  Nodep var;
  if (tok_it->value()->type == ID) {
    var = new Node(VAR);
    var->append(Rules::node(tok_it, NAME));
  } else if (tok_it->value()->type == OPAR) {
    tok_it = tok_it->next();
    var = new Node(VAR);
    var->append(Rules::exp(tok_it, ok));
    if (tok_it->value()->type != CPAR && ok) {
      AST_ERROR(tok_it->value(), "Missing closing parenthesys");
      ok = false;
    } else {
      tok_it = tok_it->next();
    }
    if (ok &&
        (tok_it->value()->type != OBRACK || tok_it->value()->type != DOT)) {
      AST_ERROR(tok_it->value(), "Expecting `[` or `.` after `)`");
      ok = false;
    }
  } else {
    ok = false;
  }

  while (tok_it->value()->type != ENDCODE && ok) {
    if (tok_it->value()->type == DOT && tok_it->next() &&
        tok_it->next()->value()->type == ID) {
      tok_it = tok_it->next();
      var->sub_nodes[-1]->type = PREFIXEXP;
      var->append(Rules::node(tok_it, NAME));
      var->type = VAR;
    } else if (tok_it->value()->type == OBRACK) {
      tok_it = tok_it->next();
      var->sub_nodes[-1]->type = PREFIXEXP;
      var->append(Rules::exp(tok_it, ok));
      var->type = VAR;
      if (tok_it->value()->type != CBRACK && ok) {
        AST_ERROR(tok_it->value(), "Missing closing bracket.")
        ok = false;
      } else {
        tok_it = tok_it->next();
      }
    } else if (tok_it->value()->type == COL && tok_it->next() &&
               tok_it->next()->value()->type == ID) {
      tok_it = tok_it->next();
      var->sub_nodes[-1]->type = PREFIXEXP;
      var->append(Rules::node(tok_it, NAME));
      var->append(Rules::args(tok_it, ok));
      var->type = FUNCTIONCALL;
    } else if (tok_it->value()->type == OPAR ||
               tok_it->value()->type == STRING ||
               tok_it->value()->type == OBRACE) {
      var->sub_nodes[-1]->type = PREFIXEXP;
      var->append(Rules::args(tok_it, ok));
      var->type = FUNCTIONCALL;
    } else {
      break;
    }
  }

  if (var && var->type != expected) {
    ok = false;
  }

  if (!ok) {
    var.del();
    tok_it = init_it;
  }

  return var;
}

Nodep Rules::explist(tokens_t::iterator *&tok_it, bool &ok) {
  tokens_t::iterator *init_it = tok_it;
  Nodep explist = new Node(EXPLIST);
  explist->append(Rules::exp(tok_it, ok));
  while (tok_it->value()->type == COMMA && ok) {
    tok_it = tok_it->next();
    explist->append(Rules::exp(tok_it, ok));
  }

  if (!ok) {
    explist.del();
    tok_it = init_it;
  }
  return explist;
}

Nodep Rules::exp(tokens_t::iterator *&tok_it, bool &ok) {
  tokens_t::iterator *init_it = tok_it;
  Nodep exp = new Node(EXP);
  bool expect_exp = false;
  while (tok_it->value()->type != ENDCODE && ok) {
    expect_exp = false;
    if (tok_it->value()->type == NIL || tok_it->value()->type == FALSE ||
        tok_it->value()->type == TRUE || tok_it->value()->type == VARARGS) {
      exp->append(Rules::node(tok_it, VALUE));
    } else if (tok_it->value()->type == FUNCTION) {
      exp->append(Rules::node(tok_it, FUNCTIONDEF));
    } else if (tok_it->value()->type == STRING) {
      exp->append(Rules::node(tok_it, LITERALSTRING));
    } else if (tok_it->value()->type == NUM) {
      exp->append(Rules::node(tok_it, NUMERAL));
    } else if (tok_it->value()->type == ID || tok_it->value()->type == OPAR) {
      exp->append(Rules::prefixexp(tok_it, ok));
    } else if (tok_it->value()->type == OBRACE) {
      exp->append(Rules::table(tok_it, ok));
    }

    if (tok_it && tok_it->value()->unop && tok_it->value()->binop) {
      switch (tok_it->prev()->value()->type) {
      case ID:
      case NUM:
      case STRING:
        exp->append(Rules::node(tok_it, BINOP));
        break;
      default:
        exp->append(Rules::node(tok_it, UNOP));
      }
      expect_exp = true;
    } else if (tok_it && tok_it->value()->unop) {
      exp->append(Rules::node(tok_it, UNOP));
      expect_exp = true;
    } else if (tok_it && tok_it->value()->binop) {
      if (tok_it->prev()->value()->binop) {
        AST_ERROR(tok_it->value(), "Invalid expression");
        ok = false;
      } else {
        exp->append(Rules::node(tok_it, BINOP));
        expect_exp = true;
      }
    } else {
      break;
    }
  }

  if (expect_exp) {
    AST_ERROR(tok_it->value(), "Expecting expression after operator");
    ok = false;
  }

  if (!ok) {
    exp.del();
    tok_it = init_it;
  }

  return exp;
}

Nodep Rules::prefixexp(tokens_t::iterator *&tok_it, bool &ok) {
  tokens_t::iterator *init_it = tok_it;
  bool ret = true;
  Nodep prefixexp;
  if (tok_it->value()->type == OPAR) {
    tok_it = tok_it->next();
    prefixexp = Rules::exp(tok_it, ret);
    if (ret && tok_it->value()->type != CPAR) {
      AST_ERROR(tok_it->value(), "Missing closing parenhtesis");
      ok = false;
    } else if (ret) {
      tok_it = tok_it->next();
    }
  } else {
    ret = false;
  }
  if (!ret) {
    ret = true;
    prefixexp = Rules::var_or_funcall(tok_it, ret, FUNCTIONCALL);
  }
  if (!ret) {
    ret = true;
    prefixexp = Rules::var_or_funcall(tok_it, ret, VAR);
  }

  ok &= ret;

  if (!ok) {
    prefixexp.del();
    tok_it = init_it;
  }

  return prefixexp;
}

Nodep Rules::args(tokens_t::iterator *&tok_it, bool &ok) {
  tokens_t::iterator *init_it = tok_it;
  Nodep args = new Node(ARGS);

  if (tok_it->value()->type == OPAR) {
    tok_it = tok_it->next();
    args->append(Rules::explist(tok_it, ok));
    if (ok && tok_it->value()->type != CPAR) {
      AST_ERROR(tok_it->value(), "Missing closing parenthesys after arguments");
      ok = false;
    } else {
      tok_it = tok_it->next();
    }
  } else if (tok_it->value()->type == STRING) {
    args->append(Rules::node(tok_it, LITERALSTRING));
  } else if (tok_it->value()->type == OBRACE) {
    args->append(Rules::table(tok_it, ok));
  } else {
    AST_ERROR(tok_it->value(), "Invalid arguments for function call");
    ok = false;
    args.del();
    tok_it = init_it;
  }

  return args;
}

Nodep Rules::table(tokens_t::iterator *&tok_it, bool &ok) {
  tokens_t::iterator *init_it = tok_it;
  if (tok_it->value()->type != OBRACE) {
    AST_ERROR(tok_it->value(), "Missing opening brace for table");
    ok = false;
    return Nodep();
  }
  tok_it = tok_it->next();
  Nodep table = new Node(TABLECONSTRUCTOR);
  Nodep fieldlist = new Node(FIELDLIST);
  table->append(fieldlist);

  while (true) {
    Nodep field = new Node(FIELD);
    bool ret = true;
    if (tok_it->value()->type == OBRACK) {
      tok_it = tok_it->next();
      field->append(Rules::exp(tok_it, ret));
      if (ret && tok_it->value()->type == CBRACK) {
        tok_it = tok_it->next();
        if (tok_it->value()->type == ASSIGN) {
          tok_it = tok_it->next();
          // field->append(Rules::node(tok_it, BINOP));
          field->append(Rules::exp(tok_it, ret));
        }
      }
    } else if (tok_it->value()->type == ID && tok_it->next() &&
               tok_it->next()->value()->type == ASSIGN) {
      field->append(Rules::node(tok_it, NAME));
      tok_it = tok_it->next();
      // field->append(Rules::node(tok_it, BINOP));
      field->append(Rules::exp(tok_it, ret));
    } else {
      field->append(Rules::exp(tok_it, ret));
    }

    if (!ret) {
      field.del();
      break;
    }

    fieldlist->append(field);
    if (tok_it->value()->type == COMMA || tok_it->value()->type == SEMCOL) {
      fieldlist->append(Rules::node(tok_it, FIELDSEP));
    } else {
      break;
    }
  }

  if (tok_it->value()->type != CBRACE) {
    AST_ERROR(tok_it->value(), "Missing closing brace for table");
    ok = false;
  } else {
    tok_it = tok_it->next();
  }

  if (!ok) {
    table.del();
    fieldlist.del();
    tok_it = init_it;
  }

  return table;
}

Nodep Rules::funcname(tokens_t::iterator *&tok_it, bool &ok) {
  tokens_t::iterator *init_it = tok_it;
  Nodep funcname;

  if (tok_it->value()->type == ID && ok) {
    funcname = Rules::node(tok_it, FUNCNAME);
  } else {
    AST_ERROR(tok_it->value(), "Missing name for function");
    ok = false;
  }

  while (tok_it->value()->type == DOT && ok) {
    tok_it = tok_it->next();
    if (tok_it->value()->type == ID && ok) {
      funcname->append(Rules::node(tok_it, NAME));
    } else {
      break;
    }
  }

  if (tok_it->value()->type == COL && ok) {
    tok_it = tok_it->next();
    funcname->append(Rules::node(tok_it, NAME));
  }

  if (!ok) {
    funcname.del();
    tok_it = init_it;
  }

  return funcname;
}

Nodep Rules::funcbody(tokens_t::iterator *&tok_it, bool &ok) {
  tokens_t::iterator *init_it = tok_it;
  Nodep funcbody;

  if (tok_it->value()->type == OPAR && ok) {
    funcbody = new Node(FUNCBODY);
    tok_it = tok_it->next();
    if (tok_it->value()->type != CPAR) {
      funcbody->append(Rules::parlist(tok_it, ok));
    }
    if (tok_it->value()->type != CPAR && ok) {
      AST_ERROR(tok_it->value(), "Missing closing parenthesis");
      ok = false;
    } else {
      tok_it = tok_it->next();
    }
  } else {
    AST_ERROR(tok_it->value(), "Missing function arguments");
    ok = false;
  }

  if (ok) {
    funcbody->append(Rules::block(tok_it, ok));
  }
  if (ok && tok_it->value()->type != END) {
    AST_ERROR(tok_it->value(), "Missing `end` for function body");
    ok = false;
  }

  if (ok) {
    tok_it = tok_it->next();
  }

  if (!ok) {
    funcbody.del();
    tok_it = init_it;
  }

  return funcbody;
}

Nodep Rules::parlist(tokens_t::iterator *&tok_it, bool &ok) {
  tokens_t::iterator *init_it = tok_it;
  Nodep parlist;
  if (tok_it->value()->type == VARARGS) {
    parlist = Rules::node(tok_it, PARLIST);
  } else {
    parlist = new Node(PARLIST);
    parlist->append(Rules::namelist(tok_it, ok));
    if (tok_it->value()->type == VARARGS) {
      parlist->append(Rules::node(tok_it, NAME));
    }
  }

  if (!ok) {
    parlist.del();
    tok_it = init_it;
  }

  return parlist;
}

Nodep Rules::retstat(tokens_t::iterator *&tok_it, bool &ok) {
  tokens_t::iterator *init_it = tok_it;
  Nodep retstat;

  if (tok_it->value()->type == RETURN) {
    retstat = Rules::node(tok_it, RETSTAT);
    retstat->append(Rules::explist(tok_it, ok));
    if (ok && tok_it->value()->type == SEMCOL) {
      retstat->append(Rules::node(tok_it, UNDEFINED));
    }
  }

  if (!ok) {
    retstat.del();
    tok_it = init_it;
  }

  return retstat;
}

Nodep Rules::namelist(tokens_t::iterator *&tok_it, bool &ok) {
  tokens_t::iterator *init_it = tok_it;
  Nodep namelist;

  if (tok_it->value()->type == ID && ok) {
    namelist = new Node(NAMELIST);
    namelist->append(Rules::node(tok_it, NAME));
  } else {
    AST_ERROR(tok_it->value(), "Empty namelist");
    ok = false;
  }

  if (tok_it->value()->type == COMMA && ok) {
    tok_it = tok_it->next();
    while (tok_it->value()->type == ID) {
      namelist->append(Rules::node(tok_it, NAME));
      if (tok_it->value()->type == COMMA) {
        tok_it = tok_it->next();
      } else {
        break;
      }
    }
  }

  if (!ok) {
    namelist.del();
    tok_it = init_it;
  }

  return namelist;
}

Nodep Rules::attnamelist(tokens_t::iterator *&tok_it, bool &ok) {
  tokens_t::iterator *init_it = tok_it;
  Nodep attnamelist;

  if (tok_it->value()->type == ID && ok) {
    attnamelist = new Node(ATTNAMELIST);
    attnamelist->append(Rules::node(tok_it, NAME));
    if (tok_it->value()->type == LT) {
      tok_it = tok_it->next();
      if (tok_it->value()->type == ID) {
        attnamelist->append(Rules::node(tok_it, ATTRIB));
        if (tok_it->value()->type == GT) {
          tok_it = tok_it->next();
        } else {
          AST_ERROR(tok_it->value(), "Missing closing attribute token");
          ok = false;
        }
      } else {
        AST_ERROR(tok_it->value(), "Missing attribute name");
        ok = false;
      }
    }
  } else {
    AST_ERROR(tok_it->value(), "Empty namelist");
    ok = false;
  }

  while (tok_it->value()->type == COMMA && ok) {
    tok_it = tok_it->next();
    if (tok_it->value()->type == ID && ok) {
      attnamelist->append(Rules::node(tok_it, NAME));
      if (tok_it->value()->type == LT) {
        tok_it = tok_it->next();
        if (tok_it->value()->type == ID) {
          attnamelist->append(Rules::node(tok_it, ATTRIB));
          if (tok_it->value()->type == GT) {
            tok_it = tok_it->next();
          } else {
            AST_ERROR(tok_it->value(), "Missing closing attribute token");
            ok = false;
          }
        } else {
          AST_ERROR(tok_it->value(), "Missing attribute name");
          ok = false;
        }
      }
    } else {
      break;
    }
  }

  if (!ok) {
    attnamelist.del();
    tok_it = init_it;
  }

  return attnamelist;
}

Nodep Rules::node(tokens_t::iterator *&tok_it, LuaNode type) {
  Nodep n(new Node(type, tok_it->value()));
  tok_it = tok_it->next();
  return n;
}
} // namespace LUA
} // namespace MARTe
