#pragma once

#include <cstddef>
#include <cstring>
#include <utility>
#include <type_traits>

namespace hutao {

template<typename T>
class Array {
public:
    Array() noexcept : m_data(nullptr), m_size(0), m_capacity(0) {}

    explicit Array(size_t size) : m_data(nullptr), m_size(0), m_capacity(0) {
        if (size > 0) {
            Reserve(size);
            for (size_t i = 0; i < size; ++i) {
                new(&m_data[i]) T();
            }
            m_size = size;
        }
    }

    Array(size_t size, const T& value) : m_data(nullptr), m_size(0), m_capacity(0) {
        if (size > 0) {
            Reserve(size);
            for (size_t i = 0; i < size; ++i) {
                new(&m_data[i]) T(value);
            }
            m_size = size;
        }
    }

    Array(const Array& other) : m_data(nullptr), m_size(0), m_capacity(0) {
        if (other.m_size > 0) {
            Reserve(other.m_size);
            for (size_t i = 0; i < other.m_size; ++i) {
                new(&m_data[i]) T(other.m_data[i]);
            }
            m_size = other.m_size;
        }
    }

    Array(Array&& other) noexcept : m_data(other.m_data), m_size(other.m_size), m_capacity(other.m_capacity) {
        other.m_data = nullptr;
        other.m_size = 0;
        other.m_capacity = 0;
    }

    ~Array() {
        Clear();
        if (m_data) {
            ::operator delete[](m_data);
        }
    }

    Array& operator=(const Array& other) {
        if (this != &other) {
            Clear();
            if (other.m_size > 0) {
                Reserve(other.m_size);
                for (size_t i = 0; i < other.m_size; ++i) {
                    new(&m_data[i]) T(other.m_data[i]);
                }
                m_size = other.m_size;
            }
        }
        return *this;
    }

    Array& operator=(Array&& other) noexcept {
        if (this != &other) {
            Clear();
            if (m_data) {
                ::operator delete[](m_data);
            }

            m_data = other.m_data;
            m_size = other.m_size;
            m_capacity = other.m_capacity;

            other.m_data = nullptr;
            other.m_size = 0;
            other.m_capacity = 0;
        }
        return *this;
    }

    T& operator[](size_t index) {
        if (index >= m_size) {
            static T dummy;
            return dummy;
        }
        return m_data[index];
    }

    const T& operator[](size_t index) const {
        if (index >= m_size) {
            static T dummy;
            return dummy;
        }
        return m_data[index];
    }

    size_t Length() const noexcept {
        return m_size;
    }

    size_t Count() const noexcept {
        return m_size;
    }

    size_t Capacity() const noexcept {
        return m_capacity;
    }

    void Capacity(size_t capacity) noexcept {
        Reserve(capacity);
    }

    bool IsEmpty() const noexcept {
        return m_size == 0;
    }

    void Clear() {
        if (m_data) {
            for (size_t i = 0; i < m_size; ++i) {
                m_data[i].~T();
            }
        }
        m_size = 0;
    }

    void Add(const T& value) {
        EnsureCapacity(m_size + 1);
        new(&m_data[m_size]) T(value);
        ++m_size;
    }

    void Add(T&& value) {
        EnsureCapacity(m_size + 1);
        new(&m_data[m_size]) T(std::move(value));
        ++m_size;
    }

    void Insert(size_t index, const T& value) {
        if (index > m_size) {
            index = m_size;
        }

        EnsureCapacity(m_size + 1);

        // 移动元素
        for (size_t i = m_size; i > index; --i) {
            new(&m_data[i]) T(std::move(m_data[i - 1]));
            m_data[i - 1].~T();
        }

        new(&m_data[index]) T(value);
        ++m_size;
    }

    void RemoveAt(size_t index) {
        if (index >= m_size) {
            return;
        }

        // 销毁要删除的元素
        m_data[index].~T();

        // 将后面的元素向前移动
        for (size_t i = index; i < m_size - 1; ++i) {
            new(&m_data[i]) T(std::move(m_data[i + 1]));
            m_data[i + 1].~T();
        }

        --m_size;
    }

    int IndexOf(const T& value) const {
        for (size_t i = 0; i < m_size; ++i) {
            if (m_data[i] == value) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    int LastIndexOf(const T& value) const {
        for (size_t i = m_size; i > 0; --i) {
            if (m_data[i - 1] == value) {
                return static_cast<int>(i - 1);
            }
        }
        return -1;
    }

    bool Contains(const T& value) const {
        return IndexOf(value) != -1;
    }

    void Reverse() {
        if (m_size <= 1) {
            return;
        }

        size_t left = 0;
        size_t right = m_size - 1;

        while (left < right) {
            std::swap(m_data[left], m_data[right]);
            ++left;
            --right;
        }
    }

    void CopyTo(Array<T>& destination) const {
        destination.Clear();
        destination.Reserve(m_size);

        for (size_t i = 0; i < m_size; ++i) {
            new(&destination.m_data[i]) T(m_data[i]);
        }
        destination.m_size = m_size;
    }

    void CopyTo(Array<T>& destination, size_t destinationIndex) const {
        if (destinationIndex >= destination.m_size) {
            destination.Reserve(destinationIndex + m_size);
            destination.m_size = destinationIndex + m_size;
        }

        for (size_t i = 0; i < m_size; ++i) {
            if (destinationIndex + i < destination.m_capacity) {
                if (destinationIndex + i < destination.m_size) {
                    destination.m_data[destinationIndex + i] = m_data[i];
                }
                else {
                    new(&destination.m_data[destinationIndex + i]) T(m_data[i]);
                }
            }
        }
    }

    T* Data() noexcept {
        return m_data;
    }

    const T* Data() const noexcept {
        return m_data;
    }

    void Resize(size_t newSize) {
        if (newSize == m_size) {
            return;
        }

        if (newSize < m_size) {
            for (size_t i = newSize; i < m_size; ++i) {
                m_data[i].~T();
            }
            m_size = newSize;
        }
        else {
            Reserve(newSize);
            for (size_t i = m_size; i < newSize; ++i) {
                new(&m_data[i]) T();
            }
            m_size = newSize;
        }
    }

    void Reserve(size_t newCapacity) {
        if (newCapacity <= m_capacity) {
            return;
        }

        EnsureCapacity(newCapacity);
    }

    T* begin() noexcept {
        return m_data;
    }

    const T* begin() const noexcept {
        return m_data;
    }

    T* end() noexcept {
        return m_data + m_size;
    }

    const T* end() const noexcept {
        return m_data + m_size;
    }

private:
    void EnsureCapacity(size_t requiredCapacity) {
        if (requiredCapacity <= m_capacity) {
            return;
        }

        size_t newCapacity = m_capacity == 0 ? 4 : m_capacity + m_capacity / 2;
        if (newCapacity < requiredCapacity) {
            newCapacity = requiredCapacity;
        }

        T* newData = static_cast<T*>(::operator new[](newCapacity * sizeof(T)));

        // 移动现有元素到新数组
        for (size_t i = 0; i < m_size; ++i) {
            new(&newData[i]) T(std::move(m_data[i]));
            m_data[i].~T();
        }

        if (m_data) {
            ::operator delete[](m_data);
        }

        m_data = newData;
        m_capacity = newCapacity;
    }

private:
    T* m_data;
    size_t m_size;
    size_t m_capacity;
};
} // namespace hutao
