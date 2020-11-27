#ifndef LESS_RELATION_ITERATOR_H_
#define LESS_RELATION_ITERATOR_H_

template<typename T>
bool LessThan(const T * & a, const T * & b) {
    return *a < *b;
}

template<typename T>
bool LessThan(const std::shared_ptr<T> & a, const std::shared_ptr<T> & b) {
    return *a < *b;
}

template<typename T>
bool LessThan(const T& a, const T&b) {
    return a < b;
}

template<typename ContainerType>
class LessRelationIterator {
public:
    LessRelationIterator();
    LessRelationIterator & set_container(const std::shared_ptr<ContainerType> &);
    LessRelationIterator & set_begin();
    LessRelationIterator & set_end();
    const std::pair<size_t, size_t> & operator*() const;
    LessRelationIterator & operator++();
    bool operator==(const LessRelationIterator &) const;
    bool operator!=(const LessRelationIterator &) const;
private:
    std::shared_ptr<ContainerType> pContainer_;
    size_t container_size_;
    std::pair<size_t, size_t> position_;

    bool increment_position();
    void find_next();
};

template<typename ContainerType>
LessRelationIterator<ContainerType>::LessRelationIterator() :
pContainer_(nullptr), container_size_(0), position_({0,0}) {}

template<typename ContainerType>
LessRelationIterator<ContainerType> &LessRelationIterator<ContainerType>::
        set_container(const std::shared_ptr<ContainerType> & pContainer) {
    pContainer_ = pContainer;
    container_size_ = pContainer ->size();
    return *this;
}

template<typename ContainerType>
LessRelationIterator<ContainerType> &LessRelationIterator<ContainerType>::set_begin() {
    position_ = {0,0};
    find_next();
    return *this;
}

template<typename ContainerType>
LessRelationIterator<ContainerType> &LessRelationIterator<ContainerType>::set_end() {
    position_ = {container_size_, container_size_};
    return *this;
}

template<typename ContainerType>
const std::pair<size_t, size_t> &LessRelationIterator<ContainerType>::operator*() const {
    return position_;
}

template<typename ContainerType>
LessRelationIterator<ContainerType> &LessRelationIterator<ContainerType>::operator++() {
    find_next();
    return *this;
}

template<typename ContainerType>
bool LessRelationIterator<ContainerType>::operator==(const LessRelationIterator<ContainerType> & it) const {
    return position_ == it.position_;
}

template<typename ContainerType>
bool LessRelationIterator<ContainerType>::operator!=(const LessRelationIterator<ContainerType> & it) const {
    return position_ != it.position_;
}

template<typename ContainerType>
void LessRelationIterator<ContainerType>::find_next() {
    if ( container_size_ != pContainer_->size() )
        throw std::runtime_error("Container has changed while using the iterator!");
    while( increment_position() ) {
        if (position_.first == position_.second) continue;
        if (LessThan(
                (*pContainer_)[position_.first],
                (*pContainer_)[position_.second]
                )) return;
    }
}

template<typename ContainerType>
bool LessRelationIterator<ContainerType>::increment_position() {
    if (position_.first == container_size_ && position_.second == container_size_) return false;
    if (position_.first == container_size_-1 && position_.second == container_size_-1) {
        position_.first = container_size_;
        position_.second = container_size_;
        return false;
    }

    position_.second++;
    if (position_.second < container_size_) return true;

    position_.first++;
    position_.second = 0;
    return true;
}

#endif
