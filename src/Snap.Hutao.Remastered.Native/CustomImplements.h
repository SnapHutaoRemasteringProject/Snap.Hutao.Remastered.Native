#pragma once

#include <Unknwn.h>
#include <objidl.h>
#include <cstddef>
#include <type_traits>

namespace hutao {

// Forward declarations
template<typename Derived, typename... Interfaces>
class CustomImplements;

// Helper to count interfaces
template<typename...>
struct InterfaceCount;

template<>
struct InterfaceCount<> {
    static constexpr size_t value = 0;
};

template<typename First, typename... Rest>
struct InterfaceCount<First, Rest...> {
    static constexpr size_t value = 1 + InterfaceCount<Rest...>::value;
};

// Helper to check if type is IUnknown
template<typename T>
struct IsIUnknown : std::false_type {};

template<>
struct IsIUnknown<IUnknown> : std::true_type {};

// Helper to get IUnknown pointer from interface
template<typename Interface>
inline IUnknown* GetIUnknownFromInterface(Interface* pInterface) {
    return static_cast<IUnknown*>(static_cast<void*>(pInterface));
}

// CustomImplements class template
template<typename Derived, typename... Interfaces>
class CustomImplements : public Interfaces... {
    static_assert(sizeof...(Interfaces) > 0, "CustomImplements requires at least one interface");
    static_assert(!std::disjunction_v<IsIUnknown<Interfaces>...>, 
                  "CustomImplements should not explicitly inherit from IUnknown");

public:
    // IUnknown methods
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override {
        if (ppvObject == nullptr) {
            return E_POINTER;
        }

        *ppvObject = nullptr;

        // Check for IUnknown
        if (IsEqualIID(riid, __uuidof(IUnknown))) {
            // 使用reinterpret_cast避免多重继承导致的转换不明确
            *ppvObject = reinterpret_cast<IUnknown*>(this);
            AddRef();
            return S_OK;
        }

        // Check for each interface
        bool found = ((CheckInterface<Interfaces>(riid, ppvObject) || ...));
        
        if (found) {
            return S_OK;
        }

        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef() override {
        return InterlockedIncrement(&m_refCount);
    }

    ULONG STDMETHODCALLTYPE Release() override {
        ULONG refCount = InterlockedDecrement(&m_refCount);
        if (refCount == 0) {
            delete static_cast<Derived*>(this);
        }
        return refCount;
    }

protected:
    CustomImplements() : m_refCount(1) {}
    virtual ~CustomImplements() = default;

private:
    template<typename Interface>
    bool CheckInterface(REFIID riid, void** ppvObject) {
        if (IsEqualIID(riid, __uuidof(Interface))) {
            *ppvObject = static_cast<Interface*>(this);
            AddRef();
            return true;
        }
        return false;
    }

    volatile LONG m_refCount;
};

// Helper function to create instances (similar to winrt::make_self)
template<typename T, typename... Args>
inline T* make_custom(Args&&... args) {
    return new T(std::forward<Args>(args)...);
}

// Helper function to detach interface pointer (similar to detach_abi)
template<typename Interface>
inline Interface* detach_custom(Interface** ptr) {
    Interface* result = *ptr;
    *ptr = nullptr;
    return result;
}

// Helper function to attach interface pointer
template<typename Interface>
inline void attach_custom(Interface** ptr, Interface* p) {
    if (*ptr) {
        (*ptr)->Release();
    }
    *ptr = p;
    if (p) {
        p->AddRef();
    }
}

// Smart pointer for COM objects
template<typename T>
class com_ptr {
public:
    com_ptr() noexcept : m_ptr(nullptr) {}
    
    explicit com_ptr(T* ptr) noexcept : m_ptr(ptr) {
        if (m_ptr) {
            m_ptr->AddRef();
        }
    }
    
    // Conversion constructor from U* to T*
    template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    explicit com_ptr(U* ptr) noexcept : m_ptr(ptr) {
        if (m_ptr) {
            m_ptr->AddRef();
        }
    }
    
    com_ptr(const com_ptr& other) noexcept : m_ptr(other.m_ptr) {
        if (m_ptr) {
            m_ptr->AddRef();
        }
    }
    
    // Conversion constructor from com_ptr<U> to com_ptr<T>
    template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    com_ptr(const com_ptr<U>& other) noexcept : m_ptr(other.get()) {
        if (m_ptr) {
            m_ptr->AddRef();
        }
    }
    
    // Move conversion constructor from com_ptr<U> to com_ptr<T>
    template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    com_ptr(com_ptr<U>&& other) noexcept : m_ptr(other.detach()) {}
    
    com_ptr(com_ptr&& other) noexcept : m_ptr(other.m_ptr) {
        other.m_ptr = nullptr;
    }
    
    ~com_ptr() {
        if (m_ptr) {
            m_ptr->Release();
        }
    }
    
    com_ptr& operator=(const com_ptr& other) noexcept {
        if (this != &other) {
            if (m_ptr) {
                m_ptr->Release();
            }
            m_ptr = other.m_ptr;
            if (m_ptr) {
                m_ptr->AddRef();
            }
        }
        return *this;
    }
    
    // Assignment operator from com_ptr<U>
    template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    com_ptr& operator=(const com_ptr<U>& other) noexcept {
        if (m_ptr != other.get()) {
            if (m_ptr) {
                m_ptr->Release();
            }
            m_ptr = other.get();
            if (m_ptr) {
                m_ptr->AddRef();
            }
        }
        return *this;
    }
    
    com_ptr& operator=(com_ptr&& other) noexcept {
        if (this != &other) {
            if (m_ptr) {
                m_ptr->Release();
            }
            m_ptr = other.m_ptr;
            other.m_ptr = nullptr;
        }
        return *this;
    }
    
    // Move assignment operator from com_ptr<U>
    template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    com_ptr& operator=(com_ptr<U>&& other) noexcept {
        if (m_ptr) {
            m_ptr->Release();
        }
        m_ptr = other.detach();
        return *this;
    }
    
    T* operator->() const noexcept {
        return m_ptr;
    }
    
    T* get() const noexcept {
        return m_ptr;
    }
    
    T** put() noexcept {
        if (m_ptr) {
            m_ptr->Release();
            m_ptr = nullptr;
        }
        return &m_ptr;
    }
    
    T* detach() noexcept {
        T* result = m_ptr;
        m_ptr = nullptr;
        return result;
    }
    
    void attach(T* ptr) noexcept {
        if (m_ptr) {
            m_ptr->Release();
        }
        m_ptr = ptr;
        if (m_ptr) {
            m_ptr->AddRef();
        }
    }
    
    explicit operator bool() const noexcept {
        return m_ptr != nullptr;
    }
    
private:
    T* m_ptr;
};

// Helper to create com_ptr
template<typename T, typename... Args>
inline com_ptr<T> make_com_ptr(Args&&... args) {
    return com_ptr<T>(make_custom<T>(std::forward<Args>(args)...));
}

} // namespace hutao
