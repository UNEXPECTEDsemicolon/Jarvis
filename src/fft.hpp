#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <complex>
#include <iostream>
#include <type_traits>
#include <vector>

template <typename Field>
class IField {
  public:
    using object_t = Field;

    virtual object_t GetPrimeRoot(size_t n) const = 0;

    virtual object_t operator+(const IField& n) const = 0;

    virtual object_t operator-(const IField& n) const = 0;

    virtual object_t operator*(const IField& n) const = 0;

    virtual object_t operator/(const IField& n) const = 0;

    virtual object_t operator^(const IField& n) const = 0;

    virtual ~IField() = default;
};

class ComplexField : public IField<ComplexField>, public std::complex<double> {
    using complex = std::complex<double>;
    inline static constexpr complex kImagI = complex(0, 1);

  public:
    using complex::complex;
    using complex::operator=;

    virtual object_t GetPrimeRoot(size_t n) const override {
        return object_t(std::exp(kImagI * (2 * M_PI / static_cast<double>(n))));
    }

    virtual object_t operator+(const IField& right) const override {
        return object_t(
            std::operator+(*this, dynamic_cast<const ComplexField&>(right)));
    }

    virtual object_t operator-(const IField& right) const override {
        return object_t(
            std::operator-(*this, dynamic_cast<const ComplexField&>(right)));
    }

    virtual object_t operator*(const IField& right) const override {
        return object_t(
            std::operator*(*this, dynamic_cast<const ComplexField&>(right)));
    }

    virtual object_t operator/(const IField& right) const override {
        return object_t(
            std::operator/(*this, dynamic_cast<const ComplexField&>(right)));
    }

    virtual object_t operator^(const IField<ComplexField>& n) const override {
        return object_t(std::pow(*this, dynamic_cast<const ComplexField&>(n)));
    }
};

template <size_t P>
class ZpField : public IField<ZpField<P>> {};

template <typename Field>
class FFTConverter {
  public:
    // using Field = IField<Field>;

  private:
    using Array = std::vector<Field>;
    Array root_pows_;

    template <bool is_reversed = false>
    Field GetRootPow(ssize_t i) const {
        if constexpr (is_reversed)
            i = -i;
        return root_pows_[i % root_pows_.size()];
    }

    template <bool is_reversed = false>
    Array ConvertImpl(const Array& x) const {
        auto k = static_cast<size_t>(std::round(std::log2l(x.size())));
        auto dp = Array(x.size());
        for (size_t i = 0; i < dp.size(); ++i) {
            size_t ind = 0;
            for (size_t j = 0; j < k; ++j) {
                ind = (ind << 1) | (1 & (i >> j));
            }
            dp[ind] = x[i];
        }
        for (size_t j = 0; j < k; ++j) {
            size_t n = 1 << j;
            size_t m = k - j - 1;
            for (size_t block = 0; block < (1 << m); ++block) {
                for (size_t i = 0; i < n; ++i) {
                    auto& dp1 = dp[i + block * (n << 1)];
                    auto& dp2 = dp[i + block * (n << 1) + n];
                    auto omega1 = GetRootPow<is_reversed>((1 << m) * i);
                    auto omega2 = GetRootPow<is_reversed>((1 << m) * (i + n));
                    auto dp1_new =
                        dp1 + GetRootPow<is_reversed>((1 << m) * i) * dp2;
                    auto dp2_new =
                        dp1 + GetRootPow<is_reversed>((1 << m) * (i + n)) * dp2;
                    dp1 = dp1_new;
                    dp2 = dp2_new;
                }
            }
        }
        return dp;
    }

  public:
    FFTConverter(size_t n) : root_pows_(n) {
        auto primeRoot = root_pows_[0].GetPrimeRoot(n);
        root_pows_[0] = primeRoot ^ decltype(primeRoot)(0);
        for (size_t i = 1; i < root_pows_.size(); ++i) {
            root_pows_[i] = root_pows_[i - 1] * primeRoot;
        }
    }

    Array Convert(const Array& x) const { return ConvertImpl(x); }

    Array Invert(const Array& x) const { return ConvertImpl<true>(x); }
};

template <typename Field>
class Polynom {
  public:
    size_t deg;
    std::vector<Field> coefs;

    Polynom(size_t deg, const std::vector<Field>& koefs)
        : deg(deg), coefs(koefs.rbegin(), koefs.rend()) {
        this->coefs.resize(deg + 1);
    }

    Polynom(const Polynom&) = default;
    Polynom(Polynom&&) = default;

    Field operator()(const Field& x) const {
        auto res = coefs[0];
        auto x_pow = x;
        for (auto a : coefs) {
            res += x_pow * a;
            x_pow *= x;
        }
        return res;
    }

    template <typename AnyField>
    friend Polynom<AnyField> operator*(const Polynom<AnyField>&,
                                       const Polynom<AnyField>&);
};

template <typename Field>
Polynom<Field> operator*(const Polynom<Field>& left,
                         const Polynom<Field>& right) {
    auto koefs_left = left.coefs;
    auto koefs_right = right.coefs;
    size_t k = static_cast<int>(std::ceil(std::log2(left.deg + right.deg + 1)));
    koefs_left.resize(1 << k);
    koefs_right.resize(1 << k);
    FFTConverter<Field> fft(koefs_left.size());
    auto p = fft.Convert(koefs_left);
    auto q = fft.Convert(koefs_right);
    auto r = p;
    for (size_t i = 0; i < r.size(); ++i) {
        r[i] *= q[i];
    }
    auto res_koefs = fft.Invert(r);
    for (auto& x : res_koefs)
        x /= res_koefs.size();
    res_koefs.resize(left.deg + right.deg + 1);
    return Polynom<Field>(res_koefs.size() - 1,
                          {res_koefs.rbegin(), res_koefs.rend()});
}
