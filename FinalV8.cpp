#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <omp.h>
#include <string>
#include <iomanip>   // for formatting
#include <stdexcept>
using namespace std;

class HeatSimulation {
public:
    HeatSimulation(size_t N, size_t steps, double alpha)
        : N(N), steps(steps), alpha(alpha),
          grid(N * N, 0.0), new_grid(N * N, 0.0)
    {
        if (alpha > 0.25) {
            throw runtime_error("Alpha too large (unstable scheme)");
        }
        initialize();
    }

    void run() {
        const auto start = chrono::high_resolution_clock::now();

        for (size_t step = 0; step <= steps; ++step) {

            // Parallel update controls how many threads used
#pragma omp parallel for collapse(2) schedule(static)
            for (size_t i = 1; i < N - 1; ++i) {
                for (size_t j = 1; j < N - 1; ++j) {

                    const size_t center = i * N + j;

                    new_grid[center] =
                        grid[center] +
                        alpha * (
                            grid[center + N] +     // down
                            grid[center - N] +     // up
                            grid[center + 1] +     // right
                            grid[center - 1]       // left
                            - 4.0 * grid[center]   
                        );
                }
            }

            apply_boundary(new_grid);

            swap(grid, new_grid);

            if (step % 200 == 0) {
                save_snapshot(step);
            }
        }

        const auto end = chrono::high_resolution_clock::now();
        const chrono::duration<double> duration = end - start;

        cout << "Execution time: " << duration.count() << " seconds\n";
    }

private:
    size_t N;
    size_t steps;
    double alpha;

    vector<double> grid;
    vector<double> new_grid;

    void initialize() {
        apply_boundary(grid);
    }

    void apply_boundary(vector<double>& g) {
        for (size_t i = 0; i < N; ++i) {
            g[i * N + 0]       = 100.0;
            g[i * N + N - 1]   = 100.0;
            g[0 * N + i]       = 100.0;
            g[(N - 1) * N + i] = 100.0;
        }
    }

    void save_snapshot(size_t step) {
        ofstream file("heat_" + to_string(step) + ".dat");

        if (!file) {
            cerr << "Error: Could not open file for writing\n";
            return;
        }

        file << fixed << setprecision(2);

        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < N; ++j) {
                file << grid[i * N + j];
                if (j < N - 1) file << " ";  // avoid trailing space
            }
            file << "\n";
        }
    }
};

int main(int argc, char* argv[]) {
    size_t N = 500;
    //size_t N = 2000;
    size_t steps = 1000;
    double alpha = 0.1;

    if (argc > 1) N = stoul(argv[1]);

    try {
        HeatSimulation sim(N, steps, alpha);
        sim.run();
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

/*

Run:
examples:
./FinalV8 100
./FinalV8 500
./FinalV8 1000
./FinalV8 2000

export OMP_NUM_THREADS=1
./FinalV8 100

export OMP_NUM_THREADS=2
./FinalV8 500

export OMP_NUM_THREADS=4
./FinalV8 1000

export OMP_NUM_THREADS=8
./FinalV8 2000

export OMP_NUM_THREADS=16
./FinalV8 2000
*/