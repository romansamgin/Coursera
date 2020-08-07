#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

using namespace std;

template<typename T>
class Deque {
private:
    std::vector<T>_front, _back;
public:


    Deque() :
        _front(vector<T>{}),
        _back(vector<T>{})
    {}

    bool Empty() const {
        return _front.empty() && _back.empty();
    }

    size_t Size() const {
        return _front.size() + _back.size();
    }

    T& operator[](size_t index) {
        if (index < _front.size()) {
            return *(_front.end() - 1 - index);
        }
        else {           
            return *(_back.begin() + (index - _front.size()));
        }
    }

    const T& operator[](size_t index) const {
        if (index < _front.size()) {
            return *(_front.end() - 1 - index);
        }
        else {
            return *(_back.begin() + (index - _front.size()));
        }
    }

    T& At(size_t index) {
        if (index >= Size()) {
            throw out_of_range("");
        }
        if (index < _front.size()) {
            return *(_front.end() - 1 - index);
        }
        else {
            return *(_back.begin() + (index - _front.size()));
        }
    }

    const T& At(size_t index) const {
        if (index >= Size()) {
            throw out_of_range("");
        }
        if (index < _front.size()) {
            return *(_front.end() - 1 - index);
        }
        else {
            return *(_back.begin() + (index - _front.size()));
        }
    }

    T& Front() {
        if (!_front.empty()) {
            return *(_front.end() - 1);
        }
        return *(_back.begin());
    }

    const T& Front() const {
        if (!_front.empty()) {
            return *(_front.end() - 1);
        }
        return *(_back.begin());
    }

    T& Back() {
        if (!_back.empty()) {
            return *(_back.end() - 1);
        }
        return *(_front.begin());
    }

    const T& Back() const {
        if (!_back.empty()) {
            return *(_back.end() - 1);
        }
        return *(_front.begin());
    }

    void PushBack(const T& value) {
        _back.push_back(value);
    }

    void PushFront(const T& value) {
        _front.push_back(value);
    }


    std::vector<T> get_back() {
        return _back;
    }
};