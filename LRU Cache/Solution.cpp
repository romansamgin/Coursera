#include "Common.h"

#include <unordered_map>
#include <mutex>
#include <list>

using namespace std;

class LruCache : public ICache {
public:
    LruCache(
        shared_ptr<IBooksUnpacker> books_unpacker,
        const Settings& settings
    ) : books_unpacker_(books_unpacker), settings_(settings) 
    {
    }

    BookPtr GetBook(const string& book_name) override {
        lock_guard<mutex> lock(mtx);
        if (content.count(book_name) == 0) {
            BookPtr ptr = books_unpacker_->UnpackBook(book_name);
            if (ptr->GetContent().size() > settings_.max_memory) {
                content.clear();
                book_names.clear();
                return ptr;
            }

            content[book_name] = ptr;
            book_names.push_back(book_name);
            full_size += ptr->GetContent().size();

            while (full_size > settings_.max_memory) {
                full_size -= content[book_names.front()]->GetContent().size();
                content.erase(book_names.front());
                book_names.pop_front();
            }
            return content[book_name];
        }
        else {
            for (auto it = book_names.begin(); it != book_names.end(); it++) {
                if (*it == book_name) {
                    book_names.erase(it);
                    break;
                }
            }
            book_names.push_back(book_name);         
            return content[book_name];
        }          
    }

private:
    shared_ptr<IBooksUnpacker> books_unpacker_;
    Settings settings_;

    list<string> book_names;
    unordered_map<string, BookPtr> content;

    size_t full_size = 0;
    mutex mtx;
};


unique_ptr<ICache> MakeCache(
    shared_ptr<IBooksUnpacker> books_unpacker,
    const ICache::Settings& settings
) {
    return make_unique<LruCache>(books_unpacker, settings);
}
