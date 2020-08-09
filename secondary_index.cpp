#include "test_runner.h"

#include <iostream>
#include <map>
#include <string>
#include <unordered_map>

using namespace std;

struct Record {
    string id;
    string title;
    string user;
    int timestamp;
    int karma;
};

class Database {
public:
    bool Put(const Record& record) {
        auto [it, inserted] = storage.insert({ record.id, {record, {}, {}, {}} });
        if (!inserted) {
            return false;
        }
        const Record* item_ptr = &it->second.record;
        it->second.timestamps_its = timestamps.insert({ record.timestamp, item_ptr });
        it->second.karmas_its = karmas.insert({ record.karma, item_ptr });
        it->second.users_its = users.insert({ record.user, item_ptr });
        return true;  
    }

    const Record* GetById(const string& id) const {
        auto it = storage.find(id);
        if (it != storage.end()) {
            return &it->second.record;
        }
        return nullptr;
    }

    bool Erase(const string& id) {
        auto it = storage.find(id);
        if (it == storage.end()) {
            return false;
        }
        timestamps.erase(it->second.timestamps_its);
        karmas.erase(it->second.karmas_its);
        users.erase(it->second.users_its);
        storage.erase(it);
        return true;
    }

    template <typename Callback>
    void RangeByTimestamp(int low, int high, Callback callback) const {
        auto begin = timestamps.lower_bound(low);
        auto end = timestamps.upper_bound(high);
        for (auto it = begin; it != end; ++it) {
            if (!callback(*it->second)) {
                break;
            }
        }
    }

    template <typename Callback>
    void RangeByKarma(int low, int high, Callback callback) const {
        auto begin = karmas.lower_bound(low);
        auto end = karmas.upper_bound(high);
        for (auto it = begin; it != end; ++it) {
            if (!callback(*it->second)) {
                break;
            }
        }
    }

    template <typename Callback>
    void AllByUser(const string& user, Callback callback) const {
        auto [begin, end] = users.equal_range(user);
        for (auto it = begin; it != end; ++it) {
            if (!callback(*it->second)) {
                break;
            }
        }
    }

private:
    struct Item {
        Record record;
        multimap<int, const Record*>::iterator timestamps_its;
        multimap<int, const Record*>::iterator karmas_its;
        multimap<string, const Record*>::iterator users_its;
    };

    unordered_map<string, Item> storage;

    multimap<int, const Record*> timestamps;
    multimap<int, const Record*> karmas;
    multimap<string, const Record*> users;   
};



void TestRangeBoundaries() {
    const int good_karma = 1000;
    const int bad_karma = -10;

    Database db;
    db.Put({ "id1", "Hello there", "master", 1536107260, good_karma });
    db.Put({ "id2", "O>>-<", "general2", 1536107260, bad_karma });

    int count = 0;
    db.RangeByKarma(bad_karma, good_karma, [&count](const Record&) {
        ++count;
        return true;
        });

    ASSERT_EQUAL(2, count);
}

void TestSameUser() {
    Database db;
    db.Put({ "id1", "Don't sell", "master", 1536107260, 1000 });
    db.Put({ "id2", "Rethink life", "master", 1536107260, 2000 });

    int count = 0;
    db.AllByUser("master", [&count](const Record&) {
        ++count;
        return true;
        });

    ASSERT_EQUAL(2, count);
}

void TestReplacement() {
    const string final_body = "Feeling sad";

    Database db;
    db.Put({ "id", "Have a hand", "not-master", 1536107260, 10 });
    db.Erase("id");
    db.Put({ "id", final_body, "not-master", 1536107260, -10 });

    auto record = db.GetById("id");
    ASSERT(record != nullptr);
    ASSERT_EQUAL(final_body, record->title);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestRangeBoundaries);
    RUN_TEST(tr, TestSameUser);
    RUN_TEST(tr, TestReplacement);
    return 0;
}
