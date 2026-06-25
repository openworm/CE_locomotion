// ********************************
// Classes for vectors and matrices
//
// RDB 7/24/95
// ********************************

#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdarg>
#include <stdexcept>
#include <cassert>
#include <stdlib.h>

#define DEBUG 0

// =======================
//        TVector
// =======================

template<class EltType>
class TVector {
public:
    // Constructors
    TVector() : lb(1), ub(0) {}
    TVector(int LowerBound, int UpperBound) { SetBounds(LowerBound, UpperBound); }
    TVector(const TVector<EltType>& v) { *this = v; }

    // Destructor
    ~TVector() = default;

    // Accessors
    int Size() const { return ub - lb + 1; }
    int LowerBound() const { return lb; }
    int UpperBound() const { return ub; }

    void SetSize(int NewSize) { SetBounds(lb, lb + NewSize - 1); }
    void SetLowerBound(int NewLB) { SetBounds(NewLB, ub); }
    void SetUpperBound(int NewUB) { SetBounds(lb, NewUB); }

    void SetBounds(int NewLB, int NewUB) {
        //if (NewUB < NewLB){
        //std::cout << NewUB << " " << NewLB << std::endl;
        //assert(0);}
        //if (NewUB < NewLB)
        // 	throw std::invalid_argument("Attempt to allocate a negative length TVector");
        lb = NewLB;
        ub = NewUB;
        if (Size()<0)
            throw std::invalid_argument("Attempt to allocate a negative length TVector");
        data.resize(Size());
    }

    // Fill / modify
    void FillContents(const EltType& value) {
        std::fill(data.begin(), data.end(), value);
    }

    double PushFront(const EltType& value) {
        if (data.empty()) return 0.0;
        double sum = 0.0;
        for (int i = ub; i > lb; --i){
            (*this)(i) = (*this)(i - 1);
            sum+=(*this)(i);
        }
        (*this)(lb) = value;
        sum+=(*this)(lb);
        return sum;
    }

    void InitializeContents(EltType v1, ...) {
        if (data.empty()) return;
        va_list ap;
        va_start(ap, v1);
        (*this)(lb) = v1;
        for (int i = lb + 1; i <= ub; ++i)
            (*this)(i) = va_arg(ap, EltType);
        va_end(ap);
    }

    // Binary I/O
    void BinaryWriteVector(std::ofstream& bofs) const {
        bofs.write(reinterpret_cast<const char*>(&lb), sizeof(lb));
        bofs.write(reinterpret_cast<const char*>(&ub), sizeof(ub));
        for (int i = lb; i <= ub; ++i)
            bofs.write(reinterpret_cast<const char*>(&(*this)(i)), sizeof(EltType));
    }

    void BinaryReadVector(std::ifstream& bifs) {
        int LB, UB;
        bifs.read(reinterpret_cast<char*>(&LB), sizeof(LB));
        bifs.read(reinterpret_cast<char*>(&UB), sizeof(UB));
        SetBounds(LB, UB);
        for (int i = LB; i <= UB; ++i)
            bifs.read(reinterpret_cast<char*>(&(*this)(i)), sizeof(EltType));
    }

    // Operators
    EltType& operator[](int index) {
        return (*this)(index);
    }

    const EltType& operator()(int index) const {
        if (index < lb || index > ub)
            throw std::out_of_range("Vector index out of bounds");
        return data[index - lb];
    }

    EltType& operator()(int index) {
        if (index < lb || index > ub)
            throw std::out_of_range("Vector index out of bounds");
        return data[index - lb];
    }

   

    TVector<EltType>& operator=(const vector<EltType>& v) {
        lb = v.lb;
        ub = v.ub;
        data = v.data;
        return *this;
    }
    


    friend std::ostream& operator<<(std::ostream& os, const TVector<EltType>& v) {
        for (int i = v.LowerBound(); i <= v.UpperBound(); ++i) {
            os << v(i);
            if (i != v.UpperBound()) os << " ";
        }
        return os;
    }

    friend std::istream& operator>>(std::istream& is, TVector<EltType>& v) {
        for (int i = v.LowerBound(); i <= v.UpperBound(); ++i)
            is >> v(i);
        return is;
    }

private:
    int lb = 1, ub = 0;
    std::vector<EltType> data;
};

// =======================
//        TMatrix
// =======================

template<class EltType>
class TMatrix {
public:
    // Constructors
    TMatrix() : lb1(1), ub1(0), lb2(1), ub2(0) {}
    TMatrix(int RowLB, int RowUB, int ColLB, int ColUB) {
        SetBounds(RowLB, RowUB, ColLB, ColUB);
    }
    TMatrix(const TMatrix<EltType>& m) { *this = m; }

    // Destructor
    ~TMatrix() = default;

    // Accessors
    int RowSize() const { return ub1 - lb1 + 1; }
    int ColumnSize() const { return ub2 - lb2 + 1; }

    int RowLowerBound() const { return lb1; }
    int RowUpperBound() const { return ub1; }
    int ColumnLowerBound() const { return lb2; }
    int ColumnUpperBound() const { return ub2; }

    void SetBounds(int newlb1, int newub1, int newlb2, int newub2) {
        //if (newub1 < newlb1 || newub2 < newlb2)
          //  throw std::invalid_argument("Attempt to allocate negative sized TMatrix");
        lb1 = newlb1; ub1 = newub1;
        lb2 = newlb2; ub2 = newub2;
        if (RowSize()<0 || ColumnSize()<0)
          throw std::invalid_argument("Attempt to allocate negative sized TMatrix");
        matrix.resize(RowSize());
        for (auto& row : matrix)
            row.resize(ColumnSize());
    }

    // Fill
    void FillContents(const EltType& x) {
        for (auto& row : matrix)
            std::fill(row.begin(), row.end(), x);
    }

    // Initialize with variadic list (unsafe legacy-style)
    void InitializeContents(EltType v1, ...) {
        if (RowSize() == 0 || ColumnSize() == 0) return;
        va_list ap;
        va_start(ap, v1);
        int i = lb1, j = lb2;
        (*this)(i, j) = v1;
        for (int r = 0; r < RowSize(); ++r)
            for (int c = 0; c < ColumnSize(); ++c)
                if (!(r == 0 && c == 0))
                    (*this)(lb1 + r, lb2 + c) = va_arg(ap, EltType);
        va_end(ap);
    }

    // Operators
    EltType& operator()(int i, int j) {
        if (i < lb1 || i > ub1 || j < lb2 || j > ub2)
            throw std::out_of_range("Matrix index out of bounds");
        return matrix[i - lb1][j - lb2];
    }

    const EltType& operator()(int i, int j) const {
        if (i < lb1 || i > ub1 || j < lb2 || j > ub2)
            throw std::out_of_range("Matrix index out of bounds");
        return matrix[i - lb1][j - lb2];
    }

    TMatrix<EltType>& operator=(const TMatrix<EltType>& m) {
        lb1 = m.lb1; ub1 = m.ub1;
        lb2 = m.lb2; ub2 = m.ub2;
        matrix = m.matrix;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const TMatrix<EltType>& m) {
        for (int i = m.RowLowerBound(); i <= m.RowUpperBound(); ++i) {
            for (int j = m.ColumnLowerBound(); j <= m.ColumnUpperBound(); ++j) {
                os << m(i, j);
                if (j != m.ColumnUpperBound()) os << " ";
            }
            if (i != m.RowUpperBound()) os << "\n";
        }
        return os;
    }

    EltType* operator[](int i) {
#if DEBUG
        if (i < lb1 || i > ub1)
            throw std::out_of_range("Matrix row index out of bounds");
#endif
        return matrix[i - lb1].data();
    }

    const EltType* operator[](int i) const {
#if DEBUG
        if (i < lb1 || i > ub1)
            throw std::out_of_range("Matrix row index out of bounds");
#endif
        return matrix[i - lb1].data();
    }


private:
    int lb1 = 1, ub1 = 0, lb2 = 1, ub2 = 0;
    std::vector<std::vector<EltType>> matrix;
};
