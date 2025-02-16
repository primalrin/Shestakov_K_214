#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <map>
#include <stdexcept>

using namespace std;

class Product
{
private:
    string name;
    int id;
    double weight;
    double price;
    int shelfLife;

public:
    Product(const string &name, int id, double weight, double price, int shelfLife)
        : name(name), id(id), weight(weight), price(price), shelfLife(shelfLife)
    {
        if (name.empty())
        {
            throw invalid_argument("Product name cannot be empty.");
        }
        if (id <= 0)
        {
            throw invalid_argument("Product ID must be positive.");
        }
        if (weight < 0)
        {
            throw invalid_argument("Product weight cannot be negative.");
        }
        if (price < 0)
        {
            throw invalid_argument("Product price cannot be negative.");
        }
        if (shelfLife < 0)
        {
            throw invalid_argument("Product shelf life cannot be negative.");
        }
    }

    Product(const Product &other)
        : name(other.name), id(other.id), weight(other.weight), price(other.price), shelfLife(other.shelfLife) {}

    virtual ~Product() = default;

    Product &operator=(const Product &other)
    {
        if (this != &other)
        {
            name = other.name;
            id = other.id;
            weight = other.weight;
            price = other.price;
            shelfLife = other.shelfLife;
        }
        return *this;
    }

    virtual void displayInfo() const
    {
        cout << "Name: " << name
             << ", ID: " << id
             << ", Weight: " << fixed << setprecision(2) << weight << "kg"
             << ", Price: $" << fixed << setprecision(2) << price
             << ", Shelf Life: " << shelfLife << " days" << endl;
    }

    virtual double calculateStorageFee() const
    {
        return weight * 0.1;
    }

    int getId() const
    {
        return id;
    }
    string getName() const
    {
        return name;
    }
    double getWeight() const
    {
        return weight;
    }
    double getPrice() const
    {
        return price;
    }
    int getShelfLife() const
    {
        return shelfLife;
    }
};

class PerishableProduct : public Product
{
private:
    time_t expirationDate;

public:
    PerishableProduct(const string &name, int id, double weight, double price, int shelfLife, time_t expirationDate)
        : Product(name, id, weight, price, shelfLife), expirationDate(expirationDate)
    {
        if (expirationDate <= time(0))
        {
            throw invalid_argument("Expiration date must be in the future.");
        }
    }

    PerishableProduct(const PerishableProduct &other)
        : Product(other), expirationDate(other.expirationDate) {}

    PerishableProduct &operator=(const PerishableProduct &other)
    {
        if (this != &other)
        {
            Product::operator=(other);
            expirationDate = other.expirationDate;
        }
        return *this;
    }
    ~PerishableProduct() override = default;

    double calculateStorageFee() const override
    {
        time_t now = time(0);
        double daysToExpire = difftime(expirationDate, now) / (60 * 60 * 24);
        if (daysToExpire < 0)
            daysToExpire = 0;
        return getWeight() * (0.2 + (1.0 / (daysToExpire + 1)));
    }

    void displayInfo() const override
    {
        Product::displayInfo();
        tm *expiration_tm = localtime(&expirationDate);
        if (expiration_tm == nullptr)
        {
            cerr << "Error converting expiration date to local time." << endl;
            return;
        }
        char buffer[26];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", expiration_tm);
        cout << "Expiration Date: " << buffer << endl;
    }

    time_t getExpirationDate() const
    {
        return expirationDate;
    }
};

class BuildingMaterial : public Product
{
private:
    bool flammable;

public:
    BuildingMaterial(const string &name, int id, double weight, double price, int shelfLife, bool flammable)
        : Product(name, id, weight, price, shelfLife), flammable(flammable) {}

    BuildingMaterial(const BuildingMaterial &other)
        : Product(other), flammable(other.flammable) {}

    BuildingMaterial &operator=(const BuildingMaterial &other)
    {
        if (this != &other)
        {
            Product::operator=(other);
            flammable = other.flammable;
        }
        return *this;
    }
    ~BuildingMaterial() override = default;

    double calculateStorageFee() const override
    {
        return getWeight() * (flammable ? 0.5 : 0.2);
    }

    void displayInfo() const override
    {
        Product::displayInfo();
        cout << "Flammable: " << (flammable ? "Yes" : "No") << endl;
    }

    bool isFlammable() const
    {
        return flammable;
    }
};

class ElectronicProduct : public Product
{
private:
    int warrantyPeriod;
    double powerRating;

public:
    ElectronicProduct(const string &name, int id, double weight, double price, int shelfLife, int warrantyPeriod, double powerRating)
        : Product(name, id, weight, price, shelfLife), warrantyPeriod(warrantyPeriod), powerRating(powerRating)
    {
        if (warrantyPeriod < 0)
        {
            throw invalid_argument("Warranty period cannot be negative.");
        }
        if (powerRating < 0)
        {
            throw invalid_argument("Power rating cannot be negative.");
        }
    }

    ElectronicProduct(const ElectronicProduct &other)
        : Product(other), warrantyPeriod(other.warrantyPeriod), powerRating(other.powerRating) {}

    ElectronicProduct &operator=(const ElectronicProduct &other)
    {
        if (this != &other)
        {
            Product::operator=(other);
            warrantyPeriod = other.warrantyPeriod;
            powerRating = other.powerRating;
        }
        return *this;
    }
    ~ElectronicProduct() override = default;

    void displayInfo() const override
    {
        Product::displayInfo();
        cout << "Warranty Period: " << warrantyPeriod << " months"
             << ", Power Rating: " << fixed << setprecision(2) << powerRating << "W" << endl;
    }

    int getWarrantyPeriod() const
    {
        return warrantyPeriod;
    }
    double getPowerRating() const
    {
        return powerRating;
    }
};

class Warehouse
{
private:
    vector<shared_ptr<Product>> inventory;

public:
    Warehouse() = default;
    Warehouse(const Warehouse &other) : inventory(other.inventory) {}
    Warehouse &operator=(const Warehouse &other)
    {
        if (this != &other)
        {
            inventory = other.inventory;
        }
        return *this;
    }
    ~Warehouse() = default;

    void addProduct(shared_ptr<Product> product)
    {
        if (!product)
        {
            throw invalid_argument("Product pointer is null.");
        }
        inventory.push_back(product);
    }

    void removeProduct(int id)
    {
        if (id <= 0)
        {
            throw invalid_argument("Product ID for removal must be positive.");
        }
        inventory.erase(remove_if(inventory.begin(), inventory.end(),
                                  [id](const shared_ptr<Product> &product)
                                  {
                                      return product->getId() == id;
                                  }),
                        inventory.end());
    }

    shared_ptr<Product> findProduct(int id) const
    {
        if (id <= 0)
        {
            throw invalid_argument("Product ID for finding must be positive.");
        }
        for (const auto &product : inventory)
        {
            if (product->getId() == id)
                return product;
        }
        return nullptr;
    }

    vector<shared_ptr<Product>> getExpiringProducts(int days) const
    {
        if (days < 0)
        {
            throw invalid_argument("Days must be non-negative.");
        }
        vector<shared_ptr<Product>> expiring;
        time_t now = time(0);
        for (const auto &product : inventory)
        {
            auto perishable = dynamic_pointer_cast<PerishableProduct>(product);
            if (perishable)
            {
                double remainingDays = difftime(perishable->getExpirationDate(), now) / (60 * 60 * 24);
                if (remainingDays <= days && remainingDays >= 0)
                {
                    expiring.push_back(perishable);
                }
            }
        }
        return expiring;
    }

    void displayInventory() const
    {
        map<string, vector<shared_ptr<Product>>> categories;
        for (const auto &product : inventory)
        {
            if (dynamic_pointer_cast<PerishableProduct>(product))
            {
                categories["Perishable Products"].push_back(product);
            }
            else if (dynamic_pointer_cast<ElectronicProduct>(product))
            {
                categories["Electronic Products"].push_back(product);
            }
            else if (dynamic_pointer_cast<BuildingMaterial>(product))
            {
                categories["Building Materials"].push_back(product);
            }
            else
            {
                categories["Other Products"].push_back(product);
            }
        }

        for (const auto &[category, products] : categories)
        {
            cout << "Category: " << category << endl;
            for (const auto &product : products)
            {
                product->displayInfo();
            }
        }
    }

    Warehouse &operator+=(shared_ptr<Product> product)
    {
        addProduct(product);
        return *this;
    }

    Warehouse &operator-=(int id)
    {
        removeProduct(id);
        return *this;
    }

    shared_ptr<Product> operator[](int id) const
    {
        return findProduct(id);
    }

    friend ostream &operator<<(ostream &os, const Warehouse &warehouse)
    {
        os << "Warehouse Inventory:" << endl;
        warehouse.displayInventory();
        os << "Total Storage Fee: $" << fixed << setprecision(2) << warehouse.calculateTotalStorageFee() << endl;
        return os;
    }

private:
    double calculateTotalStorageFee() const
    {
        double total = 0;
        for (const auto &product : inventory)
        {
            total += product->calculateStorageFee();
        }
        return total;
    }
};

int main()
{
    Warehouse warehouse;

    time_t now = time(0);
    time_t expiration = now + 5 * 24 * 60 * 60;

    try
    {
        warehouse += make_shared<PerishableProduct>("Milk", 1, 2.0, 1.5, 7, expiration);
        warehouse += make_shared<ElectronicProduct>("Laptop", 2, 3.0, 1500.0, 0, 24, 65.0);
        warehouse += make_shared<BuildingMaterial>("Bricks", 3, 100.0, 500.0, 0, true);

        cout << warehouse << endl
             << "-----------------------------------------" << endl;

        auto expiringProducts = warehouse.getExpiringProducts(7);
        cout << "Expiring Products within 7 days:" << endl;
        for (const auto &product : expiringProducts)
        {
            product->displayInfo();
        }

        cout << endl
             << "-----------------------------------------" << endl;

        warehouse -= 1;
        cout << "Warehouse after removing product ID 1:" << endl;
        cout << warehouse;

        cout << endl
             << "-----------------------------------------" << endl;
        cout << "Product with ID 2: " << endl;
        shared_ptr<Product> product2 = warehouse[2];
        if (product2)
        {
            product2->displayInfo();
        }
        else
        {
            cout << "Product with ID 2 not found." << endl;
        }

        cout << endl
             << "-----------------------------------------" << endl;
        cout << "Product with ID 1 (after removal): " << endl;
        shared_ptr<Product> product1 = warehouse[1];
        if (product1)
        {
            product1->displayInfo();
        }
        else
        {
            cout << "Product with ID 1 not found." << endl;
        }
    }
    catch (const invalid_argument &e)
    {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    catch (const exception &e)
    {
        cerr << "Unexpected error: " << e.what() << endl;
        return 2;
    }

    return 0;
}