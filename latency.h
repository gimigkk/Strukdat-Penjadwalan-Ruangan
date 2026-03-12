#ifndef LATENCY_H
#define LATENCY_H

#include <bits/stdc++.h>
#include <chrono>
#include "utils.h"

using namespace std;
using namespace chrono;

// ---------------------------------------------------------------------------
// Data
// ---------------------------------------------------------------------------

struct LatencyRecord {
    string label;
    double ms;
};

static vector<LatencyRecord> latencyLog;
static size_t latencyPrintedIdx = 0; // indeks entri terakhir yang sudah dicetak

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

// Rekam dulu, cetak nanti lewat flushLatencyPrints()
static void record(const string& label, double ms) {
    latencyLog.push_back({label, ms});
}

// Cetak semua entri yang belum dicetak - panggil setelah browse UI selesai
inline void flushLatencyPrints() {
    if (latencyPrintedIdx >= latencyLog.size()) return;
    cout << "\n";
    for (size_t i = latencyPrintedIdx; i < latencyLog.size(); i++) {
        cout << GRAY << "Latency Report: " << latencyLog[i].label << ": "
             << fixed << setprecision(4) << latencyLog[i].ms << " ms" << RESET << "\n";
    }
    latencyPrintedIdx = latencyLog.size();
}

// Repeats a multi-byte unicode string n times (needed for box-drawing chars)
static string rep(const string& s, int n) {
    string out;
    for (int i = 0; i < n; i++) out += s;
    return out;
}

// ---------------------------------------------------------------------------
// Timer - RAII, wraps a single DS operation
// ---------------------------------------------------------------------------

struct ScopeTimer {
    explicit ScopeTimer(const string& label)
        : label_(label), start_(high_resolution_clock::now()), stopped_(false) {}

    // Stop eksplisit - kalau tidak dipanggil, otomatis stop saat destruktor
    void stop() {
        if (stopped_) return;
        stopped_ = true;
        double ms = duration<double, milli>(high_resolution_clock::now() - start_).count();
        record(label_, ms);
    }

    ~ScopeTimer() { stop(); }

private:
    string                             label_;
    time_point<high_resolution_clock>  start_;
    bool                               stopped_;
};

// ---------------------------------------------------------------------------
// Summary table
// ---------------------------------------------------------------------------

void printLatencySummary() {
    if (latencyLog.empty()) return;

    // Aggregate per label, preserving first-seen order
    map<string, vector<double>> grouped;
    vector<string> order;
    for (const auto& r : latencyLog) {
        if (!grouped.count(r.label)) order.push_back(r.label);
        grouped[r.label].push_back(r.ms);
    }

    const int W_OP  = 30;
    const int W_N   =  4;
    const int W_NUM =  9;

    auto seg  = [&](int w) { return rep("─", w + 2); };
    auto segD = [&](int w) { return rep("═", w + 2); };

    auto top = [&]() { cout << "┌" << seg(W_OP)  << "┬" << seg(W_N)  << "┬" << seg(W_NUM) << "┬" << seg(W_NUM) << "┬" << seg(W_NUM) << "┐\n"; };
    auto mid = [&]() { cout << "├" << seg(W_OP)  << "┼" << seg(W_N)  << "┼" << seg(W_NUM) << "┼" << seg(W_NUM) << "┼" << seg(W_NUM) << "┤\n"; };
    auto div = [&]() { cout << "╞" << segD(W_OP) << "╪" << segD(W_N) << "╪" << segD(W_NUM)<< "╪" << segD(W_NUM)<< "╪" << segD(W_NUM)<< "╡\n"; };
    auto bot = [&]() { cout << "└" << seg(W_OP)  << "┴" << seg(W_N)  << "┴" << seg(W_NUM) << "┴" << seg(W_NUM) << "┴" << seg(W_NUM) << "┘\n"; };

    auto row = [&](const string& op, const string& n,
                   const string& avg, const string& mn, const string& mx) {
        cout << "│ " << left  << setw(W_OP)  << op.substr(0, W_OP)
             << " │ " << right << setw(W_N)   << n
             << " │ " << right << setw(W_NUM) << avg
             << " │ " << right << setw(W_NUM) << mn
             << " │ " << right << setw(W_NUM) << mx
             << " │\n";
    };

    auto fmt = [](double v) -> string {
        ostringstream s;
        s << fixed << setprecision(4) << v;
        return s.str();
    };

    cout << "\n";
    top();
    row("DS Operation", "N", "Avg (ms)", "Min (ms)", "Max (ms)");
    div();

    int    grand_n     = 0;
    double grand_total = 0;

    for (int i = 0; i < (int)order.size(); i++) {
        const auto& v  = grouped[order[i]];
        int    n   = (int)v.size();
        double avg = accumulate(v.begin(), v.end(), 0.0) / n;
        double mn  = *min_element(v.begin(), v.end());
        double mx  = *max_element(v.begin(), v.end());
        grand_n     += n;
        grand_total += avg * n;

        row(order[i], to_string(n), fmt(avg), fmt(mn), fmt(mx));
        if (i + 1 < (int)order.size()) mid();
    }

    div();
    row("Total", to_string(grand_n), "", "", fmt(grand_total));
    bot();
}

#endif