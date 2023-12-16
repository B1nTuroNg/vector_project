#ifndef VECTOR_H
#define VECTOR_H

#include <memory>
#include <cstddef>
#include <iterator>

template <class T>
class Vector {
 private:
  size_t size_{};
  size_t capacity_;
  T* buffer_;

 public:
  using ValueType = T;
  using Pointer = T*;
  using ConstPointer = const T*;
  using Reference = T&;
  using ConstReference = const T&;
  using SizeType = size_t;
  using Iterator = T*;
  using ConstIterator = const T*;
  using ReverseIterator = std::reverse_iterator<Iterator>;
  using ConstReverseIterator = std::reverse_iterator<ConstIterator>;



    void PushBack(const T& value) {
        if (size_ == capacity_) {
            auto new_buffer = capacity_ != 0 ? static_cast<T*>(operator new(sizeof(T) * capacity_ * 2))
                                             : static_cast<T*>(operator new(sizeof(T)));
            size_t i = 0;
            try {
                while (i < size_) {
                    new (new_buffer + i) T(std::move(buffer_[i]));
                    ++i;
                }
            } catch (...) {
                ClearImpl(0, i + 1, new_buffer);
                operator delete(new_buffer);
                throw;
            }
            try {
                new (new_buffer + size_) T(value);
            } catch (...) {
                ClearImpl(0, i + 1, new_buffer);
                operator delete(new_buffer);
                throw;
            }
            if (buffer_ != nullptr) {
                ClearImpl(0, size_, buffer_);
                operator delete(buffer_);
            }
            ++size_;
            if (capacity_ == 0) {
                capacity_ = 1;
            } else {
                capacity_ *= 2;
            }
            buffer_ = new_buffer;
        } else {
            try {
                new (buffer_ + size_) T(value);
                ++size_;
            } catch (...) {
                (buffer_ + size_)->~T();
                throw;
            }
        }
    }


    Iterator begin() {  //  NOLINT
    return Iterator(buffer_);
  }

  Iterator end() {  //  NOLINT
    return Iterator(buffer_ + size_);
  }

  ConstIterator begin() const {  //  NOLINT
    return ConstIterator(buffer_);
  }

  ConstIterator end() const {  //  NOLINT
    return ConstIterator(buffer_ + size_);
  }

//
//  ConstIterator cbegin() const {  //  NOLINT
//    return ConstIterator(buffer_);
//  }
//
//  ConstIterator cend() const {  //  NOLINT
//    return ConstIterator(buffer_ + size_);
//  }

  std::reverse_iterator<Iterator> rbegin() {  //  NOLINT
    return std::make_reverse_iterator(this->end());
  }

  std::reverse_iterator<Iterator> rend() {  //  NOLINT
    return std::make_reverse_iterator(this->begin());
  }

  std::reverse_iterator<ConstIterator> rbegin() const {  //  NOLINT
    return std::make_reverse_iterator(this->cend());
  }

  std::reverse_iterator<ConstIterator> rend() const {  //  NOLINT
    return std::make_reverse_iterator(this->cbegin());
  }

  std::reverse_iterator<ConstIterator> crbegin() const {  //  NOLINT
    return std::make_reverse_iterator(this->cend());
  }

  std::reverse_iterator<ConstIterator> crend() const {  //  NOLINT
    return std::make_reverse_iterator(this->cbegin());
  }

  void ClearImpl(size_t first, size_t last, T*& buffer) {
    for (size_t i = first; i < last; ++i) {
      (buffer + i)->~ValueType();
    }
  }

  ConstReference operator[](size_t idx) const {
    return buffer_[idx];
  }

  Reference operator[](size_t idx) {
    return buffer_[idx];
  }

  ConstReference At(size_t idx) const {
    if (idx > size_ - 1) {
      throw std::out_of_range("Out of range");
    }
    return buffer_[idx];
  }
// эти два товарища несильно отличаются, просто с четко выделением, где  const, а где - нет
  Reference At(size_t idx) {
    if (idx > size_ - 1) {
      throw std::out_of_range("Out of range");
    }
    return buffer_[idx];
  }

  SizeType Size() const {
    return size_;
  }

  SizeType Capacity() const {
    return capacity_;
  }

  bool Empty() const {
    return size_ == 0;
  }

  ConstReference Front() const {
    return buffer_[0];
  }

  Reference Front() {
    return buffer_[0];
  }

  ConstReference Back() const {
    return buffer_[size_ - 1];
  }

  Reference Back() {
    return buffer_[size_ - 1];
  }

  ConstPointer Data() const {
    return buffer_;
  }

  Pointer Data() {
    return buffer_;
  }

  Vector() : size_(0), capacity_(0), buffer_(nullptr) {
  }

  // ищет исключения
  explicit Vector(size_t size) : Vector() {
    if (size != 0) {
      auto new_buffer = static_cast<T*>(operator new(sizeof(T) * size));
      size_t i = 0;
      try {
        while (i < size_t(size)) {
          new (new_buffer + i) T();
          ++i;
        }
      }
      catch (...) {
        ClearImpl(0, i + 1, new_buffer);//удаляет по-хорошему
        operator delete(new_buffer);
        throw;
      }
      buffer_ = new_buffer;
      capacity_ = size;
      size_ = size;
    }
  }

  //
  Vector(size_t size, ConstReference value) : Vector() {
    if (size != 0) {
      auto new_buffer = static_cast<T*>(operator new(sizeof(T) * size));
      size_t i = 0;
      try {
        while (i < size_t(size)) {
          new (new_buffer + i) T(value);//вот тут он придает нужные значения
          ++i;
        }//если в try возникает исключение, то оно переходит в catch
      } catch (...) {
        ClearImpl(0, i + 1, new_buffer);
        operator delete(new_buffer);
        throw;//еще раз пробрасывает исключение
      }
      buffer_ = new_buffer;
      capacity_ = size;
      size_ = size;
    }
  }

  // конструктор по списку
  Vector(std::initializer_list<T> list) : Vector() {
    if (list.size() != 0) {
      buffer_ = static_cast<T*>(operator new(sizeof(T) * list.size()));
      capacity_ = list.size();
      for (auto it = list.begin(); it != list.end(); ++it) {
        new (buffer_ + size_) T(*it);
        ++size_;
      }
    }
  }

  template <typename Iterator,
            typename = std::enable_if_t<std::is_base_of_v<std::forward_iterator_tag,
                                                          typename std::iterator_traits<Iterator>::iterator_category>>>
  // конструктор вектора, приинимающий первый и последний элементы.
  // Пробегает все, что между first и last
  Vector(Iterator first, Iterator last) {
    if (std::distance(first, last) < 1) {
      buffer_ = nullptr;
      size_ = 0;
      capacity_ = 0;
    } else {
      auto size = std::distance(first, last);
      auto new_buffer = static_cast<T*>(operator new(sizeof(T) * size));
      size_ = size;
      capacity_ = size;
      size_t i = 0;
      try {
        while (i < size_t(size)) {
          new (new_buffer + i) T(*(std::next(first, i)));
          ++i;
        }
      } catch (...) {
        ClearImpl(0, i + 1, new_buffer);
        operator delete(new_buffer);
        throw;
      }
      buffer_ = new_buffer;
    }
  }
// конструктор копирования. Делает новый, старый не трогает
  Vector(const Vector& other) {
    if (other.size_ == 0) {
      capacity_ = 0;
      buffer_ = nullptr;
    } else {
      auto new_buffer = static_cast<T*>(operator new(sizeof(T) * other.capacity_));
      size_t i = 0;
      try {
        while (i < other.size_) {
          new (new_buffer + i) T(other[i]);
          ++i;
        }
      } catch (...) {
        ClearImpl(0, i + 1, new_buffer);
        operator delete(new_buffer);
        throw;
      }
      capacity_ = other.capacity_;
      size_ = other.size_;
      buffer_ = new_buffer;
    }
  }

  // move constructor
  Vector(Vector&& other) noexcept : Vector() {
    Swap(other);
  }

  Vector& operator=(const Vector& other) {
    if (this != &other) {
      auto new_vector = other;
      Swap(new_vector);
    }
    return *this;
  }

  // move равно
  Vector& operator=(Vector&& other) noexcept {
    auto new_vector = std::move(other);
    Swap(new_vector);
    return *this;
  }

  void Swap(Vector& other) {
    std::swap(buffer_, other.buffer_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
  }

  //изменяет размер вектора
  void Resize(size_t new_size) {
    if (new_size > capacity_) {
      auto new_buffer = static_cast<T*>(operator new(sizeof(T) * new_size));
      size_t i = 0;
      try {
        while (i < size_) {
          new (new_buffer + i) T(std::move(buffer_[i]));
          ++i;
        }
        while (i < new_size) {
          new (new_buffer + i) T();
          ++i;
        }
      } catch (...) {
        ClearImpl(0, i + 1, new_buffer);
        operator delete(new_buffer);
        throw;
      }
      ClearImpl(0, size_, buffer_);
      operator delete(buffer_);
      capacity_ = new_size;
      buffer_ = new_buffer;
    } else if (new_size > size_) {
      auto i = size_;
      try {
        while (i < new_size) {
          new (buffer_ + i) T{};// тут то же самое T()
          ++i;
        }
      } catch (...) {
        ClearImpl(size_, i, buffer_);
        throw;
      }
    } else {
      ClearImpl(new_size, size_, buffer_);
    }
    size_ = new_size;
  }

  //увелечение размера и заполнение новым остаток
  void Resize(size_t new_size, T&& value) {
    if (new_size > capacity_) {
      auto new_buffer = static_cast<T*>(operator new(sizeof(T) * new_size));
      size_t i = 0;
      try {
        while (i < size_) {
          new (new_buffer + i) T(std::move(buffer_[i]));
          ++i;
        }
        while (i < new_size) {
          new (new_buffer + i) T(std::move(value));
          ++i;
        }
      } catch (...) {
        ClearImpl(0, i + 1, new_buffer);
        operator delete(new_buffer);
        throw;
      }
      ClearImpl(0, size_, buffer_);
      operator delete(buffer_);
      capacity_ = new_size;
      buffer_ = new_buffer;
    } else if (new_size > size_) {
      auto i = size_;
      try {
        while (i < new_size) {
          new (buffer_ + i) T(std::move(value));
          ++i;
        }
      } catch (...) {
        ClearImpl(size_, i, buffer_);
        throw;
      }
    } else {
      ClearImpl(new_size, size_, buffer_);
    }
    size_ = new_size;
  }

  //
  void Reserve(size_t new_capacity) {
    if (new_capacity > capacity_) {
      auto new_buffer = static_cast<T*>(operator new(sizeof(T) * new_capacity));
      size_t i = 0;
      try {
        while (i < size_) {
          new (new_buffer + i) T(std::move(buffer_[i]));
          ++i;
        }
      } catch (...) {
        ClearImpl(0, i + 1, new_buffer);
        operator delete(new_buffer);
        throw;
      }
      ClearImpl(0, size_, buffer_);
      operator delete(buffer_);
      capacity_ = new_capacity;
      buffer_ = new_buffer;
    }
  }

  void PushBack(T&& value) {
    if (size_ == capacity_) {
      auto new_buffer = capacity_ != 0 ? static_cast<T*>(operator new(sizeof(T) * capacity_ * 2))
                                       : static_cast<T*>(operator new(sizeof(T)));
      size_t i = 0;
      try {
        while (i < size_) {
          new (new_buffer + i) T(std::move(buffer_[i]));
          ++i;
        }
      } catch (...) {
        ClearImpl(0, i + 1, new_buffer);
        operator delete(new_buffer);
        throw;
      }
      try {
        new (new_buffer + size_) T(std::move(value));
      } catch (...) {
        ClearImpl(0, i + 1, new_buffer);
        operator delete(new_buffer);
        throw;
      }
      ClearImpl(0, size_, buffer_);
      operator delete(buffer_);
      ++size_;
      if (capacity_ == 0) {
        capacity_ = 1;
      } else {
        capacity_ *= 2;
      }
      buffer_ = new_buffer;
    } else {
      try {
        new (buffer_ + size_) T(std::move(value));
        ++size_;
      } catch (...) {
        (buffer_ + size_)->~T();
        throw;
      }
    }
  }

  void PopBack() noexcept {
    if (size_ != 0) {
      (buffer_ + size_ - 1)->~T();
      --size_;
    }
  }

  template <typename... Args>
  void EmplaceBack(Args&&... args) {
    this->PushBack(T(std::forward<Args>(args)...));
  }

  void Clear() {
    ClearImpl(0, size_, buffer_);
    size_ = 0;
  }

  ~Vector() {
    ClearImpl(0, size_, buffer_);
    operator delete(buffer_);
  }

  friend bool operator<(const Vector& lhs, const Vector& rhs) {
    for (size_t i = 0; i < std::min(lhs.size_, rhs.size_); ++i) {
      if (lhs[i] != rhs[i]) {
        return lhs[i] < rhs[i];
      }
    }
    return lhs.size_ < rhs.size_;
  }

  friend bool operator>(const Vector& lhs, const Vector& rhs) {
    return rhs < lhs;
  }

  friend bool operator<=(const Vector& lhs, const Vector& rhs) {
    return !(rhs < lhs);
  }

  friend bool operator>=(const Vector& lhs, const Vector& rhs) {
    return !(lhs < rhs);
  }

  friend bool operator==(const Vector& lhs, const Vector& rhs) {
    if (lhs.size_ != rhs.size_) {
      return false;
    }
    for (size_t i = 0; i < lhs.size_; ++i) {
      if (lhs[i] != rhs[i]) {
        return false;
      }
    }
    return true;
  }

  friend bool operator!=(const Vector& lhs, const Vector& rhs) {
    return !(lhs == rhs);
  }


    Vector<int> PrefFunc(std::string& str) {
        Vector<int> pref(str.length(), 0);
        for (size_t i = 1; i < str.length(); ++i) {
            int k = pref[i - 1];
            while (str[i] != str[k] && k > 0) {
                k = pref[k - 1];
            }
            if (str[i] == str[k]) {
                pref[i] = k + 1;
            }
        }
        return pref;
    }

    int kmp (const Vector<int>& substr) {
        Vector<int> new_str = substr;
        std::string kmp_str;
        for (auto cur : substr) {
            kmp_str += static_cast<char>(cur);
        }
        kmp_str += "#";
        for (int i = 0; i < size_; ++i) {
            kmp_str += static_cast<char>(buffer_[i]);
        }
        auto pref = PrefFunc(kmp_str);
        for (size_t i = substr.size_ + 1; i < size_ + substr.size_ + 1; ++i) {
            if (pref[i] == static_cast<int>(substr.size_)) {
                return static_cast<int>(i - 2 * substr.size_);
            }
        }
        return -1;
    }

};

#endif