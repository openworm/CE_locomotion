// ********************************
// Classes for vectors and matrices
//
// RDB 7/24/95
// ********************************

#pragma once

#include <vector>
#include <initializer_list>
#include <algorithm>
#include <stdexcept>
#include <fstream>
#include <ostream>
#include <istream>
#include <type_traits>
#include <utility>

//
// Modern safe TVector and TMatrix
//

template <class EltType>
class TVector {
public:
    // -- ctors/dtors --
    TVector() = default;
    TVector(int lowerBound, int upperBound);
    TVector(const TVector& other);
    TVector(TVector&& other) noexcept = default;
    TVector(std::initializer_list<EltType> list); // lower bound = 1

    ~TVector() = default;

    // -- assignment --
    TVector& operator=(const TVector& other);
    TVector& operator=(TVector&& other) noexcept = default;

    // -- accessors --
    int LowerBound() const noexcept { return lb_; }
    int UpperBound() const noexcept { return ub_; }
    int Size() const noexcept { return (ub_ >= lb_) ? (ub_ - lb_ + 1) : 0; }

    void SetBounds(int newLB, int newUB);

    // -- element access --
    EltType& operator()(int index);
    const EltType& operator()(int index) const;

    // operator[] mirrors operator() (no debug macro in header)
    EltType& operator[](int index) { return operator()(index); }
    const EltType& operator[](int index) const { return operator()(index); }

    // -- utilities --
    void FillContents(const EltType& value);
    void PushFront(const EltType& value); // shifts right, drops last element if any
    void InitializeContents(std::initializer_list<EltType> list);

    // -- binary I/O --
    // Only safe for trivially copyable EltType
    void BinaryWriteVector(std::ofstream& ofs) const;
    void BinaryReadVector(std::ifstream& ifs);

private:
    std::vector<EltType> data_; // contiguous storage, index map: i -> data_[i - lb_]
    int lb_ = 1;
    int ub_ = 0;

    // helpers
    void check_index(int index) const;
};

//
// TVector implementation
//

template <class EltType>
TVector<EltType>::TVector(int lowerBound, int upperBound) {
    SetBounds(lowerBound, upperBound);
}

template <class EltType>
TVector<EltType>::TVector(const TVector& other)
    : data_(other.data_), lb_(other.lb_), ub_(other.ub_) {}

template <class EltType>
TVector<EltType>::TVector(std::initializer_list<EltType> list) {
    lb_ = 1;
    ub_ = static_cast<int>(list.size());
    data_.assign(list.begin(), list.end());
}

template <class EltType>
TVector<EltType>& TVector<EltType>::operator=(const TVector& other) {
    if (this == &other) return *this;
    data_ = other.data_;
    lb_ = other.lb_;
    ub_ = other.ub_;
    return *this;
}

template <class EltType>
void TVector<EltType>::SetBounds(int newLB, int newUB) {
    if (newUB < newLB) {
        // Represent empty vector as ub_ < lb_
        data_.clear();
        lb_ = newLB;
        ub_ = newUB;
        return;
    }

    int newSize = newUB - newLB + 1;
    std::vector<EltType> newData;
    newData.resize(static_cast<size_t>(newSize));

    // copy overlap region
    if (!data_.empty()) {
        int copyLB = std::max(lb_, newLB);
        int copyUB = std::min(ub_, newUB);
        for (int i = copyLB; i <= copyUB; ++i) {
            newData[static_cast<size_t>(i - newLB)] = data_[static_cast<size_t>(i - lb_)];
        }
    }
    data_.swap(newData);
    lb_ = newLB;
    ub_ = newUB;
}

template <class EltType>
void TVector<EltType>::check_index(int index) const {
    if (index < lb_ || index > ub_) {
        throw std::out_of_range("TVector index out of bounds");
    }
}

template <class EltType>
EltType& TVector<EltType>::operator()(int index) {
    check_index(index);
    return data_[static_cast<size_t>(index - lb_)];
}

template <class EltType>
const EltType& TVector<EltType>::operator()(int index) const {
    check_index(index);
    return data_[static_cast<size_t>(index - lb_)];
}

template <class EltType>
void TVector<EltType>::FillContents(const EltType& value) {
    std::fill(data_.begin(), data_.end(), value);
}

template <class EltType>
void TVector<EltType>::PushFront(const EltType& value) {
    if (Size() == 0) return;
    // shift right by one
    for (int i = Size() - 1; i > 0; --i)
        data_[static_cast<size_t>(i)] = data_[static_cast<size_t>(i - 1)];
    data_[0] = value;
}

template <class EltType>
void TVector<EltType>::InitializeContents(std::initializer_list<EltType> list) {
    size_t idx = 0;
    for (auto &v : list) {
        if (idx >= data_.size()) break;
        data_[idx++] = v;
    }
}

template <class EltType>
void TVector<EltType>::BinaryWriteVector(std::ofstream& ofs) const {
    static_assert(std::is_trivially_copyable_v<EltType>,
                  "BinaryWriteVector requires trivially copyable EltType");

    int lb = lb_;
    int ub = ub_;
    int size = Size();
    ofs.write(reinterpret_cast<const char*>(&lb), sizeof(lb));
    ofs.write(reinterpret_cast<const char*>(&ub), sizeof(ub));
    if (size > 0)
        ofs.write(reinterpret_cast<const char*>(data_.data()), static_cast<std::streamsize>(size * sizeof(EltType)));
}

template <class EltType>
void TVector<EltType>::BinaryReadVector(std::ifstream& ifs) {
    static_assert(std::is_trivially_copyable_v<EltType>,
                  "BinaryReadVector requires trivially copyable EltType");

    int lb, ub;
    ifs.read(reinterpret_cast<char*>(&lb), sizeof(lb));
    ifs.read(reinterpret_cast<char*>(&ub), sizeof(ub));
    if (!ifs) throw std::runtime_error("Failed to read bounds in BinaryReadVector");
    SetBounds(lb, ub);
    int size = Size();
    if (size > 0) {
        ifs.read(reinterpret_cast<char*>(data_.data()), static_cast<std::streamsize>(size * sizeof(EltType)));
        if (!ifs) throw std::runtime_error("Failed to read data in BinaryReadVector");
    }
}

// stream insertion / extraction for TVector
template <class EltType>
std::ostream& operator<<(std::ostream& os, const TVector<EltType>& v) {
    if (v.Size() <= 0) return os;
    for (int i = v.LowerBound(); i < v.UpperBound(); ++i)
        os << v[i] << ' ';
    os << v[v.UpperBound()];
    return os;
}

template <class EltType>
std::istream& operator>>(std::istream& is, TVector<EltType>& v) {
    if (v.Size() <= 0) return is;
    for (int i = v.LowerBound(); i <= v.UpperBound(); ++i)
        is >> v[i];
    return is;
}

//
// TMatrix: stored as contiguous block to avoid pointer arithmetic tricks.
// Access: element (i,j) maps to data_[(i - lb1) * cols + (j - lb2)]
//

template <class EltType>
class TMatrix {
public:
    // ctors/dtor
    TMatrix() = default;
    TMatrix(int rowLB, int rowUB, int colLB, int colUB);
    TMatrix(const TMatrix& other);
    TMatrix(TMatrix&& other) noexcept = default;
    ~TMatrix() = default;

    // assignment
    TMatrix& operator=(const TMatrix& other);
    TMatrix& operator=(TMatrix&& other) noexcept = default;

    // bounds & sizes
    int RowLowerBound() const noexcept { return lb1_; }
    int RowUpperBound() const noexcept { return ub1_; }
    int ColumnLowerBound() const noexcept { return lb2_; }
    int ColumnUpperBound() const noexcept { return ub2_; }

    int RowSize() const noexcept { return (ub1_ >= lb1_) ? (ub1_ - lb1_ + 1) : 0; }
    int ColumnSize() const noexcept { return (ub2_ >= lb2_) ? (ub2_ - lb2_ + 1) : 0; }
    void SetBounds(int newlb1, int newub1, int newlb2, int newub2);

    // element access
    EltType* operator[](int row) {
        // return pointer to first column element for row (unsafe if out of bounds)
        if (row < lb1_ || row > ub1_) throw std::out_of_range("TMatrix row out of bounds");
        return &data_[static_cast<size_t>((row - lb1_) * ColumnSize()) - 0]; // pointer to first element
    }
    const EltType* operator[](int row) const {
        if (row < lb1_ || row > ub1_) throw std::out_of_range("TMatrix row out of bounds");
        return &data_[static_cast<size_t>((row - lb1_) * ColumnSize())];
    }

    EltType& operator()(int i, int j);
    const EltType& operator()(int i, int j) const;

    // utilities
    void FillContents(const EltType& x);
    void InitializeContents(std::initializer_list<EltType> list); // row-major

    // stream output
    template <class T>
    friend std::ostream& operator<<(std::ostream& os, const TMatrix<T>& m);

private:
    std::vector<EltType> data_;
    int lb1_ = 1, ub1_ = 0; // row bounds
    int lb2_ = 1, ub2_ = 0; // column bounds

    size_t idx(int i, int j) const {
        // assumes bounds checked
        return static_cast<size_t>(i - lb1_) * static_cast<size_t>(ColumnSize()) + static_cast<size_t>(j - lb2_);
    }

    void check_bounds(int i, int j) const {
        if (i < lb1_ || i > ub1_ || j < lb2_ || j > ub2_)
            throw std::out_of_range("TMatrix indices out of bounds");
    }
};

//
// TMatrix implementation
//

template <class EltType>
TMatrix<EltType>::TMatrix(int rowLB, int rowUB, int colLB, int colUB) {
    SetBounds(rowLB, rowUB, colLB, colUB);
}

template <class EltType>
TMatrix<EltType>::TMatrix(const TMatrix& other)
    : data_(other.data_), lb1_(other.lb1_), ub1_(other.ub1_), lb2_(other.lb2_), ub2_(other.ub2_) {}

template <class EltType>
TMatrix<EltType>& TMatrix<EltType>::operator=(const TMatrix& other) {
    if (this == &other) return *this;
    data_ = other.data_;
    lb1_ = other.lb1_;
    ub1_ = other.ub1_;
    lb2_ = other.lb2_;
    ub2_ = other.ub2_;
    return *this;
}

template <class EltType>
void TMatrix<EltType>::SetBounds(int newlb1, int newub1, int newlb2, int newub2) {
    // allow empty sizes (ub < lb)
    int rows = (newub1 >= newlb1) ? (newub1 - newlb1 + 1) : 0;
    int cols = (newub2 >= newlb2) ? (newub2 - newlb2 + 1) : 0;

    if (rows < 0 || cols < 0) throw std::invalid_argument("Negative size in SetBounds");

    std::vector<EltType> newData;
    if (rows > 0 && cols > 0) newData.resize(static_cast<size_t>(rows) * static_cast<size_t>(cols));

    // copy overlap region
    if (!data_.empty() && rows > 0 && cols > 0) {
        int copyRowLB = std::max(lb1_, newlb1);
        int copyRowUB = std::min(ub1_, newub1);
        int copyColLB = std::max(lb2_, newlb2);
        int copyColUB = std::min(ub2_, newub2);

        for (int r = copyRowLB; r <= copyRowUB; ++r) {
            for (int c = copyColLB; c <= copyColUB; ++c) {
                newData[static_cast<size_t>(r - newlb1) * static_cast<size_t>(cols)
                        + static_cast<size_t>(c - newlb2)] =
                    data_[static_cast<size_t>(r - lb1_) * static_cast<size_t>(ColumnSize())
                          + static_cast<size_t>(c - lb2_)];
            }
        }
    }

    data_.swap(newData);
    lb1_ = newlb1; ub1_ = newub1;
    lb2_ = newlb2; ub2_ = newub2;
}

template <class EltType>
EltType& TMatrix<EltType>::operator()(int i, int j) {
    check_bounds(i, j);
    return data_[idx(i, j)];
}

template <class EltType>
const EltType& TMatrix<EltType>::operator()(int i, int j) const {
    check_bounds(i, j);
    return data_[idx(i, j)];
}

template <class EltType>
void TMatrix<EltType>::FillContents(const EltType& x) {
    std::fill(data_.begin(), data_.end(), x);
}

template <class EltType>
void TMatrix<EltType>::InitializeContents(std::initializer_list<EltType> list) {
    size_t n = data_.size();
    size_t idx = 0;
    for (auto &v : list) {
        if (idx >= n) break;
        data_[idx++] = v;
    }
}

// stream insertion for TMatrix
template <class EltType>
std::ostream& operator<<(std::ostream& os, const TMatrix<EltType>& m) {
    int rlb = m.RowLowerBound();
    int rub = m.RowUpperBound();
    int clb = m.ColumnLowerBound();
    int cub = m.ColumnUpperBound();

    if (m.RowSize() <= 0 || m.ColumnSize() <= 0) return os;

    for (int i = rlb; i <= rub; ++i) {
        for (int j = clb; j <= cub; ++j) {
            os << m(i, j);
            if (j < cub) os << ' ';
        }
        if (i < rub) os << '\n';
    }
    return os;
}
