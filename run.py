import subprocess
import os

s=0
cat="/home/lane1/Desktop/MyCode/CSAPP/lab/lab5/my_code/code7/"
addresses = [
    "traces/LONG_MOBILE-1.bt9.trace.gz",
    "traces/LONG_MOBILE-2.bt9.trace.gz",
    "traces/LONG_MOBILE-3.bt9.trace.gz",
    "traces/LONG_MOBILE-4.bt9.trace.gz",
    "traces/SHORT_MOBILE-1.bt9.trace.gz",
    "traces/SHORT_MOBILE-2.bt9.trace.gz",
    "traces/SHORT_MOBILE-3.bt9.trace.gz",
    "traces/SHORT_MOBILE-4.bt9.trace.gz",
    "traces/SHORT_MOBILE-24.bt9.trace.gz",
    "traces/SHORT_MOBILE-25.bt9.trace.gz",
    "traces/SHORT_MOBILE-27.bt9.trace.gz",
    "traces/SHORT_MOBILE-28.bt9.trace.gz",
    "traces/SHORT_MOBILE-30.bt9.trace.gz"
]

if not os.path.exists("data.txt"):
    open("data.txt", "w").close()

for address in addresses:
    try:
        result = subprocess.run([cat+"predictor", cat+address], capture_output=True, text=True)
        
        if result.returncode == 0:
            output = result.stdout
            num_mispredictions = None
            mispred_per_1k_inst = None
            for line in reversed(output.split("\n")):
                if "NUM_MISPREDICTIONS" in line:
                    num_mispredictions = line.split(": ")[-1].strip()
                elif "MISPRED_PER_1K_INST" in line:
                    mispred_per_1k_inst = line.split(": ")[-1].strip()
            
                if num_mispredictions is not None and mispred_per_1k_inst is not None:
                    break
            
            with open("data.txt", "a") as file:
                file.write(f"{num_mispredictions} {mispred_per_1k_inst}\n")

            print(f"{address} {num_mispredictions} {mispred_per_1k_inst}")
            s+=int(num_mispredictions)
        else:
            print(f"Error running predictor on {address}")
    except Exception as e:
        print(f"An error occurred while processing {address}: {e}")

print("----END----")
print("%.3f" % (s/6037262413*100))