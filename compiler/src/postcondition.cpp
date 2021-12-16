#include "postcondition.h"

z3::expr PostConditionGenerator::MAKE_RETURN(const TypeData &type, const z3::expr &val, const Token &loc)
{
    if (type == INT_TYPE)
        return context.int_const("result") == val;
    else if (type == BOOL_TYPE)
        return context.bool_const("result") == val;
    else
        PostConditionError(loc, "Unsupported type to make return");
}

void PostConditionGenerator::Generate(FuncDecl *fd)
{
    fd->GeneratePost(*this);
    z3::expr auto_post = GetZ3Post();
    z3::expr user_post = fd->post_cond->GeneratePost(*this);
    std::cout << auto_post << std::endl;
    std::cout << user_post << std::endl;

    z3::solver s(context);

    for (auto &pre : fd->pre_conds)
        s.add(pre->GeneratePost(*this));

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

z3::expr PostConditionGenerator::GetZ3Post()
{
    z3::expr result = And(post[0]);
    for (size_t i = 1; i < post.size(); i++)
        result = result || And(post[i]);
    return result;
}

void PostConditionGenerator::AddReturnValue(const z3::expr &ret)
{
    post.push_back(conditions);
    post.back().push_back(ret);
}

void PostConditionGenerator::AddCondition(const z3::expr &c)
{
    conditions.push_back(c);
}

void PostConditionGenerator::RemoveLastCondition()
{
    conditions.pop_back();
}

void PostConditionGenerator::PostConditionError(Token loc, std::string err)
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

void PostConditionGenerator::ReplaceFunctionCall(SP<Expr> &exp)
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

z3::expr PostConditionGenerator::GenerateFromLiteral(Literal *l)
{
    if (l->t == INT_TYPE)
        return context.int_val(std::stoi(l->loc.literal));
    else if (l->t == BOOL_TYPE)
        return context.bool_val(l->loc.literal == "true" ? true : false);
    else
        PostConditionError(l->Loc(), "Invalid literal type to generate post condition from");
}

z3::expr PostConditionGenerator::GenerateFromUnary(Unary *u)
{
    z3::expr right = u->right->GeneratePost(*this);

    if (u->op.type == TokenID::MINUS)
        return -right;
    else
        return !right;
}

z3::expr PostConditionGenerator::GenerateFromBinary(Binary *b)
{
    z3::expr left = b->left->GeneratePost(*this);
    z3::expr right = b->right->GeneratePost(*this);

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

z3::expr PostConditionGenerator::GenerateFromAssign(Assign *a)
{
    PostConditionError(a->Loc(), "Do not support variable assignment yet");
}

z3::expr PostConditionGenerator::GenerateFromVarReference(VarReference *vr)
{
    if (vr->t == INT_TYPE)
        return context.int_const(vr->name.c_str());
    else if (vr->t == BOOL_TYPE)
        return context.bool_const(vr->name.c_str());
    else
        PostConditionError(vr->Loc(), "Invalid literal type to generate post condition from");
}

z3::expr PostConditionGenerator::GenerateFromFunctionCall(FunctionCall *fc)
{
    PostConditionError(fc->Loc(), "Do not support function calls yet");
}

z3::expr PostConditionGenerator::GenerateFromArrayIndex(ArrayIndex *ai)
{
    PostConditionError(ai->Loc(), "Do not support array indices yet");
}

z3::expr PostConditionGenerator::GenerateFromBracedInitialiser(BracedInitialiser *bi)
{
    PostConditionError(bi->Loc(), "Do not support braced initialisers yet");
}

z3::expr PostConditionGenerator::GenerateFromDynamicAllocArray(DynamicAllocArray *da)
{
    PostConditionError(da->Loc(), "Do not support dynamically allocated arrays yet");
}

z3::expr PostConditionGenerator::GenerateFromFieldAccess(FieldAccess *fa)
{
    PostConditionError(fa->Loc(), "Do not support field accesses yet");
}

z3::expr PostConditionGenerator::GenerateFromTypeCast(TypeCast *tc)
{
    PostConditionError(tc->Loc(), "Do not support type casts yet");
}

z3::expr PostConditionGenerator::GenerateFromSequence(Sequence *s)
{
    PostConditionError(s->Loc(), "Do not support sequences yet");
}

//------------------STATEMENTS---------------------//

void PostConditionGenerator::GenerateFromExprStmt(ExprStmt *)
{
}

void PostConditionGenerator::GenerateFromDeclaredVar(DeclaredVar *)
{
}

void PostConditionGenerator::GenerateFromBlock(Block *b)
{
    for (auto &stmt : b->stmts)
        stmt->GeneratePost(*this);
}

void PostConditionGenerator::GenerateFromIfStmt(IfStmt *i)
{
    AddCondition(i->cond->GeneratePost(*this));
    i->then_branch->GeneratePost(*this);
    RemoveLastCondition();

    if (i->else_branch != nullptr)
    {
        AddCondition(!i->cond->GeneratePost(*this));
        i->else_branch->GeneratePost(*this);
        RemoveLastCondition();
    }
}

void PostConditionGenerator::GenerateFromWhileStmt(WhileStmt *ws)
{
    PostConditionError(ws->Loc(), "Cannot generate a post condition from a while loop");
}

void PostConditionGenerator::GenerateFromFuncDecl(FuncDecl *fd)
{
    cur_func = fd;

    if (fd->ret == VOID_TYPE)
        PostConditionError(fd->Loc(), "Cannot generate post condition errror for a void function");

    for (auto &stmt : fd->body)
        stmt->GeneratePost(*this);

    cur_func = nullptr;
}

void PostConditionGenerator::GenerateFromTemplateDecl(TemplateDecl *)
{
}

void PostConditionGenerator::GenerateFromReturn(Return *r)
{
    if (r->ret_val == nullptr)
        PostConditionError(r->Loc(), "Cannot generate post condition for a void-return");

    AddReturnValue(MAKE_RETURN(r->ret_val->GetType(), r->ret_val->GeneratePost(*this), r->Loc()));
}

void PostConditionGenerator::GenerateFromStructDecl(StructDecl *)
{
}

void PostConditionGenerator::GenerateFromImportStmt(ImportStmt *)
{
}

void PostConditionGenerator::GenerateFromBreak(Break *)
{
}

void PostConditionGenerator::GenerateFromThrow(Throw *t)
{
    PostConditionError(t->Loc(), "Cannot generate post condition for a function which potentially throws an object");
}

void PostConditionGenerator::GenerateFromTryCatch(TryCatch *tc)
{
    PostConditionError(tc->Loc(), "Cannot generate post condition for a function which uses try-catch");
}

//------------------EXPRESSIONS---------------------//

z3::expr Literal::GeneratePost(PostConditionGenerator &pc)
{
    return pc.GenerateFromLiteral(this);
}

z3::expr Unary::GeneratePost(PostConditionGenerator &pc)
{
    return pc.GenerateFromUnary(this);
}

z3::expr Binary::GeneratePost(PostConditionGenerator &pc)
{
    return pc.GenerateFromBinary(this);
}

z3::expr Assign::GeneratePost(PostConditionGenerator &pc)
{
    return pc.GenerateFromAssign(this);
}

z3::expr VarReference::GeneratePost(PostConditionGenerator &pc)
{
    return pc.GenerateFromVarReference(this);
}

z3::expr FunctionCall::GeneratePost(PostConditionGenerator &pc)
{
    return pc.GenerateFromFunctionCall(this);
}

z3::expr ArrayIndex::GeneratePost(PostConditionGenerator &pc)
{
    return pc.GenerateFromArrayIndex(this);
}

z3::expr BracedInitialiser::GeneratePost(PostConditionGenerator &pc)
{
    return pc.GenerateFromBracedInitialiser(this);
}

z3::expr DynamicAllocArray::GeneratePost(PostConditionGenerator &pc)
{
    return pc.GenerateFromDynamicAllocArray(this);
}

z3::expr FieldAccess::GeneratePost(PostConditionGenerator &pc)
{
    return pc.GenerateFromFieldAccess(this);
}

z3::expr TypeCast::GeneratePost(PostConditionGenerator &pc)
{
    return pc.GenerateFromTypeCast(this);
}

z3::expr Sequence::GeneratePost(PostConditionGenerator &pc)
{
    return pc.GenerateFromSequence(this);
}

//------------------STATEMENTS---------------------//

void ExprStmt::GeneratePost(PostConditionGenerator &pc)
{
    pc.GenerateFromExprStmt(this);
}

void DeclaredVar::GeneratePost(PostConditionGenerator &pc)
{
    pc.GenerateFromDeclaredVar(this);
}

void Block::GeneratePost(PostConditionGenerator &pc)
{
    pc.GenerateFromBlock(this);
}

void IfStmt::GeneratePost(PostConditionGenerator &pc)
{
    pc.GenerateFromIfStmt(this);
}

void WhileStmt::GeneratePost(PostConditionGenerator &pc)
{
    pc.GenerateFromWhileStmt(this);
}

void FuncDecl::GeneratePost(PostConditionGenerator &pc)
{
    pc.GenerateFromFuncDecl(this);
}

void TemplateDecl::GeneratePost(PostConditionGenerator &pc)
{
    pc.GenerateFromTemplateDecl(this);
}

void Return::GeneratePost(PostConditionGenerator &pc)
{
    pc.GenerateFromReturn(this);
}

void StructDecl::GeneratePost(PostConditionGenerator &pc)
{
    pc.GenerateFromStructDecl(this);
}

void ImportStmt::GeneratePost(PostConditionGenerator &pc)
{
    pc.GenerateFromImportStmt(this);
}

void Break::GeneratePost(PostConditionGenerator &pc)
{
    pc.GenerateFromBreak(this);
}

void Throw::GeneratePost(PostConditionGenerator &pc)
{
    pc.GenerateFromThrow(this);
}

void TryCatch::GeneratePost(PostConditionGenerator &pc)
{
    pc.GenerateFromTryCatch(this);
}