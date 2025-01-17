#pragma once

#include <functional>
#include <vector>
#include <unordered_set>

#define eachx(expr) each([&] (const auto &x) { expr; })
#define mapx(expr) map([&] (const auto &x) { return expr; })
#define filterx(expr) filter([&] (const auto &x) { return expr; })
#define whilex(expr) while_([&] (const auto &x) { return expr; })

#define anyx(expr) filterx(expr).has()
#define allx(expr) filterx(!(expr)).hasNot()

template<typename II>
concept IteratorImpl = requires(II i, const II ci) {
	{ i.next() } -> std::same_as<void>;
	{ ci.val() } -> std::same_as<typename II::T>;
	{ ci.has() } -> std::same_as<bool>;
};

struct EndIterator {
};

/**
 * Iterator is a Java-style iterator
 * Its main three functions are:
 * * bool has() const - if the iterator is on a valid element
 * * void next() - goto next element (if has() is false, the behaviour is undefined)
 * * T val() const - return value of the current element
 *
 * struct Iterator is a wrapper over the core iterator implementation, providing a lot of convenience functions like map, filter, etc
 */
template<typename Impl> requires IteratorImpl<Impl>
struct Iterator {

public:
	using T = typename Impl::T;

public:
	template<typename = void>
	auto toList() {
		std::vector<T> r;
		while(impl.has())
			r.push_back(take());

		return r;
	}

	template<typename = void>
	auto toSet() {
		std::unordered_set<T> r;
		while(impl.has())
			r += take();

		return r;
	}

public:
	template<typename F>
	inline auto map(const F &func_) const {
		struct I {
			using T = std::remove_cvref_t<decltype(func_(std::declval<Iterator::T>()))>;
			Impl sub;
			const F func;

			inline void next() { sub.next(); }

			inline bool has() const { return sub.has(); }

			inline T val() const { return func(sub.val()); }
		};
		return Iterator<I>{I{impl, func_}};
	}

	template<typename F>
	inline auto filter(const F &func_) const {
		struct I {
			using T = Iterator::T;
			Impl sub;
			const F func;

			inline void next() {
				sub.next();
				skip();
			}

			inline bool has() const { return sub.has(); }

			inline T val() const { return sub.val(); }

			inline I &skip() {
				while(sub.has() && !func(sub.val())) sub.next();
				return *this;
			}
		};
		return Iterator<I>{I{impl, func_}.skip()};
	}

	template<typename F>
	inline auto while_(const F &func_) const {
		struct I {
			using T = Iterator::T;
			Impl sub;
			const F func;

			inline void next() { sub.next(); }

			inline bool has() const { return sub.has() && func(sub.val()); }

			inline T val() const { return sub.val(); }
		};
		return Iterator<I>{I{impl, func_}};
	}

	/// Returns iterator of std::pair(qsizetype, originalValue) - pair.first is index
	inline auto enumerate() const {
		struct I {
			using T = std::pair<size_t, Iterator::T>;
			Impl sub;
			size_t ix = 0;

			inline bool has() const { return sub.has(); }

			inline void next() {
				sub.next();
				ix++;
			}

			inline auto val() const {
				return std::make_pair(ix, sub.val());
			}
		};
		return Iterator<I>{I{impl}};
	}

	/**
	 * Returns iterator with storing a given payload added.
	 * The payload does not alter the iterator functionality in any way, only stores the payload inside the iterator structure.
	 * This is useful when you want to store a handle for a container you are iterating over to prevent the container being destroyed during iterator.
	 */
	template<typename P>
	inline auto payload(const P &payload_) const {
		struct I {
			using T = Iterator::T;
			Impl sub;
			P payload;

			inline bool has() const { return sub.has(); }

			inline void next() { sub.next(); }

			inline auto val() const { return sub.val(); }
		};
		return Iterator<I>{I{impl, payload_}};
	}

	template<typename F>
	inline void each(const F &func) {
		while(impl.has()) {
			func(impl.val());
			impl.next();
		}
	}

public:
	template<typename R = T, typename G = T>
	inline R join(const G &glue) {
		if(!has())
			return {};

		R r = take();
		while(has()) {
			r += glue;
			r += take();
		}
		return r;
	}

	template<typename R = T>
	R sum() {
		R r = {};
		while(has())
			r += take();
		return r;
	}

public:
	template<typename = void>
	bool any() {
		for(; has(); next())
			if(val()) return true;

		return false;
	}

	template<typename = void>
	bool all() {
		for(; has(); next())
			if(!val()) return false;

		return true;
	}

public:
	inline T val() const {
		return impl.val();
	}

	inline bool has() const {
		return impl.has();
	}

	inline bool hasNot() const {
		return !impl.has();
	}

public:
	inline T take() {
		T r = impl.val();
		impl.next();
		return r;
	}

	inline void next() {
		impl.next();
	}

public:
	inline T operator *() const { return impl.val(); }

	inline void operator ++() { impl.next(); }

public:
	inline auto &begin() const { return *this; }

	inline EndIterator end() const { return {}; }

public:
	inline bool operator !=(EndIterator) const {
		return has();
	}

	explicit inline operator bool() const {
		return has();
	}

public:
	Impl impl;

};

static auto iteratorIota(size_t n_ = -1) {
	struct I {
		using T = size_t;
		const T n;
		T i = 0;

		inline void next() { i++; }

		inline bool has() const { return i != n; }

		inline T val() const { return i; }
	};
	return Iterator<I>{I{n_}};
}

/// Standard begin/end based iterator
template<typename IT, typename E>
static auto iteratorStd(const IT &it_, const E &end_) {
	struct I {
		using T = std::iter_value_t<IT>;
		IT it;
		E end;

		inline void next() { it++; }

		inline bool has() const { return it != end; }

		inline T val() const { return *it; }
	};
	return Iterator<I>{I{it_, end_}};
}

/// Standard begin/end based iterator
template<typename R>
requires std::ranges::range<R>
static auto iterator(const R &range) {
	return iteratorStd(std::ranges::cbegin(range), std::ranges::cend(range));
}

/// Std based iterator, returns std::pair(it.key(), it.value())
template<typename C>
static auto iteratorAssoc(const C &container) {
	struct I {
		using T = std::pair<std::remove_cvref_t<decltype(container.begin().key())>, std::remove_cvref_t<decltype(container.begin().value())>>;
		decltype(container.begin()) it;
		decltype(container.end()) end;

		inline void next() { it++; }

		inline bool has() const { return it != end; }

		inline T val() const { return std::make_pair(it.key(), it.value()); }
	};
	return Iterator<I>{I{container.begin(), container.end()}};
}

/// Std based iterator, returns the underlying std iterator
template<typename C>
static auto iteratorIt(const C &container) {
	struct I {
		using T = const decltype(container.begin());
		decltype(container.begin()) it;
		decltype(container.end()) end;

		inline void next() { it++; }

		inline bool has() const { return it != end; }

		inline T val() const { return it; }
	};
	return Iterator<I>{I{container.begin(), container.end()}};
}

/// Std based iterator, iterates over container.find(key) (while it != end && it.key() == key), returns it.value()
template<typename C, typename K>
static auto iteratorFind(const C &container, const K &key_) {
	struct I {
		using T = std::remove_cvref_t<decltype(*container.find(key_))>;
		decltype(container.find(key_)) it;
		decltype(container.end()) end;
		K key;

		inline void next() { it++; }

		inline bool has() const { return it != end && it.key() == key; }

		inline T val() const { return it.value(); }
	};
	return Iterator<I>{I{container.find(key_), container.end(), key_}};
}