#include <string>
#include <iostream>
#include <vector>
#include <memory>

class Visitor;
class Book;
class Toy;
class Food;

class Product {
public:
    Product(std::string n, double c): name(n), cost(c) {}
    virtual ~Product() = default;
    virtual void accept(Visitor& visitor) = 0;
    std::string getName(){
        return name;
    }
    double getCost(){
        return cost;
    }
protected:
    std::string name;
    double cost;
};

class Visitor {
public:
        virtual ~Visitor() = default;
        virtual void visit(Book& book) = 0;
        virtual void visit(Toy& toy) = 0;
        virtual void visit(Food& food) = 0;                
};

class Book : public Product{
protected:
    std::string author;
public:
    Book(std::string n, double c, std::string a): Product(n, c), author(a) {}
    std::string getAuthor(){
        return author;
    }
    void accept(Visitor& visitor) override {
        visitor.visit(*this);
    }
};

class Toy : public Product{
protected:
    int minAge;
public:
    Toy(std::string n, double c, int m): Product(n, c), minAge(m) {}
    int getMinAge(){
        return minAge;
    }
    void accept(Visitor& visitor) override {
        visitor.visit(*this);
    }
};

class Food : public Product{
protected:
    int expirationDays;
public:
    Food(std::string n, double c, int d): Product(n, c), expirationDays(d) {}
    int getExpirationDays(){
        return expirationDays;
    }
    void accept(Visitor& visitor) override {
        visitor.visit(*this);
    }
};

class PrintVisitor : public Visitor{
public:
    void visit(Book& book) override {
        std::cout << "Назавание книги - " << book.getName() << std::endl;
        std::cout << "Автор книги - " << book.getAuthor() << std::endl;
        std::cout << "Стоимость книги - " << book.getCost() << std::endl;
    }
    void visit(Toy& toy) override {
        std::cout << "Назавание игрушки - " << toy.getName() << std::endl;
        std::cout << "Минимальный возраст игрушки - " << toy.getMinAge() << std::endl;
        std::cout << "Стоимость игрушки - " << toy.getCost() << std::endl;
    }
    void visit(Food& food) override {
        std::cout << "Назавание игрушки - " << food.getName() << std::endl;
        std::cout << "Минимальный возраст игрушки - " << food.getExpirationDays() << std::endl;
        std::cout << "Стоимость игрушки - " << food.getCost() << std::endl;
    }
};

class DiscountVisitor : public Visitor {
public:
    double getDiscount(){
        return discountCost;
    }
    void visit(Book& book) override {
        discountCost = book.getCost() * 0.9;
    }
    void visit(Toy& toy) override {
        discountCost = toy.getCost() * 0.95;
    }
    void visit(Food& food) override {
        discountCost = food.getCost() * 0.8;
    }
private:
    double discountCost;
};

int main(int argc, char const *argv[])
{
    std::vector<std::shared_ptr<Product>> products;
    products.push_back(std::make_shared<Book>("Мастер и маргарита", 900, "Булгаков"));
    products.push_back(std::make_shared<Toy>("Юла", 1500, 2));
    products.push_back(std::make_shared<Food>("Котлета", 250, 10));

    PrintVisitor printVisitor;
    DiscountVisitor discountVisitor;

    for (std::shared_ptr<Product> product : products){
        product->accept(printVisitor);
        product->accept(discountVisitor);

        std::cout << "С учётом скидки - " << discountVisitor.getDiscount() << std::endl;
    }
    return 0;
}
