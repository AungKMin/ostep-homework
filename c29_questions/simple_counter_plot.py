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
parser.add_argument("-num_cpus", required=False, type=int, help="number of cpus to use (can't be more than `nproc`)")
args = parser.parse_args()

data = []

x = range(args.num_threads_start, args.num_threads_stop, args.num_threads_step)
for num_threads in x:
    duration_in_microseconds = float(
        subprocess.run(
            ["./simple_counter.out", str(num_threads), str(args.num_loops), str(args.num_cpus)] if args.num_cpus != None else ["./simple_counter.out", str(num_threads), str(args.num_loops)],
            capture_output=True,
            check=True,
        ).stdout
    )
    data.append(duration_in_microseconds)


plt.plot(x, data, marker="o", color="blue")
# plt.xticks(x, pages, fontsize="x-small")
plt.xlabel("Number of Threads")
plt.ylabel("Duration (microseconds)")
title = f"{platform.system()} {platform.release()} {platform.machine()} " + str(args.num_loops) + " loops " + (str(args.num_cpus) + " cpus" if args.num_cpus != None else "") 
plt.title(title)
plt.savefig(
   f'simple_counter_images/{platform.system()}_{args.num_threads_start}-{args.num_threads_stop}_threads_{args.num_loops}_loops{"_" + str(args.num_cpus) + "_cpus" if args.num_cpus != None else ""}.png', dpi=227
)
print("end")