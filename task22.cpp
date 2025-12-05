#include <iostream>

// Шаблонный класс Set с элементами типа T
template <typename T>
class Set {
private:
    // Перечисление цветов узлов: КРАСНЫЙ или ЧЁРНЫЙ
    enum Color { RED, BLACK };
    
    // Структура узла дерева
    struct Node {
        T data;           // Данные элемента
        Node* parent;     // Указатель на родителя
        Node* left;       // Указатель на левого потомка
        Node* right;      // Указатель на правого потомка
        Color color;      // Цвет узла (для балансировки)

        // Конструктор узла с заданным значением
        Node(const T& d) 
            : data(d), parent(nullptr), left(nullptr), right(nullptr), color(RED) {}
    };

    Node* root; // Корень дерева
    Node* nil;  // Специальный "нулевой" узел (заменяет nullptr для упрощения алгоритмов)

    // Левый поворот дерева вокруг узла x (для балансировки)
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

    // Правый поворот дерева вокруг узла x (для балансировки)
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

    // Восстановление свойств чёрно-красного дерева после вставки
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

    // Находит узел с минимальным значением в поддереве
    Node* minimum(Node* node) const {
        while (node->left != nil) {
            node = node->left;
        }
        return node;
    }

    // Рекурсивное удаление всех узлов дерева
    void clear(Node* node) {
        if (node == nil) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

    // Вспомогательная функция для поиска узла
    Node* find_node(const T& value) const {
        Node* current = root;
        while (current != nil) {
            if (value < current->data) {
                current = current->left;
            } else if (current->data < value) {
                current = current->right;
            } else {
                return current; // Найдено
            }
        }
        return nil; // Не найдено
    }

    // Замена одного поддерева другим
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

    // Вспомогательная функция для удаления узла
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

    // Восстановление свойств чёрно-красного дерева после удаления
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
    // Вложенный класс итератора для обхода дерева
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

    // Конструктор: создаёт пустое дерево
    Set() {
        nil = new Node(T());
        nil->color = BLACK;
        nil->parent = nil->left = nil->right = nil;
        root = nil;
    }

    // Деструктор: освобождает всю память
    ~Set() {
        clear(root);
        delete nil;
    }

    // Вставка элемента в множество
    void insert(const T& value) {
        Node* y = nil;
        Node* x = root;
        
        // Поиск места для вставки
        while (x != nil) {
            y = x;
            if (value < x->data) {
                x = x->left;
            } else if (x->data < value) {
                x = x->right;
            } else {
                // Элемент уже существует, выходим
                return;
            }
        }

        // Создаём новый узел
        Node* new_node = new Node(value);
        new_node->parent = y;
        new_node->left = nil;
        new_node->right = nil;
        new_node->color = RED;

        // Привязываем новый узел к дереву
        if (y == nil) {
            root = new_node;
        } else if (value < y->data) {
            y->left = new_node;
        } else {
            y->right = new_node;
        }

        // Балансируем дерево
        fix_insert(new_node);
    }

    // Удаление элемента из множества
    void erase(const T& value) {
        Node* z = find_node(value);
        if (z != nil) {
            delete_node(z);
        }
    }

    // Поиск элемента в множестве
    bool contains(const T& value) const {
        return find_node(value) != nil;
    }

    // Итератор на первый элемент (минимальный)
    Iterator begin() const {
        Node* current = root;
        while (current != nil && current->left != nil) {
            current = current->left;
        }
        return Iterator(current, nil);
    }

    // Итератор на элемент после последнего
    Iterator end() const {
        return Iterator(nil, nil);
    }

    bool empty() const {
        return root == nil;
    }
};

// Пример использования
int main() {
    Set<int> s;
    
    s.insert(10);
    s.insert(5);
    s.insert(15);
    s.insert(3);
    s.insert(7);
    
    std::cout << "Set contents: ";
    for (auto it = s.begin(); it != s.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    std::cout << "Range-based for loop: ";
    for (const auto& value : s) {
        std::cout << value << " ";
    }
    std::cout << std::endl;

    std::cout << "Contains 7: " << s.contains(7) << std::endl;
    std::cout << "Contains 8: " << s.contains(8) << std::endl;

    s.erase(5);
    std::cout << "After erasing 5: ";
    for (const auto& value : s) {
        std::cout << value << " ";
    }
    std::cout << std::endl;

    return 0;
}