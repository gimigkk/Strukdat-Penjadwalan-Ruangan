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

struct MemoryUsage {
    long vmSizeKb = -1;
    long vmRssKb = -1;
    long vmHwmKb = -1;
    bool ok = false;
};

struct MemoryRecord {
    string label;
    long vmSizeKb;
    long vmRssKb;
    long vmHwmKb;
    long deltaRssKb;
};

static vector<LatencyRecord> latencyLog;
static size_t latencyPrintedIdx = 0; // indeks entri terakhir yang sudah dicetak

static vector<MemoryRecord> memoryLog;
static size_t memoryPrintedIdx = 0;
static bool memoryLastRssReady = false;
static long memoryLastRssKb = 0;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

// Rekam dulu, cetak nanti lewat flushLatencyPrints()
static void record(const string& label, double ms) {
    latencyLog.push_back({label, ms});
}

static bool parseStatusKb(const string& line, const string& key, long& outKb) {
    if (line.rfind(key, 0) != 0) return false;

    string unit;
    stringstream ss(line.substr(key.size()));
    ss >> outKb >> unit;
    return !ss.fail();
}

// Baca penggunaan memori proses dari /proc/self/status (Linux).
static MemoryUsage readProcessMemoryUsage() {
    MemoryUsage usage;
    ifstream status("/proc/self/status");
    if (!status.is_open()) return usage;

    string line;
    while (getline(status, line)) {
        parseStatusKb(line, "VmSize:", usage.vmSizeKb);
        parseStatusKb(line, "VmRSS:", usage.vmRssKb);
        parseStatusKb(line, "VmHWM:", usage.vmHwmKb);
    }

    usage.ok = usage.vmSizeKb >= 0 || usage.vmRssKb >= 0 || usage.vmHwmKb >= 0;
    return usage;
}

static string fmtKb(long kb) {
    if (kb < 0) return "-";
    return to_string(kb);
}

inline void recordMemoryUsage(const string& label) {
    MemoryUsage usage = readProcessMemoryUsage();
    if (!usage.ok) {
        cout << RED << "ERR: Gagal membaca /proc/self/status untuk cek memori." << RESET << "\n";
        return;
    }

    long delta = 0;
    if (memoryLastRssReady && usage.vmRssKb >= 0) {
        delta = usage.vmRssKb - memoryLastRssKb;
    }

    memoryLog.push_back({label, usage.vmSizeKb, usage.vmRssKb, usage.vmHwmKb, delta});

    if (usage.vmRssKb >= 0) {
        memoryLastRssKb = usage.vmRssKb;
        memoryLastRssReady = true;
    }
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

inline void flushMemoryPrints() {
    if (memoryPrintedIdx >= memoryLog.size()) return;
    cout << "\n";
    for (size_t i = memoryPrintedIdx; i < memoryLog.size(); i++) {
        cout << GRAY << "Memory Report: " << memoryLog[i].label
             << ": VmRSS " << fmtKb(memoryLog[i].vmRssKb) << " KiB"
             << " (Delta RSS " << showpos << memoryLog[i].deltaRssKb << noshowpos << " KiB)"
             << ", VmHWM " << fmtKb(memoryLog[i].vmHwmKb) << " KiB"
             << ", VmSize " << fmtKb(memoryLog[i].vmSizeKb) << " KiB"
             << RESET << "\n";
    }
    memoryPrintedIdx = memoryLog.size();
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

void printMemorySummary() {
    if (memoryLog.empty()) return;

    const int W_OP    = 34;
    const int W_NUM   = 11;

    auto seg  = [&](int w) { return rep("─", w + 2); };
    auto segD = [&](int w) { return rep("═", w + 2); };

    auto top = [&]() { cout << "┌" << seg(W_OP)  << "┬" << seg(W_NUM) << "┬" << seg(W_NUM) << "┬" << seg(W_NUM) << "┬" << seg(W_NUM) << "┐\n"; };
    auto mid = [&]() { cout << "├" << seg(W_OP)  << "┼" << seg(W_NUM) << "┼" << seg(W_NUM) << "┼" << seg(W_NUM) << "┼" << seg(W_NUM) << "┤\n"; };
    auto div = [&]() { cout << "╞" << segD(W_OP) << "╪" << segD(W_NUM)<< "╪" << segD(W_NUM)<< "╪" << segD(W_NUM)<< "╪" << segD(W_NUM)<< "╡\n"; };
    auto bot = [&]() { cout << "└" << seg(W_OP)  << "┴" << seg(W_NUM) << "┴" << seg(W_NUM) << "┴" << seg(W_NUM) << "┴" << seg(W_NUM) << "┘\n"; };

    auto row = [&](const string& op, const string& rss,
                   const string& delta, const string& hwm, const string& size) {
        cout << "│ " << left  << setw(W_OP)  << op.substr(0, W_OP)
             << " │ " << right << setw(W_NUM) << rss
             << " │ " << right << setw(W_NUM) << delta
             << " │ " << right << setw(W_NUM) << hwm
             << " │ " << right << setw(W_NUM) << size
             << " │\n";
    };

    auto fmtDelta = [](long kb) -> string {
        string sign = kb > 0 ? "+" : "";
        return sign + to_string(kb);
    };

    cout << "\n";
    top();
    row("Memory Checkpoint", "RSS KiB", "Delta KiB", "HWM KiB", "Size KiB");
    div();

    long totalDeltaKb = 0;
    long peakHwmKb = -1;

    for (int i = 0; i < (int)memoryLog.size(); i++) {
        const auto& r = memoryLog[i];
        totalDeltaKb += r.deltaRssKb;
        if (r.vmHwmKb > peakHwmKb) peakHwmKb = r.vmHwmKb;

        row(r.label, fmtKb(r.vmRssKb), fmtDelta(r.deltaRssKb), fmtKb(r.vmHwmKb), fmtKb(r.vmSizeKb));
        if (i + 1 < (int)memoryLog.size()) mid();
    }

    const auto& last = memoryLog.back();
    div();
    row("Total Delta", fmtKb(last.vmRssKb), fmtDelta(totalDeltaKb), fmtKb(peakHwmKb), fmtKb(last.vmSizeKb));
    bot();
}

#endif
