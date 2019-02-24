#ifndef ITERATOR_RANGE_H
#define ITERATOR_RANGE_H


namespace ino {

template <class It>
struct IteratorRange {

	constexpr IteratorRange() = default;

	constexpr IteratorRange(It b, It e):
		begin_(b), end_(e)
	{
		
	}

	constexpr It  begin() const { return begin_; }
	constexpr It cbegin() const { return begin_; }

	constexpr It  end() const { return end_; }
	constexpr It cend() const { return end_; }

private:
	It begin_;
	It end_;
};

template <class It>
constexpr It begin(const IteratorRange<It>& range) { return range.begin(); }

template <class It>
constexpr It cbegin(const IteratorRange<It>& range) { return range.cbegin(); }

template <class It>
constexpr It end(const IteratorRange<It>& range) { return range.end(); }

template <class It>
constexpr It cend(const IteratorRange<It>& range) { return range.cend(); }

} /* namespace ino */


#endif /* ITERATOR_RANGE_H */
