#include <iostream>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <iomanip>

class Complex
{
private:
    double real;
    double imag;

public:
    Complex(double real = 0.0, double imag = 0.0) : real(real), imag(imag) {}

    Complex(const Complex &other) : real(other.real), imag(other.imag) {}

    ~Complex() = default;

    Complex &operator=(const Complex &other)
    {
        if (this != &other)
        {
            real = other.real;
            imag = other.imag;
        }
        return *this;
    }

    Complex add(const Complex &other) const
    {
        return Complex(real + other.real, imag + other.imag);
    }

    Complex subtract(const Complex &other) const
    {
        return Complex(real - other.real, imag - other.imag);
    }

    Complex multiply(const Complex &other) const
    {
        return Complex(real * other.real - imag * other.imag,
                       real * other.imag + imag * other.real);
    }

    Complex divide(const Complex &other, double epsilon = std::numeric_limits<double>::epsilon()) const
    {
        double denominator = other.real * other.real + other.imag * other.imag;
        if (std::abs(denominator) < epsilon)
        {
            throw std::runtime_error("Division by zero or near-zero value");
        }
        return Complex((real * other.real + imag * other.imag) / denominator,
                       (imag * other.real - real * other.imag) / denominator);
    }

    double magnitude() const
    {
        return std::sqrt(real * real + imag * imag);
    }

    double argument(double epsilon = std::numeric_limits<double>::epsilon()) const
    {
        if (std::abs(real) < epsilon && std::abs(imag) < epsilon)
        {
            return 0.0;
        }
        return std::atan2(imag, real);
    }

    double getReal() const { return real; }
    double getImag() const { return imag; }

    friend std::ostream &operator<<(std::ostream &os, const Complex &c);
};

std::ostream &operator<<(std::ostream &os, const Complex &c)
{
    os << std::fixed << std::setprecision(2);
    os << c.real;
    if (c.imag >= 0)
    {
        os << "+";
    }
    os << c.imag << "i";
    return os;
}

int demonstrateComplexOperations()
{
    Complex c1(2.0, 3.0);
    Complex c2(4.0, -1.0);

    std::cout << "c1 = " << c1 << std::endl;
    std::cout << "c2 = " << c2 << std::endl;

    std::cout << "c1 + c2 = " << c1.add(c2) << std::endl;
    std::cout << "c1 - c2 = " << c1.subtract(c2) << std::endl;
    std::cout << "c1 * c2 = " << c1.multiply(c2) << std::endl;

    try
    {
        std::cout << "c1 / c2 = " << c1.divide(c2) << std::endl;
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "Error during division: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "|c1| = " << c1.magnitude() << std::endl;
    std::cout << "arg(c1) = " << c1.argument() << " radians" << std::endl;

    return 0;
}

int main()
{
    int result = demonstrateComplexOperations();
    if (result != 0)
    {
        return result;
    }
    return 0;
}