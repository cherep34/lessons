#include <iostream>
#include <mutex>       
#include <thread>      
#include <cstddef> 

//тип Т
template <typename T>
class Set {
private:
    //состояния для цвета
    enum Color { RED, BLACK };
    
    // узел
    struct Node {
        T data;
        Node* parent;
        Node* left;
        Node* right;
        Color color;

        //конструктор узла с заданным значением
        Node(const T& d) 
            : data(d), parent(nullptr), left(nullptr), right(nullptr), color(RED) {}
    };

    Node* root; //корень
    Node* nil;  //ноль
    mutable std::mutex mtx;

    void left_rotate(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        
        if (y->left != nil) {
            y->left->parent = x;
        }
        
        y->parent = x->parent;
        
        if (x->parent == nil) {
            root = y;
        } else if (x == x->parent->left) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
        }
        
        y->left = x;
        x->parent = y;
    }

    void right_rotate(Node* x) {
        Node* y = x->left;
        x->left = y->right;
        
        if (y->right != nil) {
            y->right->parent = x;
        }
        
        y->parent = x->parent;
        
        if (x->parent == nil) {
            root = y;
        } else if (x == x->parent->right) {
            x->parent->right = y;
        } else {
            x->parent->left = y;
        }
        
        y->right = x;
        x->parent = y;
    }

    //восстановление после вставки
    void fix_insert(Node* k) {
        while (k != root && k->parent->color == RED) {
            if (k->parent == k->parent->parent->left) {
                Node* uncle = k->parent->parent->right;
                
                if (uncle->color == RED) {
                    k->parent->color = BLACK;
                    uncle->color = BLACK;
                    k->parent->parent->color = RED;
                    k = k->parent->parent;
                } else {
                    if (k == k->parent->right) {
                        k = k->parent;
                        left_rotate(k);
                    }
                    
                    k->parent->color = BLACK;
                    k->parent->parent->color = RED;
                    right_rotate(k->parent->parent);
                }
            } else {
                Node* uncle = k->parent->parent->left;
                
                if (uncle->color == RED) {
                    k->parent->color = BLACK;
                    uncle->color = BLACK;
                    k->parent->parent->color = RED;
                    k = k->parent->parent;
                } else {
                    if (k == k->parent->left) {
                        k = k->parent;
                        right_rotate(k);
                    }
                    
                    k->parent->color = BLACK;
                    k->parent->parent->color = RED;
                    left_rotate(k->parent->parent);
                }
            }
        }
        root->color = BLACK;
    }

    Node* minimum(Node* node) const {
        while (node->left != nil) {
            node = node->left;
        }
        return node;
    }

    void clear(Node* node) {
        if (node == nil) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

    Node* find_node(const T& value) const {
        Node* current = root;
        while (current != nil) {
            if (value < current->data) {
                current = current->left;
            } else if (current->data < value) {
                current = current->right;
            } else {
                return current;
            }
        }
        return nil;
    }

    //заменяем u на v, и теперь v на месте u
    void transplant(Node* u, Node* v) {
        if (u->parent == nil) {
            root = v;
        } else if (u == u->parent->left) {
            u->parent->left = v;
        } else {
            u->parent->right = v;
        }
        v->parent = u->parent;
    }

    void delete_node(Node* z) {
        Node* y = z;
        Node* x;
        Color y_original_color = y->color;

        if (z->left == nil) {
            x = z->right;
            transplant(z, z->right);
        } else if (z->right == nil) {
            x = z->left;
            transplant(z, z->left);
        } else {
            y = minimum(z->right);
            y_original_color = y->color;
            x = y->right;
            
            if (y->parent == z) {
                x->parent = y;
            } else {
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }

        if (y_original_color == BLACK) {
            fix_delete(x);
        }
        delete z;
    }

    void fix_delete(Node* x) {
        while (x != root && x->color == BLACK) {
            if (x == x->parent->left) {
                Node* w = x->parent->right;
                if (w->color == RED) {
                    w->color = BLACK;
                    x->parent->color = RED;
                    left_rotate(x->parent);
                    w = x->parent->right;
                }
                if (w->left->color == BLACK && w->right->color == BLACK) {
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (w->right->color == BLACK) {
                        w->left->color = BLACK;
                        w->color = RED;
                        right_rotate(w);
                        w = x->parent->right;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->right->color = BLACK;
                    left_rotate(x->parent);
                    x = root;
                }
            } else {
                Node* w = x->parent->left;
                if (w->color == RED) {
                    w->color = BLACK;
                    x->parent->color = RED;
                    right_rotate(x->parent);
                    w = x->parent->left;
                }
                if (w->right->color == BLACK && w->left->color == BLACK) {
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (w->left->color == BLACK) {
                        w->right->color = BLACK;
                        w->color = RED;
                        left_rotate(w);
                        w = x->parent->left;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->left->color = BLACK;
                    right_rotate(x->parent);
                    x = root;
                }
            }
        }
        x->color = BLACK;
    }


public:
    //итераторы
    class Iterator {
    private:
        Node* node;
        Node* nil;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

        Iterator(Node* n, Node* nil_node) : node(n), nil(nil_node) {}

        reference operator*() const { 
            return node->data; 
        }

        pointer operator->() const { 
            return &node->data; 
        }

        Iterator& operator++() {
            if (node->right != nil) {
                node = node->right;
                while (node->left != nil) {
                    node = node->left;
                }
            } else {
                Node* parent = node->parent;
                while (parent != nil && node == parent->right) {
                    node = parent;
                    parent = parent->parent;
                }
                node = parent;
            }
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const Iterator& other) const {
            return node == other.node;
        }

        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }
    };

    Set() {
        nil = new Node(T());
        nil->color = BLACK;
        nil->parent = nil->left = nil->right = nil;
        root = nil;
    }

    ~Set() {
        std::lock_guard<std::mutex> lock(mtx);
        clear(root);
        delete nil;
    }

    void insert(const T& value) {
        std::lock_guard<std::mutex> lock(mtx);
        Node* y = nil;
        Node* x = root;
        
        //поиск места
        while (x != nil) {
            y = x;
            if (value < x->data) {
                x = x->left;
            } else if (x->data < value) {
                x = x->right;
            } else {
                return;
            }
        }

        //новый узел
        Node* new_node = new Node(value);
        new_node->parent = y;
        new_node->left = nil;
        new_node->right = nil;
        new_node->color = RED;

        //привязка узла
        if (y == nil) {
            root = new_node;
        } else if (value < y->data) {
            y->left = new_node;
        } else {
            y->right = new_node;
        }

        fix_insert(new_node);
    }

    void erase(const T& value) {
        std::lock_guard<std::mutex> lock(mtx);
        Node* z = find_node(value);
        if (z != nil) {
            delete_node(z);
        }
    }

    bool contains(const T& value) const {
        std::lock_guard<std::mutex> lock(mtx);
        return find_node(value) != nil;
    }

    Iterator begin() const {
        std::lock_guard<std::mutex> lock(mtx);
        Node* current = root;
        while (current != nil && current->left != nil) {
            current = current->left;
        }
        return Iterator(current, nil);
    }

    Iterator end() const {
        std::lock_guard<std::mutex> lock(mtx);
        return Iterator(nil, nil);
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mtx);
        return root == nil;
    }
};

int main() {
    Set<int> s;
    
    s.insert(10);
    s.insert(5);
    s.insert(15);
    s.insert(2);
    s.insert(7);
    
    std::cout << "Числа в сет:";
    for (auto it = s.begin(); it != s.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    std::cout << "Содержит 7: " << s.contains(7) << std::endl;
    std::cout << "Содержит 11: " << s.contains(11) << std::endl;

    s.erase(5);
    std::cout << "После удаления: ";
    for (const auto& value : s) {
        std::cout << value << " ";
    }
    std::cout << std::endl;


    Set<int> shared_set;
    std::mutex cout_mtx;

    // Поток 1: запись
    std::thread writer([&]{
        for (int i = 0; i < 20; ++i) {
            shared_set.insert(i);
            std::cout << "вставил число - " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    });

    // Поток 2: чтение
    std::thread reader([&]{
        for (int i = 0; i < 20; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            if (shared_set.contains(i)) {
                std::lock_guard<std::mutex> lock(cout_mtx);
                std::cout << "Найдено: " << i << std::endl;
            }
            else{
                std::lock_guard<std::mutex> lock(cout_mtx);
                std::cout << "Он не успел найти число: " << i << std::endl;
            }
        }
    });

    writer.join();
    reader.join();

    return 0;
}