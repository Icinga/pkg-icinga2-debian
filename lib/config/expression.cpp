/******************************************************************************
 * Icinga 2                                                                   *
 * Copyright (C) 2012-2015 Icinga Development Team (http://www.icinga.org)    *
 *                                                                            *
 * This program is free software; you can redistribute it and/or              *
 * modify it under the terms of the GNU General Public License                *
 * as published by the Free Software Foundation; either version 2             *
 * of the License, or (at your option) any later version.                     *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program; if not, write to the Free Software Foundation     *
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.             *
 ******************************************************************************/

#include "config/expression.hpp"
#include "config/configitem.hpp"
#include "config/vmops.hpp"
#include "base/array.hpp"
#include "base/json.hpp"
#include "base/object.hpp"
#include "base/logger.hpp"
#include "base/exception.hpp"
#include "base/scriptglobal.hpp"
#include <boost/foreach.hpp>
#include <boost/exception_ptr.hpp>
#include <boost/exception/errinfo_nested_exception.hpp>

using namespace icinga;

Expression::~Expression(void)
{ }

ExpressionResult Expression::Evaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	try {
#ifdef I2_DEBUG
/*		std::ostringstream msgbuf;
		ShowCodeFragment(msgbuf, GetDebugInfo(), false);
		Log(LogDebug, "Expression")
			<< "Executing:\n" << msgbuf.str();*/
#endif /* I2_DEBUG */

		return DoEvaluate(frame, dhint);
	} catch (const ScriptError& ex) {
		throw;
	} catch (const std::exception& ex) {
		BOOST_THROW_EXCEPTION(ScriptError("Error while evaluating expression: " + String(ex.what()), GetDebugInfo())
		    << boost::errinfo_nested_exception(boost::current_exception()));
	}
}

bool Expression::GetReference(ScriptFrame& frame, bool init_dict, Value *parent, String *index, DebugHint **dhint) const
{
	return false;
}

const DebugInfo& Expression::GetDebugInfo(void) const
{
	static DebugInfo debugInfo;
	return debugInfo;
}

Expression *icinga::MakeIndexer(ScopeSpecifier scopeSpec, const String& index)
{
	Expression *scope = new GetScopeExpression(scopeSpec);
	return new IndexerExpression(scope, MakeLiteral(index));
}

void DictExpression::MakeInline(void)
{
	m_Inline = true;
}

LiteralExpression::LiteralExpression(const Value& value)
	: m_Value(value)
{ }

ExpressionResult LiteralExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	return m_Value;
}

const DebugInfo& DebuggableExpression::GetDebugInfo(void) const
{
	return m_DebugInfo;
}

ExpressionResult VariableExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	if (frame.Locals && frame.Locals->Contains(m_Variable))
		return frame.Locals->Get(m_Variable);
	else if (frame.Self.IsObject() && frame.Locals != static_cast<Object::Ptr>(frame.Self) && VMOps::HasField(frame.Self, m_Variable))
		return VMOps::GetField(frame.Self, m_Variable, m_DebugInfo);
	else
		return ScriptGlobal::Get(m_Variable);
}

bool VariableExpression::GetReference(ScriptFrame& frame, bool init_dict, Value *parent, String *index, DebugHint **dhint) const
{
	*index = m_Variable;

	if (frame.Locals && frame.Locals->Contains(m_Variable)) {
		*parent = frame.Locals;

		if (dhint)
			*dhint = NULL;
	} else if (frame.Self.IsObject() && frame.Locals != static_cast<Object::Ptr>(frame.Self) && VMOps::HasField(frame.Self, m_Variable)) {
		*parent = frame.Self;

		if (dhint && *dhint)
			*dhint = new DebugHint((*dhint)->GetChild(m_Variable));
	} else if (ScriptGlobal::Exists(m_Variable)) {
		*parent = ScriptGlobal::GetGlobals();

		if (dhint)
			*dhint = NULL;
	} else
		*parent = frame.Self;

	return true;
}

ExpressionResult NegateExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	ExpressionResult operand = m_Operand->Evaluate(frame);
	CHECK_RESULT(operand);

	return ~(long)operand.GetValue();
}

ExpressionResult LogicalNegateExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	ExpressionResult operand = m_Operand->Evaluate(frame);
	CHECK_RESULT(operand);

	return !operand.GetValue().ToBool();
}

ExpressionResult AddExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	ExpressionResult operand1 = m_Operand1->Evaluate(frame);
	CHECK_RESULT(operand1);

	ExpressionResult operand2 = m_Operand2->Evaluate(frame);
	CHECK_RESULT(operand2);

	return operand1.GetValue() + operand2.GetValue();
}

ExpressionResult SubtractExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	ExpressionResult operand1 = m_Operand1->Evaluate(frame);
	CHECK_RESULT(operand1);

	ExpressionResult operand2 = m_Operand2->Evaluate(frame);
	CHECK_RESULT(operand2);

	return operand1.GetValue() - operand2.GetValue();
}

ExpressionResult MultiplyExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	ExpressionResult operand1 = m_Operand1->Evaluate(frame);
	CHECK_RESULT(operand1);

	ExpressionResult operand2 = m_Operand2->Evaluate(frame);
	CHECK_RESULT(operand2);

	return operand1.GetValue() * operand2.GetValue();
}

ExpressionResult DivideExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	ExpressionResult operand1 = m_Operand1->Evaluate(frame);
	CHECK_RESULT(operand1);

	ExpressionResult operand2 = m_Operand2->Evaluate(frame);
	CHECK_RESULT(operand2);

	return operand1.GetValue() / operand2.GetValue();
}

ExpressionResult ModuloExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	ExpressionResult operand1 = m_Operand1->Evaluate(frame);
	CHECK_RESULT(operand1);

	ExpressionResult operand2 = m_Operand2->Evaluate(frame);
	CHECK_RESULT(operand2);

	return operand1.GetValue() % operand2.GetValue();
}

ExpressionResult XorExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	ExpressionResult operand1 = m_Operand1->Evaluate(frame);
	CHECK_RESULT(operand1);

	ExpressionResult operand2 = m_Operand2->Evaluate(frame);
	CHECK_RESULT(operand2);

	return operand1.GetValue() ^ operand2.GetValue();
}

ExpressionResult BinaryAndExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	ExpressionResult operand1 = m_Operand1->Evaluate(frame);
	CHECK_RESULT(operand1);

	ExpressionResult operand2 = m_Operand2->Evaluate(frame);
	CHECK_RESULT(operand2);

	return operand1.GetValue() & operand2.GetValue();
}

ExpressionResult BinaryOrExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	ExpressionResult operand1 = m_Operand1->Evaluate(frame);
	CHECK_RESULT(operand1);

	ExpressionResult operand2 = m_Operand2->Evaluate(frame);
	CHECK_RESULT(operand2);

	return operand1.GetValue() | operand2.GetValue();
}

ExpressionResult ShiftLeftExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	ExpressionResult operand1 = m_Operand1->Evaluate(frame);
	CHECK_RESULT(operand1);

	ExpressionResult operand2 = m_Operand2->Evaluate(frame);
	CHECK_RESULT(operand2);

	return operand1.GetValue() << operand2.GetValue();
}

ExpressionResult ShiftRightExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	ExpressionResult operand1 = m_Operand1->Evaluate(frame);
	CHECK_RESULT(operand1);

	ExpressionResult operand2 = m_Operand2->Evaluate(frame);
	CHECK_RESULT(operand2);

	return operand1.GetValue() >> operand2.GetValue();
}

ExpressionResult EqualExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	ExpressionResult operand1 = m_Operand1->Evaluate(frame);
	CHECK_RESULT(operand1);

	ExpressionResult operand2 = m_Operand2->Evaluate(frame);
	CHECK_RESULT(operand2);

	return operand1.GetValue() == operand2.GetValue();
}

ExpressionResult NotEqualExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	ExpressionResult operand1 = m_Operand1->Evaluate(frame);
	CHECK_RESULT(operand1);

	ExpressionResult operand2 = m_Operand2->Evaluate(frame);
	CHECK_RESULT(operand2);

	return operand1.GetValue() != operand2.GetValue();
}

ExpressionResult LessThanExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	ExpressionResult operand1 = m_Operand1->Evaluate(frame);
	CHECK_RESULT(operand1);

	ExpressionResult operand2 = m_Operand2->Evaluate(frame);
	CHECK_RESULT(operand2);

	return operand1.GetValue() < operand2.GetValue();
}

ExpressionResult GreaterThanExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	ExpressionResult operand1 = m_Operand1->Evaluate(frame);
	CHECK_RESULT(operand1);

	ExpressionResult operand2 = m_Operand2->Evaluate(frame);
	CHECK_RESULT(operand2);

	return operand1.GetValue() > operand2.GetValue();
}

ExpressionResult LessThanOrEqualExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	ExpressionResult operand1 = m_Operand1->Evaluate(frame);
	CHECK_RESULT(operand1);

	ExpressionResult operand2 = m_Operand2->Evaluate(frame);
	CHECK_RESULT(operand2);

	return operand1.GetValue() <= operand2.GetValue();
}

ExpressionResult GreaterThanOrEqualExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	ExpressionResult operand1 = m_Operand1->Evaluate(frame);
	CHECK_RESULT(operand1);

	ExpressionResult operand2 = m_Operand2->Evaluate(frame);
	CHECK_RESULT(operand2);

	return operand1.GetValue() >= operand2.GetValue();
}

ExpressionResult InExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	ExpressionResult operand2 = m_Operand2->Evaluate(frame);
	CHECK_RESULT(operand2);

	if (operand2.GetValue().IsEmpty())
		return false;
	else if (!operand2.GetValue().IsObjectType<Array>())
		BOOST_THROW_EXCEPTION(ScriptError("Invalid right side argument for 'in' operator: " + JsonEncode(operand2.GetValue()), m_DebugInfo));

	ExpressionResult operand1 = m_Operand1->Evaluate(frame);
	CHECK_RESULT(operand1)

	Array::Ptr arr = operand2.GetValue();
	return arr->Contains(operand1.GetValue());
}

ExpressionResult NotInExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	ExpressionResult operand2 = m_Operand2->Evaluate(frame);
	CHECK_RESULT(operand2);

	if (operand2.GetValue().IsEmpty())
		return true;
	else if (!operand2.GetValue().IsObjectType<Array>())
		BOOST_THROW_EXCEPTION(ScriptError("Invalid right side argument for 'in' operator: " + JsonEncode(operand2.GetValue()), m_DebugInfo));

	ExpressionResult operand1 = m_Operand1->Evaluate(frame);
	CHECK_RESULT(operand1);

	Array::Ptr arr = operand2.GetValue();
	return !arr->Contains(operand1.GetValue());
}

ExpressionResult LogicalAndExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	ExpressionResult operand1 = m_Operand1->Evaluate(frame);
	CHECK_RESULT(operand1);

	if (!operand1.GetValue().ToBool())
		return operand1;
	else {
		ExpressionResult operand2 = m_Operand2->Evaluate(frame);
		CHECK_RESULT(operand2);

		return operand2.GetValue();
	}
}

ExpressionResult LogicalOrExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	ExpressionResult operand1 = m_Operand1->Evaluate(frame);
	CHECK_RESULT(operand1);

	if (operand1.GetValue().ToBool())
		return operand1;
	else {
		ExpressionResult operand2 = m_Operand2->Evaluate(frame);
		CHECK_RESULT(operand2);

		return operand2.GetValue();
	}
}

ExpressionResult FunctionCallExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	Value self, vfunc;
	String index;

	if (m_FName->GetReference(frame, false, &self, &index))
		vfunc = VMOps::GetField(self, index, m_DebugInfo);
	else {
		ExpressionResult vfuncres = m_FName->Evaluate(frame);
		CHECK_RESULT(vfuncres);

		vfunc = vfuncres.GetValue();
	}

	if (!vfunc.IsObjectType<Function>())
		BOOST_THROW_EXCEPTION(ScriptError("Argument is not a callable object.", m_DebugInfo));

	Function::Ptr func = vfunc;

	std::vector<Value> arguments;
	BOOST_FOREACH(Expression *arg, m_Args) {
		ExpressionResult argres = arg->Evaluate(frame);
		CHECK_RESULT(argres);

		arguments.push_back(argres.GetValue());
	}

	return VMOps::FunctionCall(frame, self, func, arguments);
}

ExpressionResult ArrayExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	Array::Ptr result = new Array();

	BOOST_FOREACH(Expression *aexpr, m_Expressions) {
		ExpressionResult element = aexpr->Evaluate(frame);
		CHECK_RESULT(element);

		result->Add(element.GetValue());
	}

	return result;
}

ExpressionResult DictExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	ScriptFrame *dframe;
	ScriptFrame rframe;

	if (!m_Inline) {
		dframe = &rframe;
		rframe.Locals = frame.Locals;
		rframe.Self = new Dictionary();
	} else {
		dframe = &frame;
	}

	Value result;

	BOOST_FOREACH(Expression *aexpr, m_Expressions) {
		ExpressionResult element = aexpr->Evaluate(*dframe, dhint);
		CHECK_RESULT(element);
		result = element.GetValue();
	}

	if (m_Inline)
		return result;
	else
		return dframe->Self;
}

ExpressionResult GetScopeExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	if (m_ScopeSpec == ScopeLocal)
		return frame.Locals;
	else if (m_ScopeSpec == ScopeCurrent)
		return frame.Self;
	else if (m_ScopeSpec == ScopeThis)
		return frame.Self;
	else if (m_ScopeSpec == ScopeGlobal)
		return ScriptGlobal::GetGlobals();
	else
		VERIFY(!"Invalid scope.");
}

ExpressionResult SetExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	DebugHint *psdhint = dhint;

	Value parent;
	String index;

	if (!m_Operand1->GetReference(frame, true, &parent, &index, &psdhint))
		BOOST_THROW_EXCEPTION(ScriptError("Expression cannot be assigned to.", m_DebugInfo));

	ExpressionResult operand2 = m_Operand2->Evaluate(frame, dhint);
	CHECK_RESULT(operand2);

	if (m_Op != OpSetLiteral) {
		Value object = VMOps::GetField(parent, index, m_DebugInfo);

		Expression *lhs = MakeLiteral(object);
		Expression *rhs = MakeLiteral(operand2);

		switch (m_Op) {
			case OpSetAdd:
				operand2 = AddExpression(lhs, rhs, m_DebugInfo).Evaluate(frame, dhint);
				CHECK_RESULT(operand2);
				break;
			case OpSetSubtract:
				operand2 = SubtractExpression(lhs, rhs, m_DebugInfo).Evaluate(frame, dhint);
				CHECK_RESULT(operand2);
				break;
			case OpSetMultiply:
				operand2 = MultiplyExpression(lhs, rhs, m_DebugInfo).Evaluate(frame, dhint);
				CHECK_RESULT(operand2);
				break;
			case OpSetDivide:
				operand2 = DivideExpression(lhs, rhs, m_DebugInfo).Evaluate(frame, dhint);
				CHECK_RESULT(operand2);
				break;
			case OpSetModulo:
				operand2 = ModuloExpression(lhs, rhs, m_DebugInfo).Evaluate(frame, dhint);
				CHECK_RESULT(operand2);
				break;
			case OpSetXor:
				operand2 = XorExpression(lhs, rhs, m_DebugInfo).Evaluate(frame, dhint);
				CHECK_RESULT(operand2);
				break;
			case OpSetBinaryAnd:
				operand2 = BinaryAndExpression(lhs, rhs, m_DebugInfo).Evaluate(frame, dhint);
				CHECK_RESULT(operand2);
				break;
			case OpSetBinaryOr:
				operand2 = BinaryOrExpression(lhs, rhs, m_DebugInfo).Evaluate(frame, dhint);
				CHECK_RESULT(operand2);
				break;
			default:
				VERIFY(!"Invalid opcode.");
		}
	}

	VMOps::SetField(parent, index, operand2.GetValue(), m_DebugInfo);

	if (psdhint) {
		psdhint->AddMessage("=", m_DebugInfo);

		if (psdhint != dhint)
			delete psdhint;
	}

	return Empty;
}

ExpressionResult ConditionalExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	ExpressionResult condition = m_Condition->Evaluate(frame, dhint);
	CHECK_RESULT(condition);

	if (condition.GetValue().ToBool())
		return m_TrueBranch->Evaluate(frame, dhint);
	else if (m_FalseBranch)
		return m_FalseBranch->Evaluate(frame, dhint);

	return Empty;
}

ExpressionResult WhileExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	for (;;) {
		ExpressionResult condition = m_Condition->Evaluate(frame, dhint);
		CHECK_RESULT(condition);

		if (!condition.GetValue().ToBool())
			break;

		ExpressionResult loop_body = m_LoopBody->Evaluate(frame, dhint);
		CHECK_RESULT_LOOP(loop_body);
	}

	return Empty;
}

ExpressionResult ReturnExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	ExpressionResult operand = m_Operand->Evaluate(frame);
	CHECK_RESULT(operand);

	return ExpressionResult(operand.GetValue(), ResultReturn);
}

ExpressionResult BreakExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	return ExpressionResult(Empty, ResultBreak);
}

ExpressionResult ContinueExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	return ExpressionResult(Empty, ResultContinue);
}

ExpressionResult IndexerExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	ExpressionResult operand1 = m_Operand1->Evaluate(frame, dhint);
	CHECK_RESULT(operand1);

	ExpressionResult operand2 = m_Operand2->Evaluate(frame, dhint);
	CHECK_RESULT(operand2);

	return VMOps::GetField(operand1.GetValue(), operand2.GetValue(), m_DebugInfo);
}

bool IndexerExpression::GetReference(ScriptFrame& frame, bool init_dict, Value *parent, String *index, DebugHint **dhint) const
{
	Value vparent;
	String vindex;
	DebugHint *psdhint = NULL;
	bool free_psd = false;

	if (dhint)
		psdhint = *dhint;

	if (m_Operand1->GetReference(frame, init_dict, &vparent, &vindex, &psdhint)) {
		if (init_dict) {
			Value old_value =  VMOps::GetField(vparent, vindex, m_Operand1->GetDebugInfo());

			if (old_value.IsEmpty() && !old_value.IsString())
				VMOps::SetField(vparent, vindex, new Dictionary(), m_Operand1->GetDebugInfo());
		}

		*parent = VMOps::GetField(vparent, vindex, m_DebugInfo);
		free_psd = true;
	} else {
		ExpressionResult operand1 = m_Operand1->Evaluate(frame);
		*parent = operand1.GetValue();
	}

	ExpressionResult operand2 = m_Operand2->Evaluate(frame);
	*index = operand2.GetValue();

	if (dhint && psdhint)
		*dhint = new DebugHint(psdhint->GetChild(*index));

	if (free_psd)
		delete psdhint;

	return true;
}

void icinga::BindToScope(Expression *& expr, ScopeSpecifier scopeSpec)
{
	DictExpression *dexpr = dynamic_cast<DictExpression *>(expr);

	if (dexpr) {
		BOOST_FOREACH(Expression *& expr, dexpr->m_Expressions)
			BindToScope(expr, scopeSpec);

		return;
	}

	SetExpression *aexpr = dynamic_cast<SetExpression *>(expr);

	if (aexpr) {
		BindToScope(aexpr->m_Operand1, scopeSpec);

		return;
	}

	IndexerExpression *iexpr = dynamic_cast<IndexerExpression *>(expr);

	if (iexpr) {
		BindToScope(iexpr->m_Operand1, scopeSpec);
		return;
	}

	LiteralExpression *lexpr = dynamic_cast<LiteralExpression *>(expr);
	ScriptFrame frame;

	if (lexpr && lexpr->Evaluate(frame).GetValue().IsString()) {
		Expression *scope = new GetScopeExpression(scopeSpec);
		expr = new IndexerExpression(scope, lexpr, lexpr->GetDebugInfo());
	}

	VariableExpression *vexpr = dynamic_cast<VariableExpression *>(expr);

	if (vexpr) {
		Expression *scope = new GetScopeExpression(scopeSpec);
		Expression *new_expr = new IndexerExpression(scope, MakeLiteral(vexpr->GetVariable()), vexpr->GetDebugInfo());
		delete expr;
		expr = new_expr;
	}
}

ExpressionResult ThrowExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	ExpressionResult messageres = m_Message->Evaluate(frame);
	CHECK_RESULT(messageres);
	Value message = messageres.GetValue();
	BOOST_THROW_EXCEPTION(ScriptError(message, m_DebugInfo));
}

ExpressionResult ImportExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	String type = VMOps::GetField(frame.Self, "type", m_DebugInfo);
	ExpressionResult nameres = m_Name->Evaluate(frame);
	CHECK_RESULT(nameres);
	Value name = nameres.GetValue();

	if (!name.IsString())
		BOOST_THROW_EXCEPTION(ScriptError("Template/object name must be a string", m_DebugInfo));

	ConfigItem::Ptr item = ConfigItem::GetObject(type, name);

	if (!item)
		BOOST_THROW_EXCEPTION(ScriptError("Import references unknown template: '" + name + "'", m_DebugInfo));

	ExpressionResult result = item->GetExpression()->Evaluate(frame, dhint);
	CHECK_RESULT(result);

	return Empty;
}

ExpressionResult FunctionExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	return VMOps::NewFunction(frame, m_Args, m_ClosedVars, m_Expression);
}

ExpressionResult ApplyExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	ExpressionResult nameres = m_Name->Evaluate(frame);
	CHECK_RESULT(nameres);

	return VMOps::NewApply(frame, m_Type, m_Target, nameres.GetValue(), m_Filter,
	    m_FKVar, m_FVVar, m_FTerm, m_ClosedVars, m_Expression, m_DebugInfo);
}

ExpressionResult ObjectExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	String name;

	if (m_Name) {
		ExpressionResult nameres = m_Name->Evaluate(frame, dhint);
		CHECK_RESULT(nameres);

		name = nameres.GetValue();
	}

	return VMOps::NewObject(frame, m_Abstract, m_Type, name, m_Filter, m_Zone,
	    m_ClosedVars, m_Expression, m_DebugInfo);
}

ExpressionResult ForExpression::DoEvaluate(ScriptFrame& frame, DebugHint *dhint) const
{
	ExpressionResult valueres = m_Value->Evaluate(frame, dhint);
	CHECK_RESULT(valueres);

	return VMOps::For(frame, m_FKVar, m_FVVar, valueres.GetValue(), m_Expression, m_DebugInfo);
}

