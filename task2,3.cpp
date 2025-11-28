#include <iostream>

// Класс узла дерева
template <typename T>
class node {
public:
    T data;              // Данные, хранящиеся в узле
    node* left;          // Указатель на левого потомка
    node* right;         // Указатель на правого потомка
    node* parent;        // Указатель на родителя
    bool is_red;         // Цвет узла: true - красный, false - чёрный

    // Конструктор узла
    node(const T& value, node* p = nullptr, node* l = nullptr, node* r = nullptr, bool red = true)
        : data(value), parent(p), left(l), right(r), is_red(red) {}
};

// Внутреннее дерево (красно-чёрное)
template <typename T>
class tree {
private:
    node<T>* root = nullptr; // Корень дерева

    // Левый поворот узла x
    void left_rotate(node<T>* x) {
        node<T>* y = x->right; // y - правый потомок x
        x->right = y->left;    // Перемещаем левого потомка y на место правого потомка x
        if (y->left) y->left->parent = x; // Если у y был левый потомок, обновляем его родителя
        y->parent = x->parent;           // y получает родителя x
        if (!x->parent) root = y;        // Если x был корнем, y становится корнем
        else if (x == x->parent->left) x->parent->left = y; // Иначе обновляем указатель у родителя
        else x->parent->right = y;
        y->left = x;          // x становится левым потомком y
        x->parent = y;        // y становится родителем x
    }

    // Правый поворот узла x
    void right_rotate(node<T>* x) {
        node<T>* y = x->left; // y - левый потомок x
        x->left = y->right;   // Перемещаем правого потомка y на место левого потомка x
        if (y->right) y->right->parent = x; // Если у y был правый потомок, обновляем его родителя
        y->parent = x->parent;            // y получает родителя x
        if (!x->parent) root = y;         // Если x был корнем, y становится корнем
        else if (x == x->parent->right) x->parent->right = y; // Иначе обновляем указатель у родителя
        else x->parent->left = y;
        y->right = x;         // x становится правым потомком y
        x->parent = y;        // y становится родителем x
    }

    // Восстановление свойств красно-чёрного дерева после вставки
    void insert_fixup(node<T>* z) {
        // Цикл продолжается, пока родитель z красный (нарушение правила)
        while (z != root && z->parent->is_red) {
            if (z->parent == z->parent->parent->left) { // Если родитель z - левый потомок дедушки
                node<T>* uncle = z->parent->parent->right; // Дядя z
                if (uncle && uncle->is_red) { // Если дядя красный
                    z->parent->is_red = false;     // Красим родителя в чёрный
                    uncle->is_red = false;         // Красим дядю в чёрный
                    z->parent->parent->is_red = true; // Красим дедушку в красный
                    z = z->parent->parent;         // Перемещаемся к дедушке
                } else { // Если дядя чёрный
                    if (z == z->parent->right) { // Если z - правый потомок
                        z = z->parent; // Поднимаемся к родителю
                        left_rotate(z); // Поворачиваем, чтобы сделать z левым потомком
                    }
                    z->parent->is_red = false;       // Красим родителя в чёрный
                    z->parent->parent->is_red = true; // Красим дедушку в красный
                    right_rotate(z->parent->parent); // Поворачиваем дедушку
                }
            } else { // Симметричный случай, когда родитель z - правый потомок дедушки
                node<T>* uncle = z->parent->parent->left; // Дядя z
                if (uncle && uncle->is_red) { // Если дядя красный
                    z->parent->is_red = false;
                    uncle->is_red = false;
                    z->parent->parent->is_red = true;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left) { // Если z - левый потомок
                        z = z->parent;
                        right_rotate(z);
                    }
                    z->parent->is_red = false;
                    z->parent->parent->is_red = true;
                    left_rotate(z->parent->parent);
                }
            }
        }
        root->is_red = false; // Корень всегда чёрный
    }

    // Находит узел с минимальным значением в поддереве
    node<T>* minimum(node<T>* x) {
        while (x && x->left) x = x->left; // Идём по левой ветви до конца
        return x;
    }

    // Находит узел с максимальным значением в поддереве
    node<T>* maximum(node<T>* x) {
        while (x && x->right) x = x->right; // Идём по правой ветви до конца
        return x;
    }

    // Заменяет один поддерево другим (вспомогательная функция для удаления)
    void transplant(node<T>* u, node<T>* v) {
        if (!u->parent) root = v; // Если u - корень, v становится корнем
        else if (u == u->parent->left) u->parent->left = v; // Иначе обновляем указатель у родителя
        else u->parent->right = v;
        if (v) v->parent = u->parent; // Если v существует, обновляем его родителя
    }

    // Восстановление свойств красно-чёрного дерева после удаления
    void delete_fixup(node<T>* x) {
        // Цикл продолжается, пока x не корень и его цвет чёрный
        while (x != root && (!x || !x->is_red)) {
            if (x == x->parent->left) { // Если x - левый потомок
                node<T>* w = x->parent->right; // Брат x
                if (w && w->is_red) { // Если брат красный
                    w->is_red = false; // Красим брата в чёрный
                    x->parent->is_red = true; // Красим родителя в красный
                    left_rotate(x->parent); // Поворачиваем родителя
                    w = x->parent->right; // Обновляем брата
                }
                // Если оба потомка брата чёрные
                if (w && (!w->left || !w->left->is_red) && (!w->right || !w->right->is_red)) {
                    w->is_red = true; // Красим брата в красный
                    x = x->parent;    // Поднимаемся выше
                } else { // Если хотя бы один потомок брата красный
                    // Если правый потомок брата чёрный
                    if (w && (!w->right || !w->right->is_red)) {
                        if (w->left) w->left->is_red = false; // Красим левого потомка в чёрный
                        w->is_red = true; // Красим брата в красный
                        right_rotate(w); // Поворачиваем брата
                        w = x->parent->right; // Обновляем брата
                    }
                    // Красим брата в цвет родителя
                    if (w) {
                        w->is_red = x->parent->is_red;
                        x->parent->is_red = false; // Красим родителя в чёрный
                        if (w->right) w->right->is_red = false; // Красим правого потомка брата в чёрный
                        left_rotate(x->parent); // Поворачиваем родителя
                        x = root; // Выходим из цикла
                    }
                }
            } else { // Симметричный случай, если x - правый потомок
                node<T>* w = x->parent->left; // Брат x
                if (w && w->is_red) {
                    w->is_red = false;
                    x->parent->is_red = true;
                    right_rotate(x->parent);
                    w = x->parent->left;
                }
                if (w && (!w->right || !w->right->is_red) && (!w->left || !w->left->is_red)) {
                    w->is_red = true;
                    x = x->parent;
                } else {
                    if (w && (!w->left || !w->left->is_red)) {
                        if (w->right) w->right->is_red = false;
                        w->is_red = true;
                        left_rotate(w);
                        w = x->parent->left;
                    }
                    if (w) {
                        w->is_red = x->parent->is_red;
                        x->parent->is_red = false;
                        if (w->left) w->left->is_red = false;
                        right_rotate(x->parent);
                        x = root;
                    }
                }
            }
        }
        if (x) x->is_red = false; // Красим x в чёрный
    }

    // Рекурсивно удаляет все узлы дерева
    void clear(node<T>* n) {
        if (!n) return; // Если узел пуст, выходим
        clear(n->left);  // Удаляем левое поддерево
        clear(n->right); // Удаляем правое поддерево
        delete n;        // Удаляем сам узел
    }

    // Считает количество узлов в дереве
    size_t count_nodes(node<T>* n) const {
        if (!n) return 0; // Если узел пуст, возвращаем 0
        return 1 + count_nodes(n->left) + count_nodes(n->right); // Считаем узел + левое + правое поддеревья
    }

public:
    tree() = default; // Конструктор по умолчанию

    // Деструктор - вызывает clear
    ~tree() {
        clear(root);
    }

    // Вставка нового значения в дерево
    void insert(const T& value) {
        node<T>* z = new node<T>(value); // Создаём новый узел
        node<T>* parent = nullptr;
        node<T>* current = root;

        // Ищем место для вставки
        while (current) {
            parent = current;
            if (value < current->data) current = current->left;
            else if (value > current->data) current = current->right;
            else { // Значение уже существует
                delete z; // Удаляем созданный узел
                return;   // Выходим
            }
        }

        // Вставляем новый узел
        z->parent = parent;
        if (!parent) root = z; // Если дерево было пустым
        else if (value < parent->data) parent->left = z;
        else parent->right = z;

        z->left = nullptr; // Инициализируем потомков
        z->right = nullptr;
        z->is_red = true; // Новый узел всегда красный
        insert_fixup(z); // Исправляем нарушения свойств дерева
    }

    // Удаление значения из дерева
    void erase(const T& value) {
        node<T>* z = root; // Ищем удаляемый узел
        while (z && z->data != value) {
            if (value < z->data) z = z->left;
            else z = z->right;
        }
        if (!z) return; // Узел не найден

        node<T>* y = z; // y - узел, который будет физически удалён
        node<T>* x = nullptr; // x - узел, который будет перемещён на место y
        bool y_original_color = y->is_red; // Запоминаем цвет удаляемого узла

        // Случай 1: у z нет левого потомка
        if (!z->left) {
            x = z->right; // x - правый потомок z
            transplant(z, z->right); // Заменяем z на x
        }
        // Случай 2: у z нет правого потомка
        else if (!z->right) {
            x = z->left; // x - левый потомок z
            transplant(z, z->left); // Заменяем z на x
        }
        // Случай 3: у z есть оба потомка
        else {
            y = minimum(z->right); // y - минимальный узел в правом поддереве z
            y_original_color = y->is_red; // Запоминаем его цвет
            x = y->right; // x - правый потомок y
            if (y->parent == z) { // Если y - непосредственный потомок z
                if (x) x->parent = y; // Обновляем родителя x
            } else { // Если y глубже в поддереве
                transplant(y, y->right); // Заменяем y его правым потомком
                y->right = z->right;     // y получает правое поддерево z
                y->right->parent = y;
            }
            transplant(z, y); // Заменяем z на y
            y->left = z->left; // y получает левое поддерево z
            y->left->parent = y;
            y->is_red = z->is_red; // y получает цвет z
        }

        // Если удаляемый узел был чёрным, нужно восстановить свойства дерева
        if (!y_original_color) delete_fixup(x);
        delete z; // Удаляем сам узел z
    }

    // Проверяет, содержится ли значение в дереве
    bool contains(const T& value) const {
        node<T>* current = root;
        while (current) {
            if (value < current->data) current = current->left;
            else if (value > current->data) current = current->right;
            else return true; // Значение найдено
        }
        return false; // Значение не найдено
    }

    // Проверяет, пусто ли дерево
    bool empty() const {
        return root == nullptr;
    }

    // Возвращает количество элементов в дереве
    size_t size() const {
        return count_nodes(root);
    }

    // Удаляет все элементы из дерева
    void clear() {
        clear(root);
        root = nullptr;
    }

    // Возвращает узел с минимальным значением (для итератора)
    node<T>* begin_node() const {
        return minimum(root);
    }

    // Возвращает "итератор" на конец (nullptr)
    node<T>* end_node() const {
        return nullptr; // "итератор" на конец — всегда nullptr
    }
};

// Обёртка my_set
template <typename T>
class my_set {
private:
    tree<T> rb_tree; // Используем внутреннее красно-чёрное дерево

public:
    // Вставляет значение в множество
    void insert(const T& value) {
        rb_tree.insert(value);
    }

    // Удаляет значение из множества
    void erase(const T& value) {
        rb_tree.erase(value);
    }

    // Проверяет, содержится ли значение в множестве
    bool contains(const T& value) const {
        return rb_tree.contains(value);
    }

    // Проверяет, пусто ли множество
    bool empty() const {
        return rb_tree.empty();
    }

    // Возвращает количество элементов в множестве
    size_t size() const {
        return rb_tree.size();
    }

    // Очищает множество
    void clear() {
        rb_tree.clear();
    }

    // begin и end возвращают "итераторы" (в виде true/false)
    // Это не полноценные итераторы STL, а упрощённая реализация
    bool begin() const {
        return rb_tree.begin_node() != nullptr; // true, если есть хотя бы один элемент
    }

    bool end() const {
        return false; // условный "конец"
    }
};

// Пример использования
int main() {
    my_set<int> s; // Создаём множество целых чисел
    s.insert(10); // Добавляем 10
    s.insert(20); // Добавляем 20
    s.insert(5);  // Добавляем 5

    std::cout << s.size() << std::endl;        // 3 - количество элементов
    std::cout << s.contains(10) << std::endl; // 10 есть в множестве
    std::cout << s.empty() << std::endl;      //  множество не пустое

    s.erase(10); // Удаляем 10
    std::cout << s.contains(10) << std::endl; // 10 больше нет

    s.clear(); // Очищаем множество
    std::cout << s.empty() << std::endl; // множество теперь пустое

    return 0;

}
