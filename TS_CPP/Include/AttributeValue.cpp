#include "AttributeValue.h"

using namespace TS_CPP;

AttributeValue::AttributeValue(const char* strPtr)
{
    if(strPtr!= nullptr)
    {
        this->data = {.strPtr = strdup(strPtr)};
        this->type = STRING;
    }
    else
    {
        this->data = {.strPtr = nullptr};
        this->type = NOATRIB;
    }

}

AttributeValue::AttributeValue(long long int integer)
{
    this->data = {.integer = integer};
    this->type = INT;
}

AttributeValue::~AttributeValue()
{
    if(this->type == STRING)
    {
        free(this->data.strPtr);
    }
}

bool AttributeValue::operator==(const AttributeValue& other)
{
    if(this->type == other.type)
    {
        if(this->type == STRING && strcmp(this->data.strPtr,other.data.strPtr) == 0)
        {
            return true;
        }
        else
        {
            return this->data.integer == other.data.integer;
        }
    }
    else
    {
        return false;
    }
}

bool AttributeValue::operator!= (const AttributeValue& other)
{
    return ! (*this == other);
}

std::pair<AttributeValue::AttributeType, AttributeValue::DataUnion> AttributeValue::getValue()
{
    return {type,data};
}

bool AttributeValue::isAValidAttributeValue()
{
    return this->type != NOATRIB;
}
