#include <bits/stdc++.h>
#include "simple_string.hpp"

int main() {
    // Simple I/O harness: read lines, perform simple operations to test MyString
    // Protocol (for local sanity):
    // First line: an integer Q, number of commands
    // Each next line: command
    //   set <text>         -> set string to <text>
    //   append <text>      -> append text
    //   size               -> print size
    //   cap                -> print capacity
    //   get                -> print c_str
    //   at <i>             -> print char at i
    //   plus <text>        -> print (s + text)
    // If no input, just run self-test and exit 0

    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    if (std::cin.peek() == EOF) {
        // self-test
        MyString a("hello");
        a.append(", world");
        std::cout << a.c_str() << "\n";
        std::cout << a.size() << "\n";
        std::cout << a.capacity() << "\n";
        MyString b("!!!");
        MyString c = a + b;
        std::cout << c.c_str() << "\n";
        size_t cnt = 0; for (auto it = c.cbegin(); it != c.cend(); ++it) ++cnt;
        std::cout << cnt << "\n";
        return 0;
    }

    MyString s;
    int Q; if (!(std::cin >> Q)) return 0;
    std::string cmd;
    std::string rest;
    std::getline(std::cin, rest); // consume eol
    for (int i = 0; i < Q; ++i) {
        std::string line; std::getline(std::cin, line);
        if (line.empty()) { --i; continue; }
        std::istringstream iss(line);
        iss >> cmd;
        if (cmd == "set") {
            std::string text; std::getline(iss, text);
            if (!text.empty() && text[0]==' ') text.erase(text.begin());
            s = MyString(text.c_str());
        } else if (cmd == "append") {
            std::string text; std::getline(iss, text);
            if (!text.empty() && text[0]==' ') text.erase(text.begin());
            s.append(text.c_str());
        } else if (cmd == "size") {
            std::cout << s.size() << "\n";
        } else if (cmd == "cap") {
            std::cout << s.capacity() << "\n";
        } else if (cmd == "get") {
            std::cout << s.c_str() << "\n";
        } else if (cmd == "at") {
            size_t idx; iss >> idx; 
            try { std::cout << s.at(idx) << "\n"; } catch (...) { std::cout << "ERR\n"; }
        } else if (cmd == "plus") {
            std::string text; std::getline(iss, text);
            if (!text.empty() && text[0]==' ') text.erase(text.begin());
            MyString t(text.c_str());
            MyString r = s + t;
            std::cout << r.c_str() << "\n";
        }
    }
    return 0;
}

