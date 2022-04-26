import csv
import sys
import matplotlib.pyplot as plt
import numpy as np

CSV_ALLOC_SIZE = 0
CSV_SYS_MALLOC = 1
CSV_SYS_FREE = 2
CSV_SLAB_MALLOC = 3
CSV_SLAB_FREE = 4

OWN_ALLOC_SIZE = 0
OWN_SYS_TIME = 1
OWN_SLAB_TIME = 2
OWN_AF = 3

def plot_af(malloc_data, free_data):
    labels = []
    malloc_afs = []
    free_afs = []
    for i in range(len(malloc_data)):
        labels.append(malloc_data[i][OWN_ALLOC_SIZE])
        malloc_afs.append(malloc_data[i][OWN_AF])
        free_afs.append(free_data[i][OWN_AF])
    
    x = np.arange(len(labels))  # the label locations
    width = 0.35  # the width of the bars
    
    fig, ax = plt.subplots()
    rects1 = ax.bar(x - width/2, malloc_afs, width, label='Malloc')
    rects2 = ax.bar(x + width/2, free_afs, width, label='Free')
    
    # Add some text for labels, title and custom x-axis tick labels, etc.
    ax.set_ylabel('Acceleration factor')
    ax.set_title('Acceleration factors')
    ax.set_xticks(x, labels)
    ax.legend()

    ax.bar_label(rects1, padding=3)
    ax.bar_label(rects2, padding=3)
    
    fig.tight_layout()
    
    plt.show()

def plot_speed_comparisons(data):
    labels = []
    sys_time = []
    slabbed_time = []
    for i in range(len(data)):
        labels.append(data[i][OWN_ALLOC_SIZE])
        sys_time.append(data[i][OWN_SYS_TIME])
        slabbed_time.append(data[i][OWN_SLAB_TIME])

    x = np.arange(len(labels))  # the label locations
    width = 0.35  # the width of the bars
    
    fig, ax = plt.subplots()
    rects1 = ax.bar(x - width/2, sys_time, width, label='System')
    rects2 = ax.bar(x + width/2, slabbed_time, width, label='Slabbed')
    
    # Add some text for labels, title and custom x-axis tick labels, etc.
    ax.set_ylabel('Execution time (ns)')
    ax.set_xlabel('Allocation size (bytes)')
    ax.set_title('Speed comparison')
    ax.set_xticks(x, labels)
    ax.legend()

    ax.bar_label(rects1, padding=3)
    ax.bar_label(rects2, padding=3)
    
    fig.tight_layout()
    
    plt.show()

def main():
    if len(sys.argv) != 2:
        print("ERROR: incorrect command line arguments")
        print("Usage: ./charts.py <benchmark_file>")

    malloc_data = []
    free_data = []

    # Read the data and process a table that looks like this:
    # [ alloc_size, system_malloc_time, slabbed_malloc_time, af ]
    with open(sys.argv[1]) as csv_file:
        csv_reader = csv.reader(csv_file)
        header = next(csv_reader)

        for row in csv_reader:
            # Read the data from the CSV file
            alloc_size = row[CSV_ALLOC_SIZE]
            system_malloc_time = int(row[CSV_SYS_MALLOC])
            system_free_time = int(row[CSV_SYS_FREE])
            slabbed_malloc_time = int(row[CSV_SLAB_MALLOC])
            slabbed_free_time = int(row[CSV_SLAB_FREE])

            # Calculate acceleration factor (af > 1 is an improvement, else slower)
            malloc_af = system_malloc_time / slabbed_malloc_time
            free_af = system_free_time / slabbed_free_time

            # Append to the global data
            malloc_data.append([ alloc_size, system_malloc_time, slabbed_malloc_time, malloc_af ])
            free_data.append([ alloc_size, system_free_time, slabbed_free_time, free_af ])


    # Plot the AF per size
    plot_af(malloc_data, free_data)

    # Plot speed comparisons
    plot_speed_comparisons(malloc_data)
    plot_speed_comparisons(free_data)



if __name__ == "__main__":
    main()
