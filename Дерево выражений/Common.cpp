#include "Common.h"

using namespace std;


class ValueExpr : public Expression {
public:
	ValueExpr() = default;

	ValueExpr(int value) : value_(value)
	{
	}

	int Evaluate() const override {
		return value_;
	}

	std::string ToString() const override{
		return std::to_string(value_);
	}

private:
	int value_;
};



class Operation : public Expression {
public:

	Operation() = default;

	Operation(ExpressionPtr lhs, ExpressionPtr rhs) :
		lhs_(move(lhs)), rhs_(move(rhs))
	{
	}

	std::string ToString() const override final {
		return  "(" + lhs_->ToString() + ")" + GetSymbol() + "(" + rhs_->ToString() + ")";
	}

	int Evaluate() const override final {
		if (GetSymbol() == '+') {
			return lhs_->Evaluate() + rhs_->Evaluate();
		}
		else if (GetSymbol() == '*') {
			return lhs_->Evaluate() * rhs_->Evaluate();
		}		
	}
	
private:
	ExpressionPtr lhs_, rhs_;
	virtual char GetSymbol() const = 0;
};



class SumExpr : public Operation {
public:
	SumExpr(ExpressionPtr lhs, ExpressionPtr rhs) :
		Operation(move(lhs), move(rhs))
	{
	}

	char GetSymbol() const override {
		return '+';
	}

};


class ProductExpr : public Operation {
public:
	ProductExpr(ExpressionPtr lhs, ExpressionPtr rhs) : 
	Operation(move(lhs), move(rhs))
	{
	}

	char GetSymbol() const override {
		return '*';
	}

};

ExpressionPtr Value(int value) {
	return make_unique<ValueExpr>(value);
}

ExpressionPtr Sum(ExpressionPtr left, ExpressionPtr right) {
	return make_unique<SumExpr>(move(left), move(right));
}

ExpressionPtr Product(ExpressionPtr left, ExpressionPtr right) {
	return make_unique<ProductExpr>(move(left), move(right));
}