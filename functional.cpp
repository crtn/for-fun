#include <iostream>
#include <memory>
#include <utility>

template<typename RetType, typename ... Args>
class Functional;

template<typename RetType, typename ... Args>
class Functional<RetType(Args ...)> {
public:
    template<typename Callable>
    Functional(Callable &&callable) : ptr(new actual<Callable>(callable)) {}

    RetType operator()(Args ... args) const {
        return ptr->operator()(std::forward<Args>(args) ...);
    }
private:
    struct placeholder {
        virtual RetType operator()(Args ...) const = 0;
        virtual ~placeholder() {}
    };

    template<typename Callable>
    struct actual : placeholder {
        template<typename U>
        actual(U &&u) : func(u) {}

        RetType operator()(Args ... args) const override {
            return func(std::forward<Args>(args) ...);
        }

        Callable func;
    };

    std::unique_ptr<placeholder> ptr;
};

int add(int x, int y, int z) {
    return x + y + z;
}

struct Adder {
    int operator()(int x, int y, int z) const {
        return x + y + z;
    }
};

int main() {
    Functional<int(int, int, int)> f0 = [](int x, int y, int z) {
        return x + y + z;
    };

    Functional<int(int, int, int)> f1 = add;

    Functional<int(int, int, int)> f2 = Adder{};

    std::cout << f0(1, 2, 3) << std::endl
                << f1(1, 2, 3) << std::endl
                << f2(1, 2, 3) << std::endl;

    return 0;
}