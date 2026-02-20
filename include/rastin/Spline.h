#pragma once

#include <vector>
#include <algorithm>
#include <cassert>
//#include <wx/string.h>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>

class Spline
{
public:
    Spline() = default;

    void init(const std::vector<double>& x, const std::vector<double>& y) {
        assert(x.size() == y.size());
        assert(x.size() >= 2);
        xArray = x;
        yArray = y;
        
        const size_t n = x.size();
        
        segment.resize(n - 1);

        if (n == 2) {
            // Линейный случай — сплайн вырождается в прямую
            double h = x[1] - x[0];
            segment[0].abscis = x[0];
            segment[0].a = y[0];
            segment[0].b = (y[1] - y[0]) / h;
            segment[0].c = 0.0;
            segment[0].d = 0.0;
            return;
        }

        std::vector<double> h(n - 1);
        for (size_t i = 0; i < n - 1; ++i) {
            h[i] = x[i + 1] - x[i];
        }

        std::vector<double> alpha(n, 0.0); // инициализируем нулями
        for (size_t i = 1; i < n - 1; ++i) {
            alpha[i] = 3.0 * ((y[i + 1] - y[i]) / h[i] - (y[i] - y[i - 1]) / h[i - 1]);
        }

        std::vector<double> l(n);
        std::vector<double> mu(n);
        std::vector<double> z(n);

        // Прямой ход (метод прогонки)
        l[0] = 1.0;
        mu[0] = 0.0;
        z[0] = 0.0;

        for (size_t i = 1; i < n - 1; ++i) {
            l[i] = 2.0 * (h[i - 1] + h[i]) - h[i - 1] * mu[i - 1];
            mu[i] = h[i] / l[i];
            z[i] = (alpha[i] - h[i - 1] * z[i - 1]) / l[i];
        }

        l[n - 1] = 1.0;
        z[n - 1] = 0.0;

        // Обратный ход
        std::vector<double> c(n);
        c[n - 1] = 0.0;
        for (size_t i = n - 2; i >= 1; --i) {
            c[i] = z[i] - mu[i] * c[i + 1];
        }
        c[0] = 0.0; //

        // Вычисление коэффициентов сплайна
        
        for (size_t i = 0; i < n - 1; ++i) {
            segment[i].abscis = x[i];
            segment[i].a = y[i];
            segment[i].b = (y[i + 1] - y[i]) / h[i] - h[i] * (2.0 * c[i] + c[i + 1]) / 3.0;
            segment[i].c = c[i];
            segment[i].d = (c[i + 1] - c[i]) / (3.0 * h[i]);
        }

        //=========================================================================//
        //Интерполяция y от x 
        auto yReverse = y;
        auto xReverse = x;
        std::reverse(yReverse.begin(), yReverse.end());
        std::reverse(xReverse.begin(), xReverse.end());

        segmentReverse.resize(n - 1);

        if (n == 2) {
            // Линейный случай — сплайн вырождается в прямую (x <--> y)
            double hReverse = yReverse[1] - yReverse[0];
            segmentReverse[0].abscis = yReverse[0];
            segmentReverse[0].a = xReverse[0];
            segmentReverse[0].b = (xReverse[1] - xReverse[0]) / hReverse;
            segmentReverse[0].c = 0.0;
            segmentReverse[0].d = 0.0;
            return;
        }

        std::vector<double> hReverse(n - 1);
        for (size_t i = 0; i < n - 1; ++i) {
            hReverse[i] = yReverse[i + 1] - yReverse[i];
        }

        std::vector<double> alphaReverse(n, 0.0); // инициализируем нулями
        for (size_t i = 1; i < n - 1; ++i) {
            alphaReverse[i] = 3.0 * ((xReverse[i + 1] - xReverse[i]) / hReverse[i] - (xReverse[i] - xReverse[i - 1]) / hReverse[i - 1]);
        }

        std::vector<double> lReverse(n);
        std::vector<double> muReverse(n);
        std::vector<double> zReverse(n);

        // Прямой ход (метод прогонки)
        lReverse[0] = 1.0;
        muReverse[0] = 0.0;
        zReverse[0] = 0.0;

        for (size_t i = 1; i < n - 1; ++i) {
            lReverse[i] = 2.0 * (hReverse[i - 1] + hReverse[i]) - hReverse[i - 1] * muReverse[i - 1];
            muReverse[i] = hReverse[i] / lReverse[i];
            zReverse[i] = (alphaReverse[i] - hReverse[i - 1] * zReverse[i - 1]) / lReverse[i];
        }

        lReverse[n - 1] = 1.0;
        zReverse[n - 1] = 0.0;

        // Обратный ход
        std::vector<double> cReverse(n);
        cReverse[n - 1] = 0.0;
        for (size_t i = n - 2; i >= 1; --i) {
            cReverse[i] = zReverse[i] - muReverse[i] * cReverse[i + 1];
        }
        cReverse[0] = 0.0; // 

        // Вычисление коэффициентов сплайна
        for (size_t i = 0; i < n - 1; ++i) {
            segmentReverse[i].abscis = yReverse[i];
            segmentReverse[i].a = xReverse[i];
            segmentReverse[i].b = (xReverse[i + 1] - xReverse[i]) / hReverse[i] - hReverse[i] * (2.0 * cReverse[i] + cReverse[i + 1]) / 3.0;
            segmentReverse[i].c = cReverse[i];
            segmentReverse[i].d = (cReverse[i + 1] - cReverse[i]) / (3.0 * hReverse[i]);
        }
    }

    double interpolate(double x_val) const {
        if (x_val <= xArray.front()) return segment.front().a;
        if (x_val >= xArray.back()) return segment.back().a +
            segment.back().b * (xArray.back() - segment.back().abscis) +
            segment.back().c * (xArray.back() - segment.back().abscis) * (xArray.back() - segment.back().abscis) +
            segment.back().d * (xArray.back() - segment.back().abscis) * (xArray.back() - segment.back().abscis) * (xArray.back() - segment.back().abscis);

        auto it = std::upper_bound(xArray.begin(), xArray.end(), x_val);
        size_t idx = static_cast<size_t>(std::distance(xArray.begin(), it) - 1);
        if (idx >= segment.size()) idx = segment.size() - 1;

        const auto& seg = segment[idx];
        double dx = x_val - seg.abscis;
        return seg.a + seg.b * dx + seg.c * dx * dx + seg.d * dx * dx * dx;
    }

    double interpolateReverse(double y_val) const {
        if (y_val <= yArray.back()) return segmentReverse.front().a;
        if (y_val >= yArray.front()) return segmentReverse.back().a +
            segmentReverse.back().b * (yArray.front() - segmentReverse.back().abscis) +
            segmentReverse.back().c * (yArray.front() - segmentReverse.back().abscis) * (yArray.front() - segmentReverse.back().abscis) +
            segmentReverse.back().d * (yArray.front() - segmentReverse.back().abscis) * (yArray.front() - segmentReverse.back().abscis) * (yArray.front() - segmentReverse.back().abscis);

        auto it = std::upper_bound(yArray.rbegin(), yArray.rend(), y_val);
        size_t idy = static_cast<size_t>(std::distance(yArray.rbegin(), it) - 1);
        if (idy >= segmentReverse.size()) idy = segmentReverse.size() - 1;

        const auto& seg = segmentReverse[idy];
        double dy = y_val - seg.abscis;
        return seg.a + seg.b * dy + seg.c * dy * dy + seg.d * dy * dy * dy;
    }

    double GetXMin() {
        return *std::min_element(xArray.begin(), xArray.end());
    }

    double GetXMax() {
        return *std::max_element(xArray.begin(), xArray.end());
    }

    double GetYMin() {
        return *std::min_element(yArray.begin(), yArray.end());
    }

    double GetYMax() {
        return *std::max_element(yArray.begin(), yArray.end());
    }

    double GetXArray(int n) {
        return xArray.at(n);
    }

    double GetYArray(int n) {
        return yArray.at(n);
    }

    size_t GetSizeArray() {
        return xArray.size();
    }

    template <class Archive>
    void serialize(Archive& ar) {
        ar(segment, segmentReverse, xArray, yArray);
    }

private:
    struct SplineSegment {

        double a = 0, b = 0, c = 0, d = 0;
        double abscis = 0;

        template <class Archive>
        void serialize(Archive& ar) {
            ar(a, b, c, d, abscis);
        }
    };
    std::vector<SplineSegment> segment;
    std::vector<SplineSegment> segmentReverse;
    std::vector<double> xArray;
    std::vector<double> yArray;
};

