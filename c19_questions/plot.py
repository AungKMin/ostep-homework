import argparse
import platform
import shutil
import subprocess 

import matplotlib.pyplot as plt
import numpy as np

parser = argparse.ArgumentParser()
parser.add_argument("pages", type=int, help="input = x then 1 to 2^(x - 1) pages")
parser.add_argument("trials", type=int)
parser.add_argument("--single_cpu", action="store_true")
args = parser.parse_args()

data = []
x = np.arange(args.pages)
pages = 2**x 
for num_pages in pages:
    if not args.single_cpu:
        average_access_time_in_ns = float(
            subprocess.run(
                ["./tlb.out", str(num_pages), str(args.trials)],
                capture_output=True,
                check=True,
            ).stdout.splitlines()[-1]
        )
    else:
        # if taskset can be run
        if shutil.which("taskset"):
            average_access_time_in_ns = float(
                subprocess.run(
                    ["taskset", "-c", "0", "./tlb.out", str(num_pages), str(args.trials)],
                    capture_output=True,
                    check=True,
                ).stdout.splitlines()[-1]
            )
        elif shutil.which("hwloc-bind"):
            average_access_time_in_ns = float(
                subprocess.run(
                    ["hwloc-bind", "package:0.pu:0", "--", "./tlb.out", str(num_pages), str(args.trials)],
                    capture_output=True,
                    check=True,
                ).stdout.splitlines()[-1]
            )
        else:
            raise Exception("hwloc-bind and taskset not available")
    data.append(average_access_time_in_ns)


plt.plot(x, data, marker="o", color="blue")
plt.xticks(x, pages, fontsize="x-small")
plt.xlabel("Number of Pages")
plt.ylabel("Average Time Per Access (ns)")
title = f"{platform.system()} {platform.release()} {platform.machine()}"
if args.single_cpu:
    title += " (single CPU)"
plt.title(title)
plt.savefig(
   f'{platform.system()}_{args.trials}{"_single.png" if args.single_cpu else ".png"}', dpi=227
)
print("end")