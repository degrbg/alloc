#include <cstdlib>
#include <iostream>
#include <map>
#include <memory>
#include <set>

template<typename T, std::size_t size>
void type_reg() {
  return;
}

struct BResource {
  BResource() {}
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
      std::cout << "BResource::deallocate(): p not found\n";
    }
    std::free(p);
  }

  std::set<void*> ptrs_;
};

template <typename T>
struct BAllocator {
  using value_type = T;

  BAllocator() :
      ts_{sizeof(T)},
      res_{std::make_shared<BResource>()} {
    type_reg<T, sizeof(T)>();
  }

  BAllocator(const BAllocator& other) :
      ts_{other.ts_},
      res_{other.res_} {
    type_reg<T, sizeof(T)>();
  }

  template <typename U>
  BAllocator(const BAllocator<U>& other) :
      ts_{other.ts_},
      res_{other.res_} {
    type_reg<U, sizeof(U)>();
  }

  T* allocate(std::size_t n) {
    void* p = res_->allocate(ts_ * n);
    return static_cast<T*>(p);
  }

  void deallocate(T* tp, std::size_t n) {
    void* p = static_cast<void*>(tp);
    res_->deallocate(p);
  }

  template <typename U>
  bool operator==(const BAllocator<U>& other) {
    type_reg<U, sizeof(U)>();
    return res_ == other.res_;
  }

  template <typename U>
  bool operator!=(const BAllocator<U>& other) {
    type_reg<U, sizeof(U)>();
    return !(*this == other);
  }

  const std::size_t ts_;
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
