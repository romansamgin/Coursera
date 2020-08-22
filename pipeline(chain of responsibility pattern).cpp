#include "test_runner.h"
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;


struct Email {
    string from;
    string to;
    string body;

    Email() = default;

    Email(string from_, string to_, string body_) :
        from(move(from_)), to(move(to_)), body(move(body_))
    {
    }
};


class Worker {
public:
    virtual ~Worker() = default;
    virtual void Process(unique_ptr<Email> email) = 0;
    virtual void Run() {
        // только первому worker-у в пайплайне нужно это имплементировать
        throw logic_error("Unimplemented");
    }

protected:
    // реализации должны вызывать PassOn, чтобы передать объект дальше
    // по цепочке обработчиков
    void PassOn(unique_ptr<Email> email) const {
        if (next_ != nullptr) {
            next_->Process(move(email));
        }
    }

public:
    void SetNext(unique_ptr<Worker> next) {
        next_ = move(next);
    }
private:
    unique_ptr<Worker> next_;
};


class Reader : public Worker {
public:
    Reader(istream& in) : in_(&in)
    {
    }

    void Run() override {
        for (string from, to, body; ; ) {
            getline(*in_, from);
            getline(*in_, to);
            getline(*in_, body);

            if (in_->eof()) {
                break;
            }
            else {
                PassOn(make_unique<Email>(from, to, body));
            }
        }
    }

    void Process(unique_ptr<Email> email) override {
        PassOn(move(email));
    }

private:
    istream* in_;
};


class Filter : public Worker {
public:
    using Function = function<bool(const Email&)>;

public:

    Filter(Function predicate) : predicate_(predicate)
    {
    }

    void Process(unique_ptr<Email> email) override {
        if (predicate_(*email)) {
            PassOn(move(email));
        }
    }

private:
    Function predicate_;
};


class Copier : public Worker {
public:
    Copier(string to) : new_to_(to)
    {
    }

    void Process(unique_ptr<Email> email) override {
        string from = (*email).from;
        string to = (*email).to;
        string body = (*email).body;
        PassOn(move(email));
        if (to != new_to_) {
            PassOn(make_unique<Email>(from, new_to_, body));
        }
    }

private:
    string new_to_;
};


class Sender : public Worker {
public:
    Sender(ostream& os) : os_(&os)
    {
    }

    void Process(unique_ptr<Email> email) override {
        *os_ << (*email).from << '\n'
            << (*email).to << '\n'
            << (*email).body << '\n';
        PassOn(move(email));
    }

private:
    ostream* os_;
};



class PipelineBuilder {
public:
    // добавляет в качестве первого обработчика Reader
    explicit PipelineBuilder(istream& in) {
        workers_.push_back(make_unique<Reader>(in));
    }

    // добавляет новый обработчик Filter
    PipelineBuilder& FilterBy(Filter::Function filter) {
        workers_.push_back(make_unique<Filter>(filter));
        return *this;
    }

    // добавляет новый обработчик Copier
    PipelineBuilder& CopyTo(string recipient) {
        workers_.push_back(make_unique<Copier>(recipient));
        return *this;
    }

    // добавляет новый обработчик Sender
    PipelineBuilder& Send(ostream& out) {
        workers_.push_back(make_unique<Sender>(out));

        return *this;
    }

    // возвращает готовую цепочку обработчиков
    unique_ptr<Worker> Build() {
        for (size_t i = workers_.size() - 1; i > 0; --i) {
            workers_[i - 1]->SetNext(move(workers_[i]));
        }
        return move(workers_.front());
    }

private:
    vector<unique_ptr<Worker>> workers_;
};


void TestSanity() {
    string input = (
        "erich@example.com\n"
        "richard@example.com\n"
        "Hello there\n"

        "erich@example.com\n"
        "ralph@example.com\n"
        "Are you sure you pressed the right button?\n"

        "ralph@example.com\n"
        "erich@example.com\n"
        "I do not make mistakes of that kind\n"
        );
    istringstream inStream(input);
    ostringstream outStream;

    PipelineBuilder builder(inStream);
    builder.FilterBy([](const Email& email) {
        return email.from == "erich@example.com";
        });

    //
    //builder.Send(outStream);

    builder.CopyTo("richard@example.com");
    builder.Send(outStream);
    auto pipeline = builder.Build();

    pipeline->Run();

    string expectedOutput = (
        "erich@example.com\n"
        "richard@example.com\n"
        "Hello there\n"

        "erich@example.com\n"
        "ralph@example.com\n"
        "Are you sure you pressed the right button?\n"

        "erich@example.com\n"
        "richard@example.com\n"
        "Are you sure you pressed the right button?\n"
        );

    ASSERT_EQUAL(expectedOutput, outStream.str());
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestSanity);
    return 0;
}
