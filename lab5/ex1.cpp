#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>
#include <algorithm>

class binary_int
{
private:
    int value_;

    static int add(int a, int b)
    {
        while (b)
        {
            int carry = a & b;
            a = a ^ b;
            b = carry << 1;
        }
        return a;
    }

    static int negate(int a)
    {
        return add(~a, 1);
    }

    static int subtract(int a, int b)
    {
        return add(a, negate(b));
    }

    static int multiply(int a, int b)
    {
        int result = 0;
        bool negative = (a < 0) ^ (b < 0);
        if (a < 0)
            a = negate(a);
        if (b < 0)
            b = negate(b);

        while (b)
        {
            if (b & 1)
            {
                result = add(result, a);
            }
            a <<= 1;
            b >>= 1;
        }
        return negative ? negate(result) : result;
    }

public:
    binary_int(int value = 0) : value_(value) {}

    binary_int(const binary_int &other) : value_(other.value_) {}

    binary_int &operator=(const binary_int &other)
    {
        if (this != &other)
        {
            value_ = other.value_;
        }
        return *this;
    }

    ~binary_int() = default;

    int get_value() const { return value_; }

    binary_int operator-() const
    {
        return binary_int(negate(value_));
    }

    binary_int &operator++()
    {
        value_ = add(value_, 1);
        return *this;
    }

    binary_int operator++(int)
    {
        binary_int temp(*this);
        ++(*this);
        return temp;
    }

    binary_int &operator--()
    {
        value_ = subtract(value_, 1);
        return *this;
    }

    binary_int operator--(int)
    {
        binary_int temp(*this);
        --(*this);
        return temp;
    }

    binary_int &operator+=(const binary_int &other)
    {
        value_ = add(value_, other.value_);
        return *this;
    }

    binary_int operator+(const binary_int &other) const
    {
        return binary_int(add(value_, other.value_));
    }

    binary_int &operator-=(const binary_int &other)
    {
        value_ = subtract(value_, other.value_);
        return *this;
    }

    binary_int operator-(const binary_int &other) const
    {
        return binary_int(subtract(value_, other.value_));
    }

    binary_int &operator*=(const binary_int &other)
    {
        value_ = multiply(value_, other.value_);
        return *this;
    }

    binary_int operator*(const binary_int &other) const
    {
        return binary_int(multiply(value_, other.value_));
    }

    binary_int &operator<<=(const binary_int &other)
    {
        if (other.value_ < 0 || other.value_ >= std::numeric_limits<int>::digits)
        {
            throw std::out_of_range("Shift amount is out of range");
        }
        value_ <<= other.value_;
        return *this;
    }

    binary_int operator<<(const binary_int &other) const
    {
        if (other.value_ < 0 || other.value_ >= std::numeric_limits<int>::digits)
        {
            throw std::out_of_range("Shift amount is out of range");
        }
        return binary_int(value_ << other.value_);
    }

    binary_int &operator>>=(const binary_int &other)
    {
        if (other.value_ < 0 || other.value_ >= std::numeric_limits<int>::digits)
        {
            throw std::out_of_range("Shift amount is out of range");
        }
        value_ >>= other.value_;
        return *this;
    }

    binary_int operator>>(const binary_int &other) const
    {
        if (other.value_ < 0 || other.value_ >= std::numeric_limits<int>::digits)
        {
            throw std::out_of_range("Shift amount is out of range");
        }
        return binary_int(value_ >> other.value_);
    }

    std::pair<binary_int, binary_int> split_bits() const
    {
        int half_bit_count = std::numeric_limits<int>::digits / 2;
        int mask_lower = (1 << half_bit_count) - 1;
        int mask_upper = mask_lower << half_bit_count;

        return std::make_pair(binary_int(value_ & mask_upper),
                              binary_int(value_ & mask_lower));
    }

    friend std::ostream &operator<<(std::ostream &os, const binary_int &obj)
    {
        for (int i = std::numeric_limits<int>::digits - 1; i >= 0; --i)
        {
            os << ((obj.value_ >> i) & 1);
        }
        return os;
    }
};

int main()
{
    try
    {
        binary_int a(10);
        binary_int b(5);

        std::cout << "a: " << a << " (" << a.get_value() << ")" << std::endl;
        std::cout << "b: " << b << " (" << b.get_value() << ")" << std::endl;

        std::cout << "-a: " << -a << " (" << (-a).get_value() << ")" << std::endl;
        std::cout << "++a: " << ++a << " (" << a.get_value() << ")" << std::endl;
        std::cout << "a++: " << a++ << " (" << (a.get_value() - 1) << ")" << std::endl;
        std::cout << "a: " << a << " (" << a.get_value() << ")" << std::endl;

        std::cout << "--b: " << --b << " (" << b.get_value() << ")" << std::endl;
        std::cout << "b--: " << b-- << " (" << (b.get_value() + 1) << ")" << std::endl;
        std::cout << "b: " << b << " (" << b.get_value() << ")" << std::endl;

        std::cout << "a + b: " << a + b << " (" << (a.get_value() + b.get_value()) << ")" << std::endl;
        std::cout << "a - b: " << a - b << " (" << (a.get_value() - b.get_value()) << ")" << std::endl;
        std::cout << "a * b: " << a * b << " (" << (a.get_value() * b.get_value()) << ")" << std::endl;

        binary_int c = a;
        c += b;
        std::cout << "a += b: " << c << " (" << (a.get_value() + b.get_value()) << ")" << std::endl;
        c = a;
        c -= b;
        std::cout << "a -= b: " << c << " (" << (a.get_value() - b.get_value()) << ")" << std::endl;
        c = a;
        c *= b;
        std::cout << "a *= b: " << c << " (" << (a.get_value() * b.get_value()) << ")" << std::endl;

        binary_int shift_val(2);
        binary_int c_sh = a;
        c_sh <<= shift_val;
        std::cout << "a <<= 2: " << c_sh << " (" << (a.get_value() << shift_val.get_value()) << ")" << std::endl;
        c_sh = a;
        std::cout << "a << 2: " << (a << shift_val) << " (" << (a.get_value() << shift_val.get_value()) << ")" << std::endl;
        c_sh = a;
        c_sh >>= shift_val;
        std::cout << "a >>= 2: " << c_sh << " (" << (a.get_value() >> shift_val.get_value()) << ")" << std::endl;
        c_sh = a;
        std::cout << "a >> 2: " << (a >> shift_val) << " (" << (a.get_value() >> shift_val.get_value()) << ")" << std::endl;

        auto splitted = a.split_bits();
        std::cout << "a split high: " << splitted.first << " (" << splitted.first.get_value() << ")"
                  << std::endl;
        std::cout << "a split low: " << splitted.second << " (" << splitted.second.get_value()
                  << ")" << std::endl;

        binary_int big_val(std::numeric_limits<int>::max());
        binary_int shift_big_val(33);
        try
        {
            std::cout << (big_val << shift_big_val);
        }
        catch (const std::out_of_range &e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}