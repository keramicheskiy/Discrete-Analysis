#include <iostream>
#include <cstdint>

const size_t KEY_LENGTH = 8;


template <typename T>
class TVec {
private:
    T* Data = nullptr;
    size_t Length = 0;
    size_t Capacity = 0;

public:
    TVec() = default;

    TVec(size_t size) {
        Resize(size);
    }

    TVec(size_t size, const T& element) {
        Resize(size);
        for (size_t i = 0; i < size; ++i) {
            new (&this->Data[i]) T(element);
        }
    }

    TVec(const std::initializer_list<T> initializerList) {
        this->Reserve(initializerList.size());
        for (const T& item : initializerList) {
            this->PushBack(item);
        }
    }

    ~TVec() noexcept {
        for (size_t i = 0; i < this->Length; ++i) {
            Data[i].~T();
        }
        operator delete(this->Data);

        this->Data = nullptr;
        this->Length = 0;
        this->Capacity = 0;
    }

    TVec(const TVec& other): Length(other.Length), Capacity(other.Capacity) {
        this->Data = static_cast<T*>(operator new(this->Capacity * sizeof(T)));

        for (size_t i = 0; i < this->Length; ++i) {
            new (&this->Data[i]) T(other.Data[i]);
        }
    }
    
    TVec(TVec&& other) noexcept: Data(other.Data), Length(other.Length), Capacity(other.Capacity) {
        other.Data = nullptr;
        other.Length = 0;
        other.Capacity = 0;
    }

    T& operator[](size_t index) {
        return Data[index];
    }
    
    const T& operator[](size_t index) const {
        return Data[index];
    }

    TVec<T>& operator=(const TVec<T>& other) {
        if (this == &other) return *this;

        for (size_t i = 0; i < this->Length; ++i) {
            this->Data[i].~T();
        }
        if (this->Capacity < other.Length) {
            operator delete(this->Data);
            this->Capacity = other.Length;
            this->Data = static_cast<T*>(operator new(this->Capacity * sizeof(T)));
        }
        for (size_t i = 0; i < other.Length; ++i) {
            new (&this->Data[i]) T(other.Data[i]);
        }
        this->Length = other.Length;
        return *this;
    }

    TVec<T>& operator=(TVec<T>&& other) noexcept {
        if (this == &other) return *this;

        for (size_t i = 0; i < this->Length; ++i) {
            this->Data[i].~T();
        }
        operator delete(this->Data);
        this->Data = other.Data;
        this->Length = other.Length;
        this->Capacity = other.Capacity;

        other.Data = nullptr;
        other.Length = 0;
        other.Capacity = 0;

        return *this;
    }

    void Reserve(size_t capacity) {
        if (capacity <= this->Capacity) return;
        T* data = static_cast<T*>(operator new(capacity * sizeof(T)));
        for (size_t i = 0; i < this->Length; ++i) {
            new (&data[i]) T(std::move(this->Data[i]));
        }

        for (size_t i = 0; i < this->Length; ++i) {
            this->Data[i].~T();
        }
        operator delete(this->Data);
        this->Data = data;
        this->Capacity = capacity;
    }

    void Resize(size_t size) {
        if (size < this->Length) {
            for (size_t i = size; i < this->Length; ++i) {
                Data[i].~T();
            }
        }
        if (size > this->Capacity) {
            this->Reserve(std::max(this->Capacity * 2, size));
        }
        if (size > this->Length) {
            for (size_t i = this->Length; i < size; ++i) {
                new (&Data[i]) T();
            }
        }
        this->Length = size;
    }

    template <typename U>
    void PushBack(U&& value) {
        this->Resize(this->Length + 1);
        new (&Data[this->Length - 1]) T(std::forward<U>(value));
    }

    void PopBack() {
        if (this->Length > 0) {
            this->Remove(this->Length - 1);
        }
    }

    size_t Size() const {
        return this->Length;
    }

    T& Get(size_t index) const {
        if (index >= this->Length) throw std::out_of_range("Out of range");
        return Data[index];
    }

    template <typename U>
    void Insert(size_t index, U&& value) {
        if (index > this->Length) throw std::out_of_range("Out of range");

        this->Resize(this->Length + 1);
        for (size_t i = this->Length - 1; i > index; --i) {
            Data[i] = std::move(Data[i - 1]);
        }
        Data[index] = std::forward<U>(value);
    }

    void Remove(size_t index) {
        if (index >= this->Length) throw std::out_of_range("Out of range");
        for (size_t i = index; i < this->Length - 1; ++i) {
            Data[i] = std::move(Data[i + 1]);
        }
        this->Resize(this->Length - 1);
    }   

    void Clear() noexcept {
        for (size_t i = 0; i < this->Length; ++i) {
            Data[i].~T();
        }
        operator delete(this->Data);

        Data = nullptr;
        this->Length = 0;
        this->Capacity = 0;
    }

    void Swap(size_t first, size_t second) {
        if (first >= this->Length || second >= this->Length) throw std::out_of_range("Out of range");
        T temporary = std::move(this[first]);
        this->Data[first] = std::move(this[second]);
        this->Data[second] = std::move(temporary);
    }

    class iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;
    private:
        pointer ptr;
    public:
        iterator(pointer ptr = nullptr): ptr(ptr) {}
        reference operator*() { return *ptr; }
        pointer operator->() { return ptr; }
        iterator operator++() { ++ptr; return *this; }
        iterator operator++(int) { iterator temporary = *this; ++(*this); return temporary; }
        iterator operator--() { --ptr; return *this; }
        iterator operator--(int) { iterator temporary = *this; --(*this); return temporary; }
        iterator operator+=(difference_type diff) { ptr += diff; return *this; }
        iterator operator-=(difference_type diff) { ptr -= diff; return *this; }
        iterator operator+(difference_type diff) const { iterator temporary = *this; temporary += diff; return temporary; }
        iterator operator-(difference_type diff) const { iterator temporary = *this; temporary -= diff; return temporary; }
        difference_type operator-(const iterator& other) const { return ptr - other.ptr; }
        bool operator>(const iterator& other) const { return this->ptr > other.ptr; }
        bool operator<(const iterator& other) const { return this->ptr < other.ptr; }
        bool operator==(const iterator& other) const { return this->ptr == other.ptr; }
        bool operator!=(const iterator& other) const { return !(this->ptr == other.ptr); }
        bool operator>=(const iterator& other) const { return *this > other || *this == other; }
        bool operator<=(const iterator& other) const { return *this < other || *this == other; }
        reference operator[](difference_type diff) const { return *(ptr + diff); }
    };

    class const_iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = const T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;
    private:
        pointer ptr;
    public:
        const_iterator(pointer ptr = nullptr): ptr(ptr) {}
        const_iterator(const iterator& it): ptr(it.ptr) {}
        reference operator*() const { return *ptr; }
        pointer operator->() const { return ptr; }
        const_iterator operator++() { ++ptr; return *this; }
        const_iterator operator++(int) { const_iterator temporary = *this; ++(*this); return temporary; }
        const_iterator operator--() { --ptr; return *this; }
        const_iterator operator--(int) { const_iterator temporary = *this; --(*this); return temporary; }
        const_iterator operator+=(difference_type diff) { ptr += diff; return *this; }
        const_iterator operator-=(difference_type diff) { ptr -= diff; return *this; }
        const_iterator operator+(difference_type diff) const { const_iterator temporary = *this; temporary += diff; return temporary; }
        const_iterator operator-(difference_type diff) const { const_iterator temporary = *this; temporary -= diff; return temporary; }
        difference_type operator-(const iterator& other) const { return ptr - other.ptr; }
        bool operator>(const const_iterator& other) const { return this->ptr > other.ptr; }
        bool operator<(const const_iterator& other) const { return this->ptr < other.ptr; }
        bool operator==(const const_iterator& other) const { return this->ptr == other.ptr; }
        bool operator!=(const const_iterator& other) const { return !(this->ptr == other.ptr); }
        bool operator>=(const const_iterator& other) const { return *this > other || *this == other; }
        bool operator<=(const const_iterator& other) const { return *this < other || *this == other; }
        reference operator[](difference_type diff) const { return *(ptr + diff); }
    };

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() noexcept { return iterator(Data); }
    iterator end() noexcept { return iterator(Data + Length); }
    const_iterator begin() const noexcept { return const_iterator(Data); }
    const_iterator end() const noexcept { return const_iterator(Data + Length); }
    const_iterator cbegin() const noexcept { return const_iterator(Data); }
    const_iterator cend() const noexcept { return const_iterator(Data + Length); }
    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }

};

struct TElement {
    TVec<char> key = TVec<char>();
    unsigned long long value = 0;

    TElement(TVec<char> key, uint64_t value): key(key), value(value) {}

    TElement() = default;

};

std::istream& operator>>(std::istream& is, TVec<char>& vec) {
    char c;
    vec.Clear();

    while(is.get(c) && c != '\n') {
        vec.PushBack(c);
    }
    return is;
}

std::ostream& operator<<(std::ostream& os, const TVec<char>& vec) {
    for (size_t i = 0; i < vec.Size(); ++i) {
        os << vec[i];
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const TElement& element) {
    os << element.key << '\t' << element.value;
    return os;
}

std::ostream& operator<<(std::ostream& os, const TVec<TElement>& vec) {
    for (size_t i = 0; i < vec.Size(); ++i) {
        os << vec[i] << std::endl;
    }
    return os;
}

std::istream& operator>>(std::istream& is, TElement& element) {
    char c;
    element.key.Clear();
    while (is.get(c) && c != '\t') {
        element.key.PushBack(c);
    }
    is >> element.value;
    return is;
}


char pos(TVec<char>& x, size_t p) {
    if (p >= x.Size()) return 0;
    return x[x.Size() - 1 - p];
}

template <typename iterator>
void RadixSort(iterator begin, iterator end) {
    size_t vecSize = end - begin;
    TVec<TElement> res(vecSize);
    for (size_t p = 0; p < KEY_LENGTH; ++p) {
        char maxChar = 0;
        for (auto it = begin; it != end; ++it) {
            if (maxChar < pos(it->key, p)) {
                maxChar = pos(it->key, p);
            }
        }
        TVec<size_t> count(static_cast<size_t>(maxChar) + 1, 0);
        for (auto it = begin; it != end; ++it) {
            count[pos(it->key, p)]++;
        }
        for (size_t i = 1; i <= maxChar; ++i) {
            count[i] += count[i - 1];
        }
        for (size_t i = vecSize; i > 0; i--) {
            iterator x = begin + i - 1;
            res[count[pos(x->key, p)] - 1] = *x;
            count[pos(x->key, p)]--;
        }
        for (size_t i = 0; i < vecSize; ++i) {
            *(begin + i) = res[i];
        }

    }
}

/*
A 000 AA	13207862122685464576
Z 999 ZZ	7670388314707853312
A 000 AA	4588010303972900864
Z 999 ZZ	12992997081104908288

*/

int main() {
    TVec<TElement> toBeSorted;

    TElement element;
    while (true) {
        int next = std::cin.peek();
        if (next == '\n' || next == EOF) {
            std::cin.get();
            break;
        }
        std::cin >> element;
        toBeSorted.PushBack(element);
        std::cin.get();
    }

    RadixSort(toBeSorted.begin(), toBeSorted.end());

    std::cout << toBeSorted << std::endl;

    return 0;
}
