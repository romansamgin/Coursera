#include <string>
#include "test_runner.h"

using namespace std;

#define SIZE 1'000'000

class Editor {
public:
    Editor() :  
        gap_left(0), 
        gap_right(SIZE), 
        bufsize(0),
        text{},
        buffer{},
        insert_count(0)
    {}

    void Left() {
        if (gap_left > 0) {
            text[--gap_right] = text[--gap_left];
        }       
    }

   void Right() {  
        if (gap_right < 1'000'000) {
            text[gap_left++] = text[gap_right++];
        }         
    }


    void Insert(char token) {
        text[gap_left++] = token;
        insert_count++;
    }


    void Cut(size_t tokens = 1) {
        if (gap_right == SIZE) {
            return;
        }
        if (tokens == 0) {
            bufsize = 0;
            return;
        }

        if (gap_right + tokens > SIZE) {
            tokens -= gap_right + tokens - SIZE;
        }

        for (size_t i = gap_right; i < gap_right + tokens; ++i) {
            buffer[i - gap_right] = text[i];
        }

        bufsize = tokens;
        gap_right += tokens;
    }

    void Copy(size_t tokens = 1) {
        if (gap_right == SIZE) {
            return;
        }
        if (tokens == 0) {
            bufsize = 0;
            return;
        }

        if (gap_right + tokens > SIZE) {
            tokens -= gap_right + tokens - SIZE;
        }

        for (size_t i = gap_right; i < gap_right + tokens; ++i) {
            buffer[i - gap_right] = text[i];
        }
        bufsize = tokens;
    }

    void Paste() {    
        for (size_t i = 0; i < bufsize; ++i) {
            text[gap_left++] = buffer[i];
        }
    }

    string GetText() const {
        if (insert_count == 0) {
            return "";
        }
        string result = "";
        for (size_t i = 0; i < gap_left; ++i) {
            result += text[i];
        }    
        for (size_t i = gap_right; i < SIZE; ++i) {
            result += text[i];
        }
        return result;
    }

private:
    char text[SIZE];
    char buffer[SIZE];

    size_t gap_left;
    size_t gap_right;
    size_t bufsize;
    int insert_count;
};

void TypeText(Editor& editor, const string& text) {
    for (char c : text) {
        editor.Insert(c);
    }
}

void TestEditing() {
    {
        Editor editor;

        const size_t text_len = 12;
        const size_t first_part_len = 7;
        TypeText(editor, "hello, world");
        for (size_t i = 0; i < text_len; ++i) {
            editor.Left();
        }
        editor.Cut(first_part_len);
        for (size_t i = 0; i < text_len - first_part_len; ++i) {
            editor.Right();
        }
        TypeText(editor, ", ");
        editor.Paste();
        editor.Left();
        editor.Left();
        editor.Cut(3);

        ASSERT_EQUAL(editor.GetText(), "world, hello");
    }
    {
        Editor editor;

        TypeText(editor, "misprnit");
        editor.Left();
        editor.Left();
        editor.Left();
        editor.Cut(1);
        editor.Right();
        editor.Paste();

        ASSERT_EQUAL(editor.GetText(), "misprint");
    }
}

void TestReverse() {
    Editor editor;

    const string text = "esreveR";
    for (char c : text) {
        editor.Insert(c);
        editor.Left();
    }

    ASSERT_EQUAL(editor.GetText(), "Reverse");
}

void TestNoText() {
    Editor editor;
    ASSERT_EQUAL(editor.GetText(), "");

    editor.Left();
    editor.Left();
    editor.Right();
    editor.Right();
    editor.Copy(0);
    editor.Cut(0);
    editor.Paste();

    ASSERT_EQUAL(editor.GetText(), "");
}

void TestEmptyBuffer() {
    Editor editor;

    editor.Paste();
    TypeText(editor, "example");
    editor.Left();
    editor.Left();
    editor.Paste();
    editor.Right();
    editor.Paste();
    editor.Copy(0);
    editor.Paste();
    editor.Left();
    editor.Cut(0);
    editor.Paste();

    ASSERT_EQUAL(editor.GetText(), "example");
}

void MyTest1() {
    Editor editor;

    TypeText(editor, "1234");

    editor.Copy(4);
    editor.Paste();

    ASSERT_EQUAL(editor.GetText(), "1234");
}

void MyTest2() {
    Editor editor;

    TypeText(editor, "1234");
    editor.Left();
    editor.Left();
    editor.Left();
    editor.Left();

    editor.Copy(4);
    editor.Paste();
    editor.Paste();

    ASSERT_EQUAL(editor.GetText(), "123412341234");
}

void MyTest3() {
    Editor editor;

    TypeText(editor, "12345678");
    editor.Left();
    editor.Left();
    editor.Left();
    editor.Left();
    editor.Cut();
    editor.Right();
    editor.Right();
    editor.Right();
    editor.Paste();

    ASSERT_EQUAL(editor.GetText(), "12346785");
}


int main() {
    TestRunner tr;
    RUN_TEST(tr, TestEditing);
    RUN_TEST(tr, TestReverse);
    RUN_TEST(tr, TestNoText);
    RUN_TEST(tr, TestEmptyBuffer);

    
    return 0;
}
