#include "test_runner.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <set>
#include <utility>
#include <vector>
#include <map>

using namespace std;

//В векторе пару хранить- обьект и приоритет. В сете- ид и приоритет.

template <typename T>
class PriorityCollection
{
public:
    PriorityCollection()    {
        objects_to_priority.reserve(1500000);
    }
    using Id = pair<T, int>*;

    // Добавить объект с нулевым приоритетом
    // с помощью перемещения и вернуть его идентификатор
    Id Add(T object)    {
        objects_to_priority.push_back({ move(object), 0 });
        auto ptr = &(objects_to_priority.back());
        priority_to_id.insert({ 0, ptr });
        return ptr;
    }

    // Добавить все элементы диапазона [range_begin, range_end)
    // с помощью перемещения, записав выданные им идентификаторы
    // в диапазон [ids_begin, ...)
    template <typename ObjInputIt, typename IdOutputIt>
    void Add(ObjInputIt range_begin, ObjInputIt range_end,
        IdOutputIt ids_begin)   {
        for (auto it = make_move_iterator(range_begin); it != make_move_iterator(range_end); ++it) {
            *(ids_begin++) = Add(*it);
        }
    }

    // Определить, принадлежит ли идентификатор какому-либо
    // хранящемуся в контейнере объекту
    bool IsValid(Id id) const   {
        for (auto x : priority_to_id)   {
            if (x.second == id) {
                return true;           
            }
        }
        return false;
    }

    // Получить объект по идентификатору
    const T& Get(Id id) const   {
        return id->first;
    }

    // Увеличить приоритет объекта на 1
    void Promote(Id id) {
        auto it = priority_to_id.find({ id->second, id });
        if (it != priority_to_id.end()) {
            auto extr = priority_to_id.extract(it).value();
            priority_to_id.insert({ extr.first + 1, extr.second });
        }   
        ++id->second;
    }

    // Получить объект с максимальным приоритетом и его приоритет
    pair<const T&, int> GetMax() const
    {
        return { priority_to_id.rbegin()->second->first, priority_to_id.rbegin()->first };
    }

    // Аналогично GetMax, но удаляет элемент из контейнера
    pair<T, int> PopMax()
    {
        auto it = priority_to_id.rbegin();
        int priority = it->first;
        Id max_id = it->second;
        priority_to_id.erase(*it);
        return { move(max_id->first), priority };
    }

private:
    // Приватные поля и методы
    set<pair<int, Id>> priority_to_id;
    vector<pair<T, int>> objects_to_priority;
};



class StringNonCopyable : public string {
public:
    using string::string;  // Позволяет использовать конструкторы строки
    StringNonCopyable(const StringNonCopyable&) = delete;
    StringNonCopyable(StringNonCopyable&&) = default;
    StringNonCopyable& operator=(const StringNonCopyable&) = delete;
    StringNonCopyable& operator=(StringNonCopyable&&) = default;
};

void TestNoCopy() {
    PriorityCollection<StringNonCopyable> strings;
    const auto white_id = strings.Add("white");
    const auto yellow_id = strings.Add("yellow");
    const auto red_id = strings.Add("red");

    strings.Promote(yellow_id);
    for (int i = 0; i < 2; ++i) {
        strings.Promote(red_id);
    }
    strings.Promote(yellow_id);
    {
        const auto item = strings.PopMax();
        ASSERT_EQUAL(item.first, "red");
        ASSERT_EQUAL(item.second, 2);
    }
    {
        const auto item = strings.PopMax();
        ASSERT_EQUAL(item.first, "yellow");
        ASSERT_EQUAL(item.second, 2);
    }
    {
        const auto item = strings.PopMax();
        ASSERT_EQUAL(item.first, "white");
        ASSERT_EQUAL(item.second, 0);
    }
}

void TheBestTest() {
    PriorityCollection<StringNonCopyable> contain;

    vector<StringNonCopyable> words;
    words.push_back("red");
    words.push_back("blue");
    words.push_back("green");
    words.push_back("yellow");
    words.push_back("white");

    vector<PriorityCollection<StringNonCopyable>::Id> check(words.size());

    contain.Add(words.begin(), words.end(), check.begin());

    //Проверка добавления вектора
    ASSERT_EQUAL(contain.Get(check[0]), "red");
    ASSERT_EQUAL(contain.Get(check[4]), "white");

    //Проверка валидности
    PriorityCollection<StringNonCopyable> spy;
    auto bomb = spy.Add({ "joke" });

    ASSERT(!contain.IsValid(bomb));
    ASSERT(contain.IsValid(check[0]));
    contain.Promote(check[0]);				//red++
    contain.PopMax();                     //del red
    ASSERT(!contain.IsValid(check[0]));

    //Проверка GetMax()
    contain.Promote(check[4]);				//white++
    auto check_max = contain.GetMax();
    ASSERT_EQUAL(check_max.first, StringNonCopyable({ "white" }));
    auto check_max_copy = contain.GetMax();
    ASSERT_EQUAL(check_max.first, check_max_copy.first);
    //Проверка извлечения максимума PopMax();

    ASSERT_EQUAL(check_max.first, "white");
    auto item = contain.PopMax();			//del white
    ASSERT_EQUAL(item.first, StringNonCopyable({ "white" }));
    ASSERT_EQUAL(item.second, 1);
    ASSERT(!contain.IsValid(check[4]));

    //проверка GetMax() по очередности добавления
    auto new_item = contain.Add({ "new" });
    contain.Promote(check[3]);	// yellow++
    contain.Promote(new_item);
    contain.Promote(check[2]);	// green++
    auto x = contain.GetMax();
    ASSERT_EQUAL(x.second, 1);
    ASSERT_EQUAL(x.first, "new");
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestNoCopy);
    RUN_TEST(tr, TheBestTest);
    return 0;
}
