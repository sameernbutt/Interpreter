#include <iostream>
#include <string>
#include <vector> 
using namespace std; 
#include "val.h"
//#include "val.cpp"

Value Value::operator/(const Value& op) const{
    if(IsErr() || op.IsErr()) // ERROR
        return Value();
    if(IsReal()){
        if(op.IsReal()) return (GetReal()/op.GetReal()); // both real
        else if(op.IsInt()) return (GetReal()/op.GetInt()); // first real second int
    }
    else if(IsInt()){
        if(op.IsReal()) return (GetInt()/op.GetReal());
        else if(op.IsInt()) return (GetInt()/op.GetInt());
    }
    return Value();
}

Value Value::operator>(const Value& op) const{
    if(IsErr() || op.IsErr())
        return Value();
    if(IsReal()){
        if(op.IsReal()) return (GetReal()>op.GetReal());
        else if(op.IsInt()) return (GetReal()>op.GetInt());
    }
    else if(IsInt()){
        if(op.IsReal()) return (GetInt()>op.GetReal());
        else if(op.IsInt()) return (GetInt()>op.GetInt());
    }
    return Value();
}

Value Value::operator<(const Value& op) const{
    if(IsErr() || op.IsErr())
        return Value();
    if(IsReal()){
        if(op.IsReal()) return (GetReal()<op.GetReal());
        else if(op.IsInt()) return (GetReal()<op.GetInt());
    }
    else if(IsInt()){
        if(op.IsReal()) return (GetInt()<op.GetReal());
        else if(op.IsInt()) return (GetInt()<op.GetInt());
    }
    return Value();
}

Value Value::operator&&(const Value& op) const{
    if(IsErr() || op.IsErr())
        return Value();
    if(IsBool() && op.IsBool()){ // bools only
        return (GetBool() && op.GetBool());
    }
    return Value();
}

Value Value::operator||(const Value& op) const{
    if(IsErr() || op.IsErr())
        return Value();
    if(IsBool() && op.IsBool()){ // bools only
        return (GetBool() || op.GetBool());
    }
    return Value();
}

Value Value::operator==(const Value& op) const{
    if(IsErr() || op.IsErr())
        return Value();
    if(IsBool() && op.IsBool()){ // bool first
        if(GetBool()==op.GetBool()){
            return true;
        }
        else return false;
    }
    else if(IsString() && op.IsString()){ // check string
        if(GetString()==op.GetString()){
            return true;
        }
        else return false;
    }
    else if(IsReal()){ // first is real and check the second
        if(op.IsReal()) return (GetReal()==op.GetReal());
        else if(op.IsInt()) return (GetReal()==op.GetInt());
    }
    else if(IsInt()){ // first is int and check the second
        if(op.IsReal()) return (GetInt()==op.GetReal());
        else if(op.IsInt()) return (GetInt()==op.GetInt());
    }
    return Value();
}

Value Value::operator!() const{
    if(IsErr()){
        return Value();
    }
    if(IsBool()){
        return !GetBool();
    }
    return Value();
}

Value Value::operator%(const Value& op) const{
    if(IsErr() || op.IsErr())
        return Value();
    if(IsInt() && op.IsInt()){
        return (GetInt() % op.GetInt());
    }
    return Value();
}

Value Value::operator+(const Value& op) const{
    if(IsErr() || op.IsErr())
        return Value();
    if(IsReal()){
        if(op.IsReal()) return (GetReal()+op.GetReal()); // both real
        else if(op.IsInt()) return (GetReal()+op.GetInt()); // first real second int
    }
    else if(IsInt()){
        if(op.IsReal()) return (GetInt()+op.GetReal());
        else if(op.IsInt()) return (GetInt()+op.GetInt());
    }
    return Value();
}

Value Value::operator-(const Value& op) const{
    if(IsErr() || op.IsErr())
        return Value();
    if(IsReal()){
        if(op.IsReal()) return (GetReal()-op.GetReal()); // both real
        else if(op.IsInt()) return (GetReal()-op.GetInt()); // first real second int
    }
    else if(IsInt()){
        if(op.IsReal()) return (GetInt()-op.GetReal());
        else if(op.IsInt()) return (GetInt()-op.GetInt());
    }
    return Value();
}

Value Value::operator*(const Value& op) const{
    if(IsErr() || op.IsErr())
        return Value();
    if(IsReal()){
        if(op.IsReal()) return (GetReal()*op.GetReal()); // both real
        else if(op.IsInt()) return (GetReal()*op.GetInt()); // first real second int
    }
    else if(IsInt()){
        if(op.IsReal()) return (GetInt()*op.GetReal());
        else if(op.IsInt()) return (GetInt()*op.GetInt());
    }
    return Value();
}

Value Value::div(const Value& oper) const{
    if(IsErr() || oper.IsErr()){
        return Value();
    }
    if(IsInt() && oper.IsInt()){
        return (GetInt()/oper.GetInt());
    }
    return Value();
}

Value Value::idiv(const Value& oper) const{
    if(IsErr() || oper.IsErr()){
        return Value();
    }
    if(IsInt() && oper.IsInt()){
        return (GetInt()/oper.GetInt());
    }
    return Value();
}
