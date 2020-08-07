#include <numeric>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>



#include "test_runner.h"

using namespace std;

template<typename Iterator>
class Page {
private:
    Iterator _begin, _end;
public:
    Page(Iterator begin, Iterator end)
        : _begin(begin)
        , _end(end)
    {}

    const Iterator& begin() const { 
        return _begin; 
    }

    const Iterator& end() const { 
        return _end; 
    }

    Iterator& begin() { 
        return _begin; 
    }
    Iterator& end() { 
        return _end; 
    }

    size_t size() const {
        return _end - _begin;
    }
};



template<typename It>
class Paginator {
private:
    vector<Page<It>> _pages;
public:
    Paginator(It begin, It end, size_t page_size) {
        while (begin != end) {
            It it = next(begin, min(static_cast<int64_t>(distance(begin, end)), static_cast<int64_t>(page_size)));
            _pages.emplace_back(begin, it);
            begin = it;
        }
    }

    typename vector<Page<It>>::iterator begin() {
        return _pages.begin();
    }

    typename vector<Page<It>>::const_iterator begin() const {
        return _pages.begin();
    }

    typename vector<Page<It>>::iterator end() {
        return _pages.end();
    }

    typename vector<Page<It>>::const_iterator end() const {
        return _pages.end();
    }

   
    size_t size() const {
        return _pages.size();
    }

};



template <typename C>
auto Paginate(C & c, size_t page_size) {
    return Paginator(c.begin(), c.end(), page_size);
}