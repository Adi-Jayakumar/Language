#include "verifier.h"

z3::expr Verifier::MAKE_RETURN(const TypeData &type, const z3::expr &val, const Token &loc)
{
    if (type == INT_TYPE)
        return context.int_const("result") == val;
    else if (type == BOOL_TYPE)
        return context.bool_const("result") == val;
    else
        PostConditionError(loc, "Unsupported type to make return");
}

void Verifier::Generate(FuncDecl *fd)
{
    fd->CreateZ3Expr(*this);
    z3::expr auto_post = GetZ3Post();
    z3::expr user_post = fd->post_cond->CreateZ3Expr(*this);
    std::cout << auto_post << std::endl;
    std::cout << user_post << std::endl;

    z3::solver s(context);

    for (auto &pre : fd->pre_conds)
        s.add(pre->CreateZ3Expr(*this));

    z3::expr conjecture = z3::implies(auto_post, user_post);
    s.add(!conjecture);

    switch (s.check())
    {
    case z3::unsat:
    {
        std::cout << "VERIFIED" << std::endl;
        break;
    }
    case z3::sat:
    {
        z3::model m = s.get_model();
        std::cout << m << std::endl;
        break;
    }
    case z3::unknown:
    {
        std::cout << "UNKNOWN" << std::endl;
        break;
    }
    }
}

static z3::expr And(std::vector<z3::expr> &clause)
{
    z3::expr result = clause[0];
    for (size_t i = 1; i < clause.size(); i++)
        result = result && clause[i];
    return result;
}

z3::expr Verifier::GetZ3Post()
{
    z3::expr result = And(post[0]);
    for (size_t i = 1; i < post.size(); i++)
        result = result || And(post[i]);
    return result;
}

void Verifier::AddReturnValue(const z3::expr &ret)
{
    post.push_back(conditions);
    post.back().push_back(ret);
}

void Verifier::AddCondition(const z3::expr &c)
{
    conditions.push_back(c);
}

void Verifier::RemoveLastCondition()
{
    conditions.pop_back();
}

void Verifier::PostConditionError(Token loc, std::string err)
{
    std::ostringstream out;
    if (cur_func != nullptr)
    {
        out << "[POST CONDITION ERROR] In function '";
        symbols.PrintType(out, cur_func->ret);
        out << " " << cur_func->name << "(";
        if (cur_func->params.size() > 0)
        {
            for (size_t i = 0; i < cur_func->params.size(); i++)
            {
                symbols.PrintType(out, cur_func->params[i].first);
                out << " " << cur_func->params[i].second;
                if (i != cur_func->params.size() - 1)
                    out << ", ";
            }
        }

        out << ")'\n"
            << "On line ";
    }
    else
        out << "[POST CONDITION ERROR] on line ";

    out << std::to_string(loc.line) << " near '" << loc.literal << "'\n"
        << err << "\n";

    throw Error(out.str());
}

void Verifier::ReplaceFunctionCall(SP<Expr> &exp)
{
    switch (exp->kind)
    {
    case ExprKind::LITERAL:
    {
        break;
    }
    case ExprKind::UNARY:
    {
        Unary *u = dynamic_cast<Unary *>(exp.get());
        ReplaceFunctionCall(u->right);
        break;
    }
    case ExprKind::BINARY:
    {
        Binary *b = dynamic_cast<Binary *>(exp.get());
        ReplaceFunctionCall(b->left);
        ReplaceFunctionCall(b->right);
        break;
    }
    case ExprKind::VAR_REFERENCE:
    {
        break;
    }
    case ExprKind::ASSIGN:
    {
        Assign *a = dynamic_cast<Assign *>(exp.get());
        ReplaceFunctionCall(a->val);
        break;
    }
    case ExprKind::FUNCTION_CALL:
    {
        FunctionCall *fc = dynamic_cast<FunctionCall *>(exp.get());
        std::vector<TypeData> argtypes;
        for (auto &arg : fc->args)
        {
            ReplaceFunctionCall(arg);
            argtypes.push_back(arg->GetType());
        }

        std::optional<FuncID> fid = symbols.GetFunc(fc->name, argtypes);
        FuncDecl *fd = dynamic_cast<FuncDecl *>(program[fid->parse_index].get());

        if (fd->post_cond == nullptr)
            PostConditionError(fd->Loc(), "Cannot have call to function which does not have a post condition in the strongest postcondition");

        exp = fd->post_cond;
        assert(fd->params.size() == fc->args.size());
        for (size_t i = 0; i < fd->params.size(); i++)
        {
            SP<Expr> arg = std::make_shared<VarReference>(Token(TokenID::IDEN, fd->params[i].second, 0));
            exp = NodeSubstituter::Substitute(exp, arg, fc->args[i]);
        }
        break;
    }
    case ExprKind::ARRAY_INDEX:
    {
        ArrayIndex *ai = dynamic_cast<ArrayIndex *>(exp.get());
        ReplaceFunctionCall(ai->index);
        break;
    }
    case ExprKind::BRACED_INITIALISER:
    {
        BracedInitialiser *bi = dynamic_cast<BracedInitialiser *>(exp.get());
        for (auto &e : bi->init)
            ReplaceFunctionCall(e);
        break;
    }
    case ExprKind::DYNAMIC_ALLOC_ARRAY:
    {
        DynamicAllocArray *da = dynamic_cast<DynamicAllocArray *>(exp.get());
        ReplaceFunctionCall(da->size);
        break;
    }
    case ExprKind::FIELD_ACCESS:
    {
        break;
    }
    case ExprKind::TYPE_CAST:
    {
        TypeCast *tc = dynamic_cast<TypeCast *>(exp.get());
        ReplaceFunctionCall(tc->arg);
        break;
    }
    case ExprKind::SEQUENCE:
    {
        Sequence *s = dynamic_cast<Sequence *>(exp.get());
        ReplaceFunctionCall(s->term);
        break;
    }
    }
}

//------------------EXPRESSIONS---------------------//

z3::expr Verifier::GenerateFromLiteral(Literal *l)
{
    if (l->t == INT_TYPE)
        return context.int_val(std::stoi(l->loc.literal));
    else if (l->t == BOOL_TYPE)
        return context.bool_val(l->loc.literal == "true" ? true : false);
    else
        PostConditionError(l->Loc(), "Invalid literal type to generate post condition from");
}

z3::expr Verifier::GenerateFromUnary(Unary *u)
{
    z3::expr right = u->right->CreateZ3Expr(*this);

    if (u->op.type == TokenID::MINUS)
        return -right;
    else
        return !right;
}

z3::expr Verifier::GenerateFromBinary(Binary *b)
{
    z3::expr left = b->left->CreateZ3Expr(*this);
    z3::expr right = b->right->CreateZ3Expr(*this);

    if (b->op.type == TokenID::OR_OR)
        return left || right;
    else if (b->op.type == TokenID::AND_AND)
        return left && right;
    else if (b->op.type == TokenID::EQ_EQ)
        return left == right;
    else if (b->op.type == TokenID::GT)
        return left > right;
    else if (b->op.type == TokenID::LT)
        return left < right;
    else if (b->op.type == TokenID::GEQ)
        return left >= right;
    else if (b->op.type == TokenID::LEQ)
        return left <= right;
    else if (b->op.type == TokenID::PLUS)
        return left + right;
    else if (b->op.type == TokenID::MINUS)
        return left - right;
    else if (b->op.type == TokenID::STAR)
        return left * right;
    else
        return left / right;
}

z3::expr Verifier::GenerateFromAssign(Assign *a)
{
    PostConditionError(a->Loc(), "Do not support variable assignment yet");
}

z3::expr Verifier::GenerateFromVarReference(VarReference *vr)
{
    if (vr->t == INT_TYPE)
        return context.int_const(vr->name.c_str());
    else if (vr->t == BOOL_TYPE)
        return context.bool_const(vr->name.c_str());
    else
        PostConditionError(vr->Loc(), "Invalid literal type to generate post condition from");
}

z3::expr Verifier::GenerateFromFunctionCall(FunctionCall *fc)
{
    PostConditionError(fc->Loc(), "Do not support function calls yet");
}

z3::expr Verifier::GenerateFromArrayIndex(ArrayIndex *ai)
{
    PostConditionError(ai->Loc(), "Do not support array indices yet");
}

z3::expr Verifier::GenerateFromBracedInitialiser(BracedInitialiser *bi)
{
    PostConditionError(bi->Loc(), "Do not support braced initialisers yet");
}

z3::expr Verifier::GenerateFromDynamicAllocArray(DynamicAllocArray *da)
{
    PostConditionError(da->Loc(), "Do not support dynamically allocated arrays yet");
}

z3::expr Verifier::GenerateFromFieldAccess(FieldAccess *fa)
{
    PostConditionError(fa->Loc(), "Do not support field accesses yet");
}

z3::expr Verifier::GenerateFromTypeCast(TypeCast *tc)
{
    PostConditionError(tc->Loc(), "Do not support type casts yet");
}

z3::expr Verifier::GenerateFromSequence(Sequence *s)
{
    PostConditionError(s->Loc(), "Do not support sequences yet");
}

//------------------STATEMENTS---------------------//

void Verifier::GenerateFromExprStmt(ExprStmt *)
{
}

void Verifier::GenerateFromDeclaredVar(DeclaredVar *)
{
}

void Verifier::GenerateFromBlock(Block *b)
{
    for (auto &stmt : b->stmts)
        stmt->CreateZ3Expr(*this);
}

void Verifier::GenerateFromIfStmt(IfStmt *i)
{
    AddCondition(i->cond->CreateZ3Expr(*this));
    i->then_branch->CreateZ3Expr(*this);
    RemoveLastCondition();

    if (i->else_branch != nullptr)
    {
        AddCondition(!i->cond->CreateZ3Expr(*this));
        i->else_branch->CreateZ3Expr(*this);
        RemoveLastCondition();
    }
}

void Verifier::GenerateFromWhileStmt(WhileStmt *ws)
{
    PostConditionError(ws->Loc(), "Cannot generate a post condition from a while loop");
}

void Verifier::GenerateFromFuncDecl(FuncDecl *fd)
{
    cur_func = fd;

    if (fd->ret == VOID_TYPE)
        PostConditionError(fd->Loc(), "Cannot generate post condition errror for a void function");

    for (auto &stmt : fd->body)
        stmt->CreateZ3Expr(*this);

    cur_func = nullptr;
}

void Verifier::GenerateFromTemplateDecl(TemplateDecl *)
{
}

void Verifier::GenerateFromReturn(Return *r)
{
    if (r->ret_val == nullptr)
        PostConditionError(r->Loc(), "Cannot generate post condition for a void-return");

    AddReturnValue(MAKE_RETURN(r->ret_val->GetType(), r->ret_val->CreateZ3Expr(*this), r->Loc()));
}

void Verifier::GenerateFromStructDecl(StructDecl *)
{
}

void Verifier::GenerateFromImportStmt(ImportStmt *)
{
}

void Verifier::GenerateFromBreak(Break *)
{
}

void Verifier::GenerateFromThrow(Throw *t)
{
    PostConditionError(t->Loc(), "Cannot generate post condition for a function which potentially throws an object");
}

void Verifier::GenerateFromTryCatch(TryCatch *tc)
{
    PostConditionError(tc->Loc(), "Cannot generate post condition for a function which uses try-catch");
}

//------------------EXPRESSIONS---------------------//

z3::expr Literal::CreateZ3Expr(Verifier &v)
{
    return v.GenerateFromLiteral(this);
}

z3::expr Unary::CreateZ3Expr(Verifier &v)
{
    return v.GenerateFromUnary(this);
}

z3::expr Binary::CreateZ3Expr(Verifier &v)
{
    return v.GenerateFromBinary(this);
}

z3::expr Assign::CreateZ3Expr(Verifier &v)
{
    return v.GenerateFromAssign(this);
}

z3::expr VarReference::CreateZ3Expr(Verifier &v)
{
    return v.GenerateFromVarReference(this);
}

z3::expr FunctionCall::CreateZ3Expr(Verifier &v)
{
    return v.GenerateFromFunctionCall(this);
}

z3::expr ArrayIndex::CreateZ3Expr(Verifier &v)
{
    return v.GenerateFromArrayIndex(this);
}

z3::expr BracedInitialiser::CreateZ3Expr(Verifier &v)
{
    return v.GenerateFromBracedInitialiser(this);
}

z3::expr DynamicAllocArray::CreateZ3Expr(Verifier &v)
{
    return v.GenerateFromDynamicAllocArray(this);
}

z3::expr FieldAccess::CreateZ3Expr(Verifier &v)
{
    return v.GenerateFromFieldAccess(this);
}

z3::expr TypeCast::CreateZ3Expr(Verifier &v)
{
    return v.GenerateFromTypeCast(this);
}

z3::expr Sequence::CreateZ3Expr(Verifier &v)
{
    return v.GenerateFromSequence(this);
}

//------------------STATEMENTS---------------------//

void ExprStmt::CreateZ3Expr(Verifier &v)
{
    v.GenerateFromExprStmt(this);
}

void DeclaredVar::CreateZ3Expr(Verifier &v)
{
    v.GenerateFromDeclaredVar(this);
}

void Block::CreateZ3Expr(Verifier &v)
{
    v.GenerateFromBlock(this);
}

void IfStmt::CreateZ3Expr(Verifier &v)
{
    v.GenerateFromIfStmt(this);
}

void WhileStmt::CreateZ3Expr(Verifier &v)
{
    v.GenerateFromWhileStmt(this);
}

void FuncDecl::CreateZ3Expr(Verifier &v)
{
    v.GenerateFromFuncDecl(this);
}

void TemplateDecl::CreateZ3Expr(Verifier &v)
{
    v.GenerateFromTemplateDecl(this);
}

void Return::CreateZ3Expr(Verifier &v)
{
    v.GenerateFromReturn(this);
}

void StructDecl::CreateZ3Expr(Verifier &v)
{
    v.GenerateFromStructDecl(this);
}

void ImportStmt::CreateZ3Expr(Verifier &v)
{
    v.GenerateFromImportStmt(this);
}

void Break::CreateZ3Expr(Verifier &v)
{
    v.GenerateFromBreak(this);
}

void Throw::CreateZ3Expr(Verifier &v)
{
    v.GenerateFromThrow(this);
}

void TryCatch::CreateZ3Expr(Verifier &v)
{
    v.GenerateFromTryCatch(this);
}