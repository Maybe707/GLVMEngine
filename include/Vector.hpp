#pragma once
#include "Constants.hpp"
#include "IContainer.hpp"
#include "Iterator.hpp"
#include "VertexMath.hpp"
#include <vector>
#include <algorithm>
#include <iostream>
#include <string_view>
#include <type_traits>
#include <utility>

namespace GLVM::core {
template<class T> class vector;
template<class T> class VectorIterator : public Iterator<T> {
    T* current_;
    T* end_;
public:
    explicit VectorIterator(vector<T>& values)
        : current_(values.GetVectorContainer()), end_(values.empty() ? current_ : current_ + values.GetSize()) {}
    bool Next() override { if (ValidStatus()) ++current_; return ValidStatus(); }
    bool ValidStatus() override { return current_ != end_; }
    T& Current() override { return *current_; }
    T& Last() override { return *(end_ - 1); }
};

// Compatibility API for older engine modules. Storage and object lifetime belong to std::vector.
template<class T> class vector : public IContainer {
    std::vector<T> values_;
public:
    vector() = default;
    vector(const vector&) = default;
    vector(vector&&) noexcept = default;
    vector& operator=(const vector&) = default;
    vector& operator=(vector&&) noexcept = default;
    ~vector() override = default;
    void Push(T value) { values_.push_back(std::move(value)); }
    void Pop() { if (!empty()) values_.pop_back(); }
    void Swap(T& a, T& b) { using std::swap; swap(a, b); }
    VectorIterator<T> Find(const T& value) {
        VectorIterator<T> result(*this);
        while (result.ValidStatus() && !(result.Current() == value)) result.Next();
        return result;
    }
    void Resize(unsigned int size) { values_.resize(size); }
    void Reserve(unsigned int size) { values_.reserve(size); }
    void Remove(unsigned int index) { if (index < values_.size()) values_.erase(values_.begin() + index); }
    void RemoveFirstItem() { Remove(0); }
    T& GetFirstItem() { return values_.front(); }
    T& GetHead() { return values_.back(); }
    T* GetVectorContainer() { return values_.data(); }
    const T* GetVectorContainer() const { return values_.data(); }
    [[nodiscard]] unsigned int GetSize() const { return static_cast<unsigned int>(values_.size()); }
    [[nodiscard]] unsigned int GetCapacity() const { return static_cast<unsigned int>(values_.capacity()); }
    T& operator[](unsigned int index) { return values_[index]; }
    const T& operator[](unsigned int index) const { return values_[index]; }
    void clear() noexcept { values_.clear(); }
    bool empty() const noexcept { return values_.empty(); }
    auto begin() noexcept { return values_.begin(); }
    auto end() noexcept { return values_.end(); }
    auto begin() const noexcept { return values_.begin(); }
    auto end() const noexcept { return values_.end(); }
    bool operator==(const char* text) const requires std::is_same_v<T, char> {
        if (!text) return false;
        const size_t size = !empty() && values_.back() == '\0' ? values_.size() - 1 : values_.size();
        return std::string_view(values_.data() ? values_.data() : "", size) == text;
    }
    void Print() { for (const auto& value : values_) std::cout << value << '\n'; }
};
}
