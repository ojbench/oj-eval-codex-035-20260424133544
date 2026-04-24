// Minimal MyString implementation with SSO (<=15), capacity mgmt, iterators
#ifndef SIMPLE_STRING_SIMPLESTRING_HPP
#define SIMPLE_STRING_SIMPLESTRING_HPP

#include <stdexcept>
#include <cstring>

class MyString {
private:
    union {
        char* heap_ptr;
        char small_buffer[16];
    } storage{};
    size_t len = 0;          // number of characters (excluding NUL)
    size_t cap = 15;         // capacity excluding NUL (15 for SSO)
    bool using_sso = true;   // true => using small_buffer

    char* data_ptr() { return using_sso ? storage.small_buffer : storage.heap_ptr; }
    const char* data_ptr() const { return using_sso ? storage.small_buffer : storage.heap_ptr; }

    void set_terminator() { data_ptr()[len] = '\0'; }

    void allocate_heap(size_t new_cap) {
        // allocate new_cap + 1 to hold terminator
        char* p = new char[new_cap + 1];
        // copy existing data (if any)
        if (len > 0) std::memcpy(p, data_ptr(), len);
        p[len] = '\0';
        if (!using_sso) delete[] storage.heap_ptr;
        storage.heap_ptr = p;
        using_sso = false;
        cap = new_cap;
    }

    void ensure_capacity(size_t need) {
        if (need <= cap) return;
        // grow: at least need, typically 2x
        size_t new_cap = cap;
        while (new_cap < need) {
            new_cap = new_cap < 32 ? new_cap * 2 : (new_cap * 3) / 2; // mild growth
            if (new_cap < need) continue;
        }
        // if currently SSO and need <= 15, keep SSO. Otherwise move to heap.
        if (using_sso && new_cap <= 15) {
            cap = 15;
            return;
        }
        if (using_sso) {
            // move from SSO to heap
            char* p = new char[new_cap + 1];
            if (len > 0) std::memcpy(p, storage.small_buffer, len);
            p[len] = '\0';
            storage.heap_ptr = p;
            using_sso = false;
            cap = new_cap;
        } else {
            // reallocate on heap
            char* p = new char[new_cap + 1];
            if (len > 0) std::memcpy(p, storage.heap_ptr, len);
            p[len] = '\0';
            delete[] storage.heap_ptr;
            storage.heap_ptr = p;
            cap = new_cap;
        }
    }

    void maybe_shrink() {
        // if len <= 15, use SSO
        if (len <= 15) {
            if (!using_sso) {
                // move to SSO
                std::memcpy(storage.small_buffer, storage.heap_ptr, len);
                storage.small_buffer[len] = '\0';
                delete[] storage.heap_ptr;
                using_sso = true;
                cap = 15;
            } else {
                cap = 15; // keep SSO cap
                storage.small_buffer[len] = '\0';
            }
        } else {
            // on heap: optionally shrink if wasteful
            size_t target = cap;
            if (len * 2 < cap) target = (len * 3) / 2 + 1; // reduce a bit
            if (target < len) target = len;
            if (target < 16) target = 16; // if >15, at least 16
            if (target != cap) {
                char* p = new char[target + 1];
                if (len > 0) std::memcpy(p, data_ptr(), len);
                p[len] = '\0';
                if (!using_sso) delete[] storage.heap_ptr;
                storage.heap_ptr = p;
                using_sso = false;
                cap = target;
            }
        }
    }

public:
    MyString() {
        using_sso = true;
        len = 0;
        cap = 15;
        storage.small_buffer[0] = '\0';
    }

    MyString(const char* s) {
        if (!s) {
            using_sso = true;
            len = 0; cap = 15; storage.small_buffer[0] = '\0';
            return;
        }
        size_t n = std::strlen(s);
        if (n <= 15) {
            using_sso = true;
            cap = 15;
            len = n;
            if (n) std::memcpy(storage.small_buffer, s, n);
            storage.small_buffer[n] = '\0';
        } else {
            using_sso = false;
            cap = n; // exact, may expand later
            storage.heap_ptr = new char[cap + 1];
            len = n;
            std::memcpy(storage.heap_ptr, s, n);
            storage.heap_ptr[n] = '\0';
        }
    }

    MyString(const MyString& other) {
        len = other.len;
        using_sso = other.using_sso;
        if (other.using_sso) {
            cap = 15;
            std::memcpy(storage.small_buffer, other.storage.small_buffer, len + 1);
        } else {
            cap = other.cap;
            storage.heap_ptr = new char[cap + 1];
            std::memcpy(storage.heap_ptr, other.storage.heap_ptr, len + 1);
        }
    }

    MyString(MyString&& other) noexcept {
        len = other.len;
        cap = other.cap;
        using_sso = other.using_sso;
        if (using_sso) {
            std::memcpy(storage.small_buffer, other.storage.small_buffer, len + 1);
        } else {
            storage.heap_ptr = other.storage.heap_ptr;
            other.storage.heap_ptr = nullptr;
        }
        other.len = 0;
        other.cap = 15;
        other.using_sso = true;
        other.storage.small_buffer[0] = '\0';
    }

    MyString& operator=(MyString&& other) noexcept {
        if (this == &other) return *this;
        if (!using_sso && storage.heap_ptr) delete[] storage.heap_ptr;
        len = other.len;
        cap = other.cap;
        using_sso = other.using_sso;
        if (using_sso) {
            std::memcpy(storage.small_buffer, other.storage.small_buffer, len + 1);
        } else {
            storage.heap_ptr = other.storage.heap_ptr;
            other.storage.heap_ptr = nullptr;
        }
        other.len = 0; other.cap = 15; other.using_sso = true; other.storage.small_buffer[0] = '\0';
        return *this;
    }

    MyString& operator=(const MyString& other) {
        if (this == &other) return *this;
        if (!using_sso && storage.heap_ptr) delete[] storage.heap_ptr;
        len = other.len;
        using_sso = other.using_sso;
        if (other.using_sso) {
            cap = 15;
            std::memcpy(storage.small_buffer, other.storage.small_buffer, len + 1);
        } else {
            cap = other.cap;
            storage.heap_ptr = new char[cap + 1];
            std::memcpy(storage.heap_ptr, other.storage.heap_ptr, len + 1);
        }
        return *this;
    }

    ~MyString() {
        if (!using_sso && storage.heap_ptr) {
            delete[] storage.heap_ptr;
            storage.heap_ptr = nullptr;
        }
    }

    const char* c_str() const { return data_ptr(); }

    size_t size() const { return len; }

    size_t capacity() const { return using_sso ? 15 : cap; }

    void reserve(size_t new_capacity) {
        if (new_capacity <= capacity()) return;
        ensure_capacity(new_capacity);
        set_terminator();
    }

    void resize(size_t new_size) {
        ensure_capacity(new_size);
        if (new_size > len) {
            // fill with '\0'
            std::memset(const_cast<char*>(data_ptr()) + len, '\0', new_size - len);
        }
        len = new_size;
        set_terminator();
        maybe_shrink();
    }

    char& operator[](size_t index) {
        if (index >= len) throw std::out_of_range("index out of range");
        return const_cast<char&>(data_ptr()[index]);
    }

    MyString operator+(const MyString& rhs) const {
        MyString res;
        size_t total = len + rhs.len;
        if (total <= 15) {
            res.using_sso = true;
            res.cap = 15;
            res.len = total;
            if (len) std::memcpy(res.storage.small_buffer, data_ptr(), len);
            if (rhs.len) std::memcpy(res.storage.small_buffer + len, rhs.data_ptr(), rhs.len);
            res.storage.small_buffer[total] = '\0';
        } else {
            res.using_sso = false;
            res.cap = total;
            res.storage.heap_ptr = new char[res.cap + 1];
            res.len = total;
            if (len) std::memcpy(res.storage.heap_ptr, data_ptr(), len);
            if (rhs.len) std::memcpy(res.storage.heap_ptr + len, rhs.data_ptr(), rhs.len);
            res.storage.heap_ptr[total] = '\0';
        }
        return res;
    }

    void append(const char* str) {
        if (!str) return;
        size_t add = std::strlen(str);
        if (add == 0) return;
        ensure_capacity(len + add);
        std::memcpy(const_cast<char*>(data_ptr()) + len, str, add);
        len += add;
        set_terminator();
    }

    const char& at(size_t pos) const {
        if (pos >= len) throw std::out_of_range("pos out of range");
        return data_ptr()[pos];
    }

    class const_iterator;
    class iterator {
    private:
        char* ptr = nullptr;
    public:
        explicit iterator(char* p=nullptr): ptr(p) {}
        iterator& operator++() { ++ptr; return *this; }
        iterator operator++(int) { iterator tmp(*this); ++ptr; return tmp; }
        iterator& operator--() { --ptr; return *this; }
        iterator operator--(int) { iterator tmp(*this); --ptr; return tmp; }
        char& operator*() const { return *ptr; }
        bool operator==(const iterator& other) const { return ptr == other.ptr; }
        bool operator!=(const iterator& other) const { return ptr != other.ptr; }
        bool operator==(const const_iterator& other) const { return ptr == other.ptr; }
        bool operator!=(const const_iterator& other) const { return ptr != other.ptr; }
        friend class MyString;
    };

    class const_iterator {
    private:
        const char* ptr = nullptr;
    public:
        explicit const_iterator(const char* p=nullptr): ptr(p) {}
        const_iterator& operator++() { ++ptr; return *this; }
        const_iterator operator++(int) { const_iterator tmp(*this); ++ptr; return tmp; }
        const_iterator& operator--() { --ptr; return *this; }
        const_iterator operator--(int) { const_iterator tmp(*this); --ptr; return tmp; }
        const char& operator*() const { return *ptr; }
        bool operator==(const const_iterator& other) const { return ptr == other.ptr; }
        bool operator!=(const const_iterator& other) const { return ptr != other.ptr; }
        friend class MyString;
    };

public:
    iterator begin() { return iterator(const_cast<char*>(data_ptr())); }
    iterator end() { return iterator(const_cast<char*>(data_ptr()) + len); }
    const_iterator cbegin() const { return const_iterator(data_ptr()); }
    const_iterator cend() const { return const_iterator(data_ptr() + len); }
};

#endif

