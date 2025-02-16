#include <iostream>
#include <string>
#include <algorithm>
#include <limits>
#include <vector>

using namespace std;

enum class ret_type_t : int
{
    SUCCESS = 0,
    ERROR_MALLOC = -1,
    ERROR_ARGUMENTS = -2,
    ERROR_INVALID_VALUE = -3
};

void logError(ret_type_t code)
{
    switch (code)
    {
    case ret_type_t::ERROR_MALLOC:
    {
        cerr << "Error: Failed to allocate memory." << endl;
        break;
    }
    case ret_type_t::ERROR_ARGUMENTS:
    {
        cerr << "Error: Wrong number of arguments provided." << endl;
        break;
    }
    case ret_type_t::ERROR_INVALID_VALUE:
    {
        cerr << "Error: Invalid input value." << endl;
        break;
    }
    default:
    {
        cerr << "Error: Unknown error." << endl;
        break;
    }
    }
}

unsigned int ultimateInc(unsigned int n)
{
    unsigned int mask = 1;
    while (n & mask)
    {
        n ^= mask;
        mask <<= 1;
    }
    n ^= mask;
    return n;
}

unsigned int ultimateDec(unsigned int n)
{
    unsigned int mask = 1;
    while (!(n & mask))
    {
        n ^= mask;
        mask <<= 1;
    }
    n ^= mask;
    return n;
}

class logical_values_array
{
private:
    unsigned int value;

public:
    logical_values_array() : value(0) {}

    logical_values_array(unsigned int val) : value(val) {}

    logical_values_array(const logical_values_array &other) : value(other.value)
    {
    }

    ~logical_values_array()
    {
    }

    logical_values_array &operator=(const logical_values_array &other)
    {
        if (this != &other)
        {
            value = other.value;
        }
        return *this;
    }

    unsigned int get_value() const
    {
        return value;
    }

    void set_value(const unsigned int val)
    {
        value = val;
    }

    logical_values_array operator~() const
    {
        return logical_values_array(~value);
    }

    logical_values_array operator&(const logical_values_array &other) const
    {
        return logical_values_array(value & other.value);
    }

    logical_values_array operator|(const logical_values_array &other) const
    {
        return logical_values_array(value | other.value);
    }

    logical_values_array implication(const logical_values_array &other) const
    {
        return ~(*this) | other;
    }

    logical_values_array coimplication(const logical_values_array &other) const
    {
        return ~(*this ^ other);
    }

    logical_values_array operator^(const logical_values_array &other) const
    {
        return logical_values_array((value & ~other.value) | (~value & other.value));
    }

    logical_values_array equalant(const logical_values_array &other) const
    {
        return ~(*this ^ other);
    }

    logical_values_array pearce(const logical_values_array &other) const
    {
        return ~(*this | other);
    }

    logical_values_array sheffer(const logical_values_array &other) const
    {
        return ~(*this & other);
    }

    static bool equals(const logical_values_array &a, const logical_values_array &b)
    {
        return a.value == b.value;
    }

    bool get_bit(const int position) const
    {
        if (position < 0)
        {
            return false;
        }
        return (value >> position) & 1;
    }

    ret_type_t to_binary_string(char *destiny) const
    {
        if (destiny == nullptr)
        {
            return ret_type_t::ERROR_INVALID_VALUE;
        }
        unsigned int number = value;
        vector<char> buffer;

        if (number == 0)
        {
            buffer.push_back('0');
        }
        else
        {
            while (number > 0)
            {
                buffer.push_back((number & 1) ? '1' : '0');
                number >>= 1;
            }
            reverse(buffer.begin(), buffer.end());
        }

        if (buffer.size() + 1 > numeric_limits<ptrdiff_t>::max())
        {
            return ret_type_t::ERROR_MALLOC;
        }

        for (size_t i = 0; i < buffer.size(); ++i)
        {
            destiny[i] = buffer[i];
        }
        destiny[buffer.size()] = '\0';

        return ret_type_t::SUCCESS;
    }
};

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        logError(ret_type_t::ERROR_ARGUMENTS);
        cerr << "Usage: " << argv[0] << " (no arguments expected)" << endl;
        return static_cast<int>(ret_type_t::ERROR_ARGUMENTS);
    }

    logical_values_array a(5);
    logical_values_array b(3);

    cout << "a: " << a.get_value() << endl;
    cout << "b: " << b.get_value() << endl;

    cout << "a & b: " << (a & b).get_value() << endl;
    cout << "a | b: " << (a | b).get_value() << endl;
    cout << "a ^ b: " << (a ^ b).get_value() << endl;
    cout << "~a: " << (~a).get_value() << endl;
    cout << "a -> b: " << a.implication(b).get_value() << endl;
    cout << "a <-> b: " << a.coimplication(b).get_value() << endl;
    cout << "a == b: " << a.equalant(b).get_value() << endl;
    cout << "a ↓ b: " << a.pearce(b).get_value() << endl;
    cout << "a | b: " << a.sheffer(b).get_value() << endl;

    cout << "Bit 0 of a: " << a.get_bit(0) << endl;
    cout << "Bit 1 of a: " << a.get_bit(1) << endl;
    cout << "Bit 2 of a: " << a.get_bit(2) << endl;

    cout << "Bit 0 of b: " << b.get_bit(0) << endl;
    cout << "Bit 1 of b: " << b.get_bit(1) << endl;

    size_t max_binary_str_len = sizeof(unsigned int) * 8 + 1;
    char *binary_str = new char[max_binary_str_len];
    if (!binary_str)
    {
        logError(ret_type_t::ERROR_MALLOC);
        return static_cast<int>(ret_type_t::ERROR_MALLOC);
    }

    ret_type_t code = a.to_binary_string(binary_str);
    if (code != ret_type_t::SUCCESS)
    {
        logError(code);
        delete[] binary_str;
        return static_cast<int>(code);
    }
    cout << "Binary representation of a: " << binary_str << endl;
    delete[] binary_str;

    logical_values_array c(5);
    cout << "a == c: " << logical_values_array::equals(a, c) << endl;
    cout << "a == b: " << logical_values_array::equals(a, b) << endl;

    return static_cast<int>(ret_type_t::SUCCESS);
}