from . import *
from .instr import *
from .namer import NameInfo
from ..utils import *
from ..generated.MiniDecafParser import MiniDecafParser
from ..generated.MiniDecafVisitor import MiniDecafVisitor


class LabelManager:
    def __init__(self):
        self.nlabels = {}
        self.loopEntry = []
        self.loopExit = []

    def newLabel(self, scope="_L"):
        incOrInit(self.nlabels, scope)
        return f"{scope}_{self.nlabels[scope]}"

    def enterLoop(self, entry, exit):
        self.loopEntry.append(entry)
        self.loopExit.append(exit)

    def exitLoop(self):
        self.loopEntry.pop()
        self.loopExit.pop()

    def breakLabel(self):
        if len(self.loopExit) == 0:
            raise MiniDecafLocatedError("not in a loop")
        return self.loopExit[-1]

    def continueLabel(self):
        if len(self.loopExit) == 0:
            raise MiniDecafLocatedError("not in a loop")
        return self.loopEntry[-1]



class StackIRGen(MiniDecafVisitor):
    def __init__(self, emitter:IREmitter, nameInfo:NameInfo):
        self._E = emitter
        self.lbl = LabelManager()
        self.ni = nameInfo
        self._curFuncNameInfo = None

    def _var(self, term):
        return self._curFuncNameInfo[term]

    def visitReturnStmt(self, ctx:MiniDecafParser.ReturnStmtContext):
        self._E([Comment("[ir-stmt] Ret")])
        self.visitChildren(ctx)
        self._E([Ret()])

    def visitExprStmt(self, ctx:MiniDecafParser.ExprStmtContext):
        self._E([Comment("[ir-stmt] Expr")])
        self.visitChildren(ctx)
        self._E([Pop()])

    def visitDeclStmt(self, ctx:MiniDecafParser.DeclStmtContext):
        self._E([Comment("[ir-stmt] Decl")])
        self.visitChildren(ctx)

    def visitIfStmt(self, ctx:MiniDecafParser.IfStmtContext):
        self._E([Comment("[ir-stmt] If")])
        ctx.expr().accept(self)
        exitLabel = self.lbl.newLabel("if_end")
        elseLabel = self.lbl.newLabel("if_else")
        if ctx.el is not None:
            self._E([Branch("beqz", elseLabel)])
            ctx.th.accept(self)
            self._E([Branch("br", exitLabel), Label(elseLabel)])
            ctx.el.accept(self)
            self._E([Label(exitLabel)])
        else:
            self._E([Branch("beqz", exitLabel)])
            ctx.th.accept(self)
            self._E([Label(exitLabel)])

    def visitBlock(self, ctx:MiniDecafParser.BlockContext):
        self._E([Comment("[ir-block] Enter")])
        self.visitChildren(ctx)
        self._E([Pop()] * self._curFuncNameInfo.blockSlots[ctx])
        self._E([Comment("[ir-block] Exit")])

    def loop(self, name, init, cond, body, post):
        entryLabel = self.lbl.newLabel(f"{name}_entry")
        if post is not None:
            continueLabel = self.lbl.newLabel(f"{name}_continue")
        else:
            continueLabel = entryLabel
        exitLabel = self.lbl.newLabel(f"{name}_exit")
        self.lbl.enterLoop(continueLabel, exitLabel)
        if init is not None:
            init.accept(self)
            if isinstance(init, MiniDecafParser.ExprContext):
                self._E([Pop()])
        self._E([Label(entryLabel)])
        if cond is not None:
            cond.accept(self)
        else:
            self._E([Const(1)])
        self._E([Branch("beqz", exitLabel)])
        body.accept(self)
        if post is not None:
            self._E([Label(continueLabel)])
            post.accept(self)
            if isinstance(post, MiniDecafParser.ExprContext):
                self._E([Pop()])
        self._E([Branch("br", entryLabel), Label(exitLabel)])
        self.lbl.exitLoop()

    def visitForDeclStmt(self, ctx:MiniDecafParser.ForDeclStmtContext):
        self.loop("for", ctx.init, ctx.ctrl, ctx.stmt(), ctx.post)
        self._E([Pop()] * self._curFuncNameInfo.blockSlots[ctx])

    def visitForStmt(self, ctx:MiniDecafParser.ForStmtContext):
        self.loop("for", ctx.init, ctx.ctrl, ctx.stmt(), ctx.post)

    def visitWhileStmt(self, ctx:MiniDecafParser.WhileStmtContext):
        self.loop("while", None, ctx.expr(), ctx.stmt(), None)

    def visitDoWhileStmt(self, ctx:MiniDecafParser.DoWhileStmtContext):
        self.loop("dowhile", ctx.stmt(), ctx.expr(), ctx.stmt(), None)

    def visitAtomInteger(self, ctx:MiniDecafParser.AtomIntegerContext):
        self._E([Const(int(text(ctx.Integer())))])

    def visitBreakStmt(self, ctx:MiniDecafParser.BreakStmtContext):
        self._E([Branch("br", self.lbl.breakLabel())])

    def visitContinueStmt(self, ctx:MiniDecafParser.ContinueStmtContext):
        self._E([Branch("br", self.lbl.continueLabel())])

    def visitCUnary(self, ctx:MiniDecafParser.CUnaryContext):
        self.visitChildren(ctx)
        self._E([Unary(text(ctx.unaryOp()))])

    def _binaryExpr(self, ctx, op):
        self.visitChildren(ctx)
        self._E([Binary(text(op))])
    def visitCAdd(self, ctx:MiniDecafParser.CAddContext):
        self._binaryExpr(ctx, ctx.addOp())
    def visitCMul(self, ctx:MiniDecafParser.CMulContext):
        self._binaryExpr(ctx, ctx.mulOp())
    def visitCRel(self, ctx:MiniDecafParser.CRelContext):
        self._binaryExpr(ctx, ctx.relOp())
    def visitCEq(self, ctx:MiniDecafParser.CEqContext):
        self._binaryExpr(ctx, ctx.eqOp())

    def visitCLand(self, ctx:MiniDecafParser.CLandContext):
        falseLabel = self.lbl.newLabel("land_false")
        exitLabel = self.lbl.newLabel("land_exit")
        ctx.land().accept(self)
        self._E([Branch("beqz", falseLabel)])
        ctx.eq().accept(self)
        self._E([Branch("beqz", falseLabel), Const(1), Branch("br", exitLabel),
            Label(falseLabel), Const(0), Label(exitLabel)])

    def visitCLor(self, ctx:MiniDecafParser.CLorContext):
        trueLabel = self.lbl.newLabel("lor_true")
        exitLabel = self.lbl.newLabel("lor_exit")
        ctx.lor().accept(self)
        self._E([Branch("bnez", trueLabel)])
        ctx.land().accept(self)
        self._E([Branch("bnez", trueLabel), Const(0), Branch("br", exitLabel),
            Label(trueLabel), Const(1), Label(exitLabel)])

    def visitDecl(self, ctx:MiniDecafParser.DeclContext):
        var = self._var(ctx.Ident())
        if ctx.expr() is not None:
            ctx.expr().accept(self)
        else:
            self._E([Const(0)])
        self._E([Comment(f"[ir-offset]: {var} -> {var.offset}")])

    def visitAtomIdent(self, ctx:MiniDecafParser.AtomIdentContext):
        var = self._var(ctx.Ident())
        self._E([FrameSlot(var.offset), Load()])

    def _computeAddr(self, lvalue:Unary):
        if isinstance(lvalue, MiniDecafParser.TUnaryContext):
            return self._computeAddr(lvalue.atom())
        if isinstance(lvalue, MiniDecafParser.AtomIdentContext):
            var = self._var(lvalue.Ident())
            return self._E([FrameSlot(var.offset)])
        elif isinstance(lvalue, MiniDecafParser.AtomParenContext):
            return self._computeAddr(lvalue.expr())
        raise MiniDecafLocatedError(lvalue, f"{text(lvalue)} is not a lvalue")

    def visitCAsgn(self, ctx:MiniDecafParser.CAsgnContext):
        ctx.asgn().accept(self)
        self._computeAddr(ctx.unary())
        self._E([Store()])

    def visitCCond(self, ctx:MiniDecafParser.CCondContext):
        ctx.lor().accept(self)
        exitLabel = self.lbl.newLabel("cond_end")
        elseLabel = self.lbl.newLabel("cond_else")
        self._E([Branch("beqz", elseLabel)])
        ctx.expr().accept(self)
        self._E([Branch("br", exitLabel), Label(elseLabel)])
        ctx.cond().accept(self)
        self._E([Label(exitLabel)])

    def visitFuncDef(self, ctx:MiniDecafParser.FuncDefContext):
        func = text(ctx.Ident())
        self._curFuncNameInfo = self.ni.funcNameInfos[func]
        paramInfo = self.ni.paramInfos[func]
        self._E.enterFunction(func, paramInfo)
        for var in paramInfo.vars:
            self._E([Comment(f"[ir-offset]: {var} -> {var.offset}  # param")])
        ctx.block().accept(self)
        self._E.exitFunction()

    def visitFuncDecl(self, ctx:MiniDecafParser.FuncDeclContext):
        pass

    def visitAtomCall(self, ctx:MiniDecafParser.AtomCallContext):
        args = ctx.argList().expr()
        for arg in reversed(args):
            arg.accept(self)
        func = text(ctx.Ident())
        self._E([Call(func)])
