#include <iostream>
#include <stdexcept>
#include <initializer_list>
#include <algorithm>
#include <compare>
#include <iterator>
#include <new>
#include <sstream>

class Vector
{
private:
    double *data_;
    size_t size_;
    size_t capacity_;

    void check_index(size_t index) const
    {
        if (index >= size_)
        {
            throw std::out_of_range("Index out of bounds: " + std::to_string(index) + ", size: " + std::to_string(size_));
        }
    }

    void ensure_capacity(size_t min_capacity)
    {
        if (capacity_ < min_capacity)
        {
            size_t new_capacity = std::max(min_capacity, capacity_ * 2);
            reserve_internal(new_capacity);
        }
    }

    void reserve_internal(size_t num)
    {
        if (num > capacity_)
        {
            double *new_data = nullptr;
            try
            {
                new_data = new double[num];
            }
            catch (const std::bad_alloc &e)
            {
                throw std::runtime_error("Memory allocation failed in reserve_internal: " + std::string(e.what()));
            }
            if (data_)
            {
                std::copy(data_, data_ + size_, new_data);
                delete[] data_;
            }
            data_ = new_data;
            capacity_ = num;
        }
    }

public:
    Vector() : data_(nullptr), size_(0), capacity_(0) {}

    Vector(size_t count, double value) : size_(count), capacity_(count)
    {
        try
        {
            data_ = new double[capacity_];
        }
        catch (const std::bad_alloc &e)
        {
            throw std::runtime_error("Memory allocation failed in Vector(size_t, double): " + std::string(e.what()));
        }
        std::fill(data_, data_ + size_, value);
    }

    Vector(size_t count) : Vector(count, 0.0) {}

    template <typename Iterator>
    Vector(Iterator first, Iterator last)
    {
        size_ = std::distance(first, last);
        capacity_ = size_;
        try
        {
            data_ = new double[capacity_];
        }
        catch (const std::bad_alloc &e)
        {
            throw std::runtime_error("Memory allocation failed in Vector(Iterator, Iterator): " + std::string(e.what()));
        }
        std::copy(first, last, data_);
    }

    Vector(std::initializer_list<double> init) : Vector(init.begin(), init.end()) {}

    Vector(const Vector &other) : size_(other.size_), capacity_(other.capacity_)
    {
        try
        {
            data_ = new double[capacity_];
        }
        catch (const std::bad_alloc &e)
        {
            throw std::runtime_error("Memory allocation failed in copy constructor: " + std::string(e.what()));
        }
        std::copy(other.data_, other.data_ + other.size_, data_);
    }

    Vector(Vector &&other) noexcept : data_(other.data_), size_(other.size_), capacity_(other.capacity_)
    {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    ~Vector()
    {
        delete[] data_;
    }

    Vector &operator=(const Vector &other)
    {
        if (this == &other)
        {
            return *this;
        }
        if (capacity_ < other.size_)
        {
            delete[] data_;
            capacity_ = other.size_;
            try
            {
                data_ = new double[capacity_];
            }
            catch (const std::bad_alloc &e)
            {
                throw std::runtime_error("Memory allocation failed in copy assignment operator: " + std::string(e.what()));
            }
        }
        size_ = other.size_;
        std::copy(other.data_, other.data_ + size_, data_);
        return *this;
    }

    Vector &operator=(Vector &&other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }
        delete[] data_;
        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;

        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
        return *this;
    }

    double &at(size_t index)
    {
        check_index(index);
        return data_[index];
    }

    const double &at(size_t index) const
    {
        check_index(index);
        return data_[index];
    }

    double &front()
    {
        if (empty())
        {
            throw std::underflow_error("front() called on empty vector");
        }
        return data_[0];
    }

    const double &front() const
    {
        if (empty())
        {
            throw std::underflow_error("front() called on empty vector");
        }
        return data_[0];
    }

    double &back()
    {
        if (empty())
        {
            throw std::underflow_error("back() called on empty vector");
        }
        return data_[size_ - 1];
    }

    const double &back() const
    {
        if (empty())
        {
            throw std::underflow_error("back() called on empty vector");
        }
        return data_[size_ - 1];
    }

    double *data()
    {
        return data_;
    }

    const double *data() const
    {
        return data_;
    }

    bool empty() const
    {
        return size_ == 0;
    }

    size_t size() const
    {
        return size_;
    }

    void reserve(size_t num)
    {
        reserve_internal(num);
    }

    size_t capacity() const
    {
        return capacity_;
    }

    void shrink_to_fit()
    {
        if (capacity_ > size_)
        {
            if (size_ == 0)
            {
                delete[] data_;
                data_ = nullptr;
                capacity_ = 0;
            }
            else
            {
                double *new_data = nullptr;
                try
                {
                    new_data = new double[size_];
                }
                catch (const std::bad_alloc &e)
                {
                    throw std::runtime_error("Memory allocation failed in shrink_to_fit: " + std::string(e.what()));
                }
                std::copy(data_, data_ + size_, new_data);
                delete[] data_;
                data_ = new_data;
                capacity_ = size_;
            }
        }
    }

    void clear()
    {
        size_ = 0;
    }

    void insert(size_t index, double elem)
    {
        if (index > size_)
        {
            throw std::out_of_range("Index out of bounds for insert: " + std::to_string(index) + ", size: " + std::to_string(size_));
        }
        ensure_capacity(size_ + 1);
        std::move_backward(data_ + index, data_ + size_, data_ + size_ + 1);
        data_[index] = elem;
        ++size_;
    }

    void erase(size_t index)
    {
        check_index(index);
        std::move(data_ + index + 1, data_ + size_, data_ + index);
        --size_;
    }

    void push_back(double elem)
    {
        ensure_capacity(size_ + 1);
        data_[size_++] = elem;
    }

    void pop_back()
    {
        if (empty())
        {
            throw std::underflow_error("pop_back() called on empty vector");
        }
        --size_;
    }

    void resize(size_t new_size, double elem = 0.0)
    {
        if (new_size > capacity_)
        {
            reserve_internal(new_size);
        }
        if (new_size > size_)
        {
            std::fill(data_ + size_, data_ + new_size, elem);
        }
        size_ = new_size;
    }

    bool operator==(const Vector &other) const
    {
        return size_ == other.size_ && std::equal(data_, data_ + size_, other.data_);
    }

    bool operator!=(const Vector &other) const
    {
        return !(*this == other);
    }

    bool operator<(const Vector &other) const
    {
        return std::lexicographical_compare(data_, data_ + size_, other.data_, other.data_ + other.size_);
    }

    bool operator<=(const Vector &other) const
    {
        return (*this < other) || (*this == other);
    }

    bool operator>(const Vector &other) const
    {
        return !(*this <= other);
    }

    bool operator>=(const Vector &other) const
    {
        return !(*this < other);
    }

    class iterator
    {
    private:
        double *ptr_;

    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = double;
        using difference_type = std::ptrdiff_t;
        using pointer = double *;
        using reference = double &;

        iterator(double *ptr) : ptr_(ptr) {}

        reference operator*() const
        {
            return *ptr_;
        }

        pointer operator->() const
        {
            return ptr_;
        }

        iterator &operator++()
        {
            ++ptr_;
            return *this;
        }

        iterator operator++(int)
        {
            iterator temp = *this;
            ++ptr_;
            return temp;
        }
        iterator &operator--()
        {
            --ptr_;
            return *this;
        }

        iterator operator--(int)
        {
            iterator temp = *this;
            --ptr_;
            return temp;
        }

        iterator operator+(difference_type offset) const
        {
            return iterator(ptr_ + offset);
        }

        iterator operator-(difference_type offset) const
        {
            return iterator(ptr_ - offset);
        }

        difference_type operator-(const iterator &other) const
        {
            return ptr_ - other.ptr_;
        }

        iterator &operator+=(difference_type offset)
        {
            ptr_ += offset;
            return *this;
        }

        iterator &operator-=(difference_type offset)
        {
            ptr_ -= offset;
            return *this;
        }

        reference operator[](difference_type offset) const
        {
            return *(ptr_ + offset);
        }

        bool operator==(const iterator &other) const
        {
            return ptr_ == other.ptr_;
        }

        bool operator!=(const iterator &other) const
        {
            return ptr_ != other.ptr_;
        }

        bool operator<(const iterator &other) const
        {
            return ptr_ < other.ptr_;
        }

        bool operator<=(const iterator &other) const
        {
            return ptr_ <= other.ptr_;
        }

        bool operator>(const iterator &other) const
        {
            return ptr_ > other.ptr_;
        }

        bool operator>=(const iterator &other) const
        {
            return ptr_ >= other.ptr_;
        }
    };

    iterator begin()
    {
        return iterator(data_);
    }

    iterator end()
    {
        return iterator(data_ + size_);
    }

    iterator begin() const
    {
        return iterator(data_);
    }

    iterator end() const
    {
        return iterator(data_ + size_);
    }
};

int main()
{
    try
    {
        Vector v = {1.0, 2.0, 3.0};

        std::cout << "Examples of using at():" << std::endl;

        std::cout << v.at(0) << std::endl;
        std::cout << v.at(1) << std::endl;
        std::cout << v.at(2) << std::endl;

        std::cout << "Example of using front(): " << v.front() << std::endl;

        Vector a = {};

        std::cout << "Example of using back(): " << v.back() << std::endl;

        std::cout << "Example of using data(): " << v.data() << std::endl;

        std::cout << "Example of using empty(): " << v.empty() << " " << a.empty() << std::endl;

        std::cout << "Example of using size(): " << v.size() << " " << a.size() << std::endl;

        std::cout << "Example of using capacity() before reserve(10): " << v.capacity() << std::endl;
        v.reserve(10);
        std::cout << "Example of using capacity() after reserve(10): " << v.capacity() << std::endl;
        v.reserve(3);
        std::cout << "Example of using capacity() after reserve(3): " << v.capacity() << std::endl;
        v.shrink_to_fit();
        std::cout << "Example of using capacity() after shrink_to_fit(): " << v.capacity() << std::endl;

        v.clear();
        std::cout << "Example of using size() after using v.clear(): " << v.size() << " " << a.size() << std::endl;

        v.push_back(4.0);
        v.insert(1, 5.0);
        v.insert(0, 1.0);
        v.insert(2, 2.0);
        v.insert(3, 3.0);
        v.erase(2);
        v.pop_back();
        v.resize(10);
        v.resize(11, 123);

        std::cout << "Vector v elements: ";
        for (const auto &elem : v)
        {
            std::cout << elem << " ";
        }
        std::cout << std::endl
                  << std::endl;

        std::cout << "Example of comparing for equality (v == a): " << (v == a) << std::endl;
        std::cout << "Example of comparing for non-equality (v != a): " << (v != a) << std::endl;
        std::cout << "Example of comparing for > (v > a): " << (v > a) << std::endl;
        std::cout << "Example of comparing for <= (v <= a): " << (v <= a) << std::endl;
        std::cout << "Example of comparing for < (a < v): " << (a < v) << std::endl;
        std::cout << "Example of comparing for >= (a >= v): " << (a >= v) << std::endl;

        Vector v2 = v;
        std::cout << "Vector v2 (copy of v) elements: ";
        for (const auto &elem : v2)
        {
            std::cout << elem << " ";
        }
        std::cout << std::endl;

        Vector v3;
        v3 = v;
        std::cout << "Vector v3 (copy assigned from v) elements: ";
        for (const auto &elem : v3)
        {
            std::cout << elem << " ";
        }
        std::cout << std::endl;

        Vector v4 = std::move(v);
        std::cout << "Vector v4 (move constructed from v) elements: ";
        for (const auto &elem : v4)
        {
            std::cout << elem << " ";
        }
        std::cout << std::endl;
        std::cout << "Vector v (after move construction) size: " << v.size() << std::endl;

        Vector v5;
        v5 = std::move(v2);
        std::cout << "Vector v5 (move assigned from v2) elements: ";
        for (const auto &elem : v5)
        {
            std::cout << elem << " ";
        }
        std::cout << std::endl;
        std::cout << "Vector v2 (after move assignment) size: " << v2.size() << std::endl;

        return 0;
    }
    catch (const std::out_of_range &e)
    {
        std::cerr << "Out of range error: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::underflow_error &e)
    {
        std::cerr << "Underflow error: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "Runtime error: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}