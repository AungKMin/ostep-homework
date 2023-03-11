import argparse
import platform
import shutil
import subprocess 

import matplotlib.pyplot as plt
import numpy as np

parser = argparse.ArgumentParser()
parser.add_argument("num_threads_start", type=int, help="start of number of threads range")
parser.add_argument("num_threads_stop", type=int, help="stop of number of threads range")
parser.add_argument("num_threads_step", type=int, help="step of number of threads range")
parser.add_argument("num_loops", type=int, help="number of times that each thread will increment the counter")
parser.add_argument("threshold", type=int, help="the threshold a local value can reach before transfering its values to global value");
parser.add_argument("num_cpus", type=int, help="number of cpus to use (can't be more than `nproc`)")
parser.add_argument("-flip_threads_cpu", required=False, type=bool, default=False, help="if this is done, the parameters for number of threads and cpus are swapped")
args = parser.parse_args()

simple_counter_data = []
approximate_counter_data = []

x = range(args.num_threads_start, args.num_threads_stop, args.num_threads_step)
if args.flip_threads_cpu:
    for num_cpus in x:
        duration_in_microseconds = float(
            subprocess.run(
                ["./simple_counter.out", str(args.num_cpus), str(args.num_loops), str(num_cpus)],
                capture_output=True,
                check=True,
            ).stdout
        )
        simple_counter_data.append(duration_in_microseconds)
        duration_in_microseconds = float(
            subprocess.run(
                ["./approximate_counter.out", str(args.num_cpus), str(args.num_loops), str(args.threshold), str(num_cpus)],
                capture_output=True,
                check=True,
            ).stdout
        )
        approximate_counter_data.append(duration_in_microseconds)  
    plt.plot(x, simple_counter_data, marker="o", color="blue", label="simple counter")
    plt.plot(x, approximate_counter_data, marker="o", color="red", label="approximate counter")
    plt.legend()
    # plt.xticks(x, pages, fontsize="x-small")
    plt.xlabel("Number of CPUs")
    plt.ylabel("Duration (microseconds)")
    title = f"{platform.system()} {platform.release()} {platform.machine()} " + str(args.num_loops) + " loops " + (str(args.num_cpus) + " threads") 
    plt.title(title)
    plt.savefig(
    f'approximate_counter_images/{platform.system()}_{args.num_threads_start}-{args.num_threads_stop}_cpus_{args.num_loops}_loops{"_" + str(args.num_cpus) + "_threads"}.png', dpi=227
    )
else:
    for num_threads in x:
        duration_in_microseconds = float(
            subprocess.run(
                ["./simple_counter.out", str(num_threads), str(args.num_loops), str(args.num_cpus)],
                capture_output=True,
                check=True,
            ).stdout
        )
        simple_counter_data.append(duration_in_microseconds)
        duration_in_microseconds = float(
            subprocess.run(
                ["./approximate_counter.out", str(num_threads), str(args.num_loops), str(args.threshold), str(args.num_cpus)],
                capture_output=True,
                check=True,
            ).stdout
        )
        approximate_counter_data.append(duration_in_microseconds)

    plt.plot(x, simple_counter_data, marker="o", color="blue", label="simple counter")
    plt.plot(x, approximate_counter_data, marker="o", color="red", label="approximate counter")
    plt.legend()
    # plt.xticks(x, pages, fontsize="x-small")
    plt.xlabel("Number of Threads")
    plt.ylabel("Duration (microseconds)")
    title = f"{platform.system()} {platform.release()} {platform.machine()} " + str(args.num_loops) + " loops " + (str(args.num_cpus) + " cpus") 
    plt.title(title)
    plt.savefig(
    f'approximate_counter_images/{platform.system()}_{args.num_threads_start}-{args.num_threads_stop}_threads_{args.num_loops}_loops{"_" + str(args.num_cpus) + "_cpus"}.png', dpi=227
    )
print("end")