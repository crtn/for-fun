#include <iostream>
#include <memory>
#include <utility>
#include <typeinfo>

class Any {
    template<typename T>
    friend T any_cast(const Any&);
public:
    template<typename T>
    Any(const T& val) : ptr(
        new holder<typename std::decay<const T>::type>(val)) {}

    Any() : ptr(nullptr) {}

    Any(const Any &any) : ptr(any.ptr->clone()) {}

    Any(Any &&any) : ptr(std::move(any.ptr)) {}

    Any& operator=(const Any &any) {
        ptr.reset(any.ptr->clone());
        return *this;
    }

    Any& operator=(Any &&any) {
        ptr.reset(std::move(any.ptr).release());
        return *this;
    }

    const std::type_info& type() const {
        return (ptr ? ptr->type() : typeid(nullptr));
    }
private:
    struct placeholder {
        virtual ~placeholder() {}
        virtual const std::type_info& type() const = 0;
        virtual placeholder* clone() const = 0;
    };

    template<typename T>
    struct holder : placeholder {
        holder(const T& val) : value(val) {}

        const std::type_info& type() const override {
            return typeid(T);
        }

        holder<T>* clone() const override {
            return new holder<T>(value);
        }

        T value;
    };

    std::unique_ptr<placeholder> ptr;
};

template<typename T>
T any_cast(const Any &any) {
    if(typeid(T) != any.type()) {
        throw std::bad_cast{};
    }
    return static_cast<Any::holder<T>*>(any.ptr.get())->value;
}

int main() {
    Any x = 1;
    Any y = 3.14;
    Any z = "alpha";

    Any xx = x;
    Any yy = y;
    Any zz = z;

    std::cout << any_cast<int>(x) << std::endl;
    std::cout << any_cast<double>(y) << std::endl;
    std::cout << any_cast<const char *>(z) << std::endl;

    std::cout << any_cast<int>(xx) << std::endl;
    std::cout << any_cast<double>(yy) << std::endl;
    std::cout << any_cast<const char *>(zz) << std::endl;

    try {
        std::cout << any_cast<int>(y) << std::endl;
    } catch(std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}