import numpy as np
import matplotlib.pyplot as plt
import argparse
import os
import matplotlib
matplotlib.use('TkAgg')
 
#how to run use:
# "C:\Users\smnea\AppData\Local\Programs\Python\Python313\python.exe" Visualizations.py heat_600.dat
#in cmd prompt for 1 or "C:\Users\smnea\AppData\Local\Programs\Python\Python313\python.exe" Visualizations.py heat_600.dat
#for all of them at once


def load_data(filename):
    try:
        return np.loadtxt(filename)
    except Exception as e:
        print(f"Error loading file: {e}")
        return None


def plot_heatmap(data, title, output_file=None):
    """
    Create and display/save a heatmap.
    Optimized for large grids.
    """
    plt.figure(figsize=(6, 5))

    im = plt.imshow(
        data,
        cmap='hot',
        origin='lower',
        interpolation='nearest'  # important for grid data
    )

    plt.colorbar(im, label='Temperature')

    plt.title(title)
    plt.xlabel('X')
    plt.ylabel('Y')

    plt.gca().set_aspect('equal')

    if output_file:
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        print(f"Saved heatmap to {output_file}")
    else:
        plt.show()


def extract_step(filename):
    """
    Extract step from filename such as heat_200.dat
    """
    try:
        base = os.path.basename(filename)
        if "heat_" in base:
            return base.split("heat_")[1].split(".")[0]
    except:
        pass
    return "Unknown"


def main():
    import glob

    parser = argparse.ArgumentParser(description="Visualize heat diffusion .dat files")
    parser.add_argument("pattern", help="File pattern (e.g., heat_*.dat)")
    args = parser.parse_args()

    files = sorted(glob.glob(args.pattern))
    print("Loaded files:", files)  # debug

    if not files:
        print("No files found.")
        return

    index = 0

    fig, ax = plt.subplots(figsize=(6, 5))
    im = None

    def update():
        nonlocal im
        ax.clear()

        file = files[index]
        data = load_data(file)

        if data is None:
            return

        step = extract_step(file)

        im = ax.imshow(
            data,
            cmap='hot',
            origin='lower',
            interpolation='nearest'
        )

        ax.set_title(f"Heat Diffusion (Step {step})")
        ax.set_xlabel('X')
        ax.set_ylabel('Y')
        ax.set_aspect('equal')

        plt.draw()

    def on_key(event):
        print("Key pressed:", event.key)  # debug
        nonlocal index

        if event.key == "right":
            index = (index + 1) % len(files)
            update()
        elif event.key == "left":
            index = (index - 1) % len(files)
            update()

    fig.canvas.mpl_connect('key_press_event', on_key)

    update()
    plt.show()


if __name__ == "__main__":
    main()