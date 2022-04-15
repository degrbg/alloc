#include <cstdlib>
#include <iostream>
#include <map>
#include <memory>
#include <set>

int BResourceID = 0;
int BAllocatorID = 0;

template <typename T, std::size_t ts>
void type_reg_T() {
  return;
}

template <typename T, std::size_t ts,
    typename U, std::size_t us>
void type_reg_TU() {
  return;
}

struct BResource {
  BResource() : id_{BResourceID} {
    std::cout << "New BResource " << id_ << "\n";
    ++BResourceID;
  }
  BResource(const BResource& other) = delete;
  BResource& operator=(const BResource& other) = delete;

  void* allocate(std::size_t n) {
    void* p = std::malloc(n);
    ptrs_.insert(p);
    return p;
  }

  void deallocate(void* p) {
    const auto it = ptrs_.find(p);
    if (it != ptrs_.end()) {
      ptrs_.erase(it);
    } else {
      std::cout
          << "BResource::deallocate(): p not found in BResource "
          << id_ << "\n";
    }
    std::free(p);
  }

  const int id_;
  std::set<void*> ptrs_;
};

template <typename T>
struct BAllocator {
  using value_type = T;

  BAllocator() :
      id_{BAllocatorID},
      ts_{sizeof(T)},
      us_{},
      res_{std::make_shared<BResource>()} {
    std::cout << "New BAllocator " << id_
        << " with BResource " << res_->id_ << "\n";
    ++BAllocatorID;
    type_reg_T<T, sizeof(T)>();
  }

  BAllocator(const BAllocator& other) :
      id_{BAllocatorID},
      ts_{other.ts_},
      us_{other.us_},
      res_{other.res_} {
    std::cout << "New BAllocator " << id_
        << " with BResource " << res_->id_ << "\n";
    ++BAllocatorID;
    type_reg_T<T, sizeof(T)>();
  }

  template <typename U>
  BAllocator(const BAllocator<U>& other) :
      id_{BAllocatorID},
      ts_{sizeof(T)},
      us_{sizeof(U)},
      res_{std::make_shared<BResource>()} {
    std::cout << "New BAllocator " << id_
        << " with BResource " << res_->id_ << "\n";
    ++BAllocatorID;
    type_reg_TU<T, sizeof(T), U, sizeof(U)>();
  }

  T* allocate(std::size_t n) {
    void* p = res_->allocate(ts_ * n);
    auto tp = static_cast<T*>(p);
    std::cout << "BAllocator " << id_ << " allocate() "
        << n << " chunk(s) of size " << ts_ << " at "
        << tp << "\n";
    return tp;
  }

  void deallocate(T* tp, std::size_t n) {
    std::cout << "BAllocator " << id_ << " deallocate() "
        << n << " chunk(s) of size " << ts_ << " at "
        << tp << "\n";
    void* p = static_cast<void*>(tp);
    res_->deallocate(p);
  }

  template <typename U>
  bool operator==(const BAllocator<U>& other) {
    type_reg_TU<T, sizeof(T), U, sizeof(U)>();
    return res_ == other.res_;
  }

  template <typename U>
  bool operator!=(const BAllocator<U>& other) {
    type_reg_TU<T, sizeof(T), U, sizeof(U)>();
    return !(*this == other);
  }

  const int id_;
  const std::size_t ts_;
  const std::size_t us_;
  std::shared_ptr<BResource> res_;
};

template <typename Key, typename T>
using BMultimap = std::multimap<Key,
    T,
    std::less<Key>,
    BAllocator<std::pair<const Key, T>>>;

int main()
{
  BMultimap<int, int> bm;

  std::cout << "bm.size() = " << bm.size() << "\n";

  bm.insert(std::pair<int, int>(0, 1));
  bm.insert(std::pair<int, int>(2, 3));

  std::cout << "bm.size() = " << bm.size() << "\n";

  bm.clear();

  std::cout << "bm.size() = " << bm.size() << "\n";
}
